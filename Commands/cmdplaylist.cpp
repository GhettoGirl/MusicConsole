#include "cmdplaylist.hpp"

#include <iostream>

#include <QQueue>

#include <Sys/mediaplayercontroller.hpp>
#include <Sys/kbhit.hpp>
#include <Sys/playlistparser.hpp>

CmdPlaylist::CmdPlaylist(const QString &cmd, MediaLibraryModel *media_model,
                         const QString &cmdAudio, const QString &cmdVideo, const QString &cmdModule,
                         const QString &cmdFileLoad,
                         const QString &plist_rootPath)
    : Command(cmd, media_model)
{
    this->cmdAudio = cmdAudio;
    this->cmdVideo = cmdVideo;
    this->cmdModule = cmdModule;

    this->cmdFileLoad = cmdFileLoad;

    this->plist_rootPath = plist_rootPath;
}

CmdPlaylist::~CmdPlaylist()
{
    this->cmdAudio.clear();
    this->cmdVideo.clear();
    this->cmdModule.clear();

    this->cmdFileLoad.clear();

    this->plist_rootPath.clear();
}

void CmdPlaylist::execute()
{
    // empty check
    if (this->m_args.isEmpty())
    {
        this->print_nothingfound();
        return;
    }

    // detect playlist type
    if (this->m_args.startsWith(this->cmdFileLoad))
    {
        int n = this->m_args.size();
        if (n > this->cmdFileLoad.size())
        {
            this->m_args.remove(0, this->cmdFileLoad.size() + 1);
            this->filePlaylist();
        }

        else
        {
            std::cerr << "Playlist file missing.\n" << std::endl;
        }
    }

    else
    {
        this->runtimePlaylist();
    }
}

void CmdPlaylist::runtimePlaylist()
{
    // filter
    MediaLibraryModel::MediaType type = this->mediaTypeFilter(this->m_args, this->cmdAudio, this->cmdVideo, this->cmdModule);

    // search for media
    QList<MediaLibraryModel::Media*> search_results = this->ptr_media_model->findMultiple(this->m_args, type);

    // empty check
    if (search_results.isEmpty())
    {
        this->print_nothingfound();
        return;
    }

    // build runtime playlist
    QQueue<MediaLibraryModel::Media*> plist;
    for (MediaLibraryModel::Media *media : search_results)
        plist.enqueue(media);
    search_results.clear();

    // start the playlist
    if (type == MediaLibraryModel::None)
    {

        KBHIT_NOT_INFINITE(plist.isEmpty())

            MediaPlayerController::i()->play(plist.dequeue(), MediaLibraryModel::Audio);

        KBHIT_NOT_INFINITE_END

    }

    else
    {

        KBHIT_NOT_INFINITE(plist.isEmpty())

            MediaPlayerController::i()->play(plist.dequeue(), type);

        KBHIT_NOT_INFINITE_END

    }

    plist.clear();
}

void CmdPlaylist::filePlaylist()
{
    // normalize playlist file path
    this->m_args = this->plist_rootPath + QDir::separator() +
            "playlists" + QDir::separator() +
            this->m_args + ".plist";

    PlaylistParser plist(this->m_args, this->ptr_media_model);
    PlaylistParser::ParseStatus status = plist.parse();

    /** Status Codes *****************************************************************************************************************
     *
     * Success,         // playlist file parsed successfully
     * FileNotFound,    // the requested file was not found
     * FileNotReadable, // the requested file is not readable; permission problems etc.
     * FileEmpty,       // the requested file was empty,
     *                  // the file is also treated as empty if it only contains
     *                  // whitespace characters according to UnicodeWhitespaceFixer (SearchPathGens/UnicodeWhitespaceFixer.hpp)
     * PlaylistEmpty,   // valid plist file, but no entries found (also: empty entries are ignored)
     * SyntaxError
     *
     */

    if (status == PlaylistParser::Success)
    {

        // build playlist queue
        QQueue<PlaylistParser::PlaylistEntry> plist_queue;
        for (const PlaylistParser::PlaylistEntry &e : plist.playlist())
            plist_queue.enqueue(e);
        // do not clear playlist here, because all unique media objects are deleted

        KBHIT_NOT_INFINITE(plist_queue.isEmpty())

            const PlaylistParser::PlaylistEntry &e = plist_queue.dequeue();
            MediaPlayerController::i()->play(e.media, e.player);

        KBHIT_NOT_INFINITE_END

        plist_queue.clear();
    }

    else if (status == PlaylistParser::FileNotFound)
    {
        std::cerr << "No such file.\n" << std::endl;
    }

    else if (status == PlaylistParser::FileNotReadable)
    {
        std::cerr << "File couldn't be read!\n" << std::endl;
    }

    else if (status == PlaylistParser::FileEmpty)
    {
        std::cerr << "File is empty.\n" << std::endl;
    }

    else if (status == PlaylistParser::PlaylistEmpty)
    {
        std::cerr << "No entries found in the playlist.\n" << std::endl;
    }

    else if (status == PlaylistParser::SyntaxError)
    {
        std::cerr << "No valid playlist file!\n" << std::endl;
    }

    // free all memory associated by the playlist
    plist.clear();
}

void CmdPlaylist::print_nothingfound()
{
    std::cout << "Nothing matches the given search criteria.\n" << std::endl;
}
