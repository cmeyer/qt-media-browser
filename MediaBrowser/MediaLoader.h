#ifndef MEDIA_BROWSER_MEDIA_LOADER_H
#define MEDIA_BROWSER_MEDIA_LOADER_H

#include <boost/smart_ptr.hpp>
#include <QObject>
#include <QIcon>
#include "MediaFile.h"

class MediaFile;
typedef boost::shared_ptr<MediaFile> MediaFilePtr;

// also gives access to the media metadata (icon or audio info).
// this class may return a proxy object. if a proxy object
// is returned, the mediaLoaded signal will be triggered when
// the real object becomes available.

class MediaLoader : public QObject
{
    Q_OBJECT
public:
    MediaLoader() { }

    void reset();
    QIcon mediaFileIcon(const QSize &icon_size, MediaFilePtr media_file, MediaBrowserPrivate::TaskGroupPtr task_group);
    AudioMediaFileInfo mediaFileAudioInfo(MediaFilePtr media_file, MediaBrowserPrivate::TaskGroupPtr task_group);

    // receive this message from media file
    void mediaFileChanged(MediaFilePtr media_file);

Q_SIGNALS:
    void mediaLoaded(MediaFilePtr media_file);

private:
};

typedef boost::shared_ptr<MediaLoader> MediaLoaderPtr;

#endif // MEDIA_BROWSER_MEDIA_LOADER_H
