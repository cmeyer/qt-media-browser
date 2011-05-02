#include <QDebug>
#include <QPainter>
#include <QThread>
#include <QImageReader>
#include <QFileInfo>
#include "MediaFile.h"
#include "MediaLoader.h"
#if USE_TAGLIB
#include <fileref.h>
#include <tag.h>
#endif
#if defined(Q_OS_MAC)
#include "MacUtility.h"
#endif
#include "TaskPool.h"
#include "TaskGroup.h"

MediaFileTaskPool *MediaFileTaskPool::m_instance = NULL;

MediaFileTaskPool *MediaFileTaskPool::instance()
{
    static QMutex mutex;

    if (!m_instance)
    {
        mutex.lock();

        if (!m_instance)
            m_instance = new MediaFileTaskPool();

        mutex.unlock();
    }

    return m_instance;
}

MediaFileTaskPool::MediaFileTaskPool()
    : MediaBrowserPrivate::TaskPool(2)
{
}

MediaFileTaskPool::MediaFileTaskPool(MediaFileTaskPool const &)
    : MediaBrowserPrivate::TaskPool(2)
{
}

MediaFileTaskPool& MediaFileTaskPool::operator=(MediaFileTaskPool const &)
{
    return *this;
}

MediaFileTaskPool::~MediaFileTaskPool()
{
}

QRectF FitToAspectRatio(const QRectF &rect, float aspect_ratio)
{
    if (rect.width() > aspect_ratio * rect.height())
    {
        // height will fill entire frame
        QSizeF new_size(rect.height()*aspect_ratio, rect.height());
        QPointF new_origin(rect.left() + 0.5*(rect.width() - new_size.width()), rect.top());

        return QRectF(new_origin, new_size);
    }
    else
    {
        QSizeF new_size(rect.width(), rect.width()/aspect_ratio);
        QPointF new_origin(rect.left(), rect.top() + 0.5*(rect.height() - new_size.height()));

        return QRectF(new_origin, new_size);
    }
}

class MediaFileIconThread : public MediaBrowserPrivate::Task
{
public:
    MediaFileIconThread(MediaLoader *media_loader, MediaFilePtr media_file) : m_media_loader(media_loader), m_media_file(media_file) { }
    virtual void run();
    virtual void wasRemoved();
private:
    MediaLoader *m_media_loader;
    MediaFilePtr m_media_file;
};

void MediaFileIconThread::run()
{
    QImageReader image_reader(m_media_file->resolvedFilePath());
    QSize size = image_reader.size();
    QSize scaled_size = FitToAspectRatio(QRectF(QPointF(), QSizeF(192,144)), (float)size.width()/size.height()).toRect().size();
    if (scaled_size.width() == 0 || scaled_size.height() == 0)
        scaled_size = size;
    image_reader.setScaledSize(scaled_size);
    QImage image = image_reader.read();

    m_media_file->setImage(image);
    m_media_loader->mediaFileChanged(m_media_file);
}

void MediaFileIconThread::wasRemoved()
{
    m_media_file->resetImage();
}

class MediaFileAudioInfoThread : public MediaBrowserPrivate::Task
{
public:
    MediaFileAudioInfoThread(MediaLoader *media_loader, MediaFilePtr media_file) : m_media_loader(media_loader), m_media_file(media_file) { }
    virtual void run();
    virtual void wasRemoved();
private:
    MediaLoader *m_media_loader;
    MediaFilePtr m_media_file;
};

void MediaFileAudioInfoThread::run()
{
    QString file_path = m_media_file->resolvedFilePath();
    QString title = m_media_file->audioInfo().title();  // use existing title unless a better one is available
    QString artist;
    unsigned duration = 0;
    bool found = false;
#if defined(Q_OS_MAC)
    {
        MediaBrowserPrivate::AutoMemoryPool pool;

        CFStringRef file_path_cf = CFStringCreateWithBytes(kCFAllocatorDefault, (const UInt8 *)file_path.toUtf8().constData(), strlen((char *)file_path.toUtf8().constData()), CFStringGetSystemEncoding(), false);
        
        MDItemRef item = MDItemCreate(kCFAllocatorDefault, file_path_cf);
        
        NSDictionary *attributes = NULL;
        
        if ( item != NULL )
        {
            NSArray *attributeNames = [NSArray arrayWithObjects:@"kMDItemMediaTypes", @"kMDItemTitle", @"kMDItemDurationSeconds", @"kMDItemAuthors", @"kMDItemKind", nil];
            
            CFDictionaryRef attributes_cf = MDItemCopyAttributes(item,(CFArrayRef)attributeNames);
            
            if ( attributes_cf != NULL )
            {
                attributes = [NSDictionary dictionaryWithDictionary:(NSDictionary *)attributes_cf];
                
                CFRelease(attributes_cf);
            }
            
            CFRelease(item);
        }
        
        CFRelease(file_path_cf);
        
        NSArray *mediaTypes = [attributes objectForKey:@"kMDItemMediaTypes"];
        
        if ( mediaTypes != nil && [mediaTypes containsObject:@"Sound"] )
        {
            // spotlight worked! return the attributes.
            
            NSString *title_ns = [attributes valueForKey:(NSString *)kMDItemTitle];
            NSArray *artist_array_ns = [attributes valueForKey:(NSString *)kMDItemAuthors];
            
            if (title_ns != NULL && [title_ns length] > 0)
                title = MediaBrowserPrivate::CFStringToQString((CFStringRef)title_ns);
            if (artist_array_ns != NULL && [artist_array_ns count] > 0)
                artist = MediaBrowserPrivate::CFStringToQString((CFStringRef)[artist_array_ns objectAtIndex:0]);
            else
                artist = QObject::tr("Unknown");
            duration = [[attributes valueForKey:(NSString *)kMDItemDurationSeconds] floatValue];
            
            found = true;
        }
    }
#endif
#if USE_TAGLIB
    if (!found)
    {
        try
        {
            //TagLib::FileRef f(QStringToTString(file_path).toCString(true), true, TagLib::AudioProperties::Fast);
            // see http://web.archiveorange.com/archive/v/sF3PjRS1aVVFaq9EOaSn
            QByteArray file_path_utf8(file_path.toUtf8());
            TagLib::String file_path_ts(file_path_utf8.data(), TagLib::String::UTF8);
            TagLib::FileRef f(file_path_ts.toCString(true), true, TagLib::AudioProperties::Fast);
            
            if (!f.isNull() && f.tag() != NULL && f.audioProperties() != NULL)
            {
                QString new_title = TStringToQString(f.tag()->title());
                if (!new_title.isEmpty())
                    title = new_title;
                artist = TStringToQString(f.tag()->artist());
                duration = f.audioProperties()->length();
                found = true;
            }
        }
        catch (...)
        {
            // ignore unexpected errors
        }
    }
#endif
    AudioMediaFileInfo audio_info(file_path, title, artist, duration);
    m_media_file->setAudioInfo(audio_info);
    m_media_loader->mediaFileChanged(m_media_file);
}

void MediaFileAudioInfoThread::wasRemoved()
{
    m_media_file->resetAudioInfo();
}

MediaFile::MediaFile(const QString &file_path, const QString &source)
    : m_file_path(file_path)
    , m_source(source)
    , m_icon_loading(false)
    , m_icon_loaded(false)
    , m_audio_info_loading(false)
    , m_audio_info_loaded(false)
{
}

void MediaFile::loadIcon(MediaLoader *media_loader, MediaBrowserPrivate::TaskGroupPtr task_group)
{
    QMutexLocker lock(&m_icon_mutex);
    if (!m_icon_loading && !m_icon_loaded)
    {
        m_icon_loading = true;
        MediaBrowserPrivate::TaskPtr media_file_thread(new MediaFileIconThread(media_loader, shared_from_this()));
        task_group->addTask(media_file_thread);
    }
}

QString MediaFile::filePath() const
{
    return m_file_path;
}

QString MediaFile::resolvedFilePath() const
{
#if defined(Q_OS_MAC)
    // handle aliases
    return PathResolved(m_file_path);
#else
    return m_file_path;
#endif
}

QString MediaFile::source() const
{
    return m_source;
}

QIcon MediaFile::icon(const QSize &icon_size) const
{
    QMutexLocker lock(&m_icon_mutex);

    // if our icon is not valid yet, check to see if the image is valid yet.
    // if the image is valid, build the icon and cache it. otherwise, just
    // return a dummy 'loading' icon.

    if (m_icon.isNull() || icon_size != m_icon_size)
    {
        if (!m_image.isNull())
        {
            QPixmap pm(icon_size + QSize(2,3));
            pm.fill(Qt::transparent);
            QPainter p(&pm);
            float aspect_ratio = (float)m_image.width()/m_image.height();
            QRectF dest_rect = FitToAspectRatio(QRect(QPoint(), icon_size), aspect_ratio);
            p.drawImage(dest_rect, m_image);
            p.setPen(Qt::gray);
            p.drawRect(dest_rect);
            QLinearGradient v_gradient(dest_rect.bottomLeft(), dest_rect.bottomLeft()+QPoint(0,4));
            v_gradient.setColorAt(0.0, QColor(Qt::gray));
            v_gradient.setColorAt(1.0, QColor(Qt::white));
            QBrush h_brush(v_gradient);
            QRectF h_rect = dest_rect;
            h_rect.setLeft(dest_rect.left() + 2);
            h_rect.setBottom(dest_rect.bottom() + 4);
            h_rect.setTop(dest_rect.bottom());
            p.fillRect(h_rect, h_brush);
            QLinearGradient h_gradient(dest_rect.topRight(), dest_rect.topRight()+QPoint(3,0));
            h_gradient.setColorAt(0.0, QColor(Qt::gray));
            h_gradient.setColorAt(1.0, QColor(Qt::white));
            QBrush v_brush(h_gradient);
            QRectF v_rect = dest_rect;
            v_rect.setLeft(dest_rect.right());
            v_rect.setRight(dest_rect.right() + 2);
            v_rect.setTop(dest_rect.top() + 2);
            p.fillRect(v_rect, v_brush);
            m_icon = QIcon(pm);
            m_icon_size = icon_size;
        }
        else
        {
            QPixmap empty(icon_size + QSize(2,3));
            empty.fill();
            QPainter painter(&empty);
            painter.setRenderHint(QPainter::Antialiasing);
            QPainterPath path1;
            path1.addRect(1,1,icon_size.width()-2,icon_size.height()-2);
            QPainterPath path2;
            path2.addEllipse(icon_size.width()/2-10, icon_size.height()/2-10, 20, 20);
            painter.strokePath(path1, QPen(Qt::black));
            painter.strokePath(path2, QPen(Qt::red));
            return QIcon(empty);
        }
    }
    return m_icon;
}

void MediaFile::setImage(const QImage &image)
{
    QMutexLocker lock(&m_icon_mutex);
    m_icon_loaded = true;
    m_image = image;
}

void MediaFile::resetImage()
{
    QMutexLocker lock(&m_icon_mutex);
    if (!m_icon_loaded)
    {
        m_icon_loading = false;
    }
}

void MediaFile::loadAudioInfo(MediaLoader *media_loader, MediaBrowserPrivate::TaskGroupPtr task_group)
{
    QMutexLocker lock(&m_audio_info_mutex);
    if (!m_audio_info_loading && !m_audio_info_loaded)
    {
        // temporary information
        QFileInfo file_info(m_file_path);
        m_audio_info = AudioMediaFileInfo(m_file_path, file_info.fileName(), QString(), 0);

        m_audio_info_loading = true;
        MediaBrowserPrivate::TaskPtr media_file_thread(new MediaFileAudioInfoThread(media_loader, shared_from_this()));
        task_group->addTask(media_file_thread);
    }
}

void MediaFile::setAudioInfo(const AudioMediaFileInfo &audio_info)
{
    QMutexLocker lock(&m_audio_info_mutex);
    m_audio_info_loaded = true;
    m_audio_info = audio_info;
}

void MediaFile::resetAudioInfo()
{
    QMutexLocker lock(&m_audio_info_mutex);
    if (!m_audio_info_loaded)
    {
        m_audio_info_loading = false;
    }
}
