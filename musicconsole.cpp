#include <musicconsole.hpp>

#include <QCoreApplication>

#include <iostream>

#include <readline/readline.h>

#include <SearchPathGens/unicodewhitespacefixer.hpp>
#include <SearchPathGens/universaljapanesekanalookup.hpp>
#include <SearchPathGens/unicodelatingen.hpp>

#include <Sys/mediaplayercontroller.hpp>
#include <Sys/historymanager.hpp>
#include <Sys/mediacache.hpp>

static const UnicodeWhitespaceFixer usf;
static const QChar space(0x20);

// macro to exit the thread with a return code
// default is always 0, this is useful to indicate errors to users
#define QuitMusicConsole(returncode) \
    this->m_statusCode = returncode; \
    return

// get value by key from config file
#define CONFIGVAL(__key__) this->m_config->value(ConfigManager::Key::__key__)

// ordered default command set
#define DEFAULT_COMMAND_SET \
    CONFIGVAL(CmdAudio), \
    CONFIGVAL(CmdVideo), \
    CONFIGVAL(CmdModule)

MusicConsole::MusicConsole(QObject *parent)
    : QThread(parent),
      m_statusCode(0) // set exit status to 0 by default
{
    // create the config manager
    this->m_config = new ConfigManager();

    // create the media cache
    MediaCache::createInstance(
        this->m_config->configDir() +
        QDir::separator() +
        "cache");

    // create the media library model
    this->m_media = new MediaLibraryModel(CONFIGVAL(LibRootPath));
    this->m_media->setPrefixDeletionPatterns(CONFIGVAL(LibPrefixDeletionPatterns));

    // create the user filters
    this->m_media->setNameFilters(MediaLibraryModel::Audio, this->createNameFilters(CONFIGVAL(LibAudioFormats)));
    this->m_media->setNameFilters(MediaLibraryModel::Video, this->createNameFilters(CONFIGVAL(LibVideoFormats)));
    this->m_media->setNameFilters(MediaLibraryModel::ModuleTracker, this->createNameFilters(CONFIGVAL(LibModuleFormats)));

    // create the media player controller
    MediaPlayerController::i()->setAudioPlayer(CONFIGVAL(PlayerAudio));
    MediaPlayerController::i()->setVideoPlayer(CONFIGVAL(PlayerVideo));
    MediaPlayerController::i()->setModulePlayer(CONFIGVAL(PlayerModule));

    // register player overrides, cannot be overriden using standard enums, means overrides are forced
    for (const QString &fileformat : this->m_media->nameFilters())
    {
        QString cmd = this->m_config->playerFor(fileformat.mid(2));
        if (!cmd.isEmpty())
            MediaPlayerController::i()->registerPlayerForFormat(fileformat.mid(2), cmd);
        cmd.clear();
    }

    // create the command set
    this->m_commands.append(new CmdAudio(CONFIGVAL(CmdAudio), this->m_media));
    this->m_commands.append(new CmdVideo(CONFIGVAL(CmdVideo), this->m_media));
    this->m_commands.append(new CmdModule(CONFIGVAL(CmdModule), this->m_media));
    this->m_commands.append(new CmdPlaylist(CONFIGVAL(CmdPlaylist), this->m_media,
        DEFAULT_COMMAND_SET,
        CONFIGVAL(CmdPlaylistFileLoad),
        this->m_config->configDir()));
    this->m_commands.append(new CmdSearch(CONFIGVAL(CmdSearch), this->m_media,
        DEFAULT_COMMAND_SET));
    this->m_commands.append(new CmdBrowse(CONFIGVAL(CmdBrowse), this->m_media,
        CONFIGVAL(ToolBrowser)));
    this->m_commands.append(new CmdRandom(CONFIGVAL(CmdRandom), this->m_media,
        DEFAULT_COMMAND_SET));
    this->m_commands.append(new CmdShuffle(CONFIGVAL(CmdShuffle), this->m_media,
        DEFAULT_COMMAND_SET));
    this->m_commands.append(new CmdRepeat(CONFIGVAL(CmdRepeat), this->m_media,
        DEFAULT_COMMAND_SET));
    this->m_commands.append(new CmdHistory(CONFIGVAL(CmdHistory)));
    this->m_commands.append(new CmdStatistics(CONFIGVAL(CmdStatistics), this->m_media));
    this->m_commands.append(new CmdRescan(CONFIGVAL(CmdRescan), this->m_media));

    // search command set for duplicates
    // if dups where found, print a warning - that's it! don't change order, don't change commands
    // just print a warning to let the user know, some command cannot be accessed because of that
    // NOTE: the 'exit' command cannot be overridden, because it's not in the command list and is checked first
    //       before every other command.

    // just to compare the 'exit' command too; NOTE: Command is abstract and cannot be declared
    this->m_commands.append(new CmdRescan(this->m_config->value(ConfigManager::Key::CmdExit)));

    bool has_dups = false;
    int list_size = this->m_commands.size();
    for (int i = 0; i < list_size; i++)
    {
        for (int j = 0; j < list_size; j++)
        {
            if (i != j && QString::compare(this->m_commands.at(i)->commandString(),
                                           this->m_commands.at(j)->commandString(),
                                           Qt::CaseInsensitive) == 0)
            {
                has_dups = true;
            }
        }
    }

    // remove the previously added 'exit' command
    delete this->m_commands.takeLast();

    if (has_dups)
    {
        std::cerr << "\033[1;38;2;144;142;0mWARNING:\033[0m One or more commands share the same identifier.\n"
                     "  You cannot access this commands. Please check you configuration.\n" << std::endl;
    }

    // create history manager
    HistoryManager::createInstance(
        this->m_config->configDir() +
        QDir::separator() +
        "history");
    HistoryManager::i()->setHistIgnorePatterns(CONFIGVAL(HistIgnore));
}

MusicConsole::~MusicConsole()
{
    delete this->m_config;
    delete this->m_media;

    for (Command *c : this->m_commands)
        delete c;
    this->m_commands.clear();

    delete MediaPlayerController::i();

    HistoryManager::i()->close();
    delete HistoryManager::i();

    delete MediaCache::i();
}

int MusicConsole::statusCode() const
{
    return this->m_statusCode;
}

void MusicConsole::executeCmd() const
{
    /** TODO
     *
     * × transfer command line arguments using QSharedMemory + (de)serialization
     *   to this thread and execute it as a Music Console command
     *
     *   QStringList::merge() ...
     *
     */
}

void MusicConsole::run()
{
    // build media list
    this->installSearchPathGens();
    this->m_media->iterateFilesystem();

    // command container; split happens at '&&', makes it possible to execute multiple commands with a one-liner
    QList<ConsoleCommand> commands;

    // enter console loop
    while (true)
    {
        this->userInput(commands);

        bool command_matched = false;
        for (const ConsoleCommand &cc : commands)
        {
            if (QString::compare(cc.cmd, CONFIGVAL(CmdExit), Qt::CaseInsensitive) == 0)
            {
                this->prepareToQuit();
                QuitMusicConsole(0);
                // app quits here
            }

            for (Command *c : this->m_commands)
            {
                if (QString::compare(cc.cmd, c->commandString(), Qt::CaseInsensitive) == 0)
                {
                    c->setArguments(cc.args);
                    c->execute();
                    c->clearArguments();
                    command_matched = true;
                }
            }

            if (!command_matched)
            {
                // if reached here, no command matched
                // instantly lookup a media without filter and play it
                // use audio player, it is recommended to use 'mplayer -novideo' (default) as audio player
                MediaLibraryModel::Media *media = this->m_media->find(cc.cmd + ' ' + cc.args);

                if (!media)
                {
                    std::cout << "Nothing found.\n" << std::endl;
                }

                else
                {
                    MediaPlayerController::i()->play(media, MediaLibraryModel::Audio);
                }
            }
        }
    }
}

void MusicConsole::installSearchPathGens()
{
    // Unicode Whitespace Fixer
    // all the different whitespaces are converted to the 0x20 whitespace
    UnicodeWhitespaceFixer *unicodeWhitespaceFixer = new UnicodeWhitespaceFixer;
    this->m_media->addSearchPathGen(unicodeWhitespaceFixer);

    // Universal Japanese Kana Lookup
    // means: あ == ア == ｱ (all the same when searching for media)
    UniversalJapaneseKanaLookup *universalJapaneseKanaLookup = new UniversalJapaneseKanaLookup;
    this->m_media->addSearchPathGen(universalJapaneseKanaLookup);

    // Unicode Latin Generator
    // generates strings for BasicLatin-1 and fullwidth latin characters
    // helpful if you have a lot of japanense, chinese and korean music, which
    // have fullwidth latin characters in their name
    // or if you use input methods like IBus/Anthy and don't feel like changing
    // the input method :D
    UnicodeLatinGen *unicodeLatinGen = new UnicodeLatinGen;
    this->m_media->addSearchPathGen(unicodeLatinGen);
}

void MusicConsole::prepareToQuit()
{
}

void MusicConsole::userInput(QList<ConsoleCommand> &commands)
{
    // whenever the command should be appended to the history
    bool append_to_history = true;

    // input buffer
    QString inputbuf;
    QStringList splitbuf;

    // clear command list
    commands.clear();

    // history preference
    append_to_history = true;

    // get user input using GNU/Readline
    inputbuf = QString::fromUtf8(readline("# "));

    // simplify string, keep first whitespace if any
    this->simplifyString(inputbuf);

    // skip empty input
    if (this->cmdHasOnlySpaces(inputbuf))
    {
        inputbuf.clear();
        splitbuf.clear();
        return;
    }

    // set history preference
    if (inputbuf.startsWith(' '))
    {
       inputbuf.remove(0, 1);
       append_to_history = false;
    }

    // split input
    splitbuf = inputbuf.split("&&", QString::SkipEmptyParts);

    // skip empty input
    if (splitbuf.isEmpty())
    {
        inputbuf.clear();
        splitbuf.clear();
        return;
    }

    // eliminate empty entries
    for (int i = 0; i < splitbuf.size(); i++)
    {
        if (this->cmdHasOnlySpaces(splitbuf.at(i)))
        {
            splitbuf.removeAt(i);
            i = 0;
        }
    }

    // check if list got empty
    if (splitbuf.isEmpty())
    {
        inputbuf.clear();
        splitbuf.clear();
        return;
    }

    // build commands
    for (QString &s : splitbuf)
    {
        // simplify string
        this->simplifyString(s);
        if (s.startsWith(' '))
            s.remove(0, 1);

        if (s.isEmpty())
            continue;

        ConsoleCommand cc;

        if (s.contains(' '))
        {
            int index = s.indexOf(' ');
            cc.cmd = s.mid(0, index);
            cc.args = s.mid(index + 1);
        }

        else
        {
            cc.cmd = s;
        }

        commands.append(cc);
    }

    // skip empty command list
    if (commands.isEmpty())
    {
        inputbuf.clear();
        splitbuf.clear();
        return;
    }

    // clear temporaries
    splitbuf.clear();

    // append to history file
    if (append_to_history)
        HistoryManager::i()->add(inputbuf);

    // clear input buffer
    inputbuf.clear();
}

QStringList MusicConsole::createNameFilters(const QString &filters) const
{
    // eliminate every whitespace
    QString str = usf.processString(filters).at(0);
    str.remove(space);

    QStringList ext = str.split(',', QString::SkipEmptyParts);

    for (QString &s : ext)
        s.prepend("*.");

    return ext;
}

bool MusicConsole::cmdHasOnlySpaces(const QString &s)
{
    QString str = usf.processString(s).at(0);

    for (const QChar &c : str)
    {
        if (c != space)
        {
            str.clear();
            return false;
        }
    }

    str.clear();
    return true;
}

void MusicConsole::removeTrailingSpaces(QString &s)
{
    s = usf.processString(s).at(0);

    for (int n = s.size() - 1; n >= 0; --n)
    {
        if (!s.at(n).isSpace())
            s = s.left(n + 1);
    }
}

void MusicConsole::simplifyString(QString &s)
{
    s = usf.processString(s).at(0);

    bool startedWithSpace = false;

    if (s.startsWith(' '))
        startedWithSpace = true;

    s = s.simplified();

    if (startedWithSpace)
        (void) s.prepend(' ');
}
