#include <boost/foreach.hpp>
#include "LibraryTreeController.h"
#include "LibraryTreeItem.h"
#include "LibraryTreeModel.h"
#include "MediaFile.h"

LibraryTreeItemIndex::LibraryTreeItemIndex(LibraryTreeItemPtr library_tree_item)
{
    buildIndex(library_tree_item);
}

LibraryTreeItemPtr LibraryTreeItemIndex::resolve(LibraryTreeItemPtr library_tree_root_item)
{
    LibraryTreeItemPtr library_tree_item = library_tree_root_item;
    BOOST_FOREACH(unsigned index, m_index_list)
    {
        library_tree_item = library_tree_item->child(index);
    }
    return library_tree_item;
}

QModelIndex LibraryTreeItemIndex::resolve(LibraryTreeModel *model)
{
    QModelIndex model_index;
    BOOST_FOREACH(unsigned index, m_index_list)
    {
        model_index = model->index(index, 0, model_index);
    }
    return model_index;
}

void LibraryTreeItemIndex::buildIndex(LibraryTreeItemPtr library_tree_item)
{
    LibraryTreeItemPtr parent = library_tree_item->parent();
    if (parent != NULL)
    {
        m_index_list.push_front(library_tree_item->row());
        buildIndex(parent);
    }
}

LibraryTreeAction::LibraryTreeAction()
{
}

LibraryTreeAction::~LibraryTreeAction()
{
}

void LibraryTreeAppendChildAction::execute(LibraryTreeItemPtr library_tree_root_item, LibraryTreeModel *model)
{
    LibraryTreeItemPtr parent = m_library_tree_index.resolve(library_tree_root_item);
    QModelIndex parent_model_index = m_library_tree_index.resolve(model);
    LibraryTreeItemPtr item(new LibraryTreeItem(m_title, parent, m_promise));
    int index = parent->childCount();
    model->beginInsertRows(parent_model_index, index, index);
    parent->appendChild(item);
    model->endInsertRows();
}

void LibraryTreeAppendMediaFile::execute(LibraryTreeItemPtr library_tree_root_item, LibraryTreeModel *model)
{
    LibraryTreeItemPtr library_tree_item = m_library_tree_index.resolve(library_tree_root_item);
    library_tree_item->addMediaFile(m_media_file);
}

LibraryTreeController::LibraryTreeController(LibraryTreeModel *library_tree_model)
    : m_library_tree_model(library_tree_model)
{
    LibraryTreeItemPtr live_root_item(new LibraryTreeItem("live"));
    m_live_root_item = live_root_item;

    LibraryTreeItemPtr display_root_item(new LibraryTreeItem("display"));
    m_display_root_item = display_root_item;
}

LibraryTreeItemPtr LibraryTreeController::appendChild(LibraryTreeItemPtr parent, const QString &title, LibraryTreeItemPromisePtr promise)
{
    // mutex must be locked while modifying the tree
    QMutexLocker locker(&m_library_tree_action_list_mutex);

    // record the action
    LibraryTreeItemIndex library_tree_index(parent);
    LibraryTreeActionPtr library_tree_action(new LibraryTreeAppendChildAction(library_tree_index, title, promise));
    m_library_tree_action_list.push_back(library_tree_action);

    // execute the action on the live tree
    LibraryTreeItemPtr item(new LibraryTreeItem(title, parent));
    parent->appendChild(item);

    return item;
}

void LibraryTreeController::appendFile(LibraryTreeItemPtr library_tree_item, const QString &file_path)
{
    MediaFilePtr media_file(new MediaFile(file_path));

    appendMediaFile(library_tree_item, media_file);
}

void LibraryTreeController::appendMediaFile(LibraryTreeItemPtr library_tree_item, MediaFilePtr media_file)
{
    // mutex must be locked while modifying the tree
    QMutexLocker locker(&m_library_tree_action_list_mutex);

    // record the action
    LibraryTreeItemIndex library_tree_index(library_tree_item);
    LibraryTreeActionPtr library_tree_action(new LibraryTreeAppendMediaFile(library_tree_index, media_file));
    m_library_tree_action_list.push_back(library_tree_action);

    // execute the action on the live tree
    library_tree_item->addMediaFile(media_file);
}

void LibraryTreeController::sync()
{
    QMutexLocker locker(&m_library_tree_action_list_mutex);

    BOOST_FOREACH(LibraryTreeActionPtr library_tree_action, m_library_tree_action_list)
    {
        library_tree_action->execute(displayRootLibraryTreeItem(), m_library_tree_model);
    }

    m_library_tree_action_list.clear();
}

void LibraryTreeController::populateItem(LibraryTreeItemPtr item)
{
    item->populate(shared_from_this());
}
