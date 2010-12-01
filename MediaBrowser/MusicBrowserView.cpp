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
#include <QToolButton>
#include <QTreeView>
#include <QWindowsStyle>

#include <QMediaPlayer>

#include "MusicBrowserView.h"
#include "MusicListModel.h"
#include "MusicFolderParser.h"
#include "iTunesLibraryParser.h"
#include "SearchField.h"

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

MusicBrowserView::MusicBrowserView(QWidget *parent)
    : MediaBrowserView(Qt::Vertical, parent)
    , m_player(NULL)
    , m_play_button(NULL)
{
    QString style_sheet;
    QTextStream ss(&style_sheet);
    ss << "* QToolButton { border: 2px groove gray; border-radius: 10px; padding: 0px 3px 0px 5px; }";
    ss << "* QTreeView#music { font: 11px; color: black; } ";
    ss << "* QTreeView#music::item { padding-top: 1px; padding-bottom: 1px; border: 1px solid #EEE; border-top-color: transparent; border-bottom-color: transparent; } ";
    ss << "* QTreeView#music::item:selected { color: palette(highlighted-text); background-color: palette(highlight); border: 1px solid palette(highlight); }";
    setStyleSheet(style_sheet);

    QWidget *search_box = new QWidget();
    QHBoxLayout *search_box_layout = new QHBoxLayout(search_box);
    search_box_layout->setContentsMargins(12, 2, 12, 2);
    search_box_layout->setSpacing(2);

    m_search_field = new SearchField(tr("Search..."));
    m_search_field->adjustSize();

    m_play_button = new QToolButton();
    m_play_button->setIcon(QIcon(":/MediaBrowser/PlayButton.png"));
    m_play_button->setMinimumHeight(m_search_field->height());
    m_play_button->hide();

    search_box_layout->addWidget(m_play_button);
    search_box_layout->addSpacerItem(new QSpacerItem(4, 4, QSizePolicy::Expanding, QSizePolicy::Minimum));
    search_box_layout->addWidget(m_search_field);

    m_layout->addWidget(search_box);

    m_music_list_model = new MusicListModel();

    m_proxy_music_list_model = new QSortFilterProxyModel();
    
    m_proxy_music_list_model->setSourceModel(m_music_list_model);
    m_proxy_music_list_model->setDynamicSortFilter(true);
    m_proxy_music_list_model->setFilterCaseSensitivity(Qt::CaseInsensitive);
    m_proxy_music_list_model->setFilterKeyColumn(-1);
    m_proxy_music_list_model->setSortRole(MediaListModel::SortRole);

    setMediaListModel(m_music_list_model);

    m_music_list_view = new MusicBrowserTreeView(m_music_list_model, m_proxy_music_list_model);
    m_music_list_view->setObjectName("music");
    m_music_list_view->setRootIsDecorated(false);
    m_music_list_view->setAlternatingRowColors(true);
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

    connect(m_play_button, SIGNAL(pressed()), this, SLOT(playPressed()));
}

void MusicBrowserView::playPressed()
{
    if (m_player != NULL)
        m_play_button->setIcon(QIcon(":/MediaBrowser/PauseButton-dark.png"));
    else
        m_play_button->setIcon(QIcon(":/MediaBrowser/PlayButton-dark.png"));
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
        m_current_file_path = qVariantValue<QString>(m_music_list_model->data(first_index, MediaListModel::FullPathRole));
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
