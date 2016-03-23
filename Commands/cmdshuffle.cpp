#include "cmdshuffle.hpp"

#include <iostream>

#include <Sys/mediaplayercontroller.hpp>
#include <Sys/kbhit.hpp>

CmdShuffle::CmdShuffle(const QString &cmd, MediaLibraryModel *media_model,
                       const QString &cmdAudio, const QString &cmdVideo, const QString &cmdModule)
    : Command(cmd, media_model)
{
    this->cmdAudio = cmdAudio;
    this->cmdVideo = cmdVideo;
    this->cmdModule = cmdModule;
}

CmdShuffle::~CmdShuffle()
{
    this->cmdAudio.clear();
    this->cmdVideo.clear();
    this->cmdModule.clear();
}

void CmdShuffle::execute()
{
    // check if media is available before we start the shuffle
    if (this->ptr_media_model->count() == 0)
        return;

    // if no args, shuffle completely random media using audio player
    if (this->m_args.isEmpty())
    {
        KBHIT

            MediaPlayerController::i()->play(
                    this->ptr_media_model->random(), MediaLibraryModel::Audio);

        KBHIT_END

        return;
    }

    // check for filter
    MediaLibraryModel::MediaType type = this->mediaTypeFilter(this->m_args, this->cmdAudio, this->cmdVideo, this->cmdModule);

    // do not filter using search criteria
    if (this->m_args.isEmpty())
    {
        if (type != MediaLibraryModel::None)
        {
            KBHIT

                MediaPlayerController::i()->play(
                        this->ptr_media_model->random(type), type);

            KBHIT_END
        }

        else
        {
            KBHIT

                MediaPlayerController::i()->play(
                        this->ptr_media_model->random(), MediaLibraryModel::Audio);

            KBHIT_END
        }
    }

    // filter using search criteria
    else {
        if (type != MediaLibraryModel::None)
        {

            if (this->ptr_media_model->random(this->m_args, type))
            {

                KBHIT

                    MediaPlayerController::i()->play(
                            this->ptr_media_model->random(this->m_args, type), type);

                KBHIT_END

            }

            else this->print_nothingfound();
        }

        else
        {

            if (this->ptr_media_model->random(this->m_args))
            {

            KBHIT

                MediaPlayerController::i()->play(this->ptr_media_model->random(this->m_args), MediaLibraryModel::Audio);

            KBHIT_END

            }

            else this->print_nothingfound();
        }
    }
}

void CmdShuffle::print_nothingfound()
{
    std::cout << "Nothing matches the given search criteria.\n" << std::endl;
}
