#ifndef MEDIA_BROWSER_LIBRARY_TREE_CONTROLLER_H
#define MEDIA_BROWSER_LIBRARY_TREE_CONTROLLER_H

#include <boost/smart_ptr.hpp>
#include <QModelIndex>
#include <QMutex>

class LibraryTreeItem;
typedef boost::shared_ptr<LibraryTreeItem> LibraryTreeItemPtr;

class MediaFile;
typedef boost::shared_ptr<MediaFile> MediaFilePtr;

typedef boost::shared_ptr<class LibraryTreeItemPromise> LibraryTreeItemPromisePtr;

class LibraryTreeModel;

// the library tree item index represents an index into the tree at the time
// the action is added to the tree.
class LibraryTreeItemIndex
{
public:
    LibraryTreeItemIndex(LibraryTreeItemPtr library_tree_item);
    LibraryTreeItemPtr resolve(LibraryTreeItemPtr library_tree_root_item);
    QModelIndex resolve(LibraryTreeModel *model);
private:
    std::list<unsigned> m_index_list;
    void buildIndex(LibraryTreeItemPtr library_tree_item);
};

// the library tree action represents a change to the live tree. the live tree
// can be synced to the display tree by replaying the actions. replaying the actions
// is a fast operation and can be done on the user interface thread at which time
// the list of actions representing the difference between the live and display tree
// will be cleared.
class LibraryTreeAction
{
public:
    LibraryTreeAction();
    virtual ~LibraryTreeAction();
    virtual void execute(LibraryTreeItemPtr library_tree_root_item, LibraryTreeModel *model) = 0;
};

typedef boost::shared_ptr<LibraryTreeAction> LibraryTreeActionPtr;
typedef std::list<LibraryTreeActionPtr> LibraryTreeActionList;

// append a child directory
class LibraryTreeAppendChildAction : public LibraryTreeAction
{
public:
    LibraryTreeAppendChildAction(LibraryTreeItemIndex library_tree_index, const QString &title, LibraryTreeItemPromisePtr promise) : m_library_tree_index(library_tree_index), m_title(title), m_promise(promise) { }
    virtual void execute(LibraryTreeItemPtr library_tree_root_item, LibraryTreeModel *model);
private:
    LibraryTreeItemIndex m_library_tree_index;
    LibraryTreeItemPromisePtr m_promise;
    QString m_title;
};

// append a media file
class LibraryTreeAppendMediaFile : public LibraryTreeAction
{
public:
    LibraryTreeAppendMediaFile(LibraryTreeItemIndex library_tree_index, MediaFilePtr media_file) : m_library_tree_index(library_tree_index), m_media_file(media_file) { }
    virtual void execute(LibraryTreeItemPtr library_tree_root_item, LibraryTreeModel *model);
private:
    LibraryTreeItemIndex m_library_tree_index;
    MediaFilePtr m_media_file;
};

// all changes to the library tree should go through this controller
// changes are made immediately to the live tree. these changes can be
// made at any time from any thread. call sync to synchronize from the
// live tree to the display tree.
class LibraryTreeController : public boost::enable_shared_from_this<LibraryTreeController>
{
public:
    LibraryTreeController(LibraryTreeModel *library_tree_model);

    LibraryTreeItemPtr appendChild(LibraryTreeItemPtr parent, const QString &title, LibraryTreeItemPromisePtr promise = LibraryTreeItemPromisePtr());
    void appendFile(LibraryTreeItemPtr library_tree_item, const QString &file_path);
    void appendMediaFile(LibraryTreeItemPtr library_tree_item, MediaFilePtr media_file);

    LibraryTreeItemPtr liveRootLibraryTreeItem() const { return m_live_root_item; }
    LibraryTreeItemPtr displayRootLibraryTreeItem() const { return m_display_root_item; }

    void sync();

    void populateItem(LibraryTreeItemPtr item);

private:
    QMutex m_live_root_item_mutex;
    LibraryTreeItemPtr m_live_root_item;

    LibraryTreeItemPtr m_display_root_item;

    QMutex m_library_tree_action_list_mutex;
    LibraryTreeActionList m_library_tree_action_list;

    // the owner
    LibraryTreeModel *m_library_tree_model;
};

typedef boost::shared_ptr<LibraryTreeController> LibraryTreeControllerPtr;

#endif // MEDIA_BROWSER_LIBRARY_TREE_CONTROLLER_H
