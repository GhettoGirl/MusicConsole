/***********************************************************************************************************************************
 *
 *                      Playlist Parser
 *                     =================
 *
 *  × Features
 *
 *      ~ custom playlists and more control over the
 *        order of songs. the runtime playlist gen is very
 *        limited - my format fixes this ;)
 *
 *      ~ better control over the filter possibilites of
 *        the media library model.
 *
 *      ~ play files outside of the library
 *
 *      ~ different media players for each file (not possible in the runtime gen)
 *
 *  × Syntax Guide
 *
 *     | MUSICCONSOLE PLAYLIST                |  <-- header, must be the first line, indicates a playlist file **
 *     | # my playlist                        |  <-- files starting with a hash are ignored (comments)
 *     |                                      |      comments can also be appended to the end of existing lines
 *     |                                      |  <-- empty lines are ignored
 *     |                                      |
 *     | A:"search for audio" audio           |
 *     | V:"search for video" audio  #comment |
 *     | V:"search for video" video           |
 *     | M:"search for mod" module            |
 *     | N:"search without filter" audio      |
 *     |                                      |
 *     | F:"/absolute/path/to/file" audio     |
 *     | F:"relative/path/to/file" audio      |
 *     |                                      |
 *     | R:""                                 |  <-- random media, without filter, use default player
 *     | R:"search term"                      |  <-- random media, with search criteria, use default player
 *     | R:A:""                               |  <-- random audio file, without search criteria, use default player
 *     | R:V:"search term" audio              |  <-- random video file, with search criteria, use audio player
 *
 *
 *
 *   ~~~ Lookup features
 *
 *    Type filters: A, V, M, N
 *
 *     A = Audio
 *     V = Video
 *     M = ModuleTracker
 *     N = None (do not filter)
 *
 *     Sets the filter when searching for media
 *
 *    Other lookup features: F, R
 *
 *     F = File
 *         Takes a file path (relative or absolute)
 *         Relative paths are always relative to the media root path (config file -> "[library]rootpath")
 *         This feature makes it to possible to play media outside of the media library.
 *
 *     R = Random
 *         Appends a random media from the library to the playlist.
 *         R takes type filters (example: 'R:A' filters by random audio files)
 *
 *
 *    NOTE: If the search term is empty the entry will be ignored, except for 'R' (random)
 *
 *    NOTE 2: double quotes (") in the search term or filename must NOT be escaped -> \"
 *            there can by an odd amout of quotes in the search term or filename
 *
 *    NOTE 3: if you don't need to change the default player, quotes can be dropped in the first place ;)
 *            this rule does not apply if the search term or filename (should) contain(s) quotes
 *
 *               "search term with " in it"          -->  search term with " in it
 *               search term with " it it            -->  [syntax error, entry is ignored]
 *               /path/to/file with " quotes.flac    -->  [syntax error, entry is ignored]
 *               "/path/to/file with " quotes.flac"  -->  /path/to/file with " quotes.flac
 *
 *
 *
 *   ~~~ Select media player
 *
 *    You can specify which media player should be used for the file.
 *    The default players can be selected using this keywords:
 *      ~ audio, video, module
 *
 *    If no player is specified, the defaults are as follows:
 *      A = audio           <-|
 *      V = video           <-|
 *      M = module          <-|
 *      N = audio           <-|
 *                            |
 *      F = audio             |
 *      R = audio             |
 *      R:[type] = [type]  <---
 *
 *
 *
 *
 * Quick summary:
 *
 *  Format: [lookup feature]:"search term or filename" [player(=optimal)]
 *  ~ empty search term fields are ignored, except for 'R' (random feature)
 *  ~ invalid fields are ignored / skipped
 *
 *
 *   ** first line means not the first line in the file,
 *      but the first line before any playlist entries.
 *      everything above this can whatever you want, it is ignored/erased anyways
 *      the syntax is only checked below this indicator ;)
 *
 *  NOTE: The PlaylistParser tries to fix and ignore as much "shit" as it can before giving up
 *        with a SyntaxError.
 *        This makes it possible to parse the worst written files and still get results from it :)
 *
 *        The only syntax error is the missing "MUSICCONSOLE PLAYLIST" line in the file :D
 *
 *  FIX: due to lazy implementation of some functionalities, quotes (") in comments are not
 *       supported, this causes undefined behavior at the moment. avoid that!
 *
 *  FIX: string detection sucks in many ways
 *       /// may help in the future? --> QRegExp str_detect(" (?=[^\"]*(\"[^\"]*\"[^\"]*)*$)");
 *                                                           (")(?:(?=(\\?))\2.)*?"
 *
 */

#ifndef PLAYLISTPARSER_HPP
#define PLAYLISTPARSER_HPP

#include <QString>

#include <Utils/medialibrarymodel.hpp>

class PlaylistParser
{
public:
    PlaylistParser(const QString &file, MediaLibraryModel *media_model);
    ~PlaylistParser();

    // Possible parsing results
    enum ParseStatus {
        Success,         // playlist file parsed successfully
        FileNotFound,    // the requested file was not found
        FileNotReadable, // the requested file is not readable; permission problems etc.
        FileEmpty,       // the requested file was empty,
                         // the file is also treated as empty if it only contains
                         // whitespace characters according to UnicodeWhitespaceFixer (SearchPathGens/UnicodeWhitespaceFixer.hpp)
        PlaylistEmpty,   // valid plist file, but no entries found (also: empty entries are ignored)
        SyntaxError      // format error, refer to the syntax guide
    };

    // contains a pointer to the (generated) media object and the player to use
    struct PlaylistEntry {
        MediaLibraryModel::MediaType player;
        MediaLibraryModel::Media *media;
    };

    // Parses the playlist file
    ParseStatus parse();

    // Clears the playlist data and deletes all unique media objects
    void clear();

    // Returns a list of files, relative or absolute paths (autodetect)
    // The MediaLibraryModel's root path is used to find out
    //  the onlyAbsolutePaths boolean gives you the possibility to return only
    //  absolute file paths.
    //  default is a mix of relative and absolute file paths.
    QStringList files(bool onlyAbsolutePaths = false) const;

    // Returns a const reference to the generated playlist, everything is ready to play and valid
    // For each media, which is not in the model, a new Media object is created, this includes MediaTags [MediaTagsReader class]
    const QList<PlaylistEntry> &playlist() const;

private:

    // Playlist data
    QString m_file;
    QString *m_data; // use QString instead of QByteArray because Unicode.

    // Unique media objects, for files which are not in the model
    QList<MediaLibraryModel::Media*> m_unique_media;

    // actual playlist, preserves original order according to the playlist file
    QList<PlaylistEntry> m_playlist;

    // maintain a pointer to the media library model
    MediaLibraryModel *ptr_media_model = nullptr;

private:

    // creates a Media object and appends it to the unique media list, returns a pointer to the media object
    // this helper function expects an existing and readable file
    // make sure to check before calling this function
    // the media type is always set to [None]
    MediaLibraryModel::Media *createMediaObject(const QString &file);

    bool byteArrayEmptyCheck() const;

    // type of the entry
    enum PlaylistEntryType {
        SearchEntry,  // = MediaLibraryModel::find()
        RandomEntry,  // = MediaLibraryModel::random()
        FileEntry     // = PlaylistParser::createMediaObject()
    };

    // contains data needed to create a [PlaylistEntry]
    struct PlaylistEntryPrivate {
        MediaLibraryModel::MediaType mediaType; // passed to MediaLibraryModel::find() together with the string
        MediaLibraryModel::MediaType player;    // copied to the PlaylistEntry
        PlaylistEntryType type;                 // type of the playlist entry, required for PlaylistEntry creation
        QString string;                         // string of the playlist entry
        bool valid;                             // syntax error handling
    };

    static QString parseEntryString(const QString &entry_data, PlaylistEntryPrivate &entry);
    static MediaLibraryModel::MediaType parseEntryPlayer(const QString &entry_data,
                                                         MediaLibraryModel::MediaType default_player);
};

#endif // PLAYLISTPARSER_HPP
