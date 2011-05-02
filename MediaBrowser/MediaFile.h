#ifndef MEDIA_BROWSER_MEDIA_FILE_H
#define MEDIA_BROWSER_MEDIA_FILE_H

#include <vector>
#include <boost/smart_ptr.hpp>
#include <QString>
#include <QIcon>
#include <QMutex>
#include "TaskPool.h"
#include "TaskGroup.h"

class MediaLoader;

class AudioMediaFileInfo
{
public:
    AudioMediaFileInfo() : m_duration(0) { }
    AudioMediaFileInfo(const QString &file_path, const QString &title, const QString &artist, unsigned duration)
            : m_file_path(file_path), m_title(title), m_artist(artist), m_duration(duration) { }
    QString filePath() const { return m_file_path; }
    QString title() const { return m_title; }
    QString artist() const { return m_artist; }
    unsigned duration() const { return m_duration; }
private:
    QString m_file_path;    // this is stored for convenience; but it is redundent from MediaFile.
    QString m_title;
    QString m_artist;
    unsigned m_duration;
};

class MediaFile : public boost::enable_shared_from_this<MediaFile>
{
public:
    MediaFile(const QString &file_path, const QString &source);

    QString filePath() const;
    QString resolvedFilePath() const;
    QString source() const;

    QIcon icon(const QSize &icon_size) const;
    void loadIcon(MediaLoader *media_loader, MediaBrowserPrivate::TaskGroupPtr task_group);

    AudioMediaFileInfo audioInfo() const { return m_audio_info; }
    void setAudioInfo(const AudioMediaFileInfo &audio_info);

private:
    QString m_file_path;
    QString m_source;

    mutable QMutex m_icon_mutex;
    mutable QIcon m_icon;
    mutable QSize m_icon_size;
    QImage m_image;
    bool m_icon_loading;
    bool m_icon_loaded;
    void setImage(const QImage &image);
    void resetImage();

    mutable QMutex m_audio_info_mutex;
    mutable AudioMediaFileInfo m_audio_info;
    bool m_audio_info_loading;
    bool m_audio_info_loaded;
    void resetAudioInfo();
    void loadAudioInfo(MediaLoader *media_loader, MediaBrowserPrivate::TaskGroupPtr task_group);

    friend class MediaFileIconThread;
    friend class MediaFileAudioInfoThread;
    friend class MediaLoader;
};

typedef boost::shared_ptr<MediaFile> MediaFilePtr;
typedef std::vector<MediaFilePtr> MediaFileArray;

// http://www.devarticles.com/c/a/Cplusplus/C-plus-plus-In-Theory-The-Singleton-Pattern-Part-I/1/
class MediaFileTaskPool : public MediaBrowserPrivate::TaskPool
{
public:
    static MediaFileTaskPool *instance();

private:
    MediaFileTaskPool(); // ctor hidden
    MediaFileTaskPool(MediaFileTaskPool const&); // copy ctor hidden
    MediaFileTaskPool& operator=(MediaFileTaskPool const&); // assign op. hidden
    ~MediaFileTaskPool(); // dtor hidden

    static MediaFileTaskPool *m_instance;
};

#endif // MEDIA_BROWSER_MEDIA_FILE_H
