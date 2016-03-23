#ifndef CMDMODULE_HPP
#define CMDMODULE_HPP

#include <Sys/command.hpp>

// filter by media type when searching for something
// [cmd] search term
//
// this command searches for module tracker files only

class CmdModule : public Command
{
public:
    CmdModule(const QString &cmd, MediaLibraryModel *media_model = nullptr);

    void execute();
};

#endif // CMDMODULE_HPP
