#ifndef CMDSTATISTICS_HPP
#define CMDSTATISTICS_HPP

#include <Sys/command.hpp>

// primitive statistics monitor
// currently only a library counter

class CmdStatistics : public Command
{
public:
    CmdStatistics(const QString &cmd, MediaLibraryModel *media_model = nullptr);

    void execute();

private:
    static std::string transformPath(QString path);
};

#endif // CMDSTATISTICS_HPP
