#ifndef COMMAND_HPP
#define COMMAND_HPP

#include <QString>

#include <Utils/medialibrarymodel.hpp>

// Command abstract base class
// execute() must be implemented in your subclass

// the MediaLibraryModel pointer is only required if you need to
// invoke a method in the model, otherwise its ok to keep a nullptr ;)

class Command
{
public:
    explicit Command(MediaLibraryModel *media_model = nullptr);
    Command(const QString &cmd, MediaLibraryModel *media_model = nullptr);
    Command(const QString &cmd, const QString &args, MediaLibraryModel *media_model = nullptr);
    virtual ~Command();

    // must not be empty, setup in the ConfigManager
    void setCommandString(const QString &cmd);
    const QString &commandString() const;

    // invoked automatically by the console
    // should not be called manually
    void setArguments(const QString &args);
    const QString &args() const;
    void clearArguments();

    virtual void execute() = 0;

protected:
    QString m_cmd;
    QString m_args;

    // most commands have filter functionalities
    // (command) "[type] search terms" is transformed to
    //     |
    //     v
    // "search terms"
    // the type is returned as MediaType enum
    static MediaLibraryModel::MediaType mediaTypeFilter(QString &command,
                                                        const QString &cmdAudio,
                                                        const QString &cmdVideo,
                                                        const QString &cmdModule);

    // maintain a pointer to the media library for command execution
    MediaLibraryModel *ptr_media_model;
};

#endif // COMMAND_HPP

#include <Commands/cmdaudio.hpp>
#include <Commands/cmdvideo.hpp>
#include <Commands/cmdmodule.hpp>
#include <Commands/cmdplaylist.hpp>
#include <Commands/cmdsearch.hpp>
#include <Commands/cmdbrowse.hpp>
#include <Commands/cmdrandom.hpp>
#include <Commands/cmdshuffle.hpp>
#include <Commands/cmdrepeat.hpp>
#include <Commands/cmdhistory.hpp>
#include <Commands/cmdstatistics.hpp>
#include <Commands/cmdrescan.hpp>
