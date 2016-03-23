#ifndef CMDAUDIO_HPP
#define CMDAUDIO_HPP

#include <Sys/command.hpp>

// filter by media type when searching for something
// [cmd] search term
//
// this command searches for audio files only

class CmdAudio : public Command
{
public:
    CmdAudio(const QString &cmd, MediaLibraryModel *media_model = nullptr);

    void execute();
};

#endif // CMDAUDIO_HPP
