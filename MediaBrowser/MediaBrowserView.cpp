#include <boost/foreach.hpp>
#include <QDebug>
#include <QDirModel>
#include <QItemSelection>
#include <QMutexLocker>
#include <QSplitter>
#include <QTimer>
#include <QTreeView>
#include <QVBoxLayout>

#include "MediaBrowserView.h"
#include "LibraryTreeController.h"
#include "LibraryTreeModel.h"
#include "LibraryTreeItem.h"
#include "PhotoListModel.h"
#include "MediaParser.h"

MediaBrowserView::MediaBrowserView(QWidget *parent)
        : QWidget(parent)
        , m_splitter(NULL)
        , m_library_tree_model(NULL)
        , m_media_list_model(NULL)
        , m_sync_timer(NULL)
        , m_initialized_media_parsers(false)
{
    setContentsMargins(0, 0, 0, 0);

    // this is the model that is displayed in the top part of the media browser view
    m_library_tree_model = new LibraryTreeModel();

    // the tree model has a controller. threads can add nodes to the controller any time they want;
    // but the controller keeps track of what the threads have done and only synchronizes to the actual
    // model at sync() time.
    m_library_tree_controller = m_library_tree_model->libraryTreeController();

    QTreeView *tree = new QTreeView();
    tree->setHeaderHidden(true);
    tree->setDragDropMode(QTreeView::DragOnly);
    tree->setDragEnabled(true);
    tree->setModel(m_library_tree_model);

    m_layout = new QVBoxLayout(this);
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->setSpacing(0);

    m_splitter = new QSplitter(this);
    m_splitter->setOrientation(Qt::Vertical);
    m_splitter->addWidget(tree);
    m_splitter->setCollapsible(0, false);

    m_layout->addWidget(m_splitter);

    m_sync_timer = new QTimer(this);
    connect(m_sync_timer, SIGNAL(timeout()), this, SLOT(sync()));
    m_sync_timer->start(500);

    connect(tree->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), this, SLOT(selectionChanged(QItemSelection,QItemSelection)));
    connect(tree, SIGNAL(expanded(QModelIndex)), this, SLOT(expanded(QModelIndex)));
}

MediaBrowserView::~MediaBrowserView()
{
    delete m_media_list_model;

    Q_FOREACH(MediaParserPtr media_parser, m_media_parsers)
    {
        media_parser->cancel();
    }

    m_sync_timer->stop();
}

void MediaBrowserView::addMediaParser(MediaParserPtr media_parser)
{
    m_media_parsers.push_back(media_parser);
}

void MediaBrowserView::setTreeContentView(QWidget *content_view)
{
    m_splitter->addWidget(content_view);
    m_splitter->setCollapsible(1, false);
}

void MediaBrowserView::sync()
{
    m_library_tree_controller->sync();
}

void MediaBrowserView::selectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
    QModelIndexList model_index_list = selected.indexes();
    if (model_index_list.count() > 0)
    {
        QModelIndex first_index = model_index_list.at(0);
        LibraryTreeItemPtr library_tree_item = static_cast<LibraryTreeItem *>(first_index.internalPointer())->shared_from_this();
        //ASSERT_EXCEPTION(mediaListModel() != NULL);
        if (mediaListModel() != NULL)
            mediaListModel()->setLibraryTreeItem(library_tree_item);
    }
}

void MediaBrowserView::showEvent(QShowEvent *event)
{
    if (!m_initialized_media_parsers)
    {
        addMediaParsers();
        m_initialized_media_parsers = true;
    }
}

void MediaBrowserView::expanded(const QModelIndex &index)
{
    // when the user expands an item, give the parser a chance to add more items
    m_library_tree_model->populate(index);
}
