#include "filesystemmodel.hpp"

#include <QDirIterator>

FileSystemModel::FileSystemModel(QObject *parent)
    : QObject(parent)
{
    // construct a file system model for the whole filesystem
    this->m_dir = QDir::root();
    this->m_rootPath = this->m_dir.absolutePath();
    this->m_rootPathStrLength = this->m_rootPath.size() + 1;
}

FileSystemModel::FileSystemModel(const QString &rootPath, QObject *parent)
    : FileSystemModel(parent)
{
    // try to change the root directory of the file system model
    (void) this->setRootPath(rootPath);
}

FileSystemModel::FileSystemModel(const QString &rootPath, const QStringList &nameFilters, QObject *parent)
    : FileSystemModel(rootPath, parent)
{
    // set name filters
    this->m_filters = nameFilters;
}

FileSystemModel::~FileSystemModel()
{
    // clear members
    this->m_rootPath.clear();
    this->m_filters.clear();

    // clear and delete the filelist
    // this->m_filelist->clear();

    /** PLEASE READ BEFORE MODIFYING THE DTOR
     *
     * There is a strange problem which causes a SEGFAULT with a message like this
     *   std::swap<QListData::Data*> <error reading variable: ...>
     * if you try to call the 'clear' function here :/
     *
     * It took me days to figure out what causes this stupid SEGFAULT.
     * I now clear the FileSystemModel from somewhere else, rather than
     * using the dtor...
     *
     */
}

bool FileSystemModel::setRootPath(const QString &rootPath)
{
    QDir tmp(rootPath);

    if (tmp.exists())
    {
        this->m_dir = tmp;
        this->m_rootPath = tmp.absolutePath();
        this->m_rootPathStrLength = this->m_rootPath.size() + 1;

        emit directoryChanged();
        emit directoryChanged(this->m_rootPath);
        emit directoryChanged(this->m_dir);

        return true;
    }

    return false;
}

bool FileSystemModel::changePwd()
{
    return QDir::setCurrent(this->m_rootPath);
}

QString FileSystemModel::rootPath() const
{
    return this->m_rootPath;
}

void FileSystemModel::setNameFilters(const QStringList &nameFilters)
{
    this->m_filters = nameFilters;
    this->m_dir.setNameFilters(this->m_filters);
}

void FileSystemModel::clearNameFilters()
{
    this->m_filters.clear();
    this->m_dir.setNameFilters(QStringList());
}

bool FileSystemModel::cd(const QString &directory)
{
#ifdef Q_OS_UNIX

    // skip dot because its useless (stays in same directory)
    if (directory == ".")
        return true;

#endif

    return this->setRootPath(this->m_rootPath + QDir::separator() + directory);
}

inline FileSystemModel *FileSystemModel::clone() const
{
    return new FileSystemModel(this->m_rootPath);
}

inline FileSystemModel *FileSystemModel::clone(const QString &toDirectory) const
{
    FileSystemModel *model = new FileSystemModel(this->m_rootPath);
    (void) model->cd(toDirectory);
    return model;
}

QStringList FileSystemModel::currentDirectories(ListingType type) const
{
    return this->entryList(type, this->qDirDirs());
}

QStringList FileSystemModel::currentFiles(ListingType type) const
{
    return this->entryList(type, this->qDirFiles());
}

void FileSystemModel::iterateFilesystem(ListingType type, bool sort)
{
    // clear and delete the filelist
    this->clear();

    // create a new filelist object
    this->m_filelist = new QStringList();

    // define a QDirIterator pointer
    QDirIterator *iterator = nullptr;

    // create the QDirIterator depending on the set name filters
    if (this->m_filters.isEmpty())
    {
        iterator = new QDirIterator(this->m_dir.absolutePath(),                                     // model directory
                                    this->qDirFiles(),                                              // filter
                                    QDirIterator::Subdirectories | QDirIterator::FollowSymlinks);   // iterator flags
    }

    else
    {
        iterator = new QDirIterator(this->m_dir.absolutePath(), this->m_filters,                    // model directory and name filters
                                    this->qDirFiles(),                                              // filter
                                    QDirIterator::Subdirectories | QDirIterator::FollowSymlinks);   // iterator flags
    }

// define iterator template
// for performance reasons, do not compare the type each time (SLOW, if for example more than 1 million + files)
// check the type once and than build the list straightforward
#define ITERATOR_BEGIN \
    while (iterator->hasNext()) { \
        (void) iterator->next(); // QDirIterator points to nothing at the beginning
#define ITERATOR_END }

        if (type == Filenames)
        {
            ITERATOR_BEGIN
                this->m_filelist->append(iterator->fileInfo().fileName());
            ITERATOR_END
        }

        else if (type == AbsolutePaths)
        {
            ITERATOR_BEGIN
                this->m_filelist->append(iterator->fileInfo().absoluteFilePath());
            ITERATOR_END
        }

        else if (type == RelativePaths)
        {
            ITERATOR_BEGIN
                this->m_filelist->append(iterator->fileInfo().absoluteFilePath().remove(0, this->m_rootPathStrLength));

                // SLOW!!!
                // list.append(this->m_dir.relativeFilePath(iterator->fileInfo().absoluteFilePath()));
            ITERATOR_END
        }

        // just for safety, in case enum casting goes wrong (or in other words: the user fucked it up)
        else {
            ITERATOR_BEGIN
                this->m_filelist->append(iterator->fileInfo().absoluteFilePath());
            ITERATOR_END
        }

    // delete the QDirIterator
    delete iterator;

    // sort the list case-sensitive unicode (icu) alphabetically using std::stable_sort()
    if (sort) std::stable_sort(this->m_filelist->begin(), this->m_filelist->end(), this->sort);
}

QStringList *FileSystemModel::filelist() const
{
    return this->m_filelist;
}

bool FileSystemModel::hasFiles() const
{
    return !this->m_dir.entryList(this->qDirFiles()).isEmpty();
}

bool FileSystemModel::hasDirectories() const
{
    return !this->m_dir.entryList(this->qDirDirs()).isEmpty();
}

bool FileSystemModel::isValid() const
{
    return this->m_dir.exists();
}

inline bool FileSystemModel::sort(const QString &s1, const QString &s2)
{
    return (s1 < s2);
}

inline QDir::SortFlags FileSystemModel::qDirSort()
{
    return (QDir::Name | QDir::DirsFirst);
}

inline QDir::Filters FileSystemModel::qDirFiles()
{
    return (QDir::NoDotAndDotDot | QDir::Files);
}

inline QDir::Filters FileSystemModel::qDirDirs()
{
    return (QDir::NoDotAndDotDot | QDir::Dirs);
}

inline QStringList FileSystemModel::entryList(ListingType type, QDir::Filters filter) const
{
    if (!this->isValid())
        return QStringList();

    QStringList list;

    if (type == Filenames)
        return this->m_dir.entryList(filter, this->qDirSort());

    else if (type == AbsolutePaths)
    {
        for (const QFileInfo &current : this->m_dir.entryInfoList(filter, this->qDirSort()))
            list.append(current.absoluteFilePath());
        return list;
    }

    else if (type == RelativePaths)
    {
        for (const QFileInfo &current : this->m_dir.entryInfoList(filter, this->qDirSort()))
            list.append(current.absoluteFilePath().remove(0, this->m_rootPathStrLength));

            // SLOW!!!
            // list.append(this->m_dir.relativeFilePath(current.absoluteFilePath()));
        return list;
    }

    return list;
}

void FileSystemModel::clear()
{
    if (this->m_filelist)
    {
        this->m_filelist->clear();
        delete this->m_filelist;
        this->m_filelist = nullptr;
    }
}
