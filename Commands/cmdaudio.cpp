#include "cmdaudio.hpp"

#include <iostream>
#include <Sys/mediaplayercontroller.hpp>

CmdAudio::CmdAudio(const QString &cmd, MediaLibraryModel *media_model)
    : Command(cmd, media_model)
{
}

void CmdAudio::execute()
{
    MediaLibraryModel::Media *media = this->ptr_media_model->find(this->m_args, MediaLibraryModel::Audio);

    if (!media)
    {
        std::cout << "Nothing found.\n" << std::endl;
        return;
    }

    MediaPlayerController::i()->play(media, MediaLibraryModel::Audio);
}
