#ifndef MEDIA_BROWSER_LIBRARY_TREE_CONTROLLER_H
#define MEDIA_BROWSER_LIBRARY_TREE_CONTROLLER_H

#include <boost/smart_ptr.hpp>
#include <QModelIndex>
#include <QMutex>

typedef boost::shared_ptr<class LibraryTreeItem> LibraryTreeItemPtr;

typedef boost::shared_ptr<class MediaFile> MediaFilePtr;

typedef boost::shared_ptr<class LibraryTreeItemPromise> LibraryTreeItemPromisePtr;

class LibraryTreeModel;

// the library tree item index represents an index into the tree at the time
// the action is added to the tree. it is used while synchronizing from the live
// tree to the display tree and even then it is only valid if used in the same
// sequence in the display tree as in the live tree.

class LibraryTreeItemIndex
{
public:
    LibraryTreeItemIndex(LibraryTreeItemPtr library_tree_item);
    LibraryTreeItemPtr resolve(LibraryTreeItemPtr library_tree_root_item);
    QModelIndex resolve(LibraryTreeModel *model);
private:
    QList<unsigned> m_index_list;
    void buildIndex(LibraryTreeItemPtr library_tree_item);
};

// the library tree action represents a change to the live tree. the live tree
// can be synced to the display tree by replaying actions. replaying the actions
// is fast and can be done on the user interface thread after which the list of
// actions representing the difference between the live and display tree will be cleared.

typedef boost::shared_ptr<class LibraryTreeAction> LibraryTreeActionPtr;
typedef QList<LibraryTreeActionPtr> LibraryTreeActionList;

class LibraryTreeAction
{
public:
    LibraryTreeAction();
    virtual ~LibraryTreeAction();
    virtual void execute(LibraryTreeItemPtr library_tree_root_item, LibraryTreeModel *model) = 0;
};

// all changes that parsers make to the library tree should go through this controller.
// changes are made immediately to the live tree and are thread safe. the sync method will
// synchronize from the live tree to the display tree. it is a fast operation and needs to
// be done from the user interface thread.

typedef boost::shared_ptr<class LibraryTreeController> LibraryTreeControllerPtr;

class LibraryTreeController : public boost::enable_shared_from_this<LibraryTreeController>
{
public:
    LibraryTreeController(LibraryTreeModel *library_tree_model);

    // thread safe calls to invoke from parsers

    // record the "append child folder" action. callers can optionally pass a promise which will be used to fill out the
    // children of the folder in the case the user opens the folder.
    LibraryTreeItemPtr appendChild(LibraryTreeItemPtr parent, const QString &title, LibraryTreeItemPromisePtr promise = LibraryTreeItemPromisePtr());

    // record the "append media file" action.
    void appendMediaFile(LibraryTreeItemPtr library_tree_item, MediaFilePtr media_file);

    // convenience function which creates a media file object from the file path and source and calls append media file
    void appendFile(LibraryTreeItemPtr library_tree_item, const QString &file_path, const QString &source);

    // accessors

    LibraryTreeItemPtr liveRootLibraryTreeItem() const { return m_live_root_item; }
    LibraryTreeItemPtr displayRootLibraryTreeItem() const { return m_display_root_item; }

    // this call must be made from the main user interface thread. it synchronizes the live tree (which might be
    // changing very actively from threads) with the display tree (which is guaranteed to only change on the main thread).
    void sync();

    // parsers can partially populate the tree. if they do this, they must provide
    // a promise to populate further if the display requires it. this method will be invoked
    // to fulfill that promise. it calls populate on the item with this objct as a parameter.
    void populateItem(LibraryTreeItemPtr item);

private:
    QMutex m_live_root_item_mutex;
    LibraryTreeItemPtr m_live_root_item;

    LibraryTreeItemPtr m_display_root_item;

    QMutex m_library_tree_action_list_mutex;
    LibraryTreeActionList m_library_tree_action_list;

    // the owner. used while syncing.
    LibraryTreeModel *m_library_tree_model;
};

#endif // MEDIA_BROWSER_LIBRARY_TREE_CONTROLLER_H
