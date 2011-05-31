// Copyright 2011. LQ Graphics, Inc. and Chris Meyer. LGPL License. See LICENSE file.

#ifndef MEDIA_BROWSER_MEDIA_LIST_MODEL_H
#define MEDIA_BROWSER_MEDIA_LIST_MODEL_H

#include <boost/smart_ptr.hpp>
#include <QAbstractItemModel>

typedef boost::shared_ptr<class LibraryTreeItem> LibraryTreeItemPtr;

typedef boost::shared_ptr<class MediaFile> MediaFilePtr;

typedef boost::shared_ptr<class MediaLoader> MediaLoaderPtr;

// the media list model is the base class for more specific media models such
// as the music list model or the photo list model. it handles basic abstract
// list model tasks such as drag and drop.

class MediaListModel : public QAbstractListModel
{
    Q_OBJECT

public:
    MediaListModel(QObject *parent = 0);
    ~MediaListModel();

    enum { FullPathRole = Qt::UserRole, SortRole = Qt::UserRole + 1 };

    void setLibraryTreeItem(LibraryTreeItemPtr library_tree_item);

    // abstract methods
    virtual int dataColumnCount() const;
    virtual QVariant decorationFromMediaFile(MediaFilePtr media_file, int column) const;
    virtual QVariant displayFromMediaFile(MediaFilePtr media_file, int column) const;
    virtual QVariant toolTipFromMediaFile(MediaFilePtr media_file, int column) const;
    virtual QVariant sortValueFromMediaFile(MediaFilePtr media_file, int column) const;
    virtual void resetTasks(const QModelIndexList &index_list);

    // from QAbstractListModel
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    virtual QStringList mimeTypes() const;
    virtual QMimeData *mimeData(const QModelIndexList &indexes) const;
    virtual Qt::ItemFlags flags(const QModelIndex &index) const;

    // used with the watcher
    void libraryTreeItemChanged();

protected Q_SLOTS:
    void mediaLoaded(MediaFilePtr media_file);

private:
    LibraryTreeItemPtr m_library_tree_item;
};

#endif // MEDIA_BROWSER_MEDIA_LIST_MODEL_H
