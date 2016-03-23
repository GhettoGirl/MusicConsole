#ifndef CMDVIDEO_HPP
#define CMDVIDEO_HPP

#include <Sys/command.hpp>

// filter by media type when searching for something
// [cmd] search term
//
// this command searches for video files only

class CmdVideo : public Command
{
public:
    CmdVideo(const QString &cmd, MediaLibraryModel *media_model = nullptr);

    void execute();
};

#endif // CMDVIDEO_HPP
