#ifndef CMDSEARCH_HPP
#define CMDSEARCH_HPP

#include <Sys/command.hpp>

// search library for something, this command invokes MediaLibraryModel::findMultiple() method
// and prints the results on screen

class CmdSearch : public Command
{
public:
    CmdSearch(const QString &cmd, MediaLibraryModel *media_model = nullptr,
              const QString &cmdAudio = QString(), const QString &cmdVideo = QString(), const QString &cmdModule = QString());
    ~CmdSearch();

    void execute();

private:
    QString cmdAudio,
            cmdVideo,
            cmdModule;
};

#endif // CMDSEARCH_HPP
