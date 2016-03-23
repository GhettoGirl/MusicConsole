#ifndef CMDRESCAN_HPP
#define CMDRESCAN_HPP

#include <Sys/command.hpp>

// rebuild the whole library from scratch
// at the moment I don't know how to efficiently watch a directory tree for changes
// with like ~30.000+ files in it (yes, there are people which have such an amount of songs :P )
// at this time, this needs to be invoked manually by command :(

class CmdRescan : public Command
{
public:
    CmdRescan(const QString &cmd, MediaLibraryModel *media_model = nullptr);

    void execute();
};

#endif // CMDRESCAN_HPP
