#include "mediatagsreader.hpp"

#include <QFile>
#include <taglib/taglib.h>
#include <taglib/fileref.h>
#include <taglib/rifffile.h>
#include <taglib/wavfile.h>

#include <QDebug>

MediaTagsReader::MediaTagsReader(MediaLibraryModel::Media *media)
{
    // don't read a nullptr... SEGFAULT
    if (!media) return;

    // store current address
    this->ptr_media = media;

    // don't open a non-existent file, taglib crashes otherwise...
    if (!QFile(this->ptr_media->path).exists())
        return;

    // RIFF INFO Tags
    if (this->ptr_media->fileformat == "wav")
    {
        if (!this->readWavFile())
            (void) this->readTags();
    }

    // Other Tag Formats
    else {
        (void) this->readTags();
    }
}

MediaTagsReader::~MediaTagsReader()
{
    // remove address from pointer
    this->ptr_media = nullptr;
}

bool MediaTagsReader::readWavFile()
{
    TagLib::RIFF::WAV::File m_wavFile(qUtf8Printable(this->ptr_media->path), false); // don't read audio props

    if (!m_wavFile.InfoTag()->isEmpty())
    {
        this->ptr_media->tags.artist = QString::fromUtf8( m_wavFile.InfoTag()->artist().toCString(true) );
        this->ptr_media->tags.album  = QString::fromUtf8( m_wavFile.InfoTag()->album().toCString(true) );
        this->ptr_media->tags.title  = QString::fromUtf8( m_wavFile.InfoTag()->title().toCString(true) );
        this->ptr_media->tags.genre  = QString::fromUtf8( m_wavFile.InfoTag()->genre().toCString(true) );

        if (!(this->ptr_media->tags.artist.isEmpty() &&
            this->ptr_media->tags.album.isEmpty() &&
            this->ptr_media->tags.title.isEmpty()))
            return true;
    }

    else if (!m_wavFile.ID3v2Tag()->isEmpty())
    {
        this->ptr_media->tags.artist = QString::fromUtf8( m_wavFile.ID3v2Tag()->artist().toCString(true) );
        this->ptr_media->tags.album  = QString::fromUtf8( m_wavFile.ID3v2Tag()->album().toCString(true) );
        this->ptr_media->tags.title  = QString::fromUtf8( m_wavFile.ID3v2Tag()->title().toCString(true) );
        this->ptr_media->tags.genre  = QString::fromUtf8( m_wavFile.ID3v2Tag()->genre().toCString(true) );

        if (!this->ptr_media->tags.isEmpty())
            return true;
    }

    return false;
}

bool MediaTagsReader::readTags()
{
    TagLib::FileRef m_fileRef(qUtf8Printable(this->ptr_media->path), false); // don't read audio props

    if (!m_fileRef.isNull())
    {
        if (!m_fileRef.tag()->isEmpty())
        {
            this->ptr_media->tags.artist = QString::fromUtf8( m_fileRef.tag()->artist().toCString(true) );
            this->ptr_media->tags.album  = QString::fromUtf8( m_fileRef.tag()->album().toCString(true) );
            this->ptr_media->tags.title  = QString::fromUtf8( m_fileRef.tag()->title().toCString(true) );
            this->ptr_media->tags.genre  = QString::fromUtf8( m_fileRef.tag()->genre().toCString(true) );

            if (!this->ptr_media->tags.isEmpty())
                return true;
        }
    }

    return false;
}

bool MediaTagsReader::readRiffInfoTags()
{
    /*TagLib::RIFF::WAV::File m_wavFile(qUtf8Printable(this->ptr_media->path), false);
    for (const auto &s : m_wavFile.InfoTag()->fieldListMap())
    {
        // trying out something
        //qDebug() << "8 bit (unicode)       -> " << s.second.to8Bit(true).c_str();
        //qDebug() << "8 bit (ascii)         -> " << s.second.to8Bit(false).c_str();
        //qDebug() << "C string (unicode)    -> " << s.second.toCString(true);
        //qDebug() << "C string (ascii)      -> " << s.second.toCString(false);
        //qDebug() << "C wstring (unicode)   -> " << s.second.toCWString();
        //qDebug() << "C++ wstring (unicode) -> " << s.second.toWString().c_str();
    }
    */

    qWarning() << "MediaTagsReader: readRiffInfoTags() is a stub";
    return false;
}

bool MediaTagsReader::readMatroskaMetadata()
{
    qWarning() << "MediaTagsReader: readMatroskaMetadata() is a stub";
    return false;
}
