#include "cmdhistory.hpp"

#include <iostream>
#include <Sys/historymanager.hpp>

CmdHistory::CmdHistory(const QString &cmd)
    : Command(cmd)
{
}

void CmdHistory::execute()
{
    for (const QString &e : HistoryManager::i()->history())
        std::cout << qUtf8Printable(e) << std::endl;
    std::endl(std::cout);
}
