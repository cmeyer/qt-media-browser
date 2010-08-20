#include <QDateTime>
#include <QDebug>
#include <QFile>
#include <QMap>
#include <QMapIterator>
#include <QSettings>
#include <QStringList>
#include <QThread>
#include <QUrl>
#include "ApertureLibraryParser.h"
#include "LibraryTreeItem.h"
#include "LibraryTreeController.h"
#include "MacUtility.h"

class ApertureLibraryParserThread : public QThread
{
public:
    ApertureLibraryParserThread(ApertureLibraryParser *aperture_library_parser, LibraryTreeControllerPtr library_tree_controller, const QString &file_path, const QString &display_name);

    virtual void run();

    void cancel();

private:
    ApertureLibraryParser *m_aperture_library_parser;
    LibraryTreeControllerPtr m_library_tree_controller;
    QString m_file_path;
    QString m_display_name;
    bool m_cancel;

    class AlbumInfo
    {
    public:
        AlbumInfo() { }
        AlbumInfo(const QString &album_id, const QString &album_name, const QString &parent, const QStringList &key_list)
            : m_album_id(album_id), m_album_name(album_name), m_parent(parent), m_key_list(key_list) { }
        QString albumId() const { return m_album_id; }
        QString albumName() const { return m_album_name; }
        QString parent() const { return m_parent; }
        QStringList keyList() const { return m_key_list; }
    private:
        QString m_album_id;
        QString m_album_name;
        QString m_parent;
        QStringList m_key_list;
    };

    QMap<QString, QString> m_photos;
    QMap<QString, AlbumInfo> m_albums;
    QString m_root_album_id;

    void parsePhotos(QSettings &plist);
    void parseAlbums(QSettings &plist);
    void parsePhotoList(LibraryTreeItemPtr base_library_tree_item, const QString &album_id);
};

ApertureLibraryParserThread::ApertureLibraryParserThread(ApertureLibraryParser *aperture_library_parser, LibraryTreeControllerPtr library_tree_controller, const QString &file_path, const QString &display_name)
    : QThread()
    , m_aperture_library_parser(aperture_library_parser)
    , m_library_tree_controller(library_tree_controller)
    , m_file_path(file_path)
    , m_display_name(display_name)
    , m_cancel(false)
{
}

void ApertureLibraryParserThread::cancel()
{
    m_cancel = true;
}

void ApertureLibraryParserThread::parsePhotos(QSettings &plist)
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

void ApertureLibraryParserThread::parseAlbums(QSettings &plist)
{
    QList<QVariant> album_list = plist.value("List of Albums").toList();

    Q_FOREACH(const QVariant &album, album_list)
    {
        QMultiMap<QString, QVariant> album_info = album.toMap();
        QString album_id = album_info.value("AlbumId").toString().trimmed();
        QString parent_id = album_info.value("Parent").toString().trimmed();
        QString name = album_info.value("AlbumName").toString().trimmed();
        QStringList photo_id_strs = QVariant(album_info.values("KeyList")).toStringList();
        m_albums.insert(album_id, AlbumInfo(album_id, name, parent_id, photo_id_strs));
        if (parent_id.isEmpty())
            m_root_album_id = album_id;
    }
}

void ApertureLibraryParserThread::parsePhotoList(LibraryTreeItemPtr base_library_tree_item, const QString &album_id)
{
    QMapIterator<QString, AlbumInfo> album_iter(m_albums);
    while (album_iter.hasNext())
    {
        if (m_cancel) break;
        album_iter.next();
        const AlbumInfo &album_info(album_iter.value());
        if (album_info.parent() == album_id)
        {
            LibraryTreeItemPtr library_tree_item = m_library_tree_controller->appendChild(base_library_tree_item, album_info.albumName());
            parsePhotoList(library_tree_item, album_info.albumId());
        }
    }

    if (m_cancel)
        return;

    //ASSERT_EXCEPTION(m_albums.contains(album_id));
    
    if (m_albums.contains(album_id))
    {
        Q_FOREACH(const QString &key, m_albums[album_id].keyList())
        {
            if (m_cancel) break;
            if (m_photos.contains(key))
                m_library_tree_controller->appendFile(base_library_tree_item, m_photos[key]);
        }
    }
}

void ApertureLibraryParserThread::run()
{
    LibraryTreeItemPtr parent_library_tree_item = m_library_tree_controller->liveRootLibraryTreeItem();
    LibraryTreeItemPtr base_library_tree_item;

    QFile file(m_file_path);
    if (file.exists())
    {
        base_library_tree_item = m_library_tree_controller->appendChild(parent_library_tree_item, m_display_name);

        QSettings plist(m_file_path, QSettings::NativeFormat);

        unsigned major_version = plist.value("Major Version").toUInt();
        if (major_version == 1 || major_version == 2)
        {
            parsePhotos(plist);
            parseAlbums(plist);
            parsePhotoList(base_library_tree_item, m_root_album_id);
        }
    }
}

ApertureLibraryParser::ApertureLibraryParser(LibraryTreeControllerPtr library_tree_controller, const QString &file_path, const QString &display_name)
        : MediaParser(library_tree_controller)
{
    boost::shared_ptr<ApertureLibraryParserThread> aperture_library_parser_thread(new ApertureLibraryParserThread(this, library_tree_controller, file_path, display_name));
    m_aperture_library_parser_thread = aperture_library_parser_thread;
}

void ApertureLibraryParser::start()
{
    m_aperture_library_parser_thread->start();
}

void ApertureLibraryParser::cancel()
{
    m_aperture_library_parser_thread->cancel();
    m_aperture_library_parser_thread->wait();
}
