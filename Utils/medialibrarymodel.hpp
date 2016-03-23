#ifndef MEDIALIBRARYMODEL_HPP
#define MEDIALIBRARYMODEL_HPP

#include "filesystemmodel.hpp"

#include <Utils/searchpathgen.hpp>
#include <Utils/searchkeys.hpp>

#include <QList>
#include <QMap>
#include <QSet>

class MediaLibraryModel : public FileSystemModel
{
public:
    MediaLibraryModel(QObject *parent = 0);
    MediaLibraryModel(const QString &rootPath, QObject *parent = 0);
    ~MediaLibraryModel();

    enum MediaType {
        Audio,
        Video,
        ModuleTracker,

        None // do not filter by media type
    };

    struct MediaTags {
        ~MediaTags();
        bool isEmpty() const;

        QString artist;
        QString album;
        QString title;
        QString genre;
    };

    struct Media {
        ~Media();
        QString fileformat; // just stores the file extension

        QString path;
        QStringList searchPaths;
        MediaTags tags;
        MediaType type;
    };

    void clear(); // delete the whole media database

    void setNameFilters(MediaType, const QStringList &nameFilters);
    void clearNameFilters(MediaType);
    void clearNameFilters();
    QStringList nameFilters(MediaType = None) const; // returns all set name filters

    void setPrefixDeletionPatterns(const QString &patterns);

    void addSearchPathGen(SearchPathGen *);

    void iterateFilesystem();

    // DEVNOTE / TODO:
    //  find() and findMultiple() has almost the same code
    //  try to split that code redundancy out
    //  and move in into a _helper function
    Media *find(const QString &search_term, MediaType = None) const; // returns nullptr if nothing was found, don't forget to check against it!!
    QList<Media*> findMultiple(const QString &search_term, MediaType = None) const; // returns empty list if nothing was found

    int count(MediaType = None) const; // Returns the number of [Media] objects of type [MediaType] in the model

    Media *random(MediaType = None) const; // Returns a random [Media] object, can be nullptr if the media list is empty
    Media *random(const QString &search_term, MediaType = None) const; // Returns a random [Media] object which matches the search term
                                                                       // Can be nullptr if nothing was found, check against it.
    Media *at(int pos, MediaType = None) const; // Returns [Media] at position [pos] in the list, returns a nullptr if out of bound

private:
    void iterateFilesystemHelper(const QStringList &nameFilters, MediaType);
    void buildMediaList(const QStringList*, MediaType);
    void finalizeMediaList();

    void moveInstrumentalTracksToBottom(); // feature: move [Instrumental] tracks to bottom of list, but keep original order
    void createSortedMediaList(); // copy pointers to a MediaType categorized media list map

    QMap<MediaType, QStringList> m_filters;
    QStringList m_prefixDeletionPatterns;

    QList<Media*> m_media;
    QMap<MediaType, QList<Media*> > m_media_sorted;
    QList<SearchPathGen*> m_searchPathGens;

    void deleteSearchPathGens();

private:

    // Random Number Generator
    static int rng(int min, int max);
};

#endif // MEDIALIBRARYMODEL_HPP
