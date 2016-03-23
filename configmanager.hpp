#ifndef CONFIGMANAGER_HPP
#define CONFIGMANAGER_HPP

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

#include <Utils/medialibrarymodel.hpp>

#include <Sys/command.hpp>

// NOTE: the ConfigManager is not universal, it is part of this application !!
//
// MILESTONE:
//     add ability to change values and write back to file
//
// TODO:
//     handle boost exceptions
//

class ConfigManager
{
public:
    explicit ConfigManager();
    ~ConfigManager();

    enum class Key {
        CmdAudio,
        CmdVideo,
        CmdModule,
        CmdPlaylist,
        CmdPlaylistFileLoad,
        CmdSearch,
        CmdBrowse,
        CmdRandom,
        CmdShuffle,
        CmdRepeat,
        CmdHistory,
        CmdStatistics,
        CmdExit,
        CmdRescan,

        LibRootPath,
        LibAudioFormats,
        LibVideoFormats,
        LibModuleFormats,
        LibPrefixDeletionPatterns,

        PlayerAudio,
        PlayerVideo,
        PlayerModule,

        ToolBrowser,

        HistIgnore
    };

    QString value(Key) const;
    bool boolean(Key) const;

    QString playerFor(const QString &fileformat) const;

    QString configDir() const;

private:

    // create default config file
    void createDefaultConfigFile();

    // load default settings into the container
    void loadDefaultSettings();

    // load settings from file
    void loadSettings();

    // configuration directory
    QString m_configdir;

    // configuration file path
    QString m_configfile;

    // configuration container
    boost::property_tree::ptree m_config;

    // transform Key into string for boost ptree
    static std::string keyStr(Key);
    static std::string keyDefaultValue(Key);

    // adds missing keys if config file lacks some of it
    void addIfMissing(Key);

    // maintain a pointer to the media library for command maintenance
    MediaLibraryModel *ptr_media_model;
};

#endif // CONFIGMANAGER_HPP
