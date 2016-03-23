#ifndef CMDHISTORY_HPP
#define CMDHISTORY_HPP

#include <Sys/command.hpp>

// print the command line history
// the HistoryManager class is responsible for this
// some info: commands which begins with a space are ignored, to keep control about what is being saved
//            similar to shells (bash, zsh, ksh, etc.)

class CmdHistory : public Command
{
public:
    CmdHistory(const QString &cmd);

    void execute();
};

#endif // CMDHISTORY_HPP
