#ifndef MEDIA_BROWSER_LIBRARY_TREE_MODEL_H
#define MEDIA_BROWSER_LIBRARY_TREE_MODEL_H

#include <boost/smart_ptr.hpp>
#include <QAbstractItemModel>

class LibraryTreeController;
typedef boost::shared_ptr<LibraryTreeController> LibraryTreeControllerPtr;

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
