#include "command.hpp"

Command::Command(MediaLibraryModel *media_model)
{
    this->ptr_media_model = media_model;
}

Command::Command(const QString &cmd, MediaLibraryModel *media_model)
    : Command(media_model)
{
    this->m_cmd = cmd;
}

Command::Command(const QString &cmd, const QString &args, MediaLibraryModel *media_model)
    : Command(media_model)
{
    this->m_cmd = cmd;
    this->m_args = args;
}

Command::~Command()
{
    this->m_cmd.clear();
    this->m_args.clear();

    // remove pointer address but NOT the data the pointer points to !!
    this->ptr_media_model = nullptr;
}

void Command::setCommandString(const QString &cmd)
{
    this->m_cmd = cmd;
}

const QString &Command::commandString() const
{
    return this->m_cmd;
}

void Command::setArguments(const QString &args)
{
    this->m_args = args;
}

const QString &Command::args() const
{
    return this->m_args;
}

void Command::clearArguments()
{
    this->m_args.clear();
}

MediaLibraryModel::MediaType Command::mediaTypeFilter(QString &command, const QString &cmdAudio, const QString &cmdVideo, const QString &cmdModule)
{
    int n;

    if (command.startsWith(cmdAudio, Qt::CaseInsensitive))
    {
        n = cmdAudio.size() + 1;
        if (command.size() > n)
        {
            command = command.mid(n);
            return MediaLibraryModel::Audio;
        }

        else
        {
            command.clear();
            return MediaLibraryModel::Audio;
        }
    }

    else if (command.startsWith(cmdVideo, Qt::CaseInsensitive))
    {
        n = cmdVideo.size() + 1;
        if (command.size() > n)
        {
            command = command.mid(n);
            return MediaLibraryModel::Video;
        }

        else
        {
            command.clear();
            return MediaLibraryModel::Video;
        }
    }

    else if (command.startsWith(cmdModule, Qt::CaseInsensitive))
    {
        n = cmdModule.size() + 1;
        if (command.size() > n)
        {
            command = command.mid(n);
            return MediaLibraryModel::ModuleTracker;
        }

        else
        {
            command.clear();
            return MediaLibraryModel::ModuleTracker;
        }
    }

    else
    {
        return MediaLibraryModel::None;
    }
}
