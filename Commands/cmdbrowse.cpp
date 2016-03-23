#include "cmdbrowse.hpp"

CmdBrowse::CmdBrowse(const QString &cmd, MediaLibraryModel *media_model,
                     const QString &browser_tool)
    : Command(cmd, media_model)
{
    this->browser_tool = browser_tool;
}

CmdBrowse::~CmdBrowse()
{
    this->browser_tool.clear();
}

void CmdBrowse::execute()
{
    QString command = this->ptr_media_model->rootPath();

    // make string ready for system()
    command.replace('\\', "\\\\");
    command.replace('"', "\\\"");

    // finish command
    command.prepend(this->browser_tool + ' ' + '"');
    command.append('"');

    (void) system(command.toUtf8().constData());
}
