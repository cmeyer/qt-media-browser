// Copyright 2011. LQ Graphics, Inc. and Chris Meyer. LGPL License. See LICENSE file.

#include <QDebug>
#include <QDesktopServices>
#include <QDrag>
#include <QHBoxLayout>
#include <QListView>
#include <QMouseEvent>
#include <QPainter>
#include <QSettings>
#include <QSlider>
#include <QSortFilterProxyModel>

#include "PhotoBrowserView.h"
#include "PhotoListModel.h"
#include "PhotoFolderParser.h"
#include "SearchField.h"

#if defined(Q_OS_MAC)
#include "iPhotoLibraryParser.h"
#include "ApertureLibraryParser.h"
#include "MacUtility.h"
#endif

void PhotoBrowserListView::paintEvent(QPaintEvent *event)
{
    int first_index = indexAt(viewport()->rect().topLeft()).row();
    int last_index = first_index + (viewport()->rect().width() / 60) * (viewport()->rect().height() / 45);

    if (first_index >= 0)
    {
        last_index = qMin(last_index, model()->rowCount() - 1);

        // build the 'mapped to source' index list
        QModelIndexList index_list;
        for (int i=first_index; i<=last_index; ++i)
        {
            index_list.append(m_proxy_photo_list_model->mapToSource(m_proxy_photo_list_model->index(i,0)));
        }

        m_photo_list_model->resetTasks(index_list);
    }

    QListView::paintEvent(event);
}

PhotoBrowserListView::ItemViewPaintPairs PhotoBrowserListView::draggablePaintPairs(const QModelIndexList &indexes, QRect *r) const
{
    QRect &rect = *r;
    const QRect viewportRect = viewport()->rect();
    PhotoBrowserListView::ItemViewPaintPairs ret;
    for (int i = 0; i < indexes.count(); ++i)
    {
        const QModelIndex &index = indexes.at(i);
        const QRect current = visualRect(index);
        if (current.intersects(viewportRect))
        {
            ret += qMakePair(current, index);
            rect |= current;
        }
    }
    rect &= viewportRect;
    return ret;
}

QPixmap PhotoBrowserListView::renderToPixmap(const QModelIndexList &indexes, QRect *r) const
{
    ItemViewPaintPairs paintPairs = draggablePaintPairs(indexes, r);
    if (paintPairs.isEmpty())
        return QPixmap();
    QPixmap pixmap(r->size());
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    painter.setOpacity(0.5);
    QStyleOptionViewItem option = viewOptions();
    option.state |= QStyle::State_Selected;
    for (int j = 0; j < paintPairs.count(); ++j)
    {
        option.rect = paintPairs.at(j).first.translated(-r->topLeft());
        const QModelIndex &current = paintPairs.at(j).second;
        itemDelegate(current)->paint(&painter, option, current);
    }
    return pixmap;
}

void PhotoBrowserListView::mousePressEvent(QMouseEvent *event)
{
    m_pressed_position = event->pos() + offset();
    
    QListView::mousePressEvent(event);
}

void PhotoBrowserListView::startDrag(Qt::DropActions supportedActions)
{
    // copied from QAbstractItemView::startDrag(supportedActions);

    QModelIndexList indexes = selectedIndexes();
    if (indexes.count() > 0)
    {
        QMimeData *data = model()->mimeData(indexes);
        if (!data) return;
        QRect rect;
        QPixmap pixmap = renderToPixmap(indexes, &rect);
        rect.adjust(horizontalOffset(), verticalOffset(), 0, 0);
        QDrag *drag = new QDrag(this);
        drag->setPixmap(pixmap);
        drag->setMimeData(data);
        drag->setHotSpot(m_pressed_position - rect.topLeft());
        Qt::DropAction action = Qt::IgnoreAction;
        if (defaultDropAction() != Qt::IgnoreAction && (supportedActions & defaultDropAction()))
            action = defaultDropAction();
        else if (supportedActions & Qt::CopyAction && dragDropMode() != QAbstractItemView::InternalMove)
            action = Qt::CopyAction;
        if (drag->exec(supportedActions, action) == Qt::MoveAction)
            ; // d->clearOrRemove();
    }
}

PhotoBrowserView::PhotoBrowserView(Qt::Orientation orientation, QWidget *parent)
    : MediaBrowserView(orientation, parent)
{
    QString style_sheet;
    QTextStream ss(&style_sheet);
    ss << "* QLineEdit { selection-color: white; border: 2px groove gray; border-radius: 10px; padding: 0px 20px 0px 20px; }";
    ss << "* QToolButton { border: 2px groove gray; border-radius: 10px; padding: 0px 3px 0px 5px; }";
    setStyleSheet(style_sheet);

    m_photo_list_model = new PhotoListModel();

    m_proxy_photo_list_model = new QSortFilterProxyModel();

    m_proxy_photo_list_model->setSourceModel(m_photo_list_model);
    m_proxy_photo_list_model->setDynamicSortFilter(true);
    m_proxy_photo_list_model->setFilterCaseSensitivity(Qt::CaseInsensitive);
    m_proxy_photo_list_model->setFilterKeyColumn(-1);
    m_proxy_photo_list_model->setFilterRole(Qt::ToolTipRole);

    setMediaListModel(m_photo_list_model);

    m_image_list_view = new PhotoBrowserListView(m_photo_list_model, m_proxy_photo_list_model);
    m_image_list_view->setFlow(QListView::LeftToRight);
    m_image_list_view->setViewMode(QListView::IconMode);
    m_image_list_view->setMovement(QListView::Static);
    m_image_list_view->setResizeMode(QListView::Adjust);
    m_image_list_view->setWrapping(true);
    m_image_list_view->setUniformItemSizes(true);
    m_image_list_view->setSelectionMode(QAbstractItemView::ExtendedSelection);
    m_image_list_view->setDragDropMode(QListView::DragOnly);
    m_image_list_view->setDragEnabled(true);
    m_image_list_view->setModel(m_proxy_photo_list_model);

    m_proxy_photo_list_model->setFilterRegExp("."); // workaround for sort filter wierdness (needs root item or something)

    // set up the font size
    QFont style_font = m_image_list_view->font();
    style_font.setPixelSize(11);
    m_image_list_view->setFont(style_font);
    m_photo_list_model->setFontMetrics(m_image_list_view->fontMetrics());

    // set up the cell size
    QSize icon_size = QSettings().value("MediaBrowser.iconSize", QSize(84, 63)).toSize();
    m_photo_list_model->setIconSize(icon_size);

    m_image_list_view->setIconSize(m_photo_list_model->cellSize());

    setTreeContentView(m_image_list_view);

    QWidget *search_box = new QWidget();
    QHBoxLayout *search_box_layout = new QHBoxLayout(search_box);
    search_box_layout->setContentsMargins(12, 1, 12, 1);
    search_box_layout->setSpacing(2);

    QSlider *size_slider = new QSlider();
    size_slider->setMinimum(40);
    size_slider->setMaximum(192);
    size_slider->setValue(icon_size.width());
    size_slider->setMinimumWidth(40);
    size_slider->setMaximumWidth((orientation == Qt::Vertical) ? 80 : 200);
    size_slider->setOrientation(Qt::Horizontal);

    m_search_field = new SearchField(tr("Search..."));
    m_search_field->setMaximumWidth(240);

    search_box_layout->addWidget(m_search_field);
    search_box_layout->addSpacerItem(new QSpacerItem(4, 4, QSizePolicy::Expanding, QSizePolicy::Minimum));
    search_box_layout->addWidget(size_slider);

    m_layout->addWidget(search_box);

    connect(size_slider, SIGNAL(valueChanged(int)), this, SLOT(setCellSize(int)));
    connect(m_search_field, SIGNAL(textChanged(QString)), this, SLOT(textChanged(QString)));
}

void PhotoBrowserView::textChanged(const QString &text)
{
    if (text.isEmpty())
        m_proxy_photo_list_model->setFilterRegExp(".");
    else
        m_proxy_photo_list_model->setFilterFixedString(text);
}

void PhotoBrowserView::setCellSize(int size)
{
    QSize icon_size = QSize(size, 3*size/4);
    m_photo_list_model->setIconSize(icon_size);
    m_image_list_view->setIconSize(m_photo_list_model->cellSize());

    QSettings().setValue("MediaBrowser.iconSize", icon_size);
}

void PhotoBrowserView::addMediaParsers()
{
    {
#if QT_VERSION >= 0x050000
        MediaParserPtr folder_parser(new PhotoFolderParser(libraryTreeController(), QStandardPaths::writableLocation(QStandardPaths::PicturesLocation), QStandardPaths::writableLocation(QStandardPaths::PicturesLocation)));
#else // QT_VERSION >= 0x050000
        MediaParserPtr folder_parser(new PhotoFolderParser(libraryTreeController(), QDesktopServices::storageLocation(QDesktopServices::PicturesLocation), QDesktopServices::displayName(QDesktopServices::PicturesLocation)));
#endif // QT_VERSION >= 0x050000

        folder_parser->start();

        addMediaParser(folder_parser);
    }

#if defined(Q_OS_MAC)
    {
        QStringList iphoto_dbs = iPhotoRecentDatabasePaths();

        unsigned index = 1;
        Q_FOREACH(const QString &iphoto_db, iphoto_dbs)
        {
            QString display_name = index > 1 ? QString("%1 %2").arg(tr("iPhoto Library")).arg(index) : tr("iPhoto Library");

            MediaParserPtr parser(new iPhotoLibraryParser(libraryTreeController(), iphoto_db, display_name));

            parser->start();

            addMediaParser(parser);

            ++index;
        }
    }

    {
        QStringList aperture_dbs = ApertureRecentDatabasePaths();

        unsigned index = 1;
        Q_FOREACH(const QString &aperture_db, aperture_dbs)
        {
            //if (index == 1) { ++index; continue; } // testing

            QString display_name = index > 1 ? QString("%1 %2").arg(tr("Aperture Library")).arg(index) : tr("Aperture Library");

            MediaParserPtr parser(new ApertureLibraryParser(libraryTreeController(), aperture_db, display_name));

            parser->start();

            addMediaParser(parser);

            ++index;
        }
    }

    {
        MediaParserPtr folder_parser(new PhotoFolderParser(libraryTreeController(), "/Library/Desktop Pictures", tr("Desktop Pictures")));

        folder_parser->start();

        addMediaParser(folder_parser);
    }

    {
        MediaParserPtr folder_parser(new PhotoFolderParser(libraryTreeController(), "/Library/User Pictures", tr("User Pictures")));

        folder_parser->start();

        addMediaParser(folder_parser);
    }

    {
        MediaParserPtr folder_parser(new PhotoFolderParser(libraryTreeController(), "/Library/Application Support/Apple/iChat Icons", tr("Chat Icons")));

        folder_parser->start();

        addMediaParser(folder_parser);
    }
#endif
}
