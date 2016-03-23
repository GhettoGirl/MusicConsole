#ifndef CMDPLAYLIST_HPP
#define CMDPLAYLIST_HPP

#include <Sys/command.hpp>

// generate playlist at runtime or load from plist file
//
// playlist search term
// from runtime invokes the MediaLibraryModel::findMultiple() method
//
// playlist load file
// the file is parsed from the "[config directory]/playlists" dir
// see the PlaylistParser class for syntax explanation
// and other details regarding my custom playlist format

class CmdPlaylist : public Command
{
public:
    CmdPlaylist(const QString &cmd, MediaLibraryModel *media_model = nullptr,
                const QString &cmdAudio = QString(), const QString &cmdVideo = QString(), const QString &cmdModule = QString(),
                const QString &cmdFileLoad = QString(),
                const QString &plist_rootPath = QString());
    ~CmdPlaylist();

    void execute();

private:
    void runtimePlaylist();
    void filePlaylist();

    QString cmdAudio,
            cmdVideo,
            cmdModule;
    QString cmdFileLoad;
    QString plist_rootPath;

    static void print_nothingfound();
};

#endif // CMDPLAYLIST_HPP
