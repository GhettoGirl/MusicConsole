#ifndef CMDSHUFFLE_HPP
#define CMDSHUFFLE_HPP

#include <Sys/command.hpp>

// shuffle randomly in an infinite loop (breakable, see Sys/kbhit.hpp)
// the command takes a filter for MediaType and search criteria
// to split out some randomness

// examples:
//   shuffle                          <-- completely random infinite loop
//   shuffle [MediaType]              <-- shuffle through [MediaType] files
//   shuffle search term              <-- shuffle through files which matches the search term
//   shuffle [MediaType] search term  <-- shuffle through [MediaType] files which matches the search term

class CmdShuffle : public Command
{
public:
    CmdShuffle(const QString &cmd, MediaLibraryModel *media_model = nullptr,
               const QString &cmdAudio = QString(), const QString &cmdVideo = QString(), const QString &cmdModule = QString());
    ~CmdShuffle();

    void execute();

private:
    QString cmdAudio,
            cmdVideo,
            cmdModule;

    static void print_nothingfound();
};

#endif // CMDSHUFFLE_HPP
