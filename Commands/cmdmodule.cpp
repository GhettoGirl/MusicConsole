#include "cmdmodule.hpp"

#include <iostream>
#include <Sys/mediaplayercontroller.hpp>

CmdModule::CmdModule(const QString &cmd, MediaLibraryModel *media_model)
    : Command(cmd, media_model)
{
}

void CmdModule::execute()
{
    MediaLibraryModel::Media *media = this->ptr_media_model->find(this->m_args, MediaLibraryModel::ModuleTracker);

    if (!media)
    {
        std::cout << "Nothing found.\n" << std::endl;
        return;
    }

    MediaPlayerController::i()->play(media, MediaLibraryModel::ModuleTracker);
}
