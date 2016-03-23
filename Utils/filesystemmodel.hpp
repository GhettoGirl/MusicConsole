/***************************************************************************
 * FileSystemModel Class for Qt (Core)
 *
 * written by Lena Stöffler, 2016
 *  github.com/GhettoGirl
 *
 *    ~ requires a C++11 compiler
 *
 *
 * This class provides a convenient way to build a list of files
 * recursively with the help of QDirIterator.
 *
 * FileSystemModel also emits signals when modifications to the model
 * were made or if specific tasks are done.
 *
 *   × directoryChanged()
 *     is emited if the 'setRootPath' or 'cd' function is called and
 *     succeed / returned true.
 *
 *   × directoryChanged(const QString&)
 *     same as above, but also contains the path were to model points
 *     to after the directory change.
 *
 *   × directoryChanged(const QDir&)
 *     same as above, but as QDir object.
 *
 */

#ifndef FILESYSTEMMODEL_HPP
#define FILESYSTEMMODEL_HPP

#include <QObject>
#include <QString>
#include <QDir>

class FileSystemModel : public QObject
{
    Q_OBJECT

public:
    FileSystemModel(QObject *parent = 0);
    FileSystemModel(const QString &rootPath, QObject *parent = 0);
    FileSystemModel(const QString &rootPath, const QStringList &nameFilters, QObject *parent = 0);
    ~FileSystemModel();

    enum ListingType {
        Filenames,       // just filenames
        AbsolutePaths,   // full path including filename
        RelativePaths    // path relative to 'rootPath' including filename
    };

    /**
     * @brief setRootPath(rootPath) [bool]
     *
     * Tries to change the root directory of the file system model.
     *
     * The directory MUST be absolute for this function to work.
     * See also cd(directory) for relative directory switching.
     *
     * Returns false if the target directory doesn't exists or
     * isn't readable. The root path stays unchanged.
     *
     * Returns true is everything went fine.
     *
     */
    bool setRootPath(const QString &rootPath);

    /**
     * @brief changePwd [bool]
     *
     * Changes the process working directory to the models root path.
     * wrapper against QDir::setCurrent(), forwards return type
     *
     */
    bool changePwd();

    /**
     * @brief rootPath [QString]
     *
     * Returns the current root path of the file system model.
     *
     */
    QString rootPath() const;

    /**
     * @brief setNameFilters(nameFilters) [void]
     *
     * Sets the name filters for the QDirIterator.
     *
     * *.ext ─ adds only files with the extension .ext to the file list.
     *
     * By default the file system model doesn't filters anything.
     * Name filters apply to all functions, which deal with files.
     *
     * @void QDir::setNameFilters(const QStringList & nameFilters)
     * gives you more hints about the syntax.
     *
     * NOTE that there are no syntax validations or fixups are performed,
     * the name filters must be valid, otherwise deal with undefined behavior.
     *
     */
    void setNameFilters(const QStringList &nameFilters);

    /**
     * @brief clearNameFilters [void]
     *
     * Removes all name filters set.
     *
     */
    void clearNameFilters();

    /**
     * @brief cd(directory) [bool]
     *
     * Tries to change the root directory of the file system model.
     *
     * The directory is parsed in UNIX-style.
     *
     *   .          stay in current directory (skipped, always returns true) [unix only]
     *   ..         go one level up in the directory tree
     *   ../..      go 2 levels up in the directory tree
     *   dir1       goto 'dir1' from the current directory
     *   dir1/dir2  goto 'dir2' in 'dir1' from the current directory
     *   ../dir0    goto 'dir0' from the parent directory of the current directory
     *
     * This function returns false if the target directory doesn't exists or
     * isn't readable. The root path stays unchanged.
     *
     * Returns true if everything went fine.
     *
     */
    bool cd(const QString &directory);

    /**
     * @brief clone [FileSystemModel]
     *
     * Clones the current FileSystemModel and returns it.
     *
     */
    inline FileSystemModel *clone() const;

    /**
     * @brief clone(toDirectory) [FileSystemModel]
     *
     * Clones the current FileSystemModel and cd's to 'toDirectory'.
     * If the directory doesn't exits the model stays unchanged.
     *
     * 'toDirectory' MUST be a relative path!
     * See also cd(directory)
     *
     */
    inline FileSystemModel *clone(const QString &toDirectory) const;

    /**
     * @brief currentDirectories(ListingType) [QStringList]
     *
     * Returns a list of directory names in the current root path.
     *
     */
    QStringList currentDirectories(ListingType = AbsolutePaths) const;

    /**
     * @brief currentFiles(ListingType) [QStringList]
     *
     * Returns a list of files in the current root path.
     *
     */
    QStringList currentFiles(ListingType = AbsolutePaths) const;

    /**
     * @brief iterateFilesystem(ListingType, sort) [QStringList]
     *
     * Iterates throw every directory including subdirectories and files
     * from the current root path.
     *
     * This function skips empty directories and directory names.
     *
     * Be aware, that this function runs in the same thread and you
     * need wait for completion, this can take very very long if the root
     * path if '/' for example.
     *
     * The finished file list can be accessed using 'filelist' [const QStringList*]
     *
     * If the sort boolean is set to true, than the file list is sorted alphabetically.
     * The list may not be always sorted, for example on UNIX platforms where
     * FTW (File Tree Walk) is used. This options makes sure the list is sorted.
     * Sorting takes extra time, use it only if required!
     *
     */
    void iterateFilesystem(ListingType = AbsolutePaths, bool sort = false);

    /**
     * @brief filelist [const QStringList*]
     *
     * Returns a const pointer to the file list built using 'iterateFilesystem'
     *
     */
    QStringList *filelist() const;

    /**
     * @brief hasFiles [bool]
     *
     * Checks if the current path has files in it.
     *
     */
    bool hasFiles() const;

    /**
     * @brief hasDirectories [bool]
     *
     * Checks if the current path has directories in it.
     *
     */
    bool hasDirectories() const;

    /**
     * @brief isValid [bool]
     *
     * Checks if the file system model is still valid.
     *
     * A FileSystemModel can became invalid if the directory it points to
     * was deleted from the filesystem.
     *
     */
    bool isValid() const;

    /**
     * @brief clear [void]
     *
     * Removes the file list and frees the most resources.
     * This functions does not change the root path of the model.
     *
     * To completely free the memory, you need to 'delete' the model.
     * delete new FileSystemModel();
     *
     */
    void clear();

signals:
    void directoryChanged();
    void directoryChanged(const QString &absolutePath);
    void directoryChanged(const QDir&);

private:

    // list sort rule = case-sensitive unicode (icu) alphabetically
    static inline bool sort(const QString &s1, const QString &s2);

    // default QDir sorting and filter flags
    static inline QDir::SortFlags qDirSort();
    static inline QDir::Filters qDirFiles();
    static inline QDir::Filters qDirDirs();

    // helper function for 'currentDirectories' and 'currentFiles'
    // to avoid unnecessary code redundancy
    inline QStringList entryList(ListingType, QDir::Filters) const;

    QStringList *m_filelist = nullptr;

protected:

    QString m_rootPath;
    QDir m_dir;

    int m_rootPathStrLength;

    QStringList m_filters;
};

#endif // FILESYSTEMMODEL_HPP
