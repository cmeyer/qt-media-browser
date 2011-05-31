#ifndef MEDIA_BROWSER_LIBRARY_TREE_MODEL_H
#define MEDIA_BROWSER_LIBRARY_TREE_MODEL_H

#include <boost/smart_ptr.hpp>
#include <QAbstractItemModel>

typedef boost::shared_ptr<class LibraryTreeController> LibraryTreeControllerPtr;

// the library tree model is the Qt model used to display the hierarchy of libraries
// and folders within those libraries. it is connected to a QTreeView in the media
// browser view. it gets its data from a the library tree controller's display root
// library tree item.

class LibraryTreeModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    LibraryTreeModel(QObject *parent = 0);

    // from QAbstractItemModel
    virtual QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
    virtual QModelIndex parent(const QModelIndex &child) const;
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    virtual Qt::ItemFlags flags(const QModelIndex &index) const;
    virtual QStringList mimeTypes() const;
    virtual QMimeData *mimeData(const QModelIndexList &indexes) const;

    LibraryTreeControllerPtr libraryTreeController() const { return m_library_tree_controller; }

    // invoked when the user opens a node
    void populate(const QModelIndex &index);

private:
    LibraryTreeControllerPtr m_library_tree_controller;

    friend class LibraryTreeAction;
    friend class LibraryTreeAppendChildAction;
};

#endif // MEDIA_BROWSER_LIBRARY_TREE_MODEL_H
