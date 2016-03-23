#include "cmdsearch.hpp"

#include <iostream>

CmdSearch::CmdSearch(const QString &cmd, MediaLibraryModel *media_model,
                     const QString &cmdAudio, const QString &cmdVideo, const QString &cmdModule)
    : Command(cmd, media_model)
{
    this->cmdAudio = cmdAudio;
    this->cmdVideo = cmdVideo;
    this->cmdModule = cmdModule;
}

CmdSearch::~CmdSearch()
{
    this->cmdAudio.clear();
    this->cmdVideo.clear();
    this->cmdModule.clear();
}

void CmdSearch::execute()
{
    MediaLibraryModel::MediaType type = this->mediaTypeFilter(this->m_args, this->cmdAudio, this->cmdVideo, this->cmdModule);

    QList<MediaLibraryModel::Media*> search_results = this->ptr_media_model->findMultiple(this->m_args, type);

    if (search_results.isEmpty())
    {
        search_results.clear();

        std::cout << "Nothing found.\n" << std::endl;
        return;
    }

    for (MediaLibraryModel::Media *media : search_results)
    {
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
            std::cout << qUtf8Printable(media->searchPaths.at(0)) << std::endl;
        }
    }

    std::endl(std::cout);

    search_results.clear();
}
