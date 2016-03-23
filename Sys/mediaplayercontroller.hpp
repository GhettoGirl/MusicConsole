#ifndef MEDIAPLAYERCONTROLLER_HPP
#define MEDIAPLAYERCONTROLLER_HPP

#include <QString>
#include <QStringList>

#include <QMap>

#include <Utils/medialibrarymodel.hpp>

class MediaPlayerController
{
public:
    static MediaPlayerController *i();
    ~MediaPlayerController();

    void setAudioPlayer(const QString &cmd);
    void setVideoPlayer(const QString &cmd);
    void setModulePlayer(const QString &cmd);

    void registerPlayerForFormat(const QString &fileformat, const QString &cmd);

    // plays the media, the [MediaType] can be temporarily overridden to use another player
    void play(MediaLibraryModel::Media *media, MediaLibraryModel::MediaType = MediaLibraryModel::None);

private:
    MediaPlayerController();

    void execute();

    static void escapeSequences(QString &str);
    void clear();

    QString m_audioplayer,
            m_videoplayer,
            m_modplayer;

    QMap<QString, QString> m_playerOverrides;

    QString m_command;
    QString m_file;

    static const QChar m_quote;
};

#endif // MEDIAPLAYERCONTROLLER_HPP
