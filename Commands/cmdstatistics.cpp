#include "cmdstatistics.hpp"

#include <iostream>

#include <QDir>

// cout -> name filters
std::ostream &operator<<(std::ostream &os, QStringList m)
{
    QString str;

    for (QString &s : m)
        str.append(s.remove("*.") + ", ");
    str = str.left(str.lastIndexOf(", "));

    return os << qUtf8Printable(str);
}

CmdStatistics::CmdStatistics(const QString &cmd, MediaLibraryModel *media_model)
    : Command(cmd, media_model)
{
}

void CmdStatistics::execute()
{
    std::cout << "\n   \033[1m\033[3mStatistics Monitor\033[0m\n\n"

                 "      # of Audio files:            " << this->ptr_media_model->count(MediaLibraryModel::Audio) << "\n"
                 "      # of Video files:            " << this->ptr_media_model->count(MediaLibraryModel::Video) << "\n"
                 "      # of Module Tracker files:   " << this->ptr_media_model->count(MediaLibraryModel::ModuleTracker) << "\n"
                 "\n"
                 "      Total # of media files:      " << this->ptr_media_model->count() << "\n"

                 "\n\n"

                 "   \033[1m\033[3mDatabase Configuration\033[0m\n\n"

                 "      Root path:             " << this->transformPath(this->ptr_media_model->rootPath()) << "\n"
                 "\n"
                 "      Audio types:           " << this->ptr_media_model->nameFilters(MediaLibraryModel::Audio) << "\n"
                 "      Video types:           " << this->ptr_media_model->nameFilters(MediaLibraryModel::Video) << "\n"
                 "      Module Tracker types:  " << this->ptr_media_model->nameFilters(MediaLibraryModel::ModuleTracker) << "\n"

              << std::endl;
}

std::string CmdStatistics::transformPath(QString path)
{
    QString homePath = QDir::homePath();
    int homePathLen = homePath.size();

    if (path.startsWith(homePath))
        path.replace(0, homePathLen, '~');

    return path.toUtf8().constData();
}
