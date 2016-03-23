#ifndef CMDBROWSE_HPP
#define CMDBROWSE_HPP

#include <Sys/command.hpp>

// invokes a file browser in the MediaLibraryModel::rootPath()
// hardcoded default is xdg-open, can be changed -> see ConfigManager tools.browser

class CmdBrowse : public Command
{
public:
    CmdBrowse(const QString &cmd, MediaLibraryModel *media_model = nullptr,
              const QString &browser_tool = QString());
    ~CmdBrowse();

    void execute();

private:
    QString browser_tool;
};

#endif // CMDBROWSE_HPP
