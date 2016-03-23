#include "configmanager.hpp"

#include <iostream>

#include <QCoreApplication>
#include <QStandardPaths>

#include <Utils/pathexpander.hpp>

ConfigManager::ConfigManager()
{
    // app config location
    this->m_configdir = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) + QDir::separator() +
        qApp->organizationName() + QDir::separator() +
        qApp->applicationName() + QDir::separator();

    // config file location
    this->m_configfile = this->m_configdir + "conf.ini";

    // load config file if available and possible
    QFile configfile(this->m_configfile);
    if (configfile.exists())
    {
        QFileInfo fileinfo(this->m_configfile);
        if (fileinfo.isReadable())
            this->loadSettings();

        else this->loadDefaultSettings();
    }

    else
    {

        // attempt to create the config location
        // if it fails use the default settings instead and don't create a QFileSystemWatcher object
        QDir dir(this->m_configdir);
        if (!dir.mkpath(dir.absolutePath()))
        {
            std::cerr << "\033[1;38;2;173;54;0mFATAL!\033[0m Unable to create or read the configuration directory at:\n"
                         "   " << qUtf8Printable(this->m_configdir) << "\n"
                         "Check if you have the sufficient permissions to create or access this directory.\n"
                         "Continue with default settings...\n" << std::endl;

            this->loadDefaultSettings();
        }

        else this->createDefaultConfigFile();
    }

    // expand environment variables in library.rootpath
    this->m_config.put(this->keyStr(Key::LibRootPath),
                       expandPath(
                           QString::fromUtf8(
                               this->m_config.get<std::string>(this->keyStr(Key::LibRootPath)).c_str()
                           )
                       ).toUtf8().constData()
    );
}

ConfigManager::~ConfigManager()
{
    this->m_configdir.clear();
    this->m_configfile.clear();
    this->m_config.clear();
}

QString ConfigManager::value(Key key) const
{
    return QString::fromUtf8(this->m_config.get<std::string>(this->keyStr(key)).c_str());
}

bool ConfigManager::boolean(Key key) const
{
    QString b = QString::fromUtf8(this->m_config.get<std::string>(this->keyStr(key)).c_str());

    if (QString::compare(b, "true", Qt::CaseInsensitive) == 0 ||
        QString::compare(b, "1") == 0)
    {
        b.clear();
        return true;
    }

    b.clear();
    return false;
}

QString ConfigManager::playerFor(const QString &fileformat) const
{
    try
    {
        // player.(fileformat)_player
        return QString::fromUtf8(this->m_config.get<std::string>(("player."+fileformat+"_player").toUtf8().constData()).c_str());
    }

    catch (...)
    {
        return QString();
    }
}

QString ConfigManager::configDir() const
{
    return this->m_configdir;
}

void ConfigManager::createDefaultConfigFile()
{
    this->loadDefaultSettings();

    try
    {
        boost::property_tree::ini_parser::write_ini(qUtf8Printable(this->m_configfile), this->m_config);
    }

    catch (...) { }
}

void ConfigManager::loadDefaultSettings()
{
#define BoostPtreePut(KeyName) \
    this->m_config.put(this->keyStr(KeyName), this->keyDefaultValue(KeyName))

    BoostPtreePut(Key::CmdAudio);
    BoostPtreePut(Key::CmdVideo);
    BoostPtreePut(Key::CmdModule);
    BoostPtreePut(Key::CmdPlaylist);
    BoostPtreePut(Key::CmdPlaylistFileLoad);
    BoostPtreePut(Key::CmdSearch);
    BoostPtreePut(Key::CmdBrowse);
    BoostPtreePut(Key::CmdRandom);
    BoostPtreePut(Key::CmdShuffle);
    BoostPtreePut(Key::CmdRepeat);
    BoostPtreePut(Key::CmdHistory);
    BoostPtreePut(Key::CmdStatistics);
    BoostPtreePut(Key::CmdExit);
    BoostPtreePut(Key::CmdRescan);

    BoostPtreePut(Key::LibRootPath);
    BoostPtreePut(Key::LibAudioFormats);
    BoostPtreePut(Key::LibVideoFormats);
    BoostPtreePut(Key::LibModuleFormats);
    BoostPtreePut(Key::LibPrefixDeletionPatterns);

    BoostPtreePut(Key::PlayerAudio);
    BoostPtreePut(Key::PlayerVideo);
    BoostPtreePut(Key::PlayerModule);

    BoostPtreePut(Key::ToolBrowser);

    BoostPtreePut(Key::HistIgnore);

#undef BoostPtreePut
}

void ConfigManager::loadSettings()
{
    try
    {
        boost::property_tree::ini_parser::read_ini(qUtf8Printable(this->m_configfile), this->m_config);
    }

    catch (...)
    {
        this->loadDefaultSettings();
        return;
    }

    this->addIfMissing(Key::CmdAudio);
    this->addIfMissing(Key::CmdVideo);
    this->addIfMissing(Key::CmdModule);
    this->addIfMissing(Key::CmdPlaylist);
    this->addIfMissing(Key::CmdPlaylistFileLoad);
    this->addIfMissing(Key::CmdSearch);
    this->addIfMissing(Key::CmdBrowse);
    this->addIfMissing(Key::CmdRandom);
    this->addIfMissing(Key::CmdShuffle);
    this->addIfMissing(Key::CmdRepeat);
    this->addIfMissing(Key::CmdHistory);
    this->addIfMissing(Key::CmdStatistics);
    this->addIfMissing(Key::CmdExit);
    this->addIfMissing(Key::CmdRescan);

    this->addIfMissing(Key::LibRootPath);
    this->addIfMissing(Key::LibAudioFormats);
    this->addIfMissing(Key::LibVideoFormats);
    this->addIfMissing(Key::LibModuleFormats);
    this->addIfMissing(Key::LibPrefixDeletionPatterns);

    this->addIfMissing(Key::PlayerAudio);
    this->addIfMissing(Key::PlayerVideo);
    this->addIfMissing(Key::PlayerModule);

    this->addIfMissing(Key::ToolBrowser);

    this->addIfMissing(Key::HistIgnore);

    // try to write back the complete config file
    try
    {
        boost::property_tree::ini_parser::write_ini(qUtf8Printable(this->m_configfile), this->m_config);
    }

    catch (...) { /* ignore errors */ }
}

std::string ConfigManager::keyStr(Key key)
{
    switch (key)
    {
        case Key::CmdAudio: return "commands.audio"; break;
        case Key::CmdVideo: return "commands.video"; break;
        case Key::CmdModule: return "commands.module"; break;
        case Key::CmdPlaylist: return "commands.playlist"; break;
        case Key::CmdPlaylistFileLoad: return "commands.playlist_fileload"; break;
        case Key::CmdSearch: return "commands.search"; break;
        case Key::CmdBrowse: return "commands.browse"; break;
        case Key::CmdRandom: return "commands.random"; break;
        case Key::CmdShuffle: return "commands.shuffle"; break;
        case Key::CmdRepeat: return "commands.repeat"; break;
        case Key::CmdHistory: return "commands.history"; break;
        case Key::CmdStatistics: return "commands.statistics"; break;
        case Key::CmdExit: return "commands.exit"; break;
        case Key::CmdRescan: return "commands.rescan"; break;

        case Key::LibRootPath: return "library.rootpath"; break;
        case Key::LibAudioFormats: return "library.audioformats"; break;
        case Key::LibVideoFormats: return "library.videoformats"; break;
        case Key::LibModuleFormats: return "library.moduleformats"; break;
        case Key::LibPrefixDeletionPatterns: return "library.prefixdeletionpatterns"; break;

        case Key::PlayerAudio: return "player.audioplayer"; break;
        case Key::PlayerVideo: return "player.videoplayer"; break;
        case Key::PlayerModule: return "player.modplayer"; break;

        case Key::ToolBrowser: return "tools.browser"; break;

        case Key::HistIgnore: return "history.histignore"; break;
    }

    return std::string();
}

std::string ConfigManager::keyDefaultValue(Key key)
{
    switch (key)
    {
        case Key::CmdAudio: return "audio"; break;
        case Key::CmdVideo: return "video"; break;
        case Key::CmdModule: return "module"; break;
        case Key::CmdPlaylist: return "playlist"; break;
        case Key::CmdPlaylistFileLoad: return "load"; break;
        case Key::CmdSearch: return "search"; break;
        case Key::CmdBrowse: return "browse"; break;
        case Key::CmdRandom: return "random"; break;
        case Key::CmdShuffle: return "shuffle"; break;
        case Key::CmdRepeat: return "repeat"; break;
        case Key::CmdHistory: return "history"; break;
        case Key::CmdStatistics: return "statistics"; break;
        case Key::CmdExit: return "exit"; break;
        case Key::CmdRescan: return "rescan"; break;

        case Key::LibRootPath: return "$HOME"; break;
        case Key::LibAudioFormats: return "wav, flac, tta, aiff, ape, pcm, alac, dts, m4a, ogg, mka, wma, asf, ra, aac, mp3"; break;
        case Key::LibVideoFormats: return "mp4, h264, h263, ts, m2ts, mov, ogm, avi, bk2, bnk, mkv, wmv, rv"; break;
        case Key::LibModuleFormats: return "xm, it, mod, med, sid, s3m"; break;
        case Key::LibPrefixDeletionPatterns: return "Music/;Video/;Videos/"; break;

        case Key::PlayerAudio: return "mplayer -novideo -really-quiet %f"; break;
        case Key::PlayerVideo: return "mplayer -fs -really-quiet %f"; break;
        case Key::PlayerModule: return "xmp %f"; break;

        case Key::ToolBrowser: return "xdg-open"; break;

        case Key::HistIgnore: return "statistics*;browse*;exit*;rescan*;history*;random;shuffle"; break;
    }

    return std::string();
}

void ConfigManager::addIfMissing(Key key)
{
    try
    {
        (void) this->m_config.get<std::string>(this->keyStr(key));
    }

    catch (...)
    {
        this->m_config.put(this->keyStr(key), this->keyDefaultValue(key));
    }
}
