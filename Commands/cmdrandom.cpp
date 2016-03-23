#include "cmdrandom.hpp"

#include <iostream>
#include <Sys/mediaplayercontroller.hpp>

CmdRandom::CmdRandom(const QString &cmd, MediaLibraryModel *media_model,
                     const QString &cmdAudio, const QString &cmdVideo, const QString &cmdModule)
    : Command(cmd, media_model)
{
    this->cmdAudio = cmdAudio;
    this->cmdVideo = cmdVideo;
    this->cmdModule = cmdModule;
}

CmdRandom::~CmdRandom()
{
    this->cmdAudio.clear();
    this->cmdVideo.clear();
    this->cmdModule.clear();
}

void CmdRandom::execute()
{
    // if no args, play completely random media using audio player
    if (this->m_args.isEmpty())
    {
        MediaPlayerController::i()->play(
            this->ptr_media_model->random(), MediaLibraryModel::Audio);
        return;
    }

    // check for filter
    MediaLibraryModel::MediaType type = this->mediaTypeFilter(this->m_args, this->cmdAudio, this->cmdVideo, this->cmdModule);

    // do not filter using search criteria
    if (this->m_args.isEmpty())
    {
        if (type != MediaLibraryModel::None)
        {
            MediaPlayerController::i()->play(
                this->ptr_media_model->random(type), type);
        }

        else
        {
            MediaPlayerController::i()->play(
                this->ptr_media_model->random(), MediaLibraryModel::Audio);
        }
    }

    // filter using search criteria
    else
    {
        MediaLibraryModel::Media *media;

        if (type != MediaLibraryModel::None)
        {
            media = this->ptr_media_model->random(this->m_args, type);

            if (media) MediaPlayerController::i()->play(media, type);
            else this->print_nothingfound();
        }

        else
        {
            media = this->ptr_media_model->random(this->m_args);

            if (media) MediaPlayerController::i()->play(media, MediaLibraryModel::Audio);
            else this->print_nothingfound();
        }
    }
}

void CmdRandom::print_nothingfound()
{
    std::cout << "Nothing matches the given search criteria.\n" << std::endl;
}
