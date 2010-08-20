#ifndef MEDIA_BROWSER_LIBRARY_TREE_ITEM_H
#define MEDIA_BROWSER_LIBRARY_TREE_ITEM_H

#include <vector>
#include <boost/smart_ptr.hpp>
#include <QString>
#include <QVariant>

typedef boost::shared_ptr<class MediaFile> MediaFilePtr;
typedef std::vector<MediaFilePtr> MediaFileArray;

typedef boost::shared_ptr<class LibraryTreeItem> LibraryTreeItemPtr;
typedef std::vector<LibraryTreeItemPtr> LibraryTreeItemArray;

typedef boost::shared_ptr<class LibraryTreeController> LibraryTreeControllerPtr;

const unsigned INDEX_NOT_FOUND = 0x7FFFFFFF;

typedef boost::shared_ptr<class LibraryTreeItemPromise> LibraryTreeItemPromisePtr;

class MediaListModel;

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

class LibraryTreeItemPromise
{
public:
    virtual void populate(LibraryTreeControllerPtr library_tree_controller) = 0;
};

#endif // MEDIA_BROWSER_LIBRARY_TREE_ITEM_H
