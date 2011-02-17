#include <QDebug>
#include <QFileInfo>
#include <QIcon>
#include <QMimeData>
#include <QPixmap>
#include <QUrl>

#include "PhotoListModel.h"
#include "LibraryTreeItem.h"
#include "MediaLoader.h"
#include "MediaFile.h"
#include "TaskGroup.h"

PhotoListModel::PhotoListModel(QObject *parent)
    : MediaListModel(parent)
    , m_task_group(new MediaBrowserPrivate::TaskGroup(MediaFileTaskPool::instance()))
    , m_font_metrics(QFont())
{
    MediaLoaderPtr media_loader(new MediaLoader());
    m_media_loader = media_loader;

    // connect the media loader signal mediaLoaded to the method in this class.
    // this allows this class to redisplay the icon when it becomes available.
    connect(m_media_loader.get(), SIGNAL(mediaLoaded(MediaFilePtr)), this, SLOT(mediaLoaded(MediaFilePtr)));
}

PhotoListModel::~PhotoListModel()
{
    disconnect(m_media_loader.get(), SIGNAL(mediaLoaded(MediaFilePtr)), this, SLOT(mediaLoaded(MediaFilePtr)));
}

QVariant PhotoListModel::decorationFromMediaFile(MediaFilePtr media_file, int column) const
{
    // asking the media loader for an icon may return a placeholder.
    // if the placeholder is returned, the media loader will signal
    // mediaLoaded when the icon becomes available. this class needs
    // to be prepared to handle those signals.

    return m_media_loader->mediaFileIcon(m_icon_size, media_file, m_task_group);
}

QVariant PhotoListModel::toolTipFromMediaFile(MediaFilePtr media_file, int column) const
{
    QFileInfo file_info(media_file->filePath());

    return file_info.fileName();
}

QVariant PhotoListModel::displayFromMediaFile(MediaFilePtr media_file, int column) const
{
    return m_font_metrics.elidedText(toolTipFromMediaFile(media_file, column).toString(), Qt::ElideMiddle, m_icon_size.width());
}

void PhotoListModel::resetTasks(const QModelIndexList &index_list)
{
    // first remove everything from the queue
    m_task_group->cancelAllTasks();

    // now prioritize the display items first
    Q_FOREACH(const QModelIndex &index, index_list)
    {
        data(index, Qt::DecorationRole);
    }
}

void PhotoListModel::setFontMetrics(const QFontMetrics &font_metrics)
{
    m_font_metrics = font_metrics;
}

void PhotoListModel::setIconSize(const QSize &icon_size)
{
    m_icon_size = icon_size;
}

QSize PhotoListModel::cellSize() const
{
    return m_icon_size + QSize(2, m_font_metrics.height() + 3);
}
