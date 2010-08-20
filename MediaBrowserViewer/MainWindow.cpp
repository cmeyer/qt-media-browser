#include <QVBoxLayout>
#include <QWidget>

#include "MainWindow.h"
#include <MediaBrowser/MediaBrowser.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    QWidget *central_widget = new QWidget(this);
    QVBoxLayout *central_layout = new QVBoxLayout(central_widget);
    central_layout->setContentsMargins(0, 0, 0, 0);
    central_layout->setSpacing(0);

    setCentralWidget(central_widget);

    const int media_width = 310;

    PhotoBrowserView *photo_browser_view = new PhotoBrowserView(this);
    photo_browser_view->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::MinimumExpanding);
    photo_browser_view->setMinimumSize(media_width, 370);

    MusicBrowserView *music_browser_view = new MusicBrowserView();
    music_browser_view->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::MinimumExpanding);
    music_browser_view->setMinimumSize(media_width, 370);

    QTabWidget *media_browser_tab = new QTabWidget();
    media_browser_tab->setObjectName("media-browser");
    media_browser_tab->setMinimumSize(media_width, 370);
    media_browser_tab->setContentsMargins(0, 0, 0, 0);
    media_browser_tab->setDocumentMode(true);
    media_browser_tab->addTab(photo_browser_view, tr("Photos"));
    media_browser_tab->addTab(music_browser_view, tr("Music"));

    central_layout->addWidget(media_browser_tab);
}

MainWindow::~MainWindow()
{

}
