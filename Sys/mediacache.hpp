#ifndef MEDIACACHE_HPP
#define MEDIACACHE_HPP

/**
 *
 * MediaCache
 * -- ${CONFIGROOT}/cache/[insert strange filenames here]/...
 *
 * The hash files stores the cached information,
 * to speed up the NEXT program startup significantly!
 *
 * Because reading serialized files is faster than re-reading the
 * tags using taglib and re-generating all SearthPathGen strings.
 *
 * Optimally detect changes to files and recreate the
 * cached file as needed.
 *
 * Directory structure:
 *  cache/{CRC32}-hashed dirnames/{SHA-1}-hashed relative media path = filename
 *
 *
 *   Contents of the hash file       DATA IS SERIALIZED using QDataStream
 *  ˙˙˙˙˙˙˙˙˙˙˙˙˙˙˙˙˙˙˙˙˙˙˙˙˙˙˙
 *    [MediaTags struct]
 *      artist, album, title
 *
 *    SearchPathGens generated strings
 *
 */

#include <Utils/medialibrarymodel.hpp>

class MediaCache
{
public:
    static void createInstance(const QString &cacheRoot);
    static MediaCache *i();
    ~MediaCache();

    void setMedia(MediaLibraryModel::Media *);

    // checks if the MediaCache contains the given Media object
    bool hasMedia() const;

    // creates a new cache file in the MediaCache
    bool createMedia() const;

    // stores the cached data into the Media object
    void getCachedData() const;

private:
    MediaCache(const QString &cacheRoot);
    QString m_dir;
    bool m_cacheReadable;

    MediaLibraryModel::Media *ptr_media = nullptr;
    QString m_mediaHash;
    QString m_dirHashed;

    // calculate a hash for the given media
    // the hash is always the same for the same media,
    // as long as the file hasn't changed on the disk
    //
    // a file change can mean, that the user has updated the tags
    // in this case the MediaCache creates a new cache file with the new tags
    static QString getHash(const MediaLibraryModel::Media *);

    // CRC-32 hash for dir names
    // always the same output for the same input
    static const quint32 crc32_table[256];
    static QString getDirHash(const QString &dirName);
};

#endif // MEDIACACHE_HPP
