#ifndef MEDIATAGSREADER_HPP
#define MEDIATAGSREADER_HPP

#include <Utils/medialibrarymodel.hpp>

// tags are written immediately into the [Media] object

class MediaTagsReader
{
public:
    MediaTagsReader(MediaLibraryModel::Media *media);
    ~MediaTagsReader();

private:

    // for WAV files
    //   | notes: requires still a lot of work, doesn't play well with hackish unicode tags
    bool readWavFile();

    // for everything else, which the taglib supports
    bool readTags();


    // pointer to the given media object
    MediaLibraryModel::Media *ptr_media;


//==========================//
/* experimental tag readers */
//==========================//

private:

    // the hard way, and with full unicode support (for hacked-in unicode tags in INFO chunk)
    bool readRiffInfoTags();

    // read matroska metadata (for MKA and MKV containers)
    bool readMatroskaMetadata();
};

#endif // MEDIATAGSREADER_HPP
