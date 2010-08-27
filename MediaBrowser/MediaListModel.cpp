#include <QDebug>
#include <QFileInfo>
#include <QIcon>
#include <QPixmap>
#include <QUrl>
#include <QMimeData>

#include "MediaListModel.h"
#include "LibraryTreeItem.h"
#include "MediaLoader.h"
#include "MediaFile.h"

MediaListModel::MediaListModel(QObject *parent)
    : QAbstractListModel(parent)
{
    qRegisterMetaType<MediaFilePtr>("MediaFilePtr");
}

MediaListModel::~MediaListModel()
{
    if (m_library_tree_item != NULL)
        m_library_tree_item->setWatcher(NULL);
}

int MediaListModel::rowCount(const QModelIndex &parent) const
{
    if (parent.column() > 0)
        return 0;

    if (m_library_tree_item == NULL)
        return 0;

    return m_library_tree_item->mediaFileCount();
}

int MediaListModel::dataColumnCount() const
{
    return 1;
}

int MediaListModel::columnCount(const QModelIndex &parent) const
{
    if (parent.column() > 0)
        return 0;

    return dataColumnCount();
}

QVariant MediaListModel::decorationFromMediaFile(MediaFilePtr media_file, int column) const
{
    return QVariant();
}

QVariant MediaListModel::displayFromMediaFile(MediaFilePtr media_file, int column) const
{
    return QVariant();
}

QVariant MediaListModel::toolTipFromMediaFile(MediaFilePtr media_file, int column) const
{
    return displayFromMediaFile(media_file, column);
}

QVariant MediaListModel::sortValueFromMediaFile(MediaFilePtr media_file, int column) const
{
    return displayFromMediaFile(media_file, column);
}

QVariant MediaListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    MediaFilePtr media_file = m_library_tree_item->mediaFileAt(index.row());

    if (role == Qt::DecorationRole)
    {
        return decorationFromMediaFile(media_file, index.column());
    }

    if (role == Qt::DisplayRole)
    {
        return displayFromMediaFile(media_file, index.column());
    }

    if (role == Qt::ToolTipRole)
    {
        return toolTipFromMediaFile(media_file, index.column());
    }

    if (role == FullPathRole)
    {
        return media_file->filePath();
    }

    if (role == SortRole)
    {
        return sortValueFromMediaFile(media_file, index.column());
    }

    return QVariant();
}

void MediaListModel::setLibraryTreeItem(LibraryTreeItemPtr library_tree_item)
{
    if (m_library_tree_item != NULL)
        m_library_tree_item->setWatcher(NULL);

    if (library_tree_item != NULL)
        library_tree_item->setWatcher(this);

    if (rowCount() > 0)
    {
        beginRemoveRows(QModelIndex(), 0, rowCount()-1);

        m_library_tree_item.reset();

        endRemoveRows();
    }

    if (library_tree_item->mediaFileCount() > 0)
    {
        beginInsertRows(QModelIndex(), 0, library_tree_item->mediaFileCount()-1);

        m_library_tree_item = library_tree_item;

        endInsertRows();
    }
}

void MediaListModel::libraryTreeItemChanged()
{
    setLibraryTreeItem(m_library_tree_item);
}

void MediaListModel::mediaLoaded(MediaFilePtr media_file)
{
    unsigned row = m_library_tree_item->indexOfMediaFile(media_file);

    if ( row != INDEX_NOT_FOUND )
    {
        QModelIndex model_index_left = index(row, 0);
        QModelIndex model_index_right = index(row, columnCount()-1);

        dataChanged(model_index_left, model_index_right);
    }
}

Qt::ItemFlags MediaListModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags defaultFlags = QAbstractListModel::flags(index);

    if (index.isValid())
        return Qt::ItemIsDragEnabled | defaultFlags;
    else
        return defaultFlags;
}

QStringList MediaListModel::mimeTypes() const
{
    QStringList mime_types;
    mime_types << "text/uri-list";
    return mime_types;
}

QMimeData *MediaListModel::mimeData(const QModelIndexList &indexes) const
{
    QMimeData *mimeData = new QMimeData();
    
    QList<QUrl> urls;
    
    Q_FOREACH(QModelIndex index, indexes) {
        if (index.isValid() && index.column() == 0) {
            QString file_path = qVariantValue<QString>(data(index, FullPathRole));
            urls.push_back(QUrl::fromLocalFile(file_path));
        }
    }

    mimeData->setUrls(urls);

    return mimeData;
}

void MediaListModel::resetTasks(const QModelIndexList &index_list)
{
}
