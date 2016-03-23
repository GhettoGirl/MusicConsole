#include "cmdrepeat.hpp"

#include <iostream>

#include <Sys/mediaplayercontroller.hpp>
#include <Sys/kbhit.hpp>

CmdRepeat::CmdRepeat(const QString &cmd, MediaLibraryModel *media_model,
                     const QString &cmdAudio, const QString &cmdVideo, const QString &cmdModule)
    : Command(cmd, media_model)
{
    this->cmdAudio = cmdAudio;
    this->cmdVideo = cmdVideo;
    this->cmdModule = cmdModule;
}

CmdRepeat::~CmdRepeat()
{
    this->cmdAudio.clear();
    this->cmdVideo.clear();
    this->cmdModule.clear();
}

void CmdRepeat::execute()
{
    // if no args, return
    if (this->m_args.isEmpty())
    {
        this->print_nothingfound();
        return;
    }

    // check for filter
    MediaLibraryModel::MediaType type = this->mediaTypeFilter(this->m_args, this->cmdAudio, this->cmdVideo, this->cmdModule);

    // if args empty after filtering, return
    if (this->m_args.isEmpty())
    {
        this->print_nothingfound();
        return;
    }

    // lookup media and loop forever
    MediaLibraryModel::Media *media = this->ptr_media_model->find(this->m_args, type);

    if (!media)
    {
        this->print_nothingfound();
        return;
    }

    if (type == MediaLibraryModel::None)
    {
        KBHIT
            MediaPlayerController::i()->play(media, MediaLibraryModel::Audio);
        KBHIT_END
    }

    else
    {
        KBHIT
            MediaPlayerController::i()->play(media, type);
        KBHIT_END
    }
}

void CmdRepeat::print_nothingfound()
{
    std::cout << "Nothing found.\n" << std::endl;
}
