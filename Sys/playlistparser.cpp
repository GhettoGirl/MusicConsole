#include "playlistparser.hpp"

#include <QFile>

#include <Utils/mediatagsreader.hpp>

#include <SearchPathGens/unicodewhitespacefixer.hpp>

PlaylistParser::PlaylistParser(const QString &file, MediaLibraryModel *media_model)
{
    this->ptr_media_model = media_model;
    this->m_file = file;
}

PlaylistParser::~PlaylistParser()
{
}

void PlaylistParser::clear()
{
    this->m_file.clear();

    // deletes all unique created media objects
    for (MediaLibraryModel::Media *media : this->m_unique_media)
        delete media;
    this->m_unique_media.clear();

    // delete playlist entries
    this->m_playlist.clear();

    this->ptr_media_model = nullptr;
}

PlaylistParser::ParseStatus PlaylistParser::parse()
{
    if (this->m_file.isEmpty())
        return FileNotFound;


    //// ***** prepare data for parsing *****

    QFile file(this->m_file);

    // check if file exists
    if (!file.exists())
        return FileNotFound;

    // (try to) open the file
    // QFile::Text, qt transforms all end-of-line separators to \n
    if (!file.open(QFile::ReadOnly | QFile::Text))
        return FileNotReadable;

    // read all data from file
    this->m_data = new QString(QByteArray(file.readAll()));
    file.close();

    // simplify whitespaces in the data array
    static const UnicodeWhitespaceFixer usf;
    usf.processTextFileData(this->m_data); // this alternative method preserves newline characters \n

    // check if we got any useable data
    if (this->byteArrayEmptyCheck())
    {
        this->m_data->clear();
        delete this->m_data;
        return FileEmpty;
    }


    //// ***** begin parsing the file here *****

    // create a list, split at every new line
    QStringList lines = this->m_data->split('\n', QString::SkipEmptyParts);

    // clear playlist data, from now on use QStringList lines
    this->m_data->clear();
    delete this->m_data;

    // simplify every line
    for (QString &line : lines)
        line = line.simplified();

    // delete empty lines
    lines.erase(
        std::remove_if(
            lines.begin(),
            lines.end(),
            [](const QString &s) {
                return s.isEmpty();
            }),
        lines.end()
    );

    // erase everything above the playlist indicator
    // --> MUSICCONSOLE PLAYLIST
    int indicator_pos = lines.indexOf("MUSICCONSOLE PLAYLIST");

    if (indicator_pos == -1)
    {
        lines.clear();
        return SyntaxError;
    }

    indicator_pos++; // remove indicator itself
    for (int i = 0; i < indicator_pos; i++)
        lines.removeAt(0);

    // return is got empty
    if (lines.isEmpty())
    {
        lines.clear();
        return PlaylistEmpty;
    }

    // erase all comments
    lines.erase(
        std::remove_if(
            lines.begin(),
            lines.end(),
            [](const QString &s) {
                return s.startsWith('#');
            }),
        lines.end()
    );

    // trim remaining comments in a very primitive and easy way :P
    // TODO: prober comment detection
    int size = lines.size();
    for (int i = 0; i < size; i++)
    {
        int last_hash_pos = lines.at(i).lastIndexOf('#');

        if (last_hash_pos == -1)
            continue;

        int last_quote_pos = lines.at(i).lastIndexOf('"');

        if (last_hash_pos > last_quote_pos)
        {
            lines[i].remove(last_hash_pos, lines.at(i).size());
            lines[i] = lines.at(i).simplified();
        }
    }

    // lookup type set
    static const QList<QChar> lookup_types = {
        'A','V','M','N','F','R'
    };

    // erase invalid entries from list
    lines.erase(
        std::remove_if(
            lines.begin(),
            lines.end(),
            [&](const QString &s) {
                for (const QChar &c : lookup_types)
                    if (s.startsWith(c+':'))
                        return false;
                return true;
            }),
        lines.end()
    );

    // empty check
    if (lines.isEmpty())
    {
        lines.clear();
        return PlaylistEmpty;
    }


    //// ***** pre-generate the playlist data *****

    // private playlist entries
    QList<PlaylistEntryPrivate> plist_data;

    // parse playlist entries
    for (const QString &line : lines)
    {
        PlaylistEntryPrivate entry;
        entry.valid = true;
        int mid_index = 2;

        // Audio
        if (line.startsWith('A'))
        {
            entry.type = SearchEntry;
            entry.mediaType = MediaLibraryModel::Audio;
        }

        // Video
        else if (line.startsWith('V'))
        {
            entry.type = SearchEntry;
            entry.mediaType = MediaLibraryModel::Video;
        }

        // Module Tracker
        else if (line.startsWith('M'))
        {
            entry.type = SearchEntry;
            entry.mediaType = MediaLibraryModel::ModuleTracker;
        }

        // (None)
        else if (line.startsWith('N'))
        {
            entry.type = SearchEntry;
            entry.mediaType = MediaLibraryModel::None;
        }

        // FILE
        else if (line.startsWith('F'))
        {
            entry.type = FileEntry;
            entry.mediaType = MediaLibraryModel::None; // a filter does not apply to files
        }

        // Random
        else if (line.startsWith('R'))
        {
            mid_index = 4;
            entry.type = RandomEntry;
            QString linetmp = line.mid(2);
            if      (linetmp.startsWith("A:")) entry.mediaType = MediaLibraryModel::Audio;
            else if (linetmp.startsWith("V:")) entry.mediaType = MediaLibraryModel::Video;
            else if (linetmp.startsWith("M:")) entry.mediaType = MediaLibraryModel::ModuleTracker;
            else if (linetmp.startsWith("N:")) entry.mediaType = MediaLibraryModel::None;
            else { mid_index = 2; entry.mediaType = MediaLibraryModel::None; } // default
            linetmp.clear();
        }


        /// ~~~ General ~~~

        entry.string = this->parseEntryString(line.mid(mid_index), entry);
        entry.player = this->parseEntryPlayer(line.mid(mid_index), entry.mediaType);

        if (entry.valid)
            plist_data.append(entry);
    }

    // clean up
    lines.clear();

    // another empty check
    if (plist_data.isEmpty())
    {
        plist_data.clear();
        return PlaylistEmpty;
    }


    //// ***** generate the playlist entries *****

    for (PlaylistEntryPrivate &p : plist_data)
    {
        PlaylistEntry entry;
        entry.media = nullptr;

        // SearchEntry
        if (p.type == SearchEntry)
        {
            MediaLibraryModel::Media *media = this->ptr_media_model->find(p.string, p.mediaType);
            if (media)
            {
                entry.media = media;
                entry.player = p.player;
            }
        }

        // RandomEntry
        else if (p.type == RandomEntry)
        {
            MediaLibraryModel::Media *media;

            if (p.string.isEmpty())
                media = this->ptr_media_model->random(p.mediaType);
            else media = this->ptr_media_model->random(p.string, p.mediaType);

            if (media)
            {
                entry.media = media;
                entry.player = p.player;
            }
        }

        // FileEntry, create a new media object if possible
        else if (p.type == FileEntry)
        {
            if (QFile(p.string).exists())
            {
                entry.media = this->createMediaObject(p.string);
                entry.player = p.player;
            }
        }

        // clean up
        p.string.clear();

        // append to playlist
        if (entry.media)
        {
            if (entry.player == MediaLibraryModel::None) // default is always the audio player
                entry.player = MediaLibraryModel::Audio;
            this->m_playlist.append(entry);
        }
    }

    // delete private entry list
    plist_data.clear();

    return Success;
}

QStringList PlaylistParser::files(bool onlyAbsolutePaths) const
{
    QStringList files;

    for (const PlaylistEntry &data : this->m_playlist)
    {

        // append only absolute file paths, also canonicalize the path if possible
        if (onlyAbsolutePaths)
        {
            QFileInfo file(data.media->path);
            file.setFile(file.canonicalFilePath());
            files.append(file.absoluteFilePath());
        }

        // mix relative and absolute paths
        else {
            files.append(data.media->path);
        }
    }

    return files;
}

const QList<PlaylistParser::PlaylistEntry> &PlaylistParser::playlist() const
{
    return this->m_playlist;
}

MediaLibraryModel::Media *PlaylistParser::createMediaObject(const QString &file)
{
    MediaLibraryModel::Media *media = new MediaLibraryModel::Media;
    media->path = file;
    media->type = MediaLibraryModel::None;
    MediaTagsReader tags(media);

    int ext_pos = media->path.lastIndexOf('.');
    if (ext_pos != -1)
        media->fileformat = media->path.mid(ext_pos+1);

    static const QString homePath = QDir::homePath();
    static const int homePathSize = homePath.size();

    if (media->path.startsWith(homePath))
        media->searchPaths.append('~' + media->path.mid(homePathSize));

    this->m_unique_media.append(media);
    return media;
}

bool PlaylistParser::byteArrayEmptyCheck() const
{
    for (const QChar &c : *this->m_data)
        if (c != ' ' && c != '\n')
            return false;

    return true;
}

QString PlaylistParser::parseEntryString(const QString &entry_data, PlaylistEntryPrivate &entry)
{
    /// DEV-NOTE: this may be the stupiest string detection you have ever seen :D
    ///
    /// any amount of quotes in filenames works for some reason :/
    ///  ( quotes doesn't need to be escaped \" )
    /// at least this method is reasonable enough for the moment

    // if no string were found return all data
    if (!entry_data.contains('"'))
        return entry_data;

    int begin = entry_data.indexOf('"'); // cannot be -1 due to of previous check
    int end = entry_data.lastIndexOf('"');

    // syntax error
    if (begin == end)
    {
        entry.valid = false;
        return QString();
    }

    return entry_data.mid(begin+1, end-begin-1);
}

MediaLibraryModel::MediaType PlaylistParser::parseEntryPlayer(const QString &entry_data, MediaLibraryModel::MediaType default_player)
{
    // if no string were found assume all data as string
    // and return the default player
    if (!entry_data.contains('"'))
        return default_player;

    int idx = entry_data.lastIndexOf('"'); // cannot be -1 due to of previous check

    QString player = entry_data.mid(idx+1).simplified();

    if (player.isEmpty())
        return default_player;

    else if (QString::compare(player, "audio", Qt::CaseInsensitive) == 0)
        return MediaLibraryModel::Audio;
    else if (QString::compare(player, "video", Qt::CaseInsensitive) == 0)
        return MediaLibraryModel::Video;
    else if (QString::compare(player, "module", Qt::CaseInsensitive) == 0)
        return MediaLibraryModel::ModuleTracker;

    return default_player;
}
