// Copyright 2011. LQ Graphics, Inc. and Chris Meyer. LGPL License. See LICENSE file.

#include "LibraryTreeModel.h"
#include "LibraryTreeController.h"
#include "LibraryTreeItem.h"
#include "MediaFile.h"

#include <QMimeData>
#include <QUrl>

LibraryTreeModel::LibraryTreeModel(QObject *parent)
        : QAbstractItemModel(parent)
{
    LibraryTreeControllerPtr library_tree_controller(new LibraryTreeController(this));
    m_library_tree_controller = library_tree_controller;
}

QModelIndex LibraryTreeModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    LibraryTreeItemPtr parent_item;

    if (!parent.isValid())
        parent_item = libraryTreeController()->displayRootLibraryTreeItem();
    else
        parent_item = static_cast<LibraryTreeItem *>(parent.internalPointer())->shared_from_this();

    LibraryTreeItemPtr child_item = parent_item->child(row);
    if (child_item != NULL)
        return createIndex(row, column, child_item.get());
    else
        return QModelIndex();
}

QModelIndex LibraryTreeModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    LibraryTreeItemPtr child_item = static_cast<LibraryTreeItem *>(index.internalPointer())->shared_from_this();
    LibraryTreeItemPtr parent_item = child_item->parent();

    if (parent_item == libraryTreeController()->displayRootLibraryTreeItem())
        return QModelIndex();

    return createIndex(parent_item->row(), 0, parent_item.get());
}

int LibraryTreeModel::rowCount(const QModelIndex &parent) const
{
    if (parent.column() > 0)
        return 0;

    LibraryTreeItemPtr parent_item;
    if (!parent.isValid())
        parent_item = libraryTreeController()->displayRootLibraryTreeItem();
    else
        parent_item = static_cast<LibraryTreeItem *>(parent.internalPointer())->shared_from_this();

    return parent_item->childCount();
}

int LibraryTreeModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return static_cast<LibraryTreeItem *>(parent.internalPointer())->shared_from_this()->columnCount();
    else
        return libraryTreeController()->displayRootLibraryTreeItem()->columnCount();
}

QVariant LibraryTreeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role != Qt::DisplayRole)
        return QVariant();

    LibraryTreeItemPtr item = static_cast<LibraryTreeItem *>(index.internalPointer())->shared_from_this();

    return item->data(index.column());
}

Qt::ItemFlags LibraryTreeModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;

    Qt::ItemFlags flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    
    LibraryTreeItemPtr child_item = static_cast<LibraryTreeItem *>(index.internalPointer())->shared_from_this();
    LibraryTreeItemPtr parent_item = child_item->parent();
    
    if (parent_item != libraryTreeController()->displayRootLibraryTreeItem())
        flags |= Qt::ItemIsDragEnabled;
    
    return flags;
}

QStringList LibraryTreeModel::mimeTypes() const
{
    QStringList mime_types;
    mime_types << "text/uri-list";
    return mime_types;
}

QMimeData *LibraryTreeModel::mimeData(const QModelIndexList &indexes) const
{
    QMimeData *mimeData = new QMimeData();
    
    QList<QUrl> urls;
    QString sources;

    Q_FOREACH(QModelIndex index, indexes)
    {
        LibraryTreeItemPtr item = static_cast<LibraryTreeItem *>(index.internalPointer())->shared_from_this();

        MediaFileArray media_files = item->mediaFiles();
        
        Q_FOREACH(MediaFilePtr media_file, media_files)
        {
            urls.append(QUrl::fromLocalFile(media_file->resolvedFilePath()));
            if (!sources.contains(media_file->source()))
                sources.append(QLatin1String(" ") + media_file->source());
        }
    }
    
    mimeData->setUrls(urls);
    mimeData->setData(QLatin1String("mac-extra/source-list"), sources.simplified().toUtf8());

    return mimeData;
}

void LibraryTreeModel::populate(const QModelIndex &index)
{
    LibraryTreeItemPtr item = static_cast<LibraryTreeItem *>(index.internalPointer())->shared_from_this();

    libraryTreeController()->populateItem(item);
}
