#ifndef MEDIA_BROWSER_MEDIA_BROWSER_VIEW_H
#define MEDIA_BROWSER_MEDIA_BROWSER_VIEW_H

#include <boost/smart_ptr.hpp>
#include <QWidget>

class QTimer;
class QItemSelection;
class QModelIndex;
class QSplitter;
class QVBoxLayout;

typedef boost::shared_ptr<class LibraryTreeController> LibraryTreeControllerPtr;

class LibraryTreeModel;
class MediaListModel;

// the media browser view is the view that gives a list of all libraries
// and sub-folders of those libraries. it gets its data from the library
// tree model. periodically, the sync method should be called to trigger
// a synchronization from the live data to the display data.

typedef boost::shared_ptr<class MediaParser> MediaParserPtr;

class MediaBrowserView : public QWidget
{
    Q_OBJECT

public:
    MediaBrowserView(Qt::Orientation orientation = Qt::Vertical, QWidget *parent = NULL);
    ~MediaBrowserView();

    QSplitter *splitter() const { return m_splitter; }

protected:
    void setMediaListModel(MediaListModel *media_list_model) { m_media_list_model = media_list_model; }
    MediaListModel *mediaListModel() const { return m_media_list_model; }

    // subclasses will need this to construct the media parsers.
    LibraryTreeControllerPtr libraryTreeController() const { return m_library_tree_controller; }

    // the content view displays the contents of the currently selected tree item.
    // call this method in the constructor of a derived class.
    void setTreeContentView(QWidget *content_view);

    // add a media parser. this will be canceled upon shutting down.
    void addMediaParser(MediaParserPtr media_parser);

    // subclasses should override this to add the media parsers. it gets called automatically
    // by show event.
    virtual void addMediaParsers() = 0;
    virtual void showEvent(QShowEvent *event);

private Q_SLOTS:
    void sync();
    void selectionChanged(const QItemSelection &selected, const QItemSelection &deselected);
    void expanded(const QModelIndex &index);

protected:
    QVBoxLayout *m_layout;

private:
    QSplitter *m_splitter;
    LibraryTreeModel *m_library_tree_model;
    LibraryTreeControllerPtr m_library_tree_controller;
    MediaListModel *m_media_list_model;
    QTimer *m_sync_timer;
    bool m_initialized_media_parsers;
    QList<MediaParserPtr> m_media_parsers;
};

#endif // MEDIA_BROWSER_MEDIA_BROWSER_VIEW_H
