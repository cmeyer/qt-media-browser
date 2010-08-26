#ifndef MEDIA_BROWSER_PHOTO_LIST_MODEL_H
#define MEDIA_BROWSER_PHOTO_LIST_MODEL_H

#include <QFontMetrics>

#include "MediaListModel.h"
#include "TaskGroup.h"

class PhotoListModel : public MediaListModel
{
    Q_OBJECT

public:
    PhotoListModel(QObject *parent = 0);
    ~PhotoListModel();

    // from MediaListModel
    virtual QVariant decorationFromMediaFile(MediaFilePtr media_file, int column) const;
    virtual QVariant displayFromMediaFile(MediaFilePtr media_file, int column) const;
    virtual void resetTasks(const QModelIndexList &index_list);

    void setFontMetrics(const QFontMetrics &font_metrics);
    void setIconSize(const QSize &icon_size);

private:
    MediaLoaderPtr m_media_loader;
    MediaBrowserPrivate::TaskGroupPtr m_task_group;
    QFontMetrics m_font_metrics;
    QSize m_icon_size;
};

#endif // MEDIA_BROWSER_PHOTO_LIST_MODEL_H
