#ifndef MEDIA_BROWSER_LIBRARY_TREE_ITEM_H
#define MEDIA_BROWSER_LIBRARY_TREE_ITEM_H

#include <boost/smart_ptr.hpp>
#include <QString>
#include <QVariant>
#include <QVector>

// forward definitions

typedef boost::shared_ptr<class MediaFile> MediaFilePtr;
typedef QVector<MediaFilePtr> MediaFileArray;

typedef boost::shared_ptr<class LibraryTreeItem> LibraryTreeItemPtr;
typedef QVector<LibraryTreeItemPtr> LibraryTreeItemArray;

typedef boost::shared_ptr<class LibraryTreeController> LibraryTreeControllerPtr;

typedef boost::shared_ptr<class LibraryTreeItemPromise> LibraryTreeItemPromisePtr;

class MediaListModel;

// constant definitions

const unsigned INDEX_NOT_FOUND = 0x7FFFFFFF;

// the library tree item is a node in the library tree. it contains media files and other
// library tree items (children). it provides methods to add children, add media items, examine
// children and media items, and fulfill promises.

class LibraryTreeItem : public boost::enable_shared_from_this<LibraryTreeItem>
{
public:
    LibraryTreeItem(const QString &title, LibraryTreeItemPtr parent = LibraryTreeItemPtr(), LibraryTreeItemPromisePtr promise = LibraryTreeItemPromisePtr());

    void appendChild(LibraryTreeItemPtr child);

    LibraryTreeItemPtr child(unsigned row);
    unsigned childCount() const;
    unsigned columnCount() const;
    QVariant data(unsigned column) const;
    unsigned row() const;
    LibraryTreeItemPtr parent();

    void addMediaFile(MediaFilePtr media_file);
    MediaFileArray mediaFiles() const { return m_media_files; }
    unsigned mediaFileCount() const { return m_media_files.size(); }
    MediaFilePtr mediaFileAt(unsigned index) const { return m_media_files[index]; }

    // return INDEX_NOT_FOUND if media file is not found
    unsigned indexOfMediaFile(MediaFilePtr media_file) const;

    // if the item isn't complete, this will populate it using the promise.
    void populate(LibraryTreeControllerPtr library_tree_controller);
    void executePromise(LibraryTreeControllerPtr library_tree_controller);

    // set this to get notified when the promise gets executed
    void setWatcher(MediaListModel *media_list_model);

private:
    LibraryTreeItemArray m_children;
    QString m_title;
    LibraryTreeItemPtr m_parent;
    MediaFileArray m_media_files;
    LibraryTreeItemPromisePtr m_promise;
    MediaListModel *m_media_list_model_watcher;
};

// the library tree item promise is an abstract class that allows parsers to promise to
// fill out a sub-folder when then user opens it in the UI. subclasses need to keep track
// of the object that they need to complete. promises only get fulfilled on the live tree,
// never the display tree. the information gets updated to the display tree at sync time,
// as usual.

class LibraryTreeItemPromise
{
public:
    virtual void populate(LibraryTreeControllerPtr library_tree_controller) = 0;
};

#endif // MEDIA_BROWSER_LIBRARY_TREE_ITEM_H
