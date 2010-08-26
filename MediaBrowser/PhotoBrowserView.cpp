#include <QDebug>
#include <QDesktopServices>
#include <QHBoxLayout>
#include <QListView>
#include <QSlider>

#include "PhotoBrowserView.h"
#include "PhotoListModel.h"
#include "PhotoFolderParser.h"

#if defined(Q_OS_MAC)
#include "iPhotoLibraryParser.h"
#include "ApertureLibraryParser.h"
#include "MacUtility.h"
#endif

class PhotoBrowserListView : public QListView
{
public:
    PhotoBrowserListView(PhotoListModel *photo_list_model) : m_photo_list_model(photo_list_model) { }
    void paintEvent(QPaintEvent *event);
private:
    PhotoListModel *m_photo_list_model;
};

void PhotoBrowserListView::paintEvent(QPaintEvent *event)
{
    int first_index = indexAt(viewport()->rect().topLeft()).row();
    int last_index = first_index + (viewport()->rect().width() / 60) * (viewport()->rect().height() / 45);

    if (first_index >= 0)
    {
        last_index = std::min(last_index, model()->rowCount() - 1);

        // build the 'mapped to source' index list
        QModelIndexList index_list;
        for (int i=first_index; i<=last_index; ++i)
            index_list.append(m_photo_list_model->index(i,0));
        
        m_photo_list_model->resetTasks(index_list);
    }

    QListView::paintEvent(event);
}

PhotoBrowserView::PhotoBrowserView(QWidget *parent)
    : MediaBrowserView(parent)
{
    QString style_sheet;
    QTextStream ss(&style_sheet);
    ss << "* QLineEdit { selection-color: white; border: 2px groove gray; border-radius: 10px; padding: 0px 20px 0px 20px; }";
    ss << "* QToolButton { border: 2px groove gray; border-radius: 10px; padding: 0px 3px 0px 5px; }";
    setStyleSheet(style_sheet);

    m_photo_list_model = new PhotoListModel();

    setMediaListModel(m_photo_list_model);

    m_image_list_view = new PhotoBrowserListView(m_photo_list_model);
    m_image_list_view->setFlow(QListView::LeftToRight);
    m_image_list_view->setViewMode(QListView::IconMode);
    m_image_list_view->setMovement(QListView::Static);
    m_image_list_view->setResizeMode(QListView::Adjust);
    m_image_list_view->setWrapping(true);
    m_image_list_view->setUniformItemSizes(true);
    m_image_list_view->setSelectionMode(QAbstractItemView::ExtendedSelection);
    m_image_list_view->setDragDropMode(QListView::DragOnly);
    m_image_list_view->setDragEnabled(true);
    m_image_list_view->setModel(m_photo_list_model);

    // set up the font size
    QFont style_font = m_image_list_view->font();
    style_font.setPixelSize(11);
    m_image_list_view->setFont(style_font);
    m_photo_list_model->setFontMetrics(m_image_list_view->fontMetrics());

    // set up the cell size
    QSize icon_size = QSize(84, 63);
    m_photo_list_model->setIconSize(icon_size);

    m_image_list_view->setIconSize(m_photo_list_model->cellSize());

    QWidget *search_box = new QWidget();
    QHBoxLayout *search_box_layout = new QHBoxLayout(search_box);
    search_box_layout->setContentsMargins(12, 2, 12, 2);
    search_box_layout->setSpacing(2);

    QSlider *size_slider = new QSlider();
    size_slider->setMinimum(40);
    size_slider->setMaximum(192);
    size_slider->setValue(icon_size.width());
    size_slider->setMinimumWidth(40);
    size_slider->setMaximumWidth(80);
    size_slider->setOrientation(Qt::Horizontal);

    search_box_layout->addWidget(size_slider);
    search_box_layout->addSpacerItem(new QSpacerItem(4, 4, QSizePolicy::Expanding, QSizePolicy::Minimum));
    //search_box_layout->addWidget(m_search_field);

    m_layout->addWidget(search_box);

    setTreeContentView(m_image_list_view);

    connect(size_slider, SIGNAL(sliderMoved(int)), this, SLOT(setCellSize(int)));
}

void PhotoBrowserView::setCellSize(int size)
{
    QSize icon_size = QSize(size, 3*size/4);
    m_photo_list_model->setIconSize(icon_size);
    m_image_list_view->setIconSize(m_photo_list_model->cellSize());
}

void PhotoBrowserView::addMediaParsers()
{
    {
        MediaParserPtr folder_parser(new PhotoFolderParser(libraryTreeController(), QDesktopServices::storageLocation(QDesktopServices::PicturesLocation), QDesktopServices::displayName(QDesktopServices::PicturesLocation)));

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
