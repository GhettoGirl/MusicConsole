#include "medialibrarymodel.hpp"

#include <Utils/mediatagsreader.hpp>
#include <Sys/mediacache.hpp>

#include <chrono>
#include <random>

MediaLibraryModel::MediaLibraryModel(QObject *parent)
    : FileSystemModel(parent)
{
    // construct a media library model of the users home directory
    this->m_dir = QDir::home();
    this->m_rootPath = this->m_dir.absolutePath();
    this->m_rootPathStrLength = this->m_rootPath.size() + 1;
    (void) this->changePwd();
}

MediaLibraryModel::MediaLibraryModel(const QString &rootPath, QObject *parent)
    : MediaLibraryModel(parent)
{
    // try to change the root directory of the media library model
    if (this->setRootPath(rootPath))
        (void) this->changePwd();
}

MediaLibraryModel::~MediaLibraryModel()
{
    this->m_filters.clear();
    this->m_prefixDeletionPatterns.clear();
    this->clear();

    this->deleteSearchPathGens();
}

MediaLibraryModel::MediaTags::~MediaTags()
{
    this->artist.clear();
    this->album.clear();
    this->title.clear();
}

bool MediaLibraryModel::MediaTags::isEmpty() const
{
    // check if everything is empty
    if (this->artist.isEmpty() &&
        this->album.isEmpty() &&
        this->title.isEmpty() &&
        this->genre.isEmpty())
        return true;

    // at least one field is not empty
    return false;
}

MediaLibraryModel::Media::~Media()
{
    this->path.clear();
    this->searchPaths.clear();
    this->fileformat.clear();
}

void MediaLibraryModel::setNameFilters(MediaType type, const QStringList &nameFilters)
{
    if (nameFilters.isEmpty())
        return;

    if (this->m_filters.contains(type))
    {
        this->m_filters[type].clear();
        this->m_filters[type] = nameFilters;
    }

    else {
        (void) this->m_filters.insert(type, nameFilters);
    }
}

void MediaLibraryModel::clearNameFilters(MediaType type)
{
    if (this->m_filters.contains(type))
    {
        this->m_filters[type].clear();
        (void) this->m_filters.remove(type);
    }
}

void MediaLibraryModel::clearNameFilters()
{
    this->m_filters.clear();
}

QStringList MediaLibraryModel::nameFilters(MediaType type) const
{
    if (type == None)
    {
        QStringList filters;

        if (this->m_filters.contains(Audio))
            filters.append(this->m_filters[Audio]);
        if (this->m_filters.contains(Video))
            filters.append(this->m_filters[Video]);
        if (this->m_filters.contains(ModuleTracker))
            filters.append(this->m_filters[ModuleTracker]);

        return filters;
    }

    else {
        if (this->m_filters.contains(type))
        {
            return this->m_filters[type];
        }
    }

    return QStringList();
}

void MediaLibraryModel::setPrefixDeletionPatterns(const QString &patterns)
{
    this->m_prefixDeletionPatterns = patterns.split(';', QString::SkipEmptyParts);
}

void MediaLibraryModel::addSearchPathGen(SearchPathGen *gen)
{
    // don't add null pointers to the list
    if (gen) this->m_searchPathGens.append(gen);
}

void MediaLibraryModel::iterateFilesystem()
{
    this->clear();

    // if no namefilters were set (userconfig), use some hardcoded defaults to keep the media list clean
    // don't confuse this default list with the one from ConfigManager, it just adds more security
    if (!this->m_filters.contains(Audio))
        this->m_filters.insert(Audio, QStringList(
            {"*.wav","*.flac","*.tta","*.aiff","*.ape","*.pcm","*.alac","*.dts","*.m4a","*.ogg","*.mka","*.wma","*.asf","*.ra","*.aac","*.mp3"}));
    if (!this->m_filters.contains(Video))
        this->m_filters.insert(Video, QStringList(
            {"*.mp4","*.h264","*.h263","*.ts","*.m2ts","*.mov","*.ogm","*.avi","*.bk2","*.bnk","*.mkv","*.wmv","*.rv"}));
    if (!this->m_filters.contains(ModuleTracker))
        this->m_filters.insert(ModuleTracker, QStringList({"*.xm","*.it","*.mod","*.med","*.sid","*.s3m"}));

    this->iterateFilesystemHelper(this->m_filters[Audio], Audio);
    this->iterateFilesystemHelper(this->m_filters[Video], Video);
    this->iterateFilesystemHelper(this->m_filters[ModuleTracker], ModuleTracker);

    this->finalizeMediaList();
}

void MediaLibraryModel::iterateFilesystemHelper(const QStringList &nameFilters, MediaType type)
{
    // build filelist with media type filters
    this->FileSystemModel::setNameFilters(nameFilters);
    this->FileSystemModel::iterateFilesystem(FileSystemModel::RelativePaths, true);

    // build [Media] objects and append to list
    this->buildMediaList(this->FileSystemModel::filelist(), type);

    // clean up
    this->FileSystemModel::clear();
    this->FileSystemModel::clearNameFilters();
}

MediaLibraryModel::Media *MediaLibraryModel::find(const QString &search_term, MediaType type) const
{
    // Create search patterns, if the search terms are empty, skip search and return nothing
    SearchKeys search(search_term);
    if (search.empty())
        return nullptr;

    // Create sub lists <since we work with pointers, there is only minimal memory usage for the list allocation itself>
    QList<Media*> filter_list;
    QList<Media*> search_list;

    // Create a media type filter list
    if (type == None)
    {
        filter_list = this->m_media;
    }

    else
    {
        for (Media *media : this->m_media)
        {
            if (media->type == type)
                filter_list.append(media);
        }
    }

    // Check if search keys has filter patterns
    if (search.containsKey(SearchKeys::WithoutAnyOfThis) ||
        search.containsKey(SearchKeys::WithoutGenre))
    {

        QList<SearchKeys::SearchPattern> WithoutAnyOfThis = search.searchPatterns(SearchKeys::WithoutAnyOfThis);
        int WithoutAnyOfThisKeyCount = search.countKeys(SearchKeys::WithoutAnyOfThis);

        QList<SearchKeys::SearchPattern> WithoutGenre = search.searchPatterns(SearchKeys::WithoutGenre);
        int WithoutGenreKeyCount = search.countKeys(SearchKeys::WithoutGenre);

        bool mark_as_dont_add = false;

        for (Media *media : filter_list)
        {
            for (const QString &searchPath : media->searchPaths)
            {

                // keyword filter
                for (int i = 0; i < WithoutAnyOfThisKeyCount; i++)
                {
                    if (WithoutAnyOfThis.at(i).searchPattern.exactMatch(searchPath))
                    {
                        mark_as_dont_add = true;
                        break; // we are done here
                    }
                }

                // genre filter
                for (int i = 0; i < WithoutGenreKeyCount; i++)
                {
                    if (WithoutGenre.at(i).searchPattern.exactMatch(media->tags.genre))
                    {
                        mark_as_dont_add = true;
                        break; // we are done here
                    }
                }
            }

            if (!mark_as_dont_add)
                search_list.append(media);

            mark_as_dont_add = false; // reset this for a new loop
        }
    }

    else {
        search_list = filter_list;
    }

    // Search: Default, IncludeIntoMainSearch
    for (Media *media : search_list)
    {
        for (const QString &searchPath : media->searchPaths)
        {
            for (const SearchKeys::SearchPattern &s : search.searchPatterns())
            {
                if (s.type == SearchKeys::Default || s.type == SearchKeys::IncludeIntoMainSearch)
                {
                    if (s.searchPattern.exactMatch(searchPath))
                    {
                        filter_list.clear(); // remove pointer copies
                        search_list.clear(); // remove pointer copies
                        return media;
                    }
                }
            }
        }
    }

    // remove pointer copies
    filter_list.clear();
    search_list.clear();

    // nothing found
    return nullptr;
}

QList<MediaLibraryModel::Media*> MediaLibraryModel::findMultiple(const QString &search_term, MediaType type) const
{
    QList<Media*> results;

    // Create search patterns, if the search terms are empty, skip search and return nothing
    SearchKeys search(search_term);
    if (search.empty())
        return results;

    // Create sub lists <since we work with pointers, there is only minimal memory usage for the list allocation itself>
    QList<Media*> filter_list;
    QList<Media*> search_list;

    // Create a media type filter list
    if (type == None)
    {
        filter_list = this->m_media;
    }

    else
    {
        for (Media *media : this->m_media)
        {
            if (media->type == type)
                filter_list.append(media);
        }
    }

    // Check if search keys has filter patterns
    if (search.containsKey(SearchKeys::WithoutAnyOfThis) ||
        search.containsKey(SearchKeys::WithoutGenre))
    {

        QList<SearchKeys::SearchPattern> WithoutAnyOfThis = search.searchPatterns(SearchKeys::WithoutAnyOfThis);
        int WithoutAnyOfThisKeyCount = search.countKeys(SearchKeys::WithoutAnyOfThis);

        QList<SearchKeys::SearchPattern> WithoutGenre = search.searchPatterns(SearchKeys::WithoutGenre);
        int WithoutGenreKeyCount = search.countKeys(SearchKeys::WithoutGenre);

        bool mark_as_dont_add = false;

        for (Media *media : filter_list)
        {
            for (const QString &searchPath : media->searchPaths)
            {

                // keyword filter
                for (int i = 0; i < WithoutAnyOfThisKeyCount; i++)
                {
                    if (WithoutAnyOfThis.at(i).searchPattern.exactMatch(searchPath))
                    {
                        mark_as_dont_add = true;
                        break; // we are done here
                    }
                }

                // genre filter
                for (int i = 0; i < WithoutGenreKeyCount; i++)
                {
                    if (WithoutGenre.at(i).searchPattern.exactMatch(media->tags.genre))
                    {
                        mark_as_dont_add = true;
                        break; // we are done here
                    }
                }
            }

            if (!mark_as_dont_add)
                search_list.append(media);

            mark_as_dont_add = false; // reset this for a new loop
        }
    }

    else {
        search_list = filter_list;
    }

    // Search: Default, IncludeIntoMainSearch
    bool next;
    for (Media *media : search_list)
    {
        for (const QString &searchPath : media->searchPaths)
        {
            for (const SearchKeys::SearchPattern &s : search.searchPatterns())
            {
                if (s.type == SearchKeys::Default || s.type == SearchKeys::IncludeIntoMainSearch)
                {
                    if (s.searchPattern.exactMatch(searchPath))
                    {
                        results.append(media);

                        next = true;
                        break;
                    }
                }

                if (next) break;
            }

            if (next) break;
        }

        next = false;
    }

    // remove pointer copies
    filter_list.clear();
    search_list.clear();

    // return results
    return results;
}

int MediaLibraryModel::count(MediaType type) const
{
    if (type == None)
        return this->m_media.size();

    return this->m_media_sorted[type].size();
}

MediaLibraryModel::Media *MediaLibraryModel::random(MediaType type) const
{
    // prevent rng crash <this can happen if min and max is 0 for example>
    if (this->m_media.isEmpty())
        return nullptr;

    int list_size = 0;

    if (type == None)
        list_size = this->m_media.size();
    else list_size = this->m_media_sorted[type].size();

    // obtain random number
    int index = this->rng(0, list_size - 1);

    if (type == None)
    {
        return this->m_media.at(index);
    }

    return this->m_media_sorted[type].at(index);
}

MediaLibraryModel::Media *MediaLibraryModel::random(const QString &search_term, MediaType type) const
{
    QList<Media*> results = this->findMultiple(search_term, type);

    // prevent rng crash <this can happen if min and max is 0 for example>
    if (results.empty())
        return nullptr;

    // obtain random number
    int index = this->rng(0, results.size() - 1);

    return results.at(index);
}

MediaLibraryModel::Media *MediaLibraryModel::at(int pos, MediaType type) const
{
    if (type == None)
    {
        if (pos >= 0 && pos < this->m_media.size())
        {
            return this->m_media.at(pos);
        }
    }

    else
    {
        if (pos >= 0 && pos < this->m_media_sorted[type].size())
        {
            return this->m_media_sorted[type].at(pos);
        }
    }

    return nullptr;
}

void MediaLibraryModel::buildMediaList(const QStringList *list, MediaType type)
{
    for (const QString &f : *list)
    {

        // create a new media object
        Media *media = new Media;

        /// basic media info
        //media->path = this->rootPath() + QDir::separator() + f; // absolute paths
        media->path = f; // relative paths, requires FileSystemModel::changePwd() to be called
        media->type = type;

        /// file format (extension)
        // everything after last dot converted to lower-case
        // used for different features around the app
        int ext_pos = media->path.lastIndexOf('.');
        if (ext_pos != -1)
            media->fileformat = media->path.mid(ext_pos+1).toLower();

        // remove set prefixes from the file path to reduce memory usage and processing time later
        // also adds the possibility to clean up the search results
        // prefix deletion patterns are case-sensitive, no wildcards or regular expressions supported
        QString _f = f;
        for (const QString &prefix : this->m_prefixDeletionPatterns)
            if (_f.startsWith(prefix))
                _f.remove(0, prefix.size());

        // don't read tags here, takes literally forever
        // read tags when the media is requested for the first time
        // and than store it [MediaTags struct] and never read it again
        //
        // the model works with pointers to [Media struct]'s, so you can
        // change the tags from literally everywhere
        //
        // example:
        //
        //   // read tags and write into [Media] objects [MediaTags struct]
        //   Media *whatever = model->find("whatever");
        //      whatever->tags is empty at this stage
        //   MediaTagsReader tags(whatever);
        //
        //   // somewhere else in the program
        //
        //   Media *whatever2 = model->find("whatever");
        //     whatever2->tags are not empty anymore
        //
        //
        // NOTE that the MediaLibraryModel does this automatically!
        //  read tags are added to the search path list which is used
        //  by the ::find() and ::findMultiple() member functions
        //

        // check for cached data and use this, if no cached data was found, we generate one
        MediaCache::i()->setMedia(media);
        if (MediaCache::i()->hasMedia())
        {
            MediaCache::i()->getCachedData();
        }

        // no cached data found, generate one
        else
        {

            // add 'cleaned' path to search paths
            media->searchPaths.append(_f);

            // generate search paths
            // more SearchPathGens means longer processing and higher memory usage
            for (const SearchPathGen *gen : this->m_searchPathGens)
                media->searchPaths.append(gen->processString(_f));

            // remove duplicates from search paths
            // the SearchPathGens may not always create a "new" string
            // so lets remove the duplicates to save memory
            (void) media->searchPaths.removeDuplicates();

            // read tags
            MediaTagsReader reader(media);
            Q_UNUSED(reader); // get rid of compiler warning

            // add artist, album and title to search paths, to provide better lookups
            // don't add, if all of these 3 fields are empty
            // ignore the other tags
            if (!(media->tags.artist.isEmpty() &&
                media->tags.album.isEmpty() &&
                media->tags.title.isEmpty()))
                media->searchPaths.append(media->tags.artist + ' ' +
                                          media->tags.album + ' ' +
                                          media->tags.title);

            // write data to cache file
            (void) MediaCache::i()->createMedia();

        }

        // add to list
        this->m_media.append(media);

        // clean up
        _f.clear();
    }
}

void MediaLibraryModel::finalizeMediaList()
{
    // remove redundant data (saves about 30% memory usage process internally)  :)
    //
    // NOTICE: I have ~6700 media files in my library, and the whole process requires just ~3MB of memory ;)
    //         including all the SearchPathGens
    //         of course this does not include the tags yet.
    //
    // In my old version of Music Console (which is private), which i originally wrote in 2012, the same database
    // required like ~50MB memory. I learned so much in this years and was able to reduce it to ~3MB. Is'n that awesome? :P
    this->FileSystemModel::clear();

    // feature: move [Instrumental] and [off vocal] tracks to bottom of list, but keep original sorting
    // I added this to prevent instrumental tracks playing ALL THE TIME, if not explicitly asked for :)
    this->moveInstrumentalTracksToBottom();

    // copy pointers to a MediaType categorized media list map
    // for quick and easy [MediaType] access
    this->createSortedMediaList();
}

void MediaLibraryModel::moveInstrumentalTracksToBottom()
{
    if (this->m_media.isEmpty())
        return;

    // middle-man lists
    QList<Media*> *list_main = new QList<Media*>(),
                  *list_inst = new QList<Media*>();

    for (Media *media : this->m_media)
    {

        // support Latin1 and Wide-Latin (Unicode)
        if (media->path.contains("instrumental", Qt::CaseInsensitive) ||
            media->path.contains("ｉｎｓｔｒｕｍｅｎｔａｌ", Qt::CaseInsensitive) ||
            media->path.contains("off vocal", Qt::CaseInsensitive) ||             // --
            media->path.contains("ｏｆｆ ｖｏｃａｌ", Qt::CaseInsensitive) ||        //  |-- typical phrases for japenese instrumental tracks
            media->path.contains("ｏｆｆ　ｖｏｃａｌ", Qt::CaseInsensitive))         // --
            list_inst->append(media);

        else list_main->append(media);
    }

    // rebuild the media list
    this->m_media.clear();
    this->m_media.append(*list_main);
    this->m_media.append(*list_inst);

    // clean up, but don't remove the actual media objects
    list_main->clear();
    delete list_main;
    list_inst->clear();
    delete list_inst;
}

void MediaLibraryModel::createSortedMediaList()
{
    if (this->m_media.isEmpty())
        return;

    this->m_media_sorted.clear();

    QList<Media*> la, lv, lm;

    for (Media *media : this->m_media)
    {

        // Audio
        if (media->type == Audio)
            la.append(media);

        // Video
        else if (media->type == Video)
            lv.append(media);

        // ModuleTracker
        else if (media->type == ModuleTracker)
            lm.append(media);
    }

    this->m_media_sorted.insert(Audio, la);
    this->m_media_sorted.insert(Video, lv);
    this->m_media_sorted.insert(ModuleTracker, lm);

    la.clear();
    lv.clear();
    lm.clear();
}

void MediaLibraryModel::clear()
{
    for (Media *media : this->m_media)
    {
        if (media) delete media;
    }

    this->m_media.clear();
    this->m_media_sorted.clear();

    this->FileSystemModel::clear();
}

void MediaLibraryModel::deleteSearchPathGens()
{
    for (SearchPathGen *gen : this->m_searchPathGens)
    {
        if (gen) delete gen;
    }

    this->m_searchPathGens.clear();
}

int MediaLibraryModel::rng(int min, int max)
{
    static int last = 0;
    static int now;

    static std::random_device rd;
    static std::mt19937 mt(rd());
    std::uniform_int_distribution<int> dist(min, max);

    now = dist(mt);

    /* prevent the same number to be returned twice in a row
     * its very rare, but it can happen
     * if it actually happens, its just annoying to hear the same song again
     */
    while (now == last)
        now = dist(mt);

    last = now;
    return now;
}
