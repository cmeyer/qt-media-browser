#ifndef MEDIA_BROWSER_MUSIC_BROWSER_VIEW_H
#define MEDIA_BROWSER_MUSIC_BROWSER_VIEW_H

#include "MediaBrowserView.h"

#include <QTreeView>

class QLineEdit;
class QMediaPlayer;
class QSortFilterProxyModel;
class QToolButton;
class QTreeView;

class MusicListModel;

class MusicBrowserView : public MediaBrowserView
{
    Q_OBJECT

public:
    MusicBrowserView(QWidget *parent = NULL);
    virtual void addMediaParsers();

protected:
    virtual void showEvent(QShowEvent *event);

private Q_SLOTS:
    void playButtonPressed();
    void selectionChanged(const QItemSelection &selected, const QItemSelection &deselected);
    void textChanged(const QString &text);

    // argh. can't Qt just handle pressed buttons properly?
    void playPressed();

private:
    QTreeView *m_music_list_view;
    MusicListModel *m_music_list_model;
    QSortFilterProxyModel *m_proxy_music_list_model;
    QMediaPlayer *m_player;
    QString m_current_file_path;
    QLineEdit *m_search_field;
    QToolButton *m_play_button;
};

class MusicBrowserTreeView : public QTreeView
{
    Q_OBJECT
public:
    MusicBrowserTreeView(MusicListModel *music_list_model, QSortFilterProxyModel *proxy_music_list_model)
        : m_music_list_model(music_list_model)
        , m_proxy_music_list_model(proxy_music_list_model)
    { }
    void paintEvent(QPaintEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);
Q_SIGNALS:
    void playMusic();
private:
    MusicListModel *m_music_list_model;
    QSortFilterProxyModel *m_proxy_music_list_model;
};

#include <QLineEdit>

class SearchLineEdit : public QLineEdit
{
    Q_OBJECT
public:
    SearchLineEdit(const QString &default_str, QWidget *parent = nil);
    virtual void paintEvent(QPaintEvent *event);
    virtual void mousePressEvent(QMouseEvent *event);
    virtual void mouseReleaseEvent(QMouseEvent *event);
    virtual void keyPressEvent(QKeyEvent *event);
private Q_SLOTS:
    void textChanged(const QString &text);
private:
    QString m_default_str;
    enum { STATE_HIDDEN, STATE_VISIBLE_NORMAL, STATE_VISIBLE_DOWN };
    int m_state;

    // images
    QImage m_magnifying_glass_image;
    QImage m_cancel_button_image;
    QImage m_cancel_button_dark_image;
    QRect cancelButtonRect();
};

#endif // MEDIA_BROWSER_MUSIC_BROWSER_VIEW_H
