#ifndef CMDREPEAT_HPP
#define CMDREPEAT_HPP

#include <Sys/command.hpp>

// loop a single media forever (breakable, see Sys/kbhit.hpp)

class CmdRepeat : public Command
{
public:
    CmdRepeat(const QString &cmd, MediaLibraryModel *media_model = nullptr,
              const QString &cmdAudio = QString(), const QString &cmdVideo = QString(), const QString &cmdModule = QString());
    ~CmdRepeat();

    void execute();

private:
    QString cmdAudio,
            cmdVideo,
            cmdModule;

    static void print_nothingfound();
};

#endif // CMDREPEAT_HPP
