#include "cmdvideo.hpp"

#include <iostream>
#include <Sys/mediaplayercontroller.hpp>

CmdVideo::CmdVideo(const QString &cmd, MediaLibraryModel *media_model)
    : Command(cmd, media_model)
{
}

void CmdVideo::execute()
{
    MediaLibraryModel::Media *media = this->ptr_media_model->find(this->m_args, MediaLibraryModel::Video);

    if (!media)
    {
        std::cout << "Nothing found.\n" << std::endl;
        return;
    }

    MediaPlayerController::i()->play(media, MediaLibraryModel::Video);
}
