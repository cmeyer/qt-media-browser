#ifndef MEDIA_BROWSER_MUSIC_LIST_MODEL_H
#define MEDIA_BROWSER_MUSIC_LIST_MODEL_H

#include "MediaListModel.h"
#include "TaskGroup.h"

// a subclass of the media list model for music.

class MusicListModel : public MediaListModel
{
    Q_OBJECT

public:
    MusicListModel(QObject *parent = 0);
    ~MusicListModel();

    // from MediaListModel
    virtual int dataColumnCount() const;
    virtual QVariant displayFromMediaFile(MediaFilePtr media_file, int column) const;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    virtual QVariant sortValueFromMediaFile(MediaFilePtr media_file, int column) const;
    virtual void resetTasks(const QModelIndexList &index_list);

private:
    MediaLoaderPtr m_media_loader;
    MediaBrowserPrivate::TaskGroupPtr m_task_group;
};

#endif // MEDIA_BROWSER_MUSIC_LIST_MODEL_H
