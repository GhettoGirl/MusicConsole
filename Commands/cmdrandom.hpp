#ifndef CMDRANDOM_HPP
#define CMDRANDOM_HPP

#include <Sys/command.hpp>

// get a random media from the library and play it
// the command takes a filter for MediaType and search criteria
// to split out some randomness

// examples:
//   random                          <-- completely random media
//   random [MediaType]              <-- random media of type [MediaType]
//   random search term              <-- random media which matches the search term
//   random [MediaType] search term  <-- random media of type [MediaType] which matches the search term

class CmdRandom : public Command
{
public:
    CmdRandom(const QString &cmd, MediaLibraryModel *media_model = nullptr,
              const QString &cmdAudio = QString(), const QString &cmdVideo = QString(), const QString &cmdModule = QString());
    ~CmdRandom();

    void execute();

private:
    QString cmdAudio,
            cmdVideo,
            cmdModule;

    static void print_nothingfound();
};

#endif // CMDRANDOM_HPP
