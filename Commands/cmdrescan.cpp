#include "cmdrescan.hpp"

#include <iostream>

CmdRescan::CmdRescan(const QString &cmd, MediaLibraryModel *media_model)
    : Command(cmd, media_model)
{
}

void CmdRescan::execute()
{
    // make the output a bit nicer using ANSI escape sequences

    static const std::string msg_plswait("Rescan, please wait...");
    static const std::string msg_finished("Rescan finished.");

    std::cout << msg_plswait << std::endl; // needs a 'endl' or 'flush' here, imo 'endl' looks nicer regarding cursor visibility

    // clear database, but not the configuration (SearchPathGens, root path, name filters, etc.)
    this->ptr_media_model->clear();

    // rescan filesystem
    this->ptr_media_model->iterateFilesystem();

    // \033[1A move cursor one line up
    // \033[nD move cursor n columns to the left
    // \033[K  clear to the end of line
    std::cout << "\033[1A\033[" << msg_plswait.size() << "D\033[K" << msg_finished << "\n" << std::endl;
}
