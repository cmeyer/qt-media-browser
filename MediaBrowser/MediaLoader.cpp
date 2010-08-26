#include <QDebug>
#include "MediaLoader.h"
#include "MediaFile.h"

void MediaLoader::reset()
{
}

QIcon MediaLoader::mediaFileIcon(const QSize &icon_size, MediaFilePtr media_file, MediaBrowserPrivate::TaskGroupPtr task_group)
{
    media_file->loadIcon(this, task_group);

    return media_file->icon(icon_size);
}

AudioMediaFileInfo MediaLoader::mediaFileAudioInfo(MediaFilePtr media_file, MediaBrowserPrivate::TaskGroupPtr task_group)
{
    media_file->loadAudioInfo(this, task_group);

    return media_file->audioInfo();
}

void MediaLoader::mediaFileChanged(MediaFilePtr media_file)
{
    mediaLoaded(media_file);
}
