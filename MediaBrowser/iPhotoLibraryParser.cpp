#include <QDateTime>
#include <QDebug>
#include <QFile>
#include <QSettings>
#include <QStringList>
#include <QThread>
#include <QUrl>
#include "iPhotoLibraryParser.h"
#include "LibraryTreeItem.h"
#include "LibraryTreeController.h"
#include "MacUtility.h"

class iPhotoLibraryParserThread : public QThread
{
public:
    iPhotoLibraryParserThread(iPhotoLibraryParser *iphoto_library_parser, LibraryTreeControllerPtr library_tree_controller, const QString &file_path, const QString &display_name);

    virtual void run();

    void cancel();

private:
    iPhotoLibraryParser *m_iphoto_library_parser;
    LibraryTreeControllerPtr m_library_tree_controller;
    QString m_file_path;
    QString m_display_name;
    bool m_cancel;

    QMap<QString, QString> m_photos;

    void parsePhotos(QSettings &plist);
    void parsePhotoList(QSettings &plist, LibraryTreeItemPtr base_library_tree_item, const QString &key, const QString &name_key, const QString &display_name);
};

iPhotoLibraryParserThread::iPhotoLibraryParserThread(iPhotoLibraryParser *iphoto_library_parser, LibraryTreeControllerPtr library_tree_controller, const QString &file_path, const QString &display_name)
    : QThread()
    , m_iphoto_library_parser(iphoto_library_parser)
    , m_library_tree_controller(library_tree_controller)
    , m_file_path(file_path)
    , m_display_name(display_name)
    , m_cancel(false)
{
}

void iPhotoLibraryParserThread::cancel()
{
    m_cancel = true;
}

void iPhotoLibraryParserThread::parsePhotos(QSettings &plist)
{
    QMap<QString, QVariant> master_image_list = plist.value("Master Image List").toMap();

    Q_FOREACH(const QString &image_key, master_image_list.keys())
    {
        if (m_cancel) break;

        QMap<QString, QVariant> image_info = master_image_list.value(image_key).toMap();

        QString media_type = image_info.value("MediaType").toString();

        if (media_type == "Image")
        {
            QString image_path = image_info.value("ImagePath").toString().trimmed();

            // debug path problems
//            QFile file(location_path);
//            if (!file.exists())
//                qDebug() << location_path;

            m_photos[image_key.trimmed()] = image_path;
        }
    }
}

void iPhotoLibraryParserThread::parsePhotoList(QSettings &plist, LibraryTreeItemPtr base_library_tree_item, const QString &key, const QString &name_key, const QString &display_name)
{
    LibraryTreeItemPtr library_tree_item1 = m_library_tree_controller->appendChild(base_library_tree_item, display_name);

    QList<QVariant> album_list = plist.value(key).toList();

    Q_FOREACH(const QVariant &album, album_list)
    {
        if (m_cancel) break;
        QMultiMap<QString, QVariant> album_info = album.toMap();
        QString album_type = album_info.value("Album Type").toString().trimmed();
        if (album_type != "SlideShow" && album_type != "Book")
        {
            QString name = album_info.value(name_key).toString().trimmed();
            QStringList photo_id_strs = QVariant(album_info.values("KeyList")).toStringList();
            if (!photo_id_strs.isEmpty())
            {
                LibraryTreeItemPtr library_tree_item = m_library_tree_controller->appendChild(library_tree_item1, name);
                Q_FOREACH(const QString &photo_id_str, photo_id_strs)
                {
                    QString photo_id = photo_id_str.trimmed();
                    if (m_photos.contains(photo_id))
                        m_library_tree_controller->appendFile(library_tree_item, m_photos[photo_id], QLatin1String("iphoto"));
                    if (m_cancel) break;
                }
            }
        }
    }
}

void iPhotoLibraryParserThread::run()
{
    LibraryTreeItemPtr parent_library_tree_item = m_library_tree_controller->liveRootLibraryTreeItem();
    LibraryTreeItemPtr base_library_tree_item;

    QFile file(m_file_path);
    if (file.exists())
    {
        base_library_tree_item = m_library_tree_controller->appendChild(parent_library_tree_item, m_display_name);

        QSettings plist(m_file_path, QSettings::NativeFormat);

        unsigned major_version = plist.value("Major Version").toUInt();
        if (major_version == 2)
        {
            parsePhotos(plist);
            parsePhotoList(plist, base_library_tree_item, "List of Albums", "AlbumName", tr("Albums"));
            parsePhotoList(plist, base_library_tree_item, "List of Rolls", "RollName", tr("Rolls"));
        }
    }
}

iPhotoLibraryParser::iPhotoLibraryParser(LibraryTreeControllerPtr library_tree_controller, const QString &file_path, const QString &display_name)
{
    boost::shared_ptr<iPhotoLibraryParserThread> iphoto_library_parser_thread(new iPhotoLibraryParserThread(this, library_tree_controller, file_path, display_name));
    m_iphoto_library_parser_thread = iphoto_library_parser_thread;
}

void iPhotoLibraryParser::start()
{
    m_iphoto_library_parser_thread->start();
}

void iPhotoLibraryParser::cancel()
{
    m_iphoto_library_parser_thread->cancel();
    m_iphoto_library_parser_thread->wait();
}
