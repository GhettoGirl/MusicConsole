#include "mediaplayercontroller.hpp"

#include <iostream>

/// QProcess doesn't do what I want
/// I'm not interested at all to communicate with the players
/// And I want to wait for the player process to exit,
/// before continuing the execution.
///
/// The system() call is exactly what I want here
/// It also allows users to add ">/dev/null" etc. to their player commands ;)

MediaPlayerController::MediaPlayerController()
{
}

MediaPlayerController::~MediaPlayerController()
{
    this->m_audioplayer.clear();
    this->m_videoplayer.clear();
    this->m_modplayer.clear();

    this->m_playerOverrides.clear();

    this->clear();
}

MediaPlayerController *MediaPlayerController::i()
{
    static MediaPlayerController *m_instance = new MediaPlayerController();
    return m_instance;
}

void MediaPlayerController::setAudioPlayer(const QString &cmd)
{
    this->m_audioplayer = cmd;
}

void MediaPlayerController::setVideoPlayer(const QString &cmd)
{
    this->m_videoplayer = cmd;
}

void MediaPlayerController::setModulePlayer(const QString &cmd)
{
    this->m_modplayer = cmd;
}

void MediaPlayerController::registerPlayerForFormat(const QString &fileformat, const QString &cmd)
{
    this->m_playerOverrides.insert(fileformat, cmd);
}

void MediaPlayerController::play(MediaLibraryModel::Media *media, MediaLibraryModel::MediaType type)
{
    // skip nullptr
    if (!media)
        return;

    // skip empty media object
    if (media->path.isEmpty())
        return;

    // safety clean up
    this->clear();

    ///
    /// ~~~ select player
    ///

    // player override
    if (this->m_playerOverrides.contains(media->fileformat))
    {
        this->m_command = this->m_playerOverrides[media->fileformat];
    }

    // default players
    else
    {
        if (type == MediaLibraryModel::None)
        {
            switch (media->type)
            {
                case MediaLibraryModel::Audio:
                    this->m_command = this->m_audioplayer;
                    break;

                case MediaLibraryModel::Video:
                    this->m_command = this->m_videoplayer;
                    break;

                case MediaLibraryModel::ModuleTracker:
                    this->m_command = this->m_modplayer;
                    break;

                // make compiler happy
                case MediaLibraryModel::None: break;
            }
        }

        else
        {
            switch (type)
            {
                case MediaLibraryModel::Audio:
                    this->m_command = this->m_audioplayer;
                    break;

                case MediaLibraryModel::Video:
                    this->m_command = this->m_videoplayer;
                    break;

                case MediaLibraryModel::ModuleTracker:
                    this->m_command = this->m_modplayer;
                    break;

                // make compiler happy
                case MediaLibraryModel::None: break;
            }
        }
    }

    // skip, if no player was specified
    if (this->m_command.isEmpty())
        return;

    // replace %f with media file path or append it
    this->m_file = media->path;
    this->escapeSequences(this->m_file);

    if (this->m_command.contains("%f"))
    {
        this->m_command.replace("%f", this->m_quote + this->m_file + this->m_quote);
    }

    else
    {
        this->m_command.append(' ' + this->m_quote + this->m_file + this->m_quote);
    }

    // print playing
    if (!media->fileformat.isEmpty())
        std::cout << "\033[1;38;2;0;97;167m[" << media->fileformat.toUtf8().constData() << "]\033[0m ";

    if (!(media->tags.album.isEmpty() && // if all 3 fields are empty, print just the relative filename
        media->tags.artist.isEmpty() &&  // otherwise print tags
        media->tags.title.isEmpty()))
    {
        std::cout << "\033[3m" << qUtf8Printable(media->tags.artist) << "\033[0m " <<
                     "\033[1m" << qUtf8Printable(media->tags.title) << "\033[0m " <<
                     "\033[4m" << qUtf8Printable(media->tags.album) << "\033[0m" << std::endl;
    }

    else
    {
        QString display_name;
        // safety check --> ASSERT failure in QList<T>::at: "index out of range"
        if (!media->searchPaths.isEmpty())
            display_name = media->searchPaths.at(0);
        else display_name = media->path;

        int ext_pos = display_name.lastIndexOf('.');
        if (ext_pos != -1)
            display_name = display_name.left(ext_pos);

        std::cout << qUtf8Printable(display_name) << std::endl;
        display_name.clear();
    }

    this->execute();

    // clean up
    this->clear();
}

const QChar MediaPlayerController::m_quote = QChar('"');

void MediaPlayerController::execute()
{
    (void) system(this->m_command.toUtf8().constData());
}

void MediaPlayerController::escapeSequences(QString &str)
{
    str.replace('\\', "\\\\");
    str.replace(MediaPlayerController::m_quote, "\\\"");
}

void MediaPlayerController::clear()
{
    this->m_command.clear();
    this->m_file.clear();
}
