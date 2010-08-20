#include <QDebug>
#include <QDesktopServices>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QKeyEvent>
#include <QLabel>
#include <QLineEdit>
#include <QMouseEvent>
#include <QPushButton>
#include <QSortFilterProxyModel>
#include <QStyleOption>
#include <QStyleOptionButton>
#include <QToolButton>
#include <QTreeView>
#include <QWindowsStyle>

#include <QMediaPlayer>

#include "MusicBrowserView.h"
#include "MusicListModel.h"
#include "MusicFolderParser.h"
#include "iTunesLibraryParser.h"

#if defined(Q_OS_MAC)
#include "MacUtility.h"
#endif

void MusicBrowserTreeView::paintEvent(QPaintEvent *event)
{
    int first_index = indexAt(viewport()->rect().topLeft()).row();
    int last_index = indexAt(viewport()->rect().bottomLeft()).row();

    if (first_index >= 0)
    {
        if (last_index < 0)
            last_index = model()->rowCount() - 1;

        // build the 'mapped to source' index list
        QModelIndexList index_list;
        for (int i=first_index; i<=last_index; ++i)
            index_list.append(m_proxy_music_list_model->mapToSource(m_proxy_music_list_model->index(i,0)));

        m_music_list_model->resetTasks(index_list);
    }

    QTreeView::paintEvent(event);
}

void MusicBrowserTreeView::mouseDoubleClickEvent(QMouseEvent *event)
{
    Q_EMIT playMusic();
}

// see http://developer.qt.nokia.com/faq/answer/how_can_i_avoid_drawing_the_focus_rect_on_my_buttons
// see http://stackoverflow.com/questions/2588743/qt-4-6-qlineedit-style-how-do-i-style-the-gray-highlight-border-so-its-rounded

class NoFocusStyle : public QWindowsStyle
{
public:
    NoFocusStyle() { }
    void drawControl(ControlElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget = 0) const
    {
        if (element == CE_PushButton)
        {
            const QStyleOptionButton *b = qstyleoption_cast<const QStyleOptionButton *>(option);
            QStyleOptionButton *button = (QStyleOptionButton *)b;
            if (button)
            {
                if (button->state & State_HasFocus)
                {
                    button->state = button->state ^ State_HasFocus;
                }
            }
            QWindowsStyle::drawControl(element, button, painter, widget);
        }
        else
        {
            QWindowsStyle::drawControl(element, option, painter, widget);
        }
    }
};


MusicBrowserView::MusicBrowserView(QWidget *parent)
    : MediaBrowserView(parent)
    , m_player(NULL)
    , m_play_button(NULL)
{
    QString style_sheet;
    QTextStream ss(&style_sheet);
    ss << "* QLineEdit { selection-color: white; border: 2px groove gray; border-radius: 10px; padding: 0px 4px; }";
    //ss << "* QLineEdit:focus { selection-color: white; border: 2px groove gray; border-radius: 10px; padding: 0px 4px; }";
    //ss << "* QLineEdit:edit-focus { selection-color: white; border: 2px groove gray; border-radius: 10px; padding: 0px 4px; }";
    //ss << "* QPushButton { min-width: 16px; max-width: 16px; min-height: 16px; max-height: 16px; border: 0px; }";
    ss << "* QToolButton { border: 0px; }";
    setStyleSheet(style_sheet);

    QWidget *search_box = new QWidget();
    QHBoxLayout *search_box_layout = new QHBoxLayout(search_box);
    search_box_layout->setContentsMargins(12, 2, 12, 2);
    search_box_layout->setSpacing(2);

    m_search_field = new QLineEdit();
    //m_search_field->setPlaceholderText(tr("Search..."));
    m_search_field->setStyle(new NoFocusStyle());
    m_search_field->adjustSize();

    m_play_button = new QToolButton();
    m_play_button->setIcon(QIcon(":/MediaBrowser/PlayButton.png"));
    m_play_button->setMinimumHeight(m_search_field->height());
    m_play_button->hide();

    QLabel *search_box_label = new QLabel();
    search_box_label->setPixmap(QPixmap(":/MediaBrowser/MagnifyingGlass.png"));
    search_box_label->setMinimumHeight(m_search_field->height());

    m_cancel_search_button = new QToolButton();
    m_cancel_search_button->setIcon(QIcon(":/MediaBrowser/CancelButton.png"));
    m_cancel_search_button->setMinimumHeight(m_search_field->height());
    m_cancel_search_button->adjustSize();
    m_cancel_search_button->setIcon(QIcon());

    search_box_layout->addWidget(m_play_button);
    search_box_layout->addSpacerItem(new QSpacerItem(4, 4, QSizePolicy::Expanding, QSizePolicy::Minimum));
    search_box_layout->addWidget(search_box_label);
    search_box_layout->addWidget(m_search_field);
    search_box_layout->addWidget(m_cancel_search_button);

    m_layout->addWidget(search_box);

    m_music_list_model = new MusicListModel();

    m_proxy_music_list_model = new QSortFilterProxyModel();
    
    m_proxy_music_list_model->setSourceModel(m_music_list_model);
    m_proxy_music_list_model->setDynamicSortFilter(true);
    m_proxy_music_list_model->setFilterCaseSensitivity(Qt::CaseInsensitive);
    m_proxy_music_list_model->setFilterKeyColumn(-1);

    setMediaListModel(m_music_list_model);

    m_music_list_view = new MusicBrowserTreeView(m_music_list_model, m_proxy_music_list_model);
    m_music_list_view->setRootIsDecorated(false);
    m_music_list_view->setSelectionMode(QAbstractItemView::ExtendedSelection);
    m_music_list_view->setDragDropMode(QTreeView::DragOnly);
    m_music_list_view->setDragEnabled(true);
    m_music_list_view->setModel(m_proxy_music_list_model);
    m_music_list_view->setSortingEnabled(true);
    
    setTreeContentView(m_music_list_view);

    connect(m_music_list_view, SIGNAL(playMusic()), this, SLOT(playButtonPressed()));
    connect(m_play_button, SIGNAL(clicked()), this, SLOT(playButtonPressed()));
    connect(m_search_field, SIGNAL(textChanged(QString)), this, SLOT(textChanged(QString)));
    connect(m_music_list_view->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), this, SLOT(selectionChanged(QItemSelection,QItemSelection)));

    connect(m_cancel_search_button, SIGNAL(clicked()), this, SLOT(cancelClicked()));
    connect(m_cancel_search_button, SIGNAL(pressed()), this, SLOT(cancelPressed()));
    connect(m_cancel_search_button, SIGNAL(released()), this, SLOT(cancelReleased()));
    connect(m_play_button, SIGNAL(pressed()), this, SLOT(playPressed()));
}

void MusicBrowserView::playPressed()
{
    if (m_player != NULL)
        m_play_button->setIcon(QIcon(":/MediaBrowser/PauseButton-dark.png"));
    else
        m_play_button->setIcon(QIcon(":/MediaBrowser/PlayButton-dark.png"));
}

void MusicBrowserView::cancelPressed()
{
    m_cancel_search_button->setIcon(QIcon(":/MediaBrowser/CancelButton-dark.png"));
}

void MusicBrowserView::cancelReleased()
{
    m_cancel_search_button->setIcon(QIcon(":/MediaBrowser/CancelButton.png"));
}

void MusicBrowserView::cancelClicked()
{
    m_search_field->clear();
}

void MusicBrowserView::showEvent(QShowEvent *event)
{
    MediaBrowserView::showEvent(event);
    int width = contentsRect().width() - 4 - 17;
    m_music_list_view->header()->resizeSection(0, width*5/8);
    m_music_list_view->header()->resizeSection(1, width*2/8);
    m_music_list_view->header()->resizeSection(2, width*1/8);
}

void MusicBrowserView::selectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
    QModelIndexList model_index_list = selected.indexes();
    if (model_index_list.count() > 0)
    {
        QModelIndex first_index = m_proxy_music_list_model->mapToSource(model_index_list.at(0));
        m_current_file_path = qVariantValue<QString>(m_music_list_model->data(first_index, Qt::UserRole));
        m_play_button->show();
    }
    else
    {
        m_current_file_path.clear();
        m_play_button->hide();
    }

    if (m_player != NULL)
    {
        m_play_button->setIcon(QIcon(":/MediaBrowser/PlayButton.png"));
        m_player->stop();
        delete m_player;
        m_player = NULL;
    }
}

void MusicBrowserView::textChanged(const QString &text)
{
    m_proxy_music_list_model->setFilterFixedString(text);

    if (text.isEmpty())
        m_cancel_search_button->setIcon(QIcon());
    else
        m_cancel_search_button->setIcon(QIcon(":/MediaBrowser/CancelButton.png"));
}

void MusicBrowserView::playButtonPressed()
{
    if (m_player != NULL)
    {
        m_play_button->setIcon(QIcon(":/MediaBrowser/PlayButton.png"));
        m_player->stop();
        delete m_player;
        m_player = NULL;
    }
    else
    {
        m_play_button->setIcon(QIcon(":/MediaBrowser/PauseButton.png"));
        m_player = new QMediaPlayer(this);
        m_player->setMedia(QUrl::fromLocalFile(m_current_file_path));
        m_player->play();
    }
}

void MusicBrowserView::addMediaParsers()
{
    {
        MediaParserPtr folder_parser(new MusicFolderParser(libraryTreeController(), QDesktopServices::storageLocation(QDesktopServices::MusicLocation), QDesktopServices::displayName(QDesktopServices::MusicLocation)));

        folder_parser->start();

        addMediaParser(folder_parser);
    }

#if defined(Q_OS_MAC)
    {
        MediaParserPtr folder_parser(new MusicFolderParser(libraryTreeController(), "/Library/Audio/Apple Loops/Apple/iLife Sound Effects", tr("iLife Sound Effects")));

        folder_parser->start();

        addMediaParser(folder_parser);
    }

    QString iMovie_path = PathForIMovie();
    if (!iMovie_path.isEmpty())
    {
        MediaParserPtr folder_parser(new MusicFolderParser(libraryTreeController(), iMovie_path, tr("iMovie Sound Effects")));

        folder_parser->start();

        addMediaParser(folder_parser);
    }

    {
        QString path = QString("%1/Library/Sounds").arg(QDesktopServices::storageLocation(QDesktopServices::HomeLocation));

        MediaParserPtr folder_parser(new MusicFolderParser(libraryTreeController(), path, tr("Sounds Folder")));

        folder_parser->start();

        addMediaParser(folder_parser);
    }

    QStringList itunes_dbs = iTunesRecentDatabasePaths();

    unsigned index = 1;
    Q_FOREACH(const QString &itunes_db, itunes_dbs)
    {
        QString display_name = index > 1 ? QString("%1 %2").arg(tr("iTunes Library")).arg(index) : tr("iTunes Library");

        MediaParserPtr parser(new iTunesLibraryParser(libraryTreeController(), itunes_db, display_name));

        parser->start();

        addMediaParser(parser);

        ++index;
    }
#endif
}
