// Copyright 2011. LQ Graphics, Inc. and Chris Meyer. LGPL License. See LICENSE file.

#ifndef MEDIA_BROWSER_MEDIA_LOADER_H
#define MEDIA_BROWSER_MEDIA_LOADER_H

#include <boost/smart_ptr.hpp>
#include <QObject>
#include <QIcon>
#include "MediaFile.h"

typedef boost::shared_ptr<class MediaFile> MediaFilePtr;

// the media loader is created by a specific media list model such
// as the music list model or the photo list model. it facilitates
// calculating icons and reading audio meta data in a thread.

// the media list model will invoke methods on this class to request
// icons or meta data. this class can then limit the number
// of threads and resources used to load icons and meta data.

// the media list models should also connect to the media loaded
// signal since the icon or meta data may initially be a proxy.
// the media loaded signal is triggered when the real object becomes
// available.

typedef boost::shared_ptr<class MediaLoader> MediaLoaderPtr;

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

#endif // MEDIA_BROWSER_MEDIA_LOADER_H
