#ifndef MEDIA_BROWSER_PHOTO_BROWSER_VIEW_H
#define MEDIA_BROWSER_PHOTO_BROWSER_VIEW_H

#include "MediaBrowserView.h"

class PhotoListModel;

class PhotoBrowserView : public MediaBrowserView
{
    Q_OBJECT

public:
    PhotoBrowserView(QWidget *parent = NULL);
    virtual void addMediaParsers();

private:
    PhotoListModel *m_photo_list_model;
};

#endif // MEDIA_BROWSER_PHOTO_BROWSER_VIEW_H
