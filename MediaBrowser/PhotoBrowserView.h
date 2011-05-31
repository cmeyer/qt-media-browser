#ifndef MEDIA_BROWSER_PHOTO_BROWSER_VIEW_H
#define MEDIA_BROWSER_PHOTO_BROWSER_VIEW_H

#include <QListView>

#include "MediaBrowserView.h"

class QLineEdit;
class QSortFilterProxyModel;

class PhotoListModel;
class PhotoBrowserListView;

// the photo browser. includes the photo browser list view and a search field.

class PhotoBrowserView : public MediaBrowserView
{
    Q_OBJECT

public:
    PhotoBrowserView(Qt::Orientation orientation = Qt::Vertical, QWidget *parent = NULL);
    virtual void addMediaParsers();

    PhotoBrowserListView *photoBrowserListView() const { return m_image_list_view; }
    QLineEdit *searchField() const { return m_search_field; }

private Q_SLOTS:
    void setCellSize(int size);
    void textChanged(const QString &text);

private:
    PhotoListModel *m_photo_list_model;
    PhotoBrowserListView *m_image_list_view;
    QSortFilterProxyModel *m_proxy_photo_list_model;
    QLineEdit *m_search_field;
};

// the list of photo icons in the browser portion of the user interface.

class PhotoBrowserListView : public QListView
{
    Q_OBJECT

public:
    PhotoBrowserListView(PhotoListModel *photo_list_model, QSortFilterProxyModel *proxy_photo_list_model)
        : m_photo_list_model(photo_list_model)
        , m_proxy_photo_list_model(proxy_photo_list_model)
    { }
    void paintEvent(QPaintEvent *event);
    void startDrag(Qt::DropActions supportedActions);

private:
    typedef QPair<QRect, QModelIndex> ItemViewPaintPair;
    typedef QList<ItemViewPaintPair> ItemViewPaintPairs;

    PhotoBrowserListView::ItemViewPaintPairs draggablePaintPairs(const QModelIndexList &indexes, QRect *r) const;
    void mousePressEvent(QMouseEvent *event);
    QPixmap renderToPixmap(const QModelIndexList &indexes, QRect *r) const;

    inline QPoint offset() const { return QPoint(isRightToLeft() ? -horizontalOffset() : horizontalOffset(), verticalOffset()); }
    
    PhotoListModel *m_photo_list_model;
    QSortFilterProxyModel *m_proxy_photo_list_model;
    QPoint m_pressed_position;
};

#endif // MEDIA_BROWSER_PHOTO_BROWSER_VIEW_H
