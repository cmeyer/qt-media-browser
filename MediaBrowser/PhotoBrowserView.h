#ifndef MEDIA_BROWSER_PHOTO_BROWSER_VIEW_H
#define MEDIA_BROWSER_PHOTO_BROWSER_VIEW_H

#include "MediaBrowserView.h"

class QLineEdit;
class QSortFilterProxyModel;

class PhotoListModel;
class PhotoBrowserListView;

class PhotoBrowserView : public MediaBrowserView
{
    Q_OBJECT

public:
    PhotoBrowserView(QWidget *parent = NULL);
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

#endif // MEDIA_BROWSER_PHOTO_BROWSER_VIEW_H
