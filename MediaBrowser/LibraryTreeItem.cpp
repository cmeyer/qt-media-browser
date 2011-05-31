
#include "LibraryTreeItem.h"
#include "MediaListModel.h" // ugh

LibraryTreeItem::LibraryTreeItem(const QString &title, LibraryTreeItemPtr parent, LibraryTreeItemPromisePtr promise)
    : m_title(title)
    , m_parent(parent)
    , m_promise(promise)
    , m_media_list_model_watcher(NULL)
{
}

void LibraryTreeItem::appendChild(LibraryTreeItemPtr item)
{
    m_children.push_back(item);
}

LibraryTreeItemPtr LibraryTreeItem::child(unsigned row)
{
    return m_children[row];
}

unsigned LibraryTreeItem::childCount() const
{
    return m_children.size();
}

unsigned LibraryTreeItem::columnCount() const
{
    return 1;
}

QVariant LibraryTreeItem::data(unsigned column) const
{
    return m_title;
}

LibraryTreeItemPtr LibraryTreeItem::parent()
{
    return m_parent;
}

unsigned LibraryTreeItem::row() const
{
    if (m_parent)
    {
        LibraryTreeItemArray::const_iterator iter = std::find(m_parent->m_children.begin(), m_parent->m_children.end(), shared_from_this());
        if (iter != m_parent->m_children.end())
            return iter - m_parent->m_children.begin();
    }

    //assert(false);

    return 0;
}

void LibraryTreeItem::addMediaFile(MediaFilePtr media_file)
{
    m_media_files.push_back(media_file);
    if (m_media_list_model_watcher != NULL)
        m_media_list_model_watcher->libraryTreeItemChanged();
}

unsigned LibraryTreeItem::indexOfMediaFile(MediaFilePtr media_file) const
{
    MediaFileArray::const_iterator iter = std::find(m_media_files.begin(), m_media_files.end(), media_file);
    if (iter != m_media_files.end())
        return iter - m_media_files.begin();
    return INDEX_NOT_FOUND;
}

void LibraryTreeItem::populate(LibraryTreeControllerPtr library_tree_controller)
{
    Q_FOREACH(LibraryTreeItemPtr child, m_children)
    {
        child->executePromise(library_tree_controller);
    }
}

void LibraryTreeItem::executePromise(LibraryTreeControllerPtr library_tree_controller)
{
    if (m_promise != NULL)
        m_promise->populate(library_tree_controller);
    m_promise.reset();
}

void LibraryTreeItem::setWatcher(MediaListModel *media_list_model)
{
    m_media_list_model_watcher = media_list_model;
}
