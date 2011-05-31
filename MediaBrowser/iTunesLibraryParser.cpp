#include <QDateTime>
#include <QDebug>
#include <QFile>
#include <QSettings>
#include <QStringList>
#include <QThread>
#include <QXmlQuery>
#include <QXmlResultItems>
#include "iTunesLibraryParser.h"
#include "LibraryTreeItem.h"
#include "LibraryTreeController.h"
#include "MediaFile.h"

class iTunesLibraryParserThread : public QThread
{
public:
    iTunesLibraryParserThread(iTunesLibraryParser *itunes_library_parser, LibraryTreeControllerPtr library_tree_controller, const QString &file_path, const QString &display_name);

    virtual void run();

    void cancel();

private:
    iTunesLibraryParser *m_itunes_library_parser;
    LibraryTreeControllerPtr m_library_tree_controller;
    QString m_file_path;
    QString m_display_name;
    bool m_cancel;

    QMap<QString, AudioMediaFileInfo> m_tracks;

    void parseTracks(QSettings &plist);
    void parse();
};

iTunesLibraryParserThread::iTunesLibraryParserThread(iTunesLibraryParser *itunes_library_parser, LibraryTreeControllerPtr library_tree_controller, const QString &file_path, const QString &display_name)
    : QThread()
    , m_itunes_library_parser(itunes_library_parser)
    , m_library_tree_controller(library_tree_controller)
    , m_file_path(file_path)
    , m_display_name(display_name)
    , m_cancel(false)
{
}

void iTunesLibraryParserThread::cancel()
{
    m_cancel = true;
}

void iTunesLibraryParserThread::parseTracks(QSettings &plist)
{
    QMap<QString, QVariant> track_list = plist.value("Tracks").toMap();

    Q_FOREACH(const QString &track_key, track_list.keys())
    {
        if (m_cancel) break;

        QMap<QString, QVariant> track_info = track_list.value(track_key).toMap();

        QString location_url_str = track_info.value("Location").toString().trimmed();

        QString title_str = track_info.value("Name").toString().trimmed();

        QString artist_str = track_info.value("Artist").toString().trimmed();

        float duration = track_info.value("Total Time").toInt() / 1000.0;

        QUrl location_url(location_url_str.trimmed());
        QString location_path = location_url.toLocalFile();
        location_path.remove("//localhost");
        location_path = QUrl::fromPercentEncoding(location_path.toUtf8());

        // insert info into the tracks map
        if (!location_path.isEmpty())
        {
            AudioMediaFileInfo audio_media_file_info(location_path, title_str, artist_str, duration);
            m_tracks[track_key.trimmed()] = audio_media_file_info;
        }

        // debug path problems
        //            QFile file(location_path);
        //            if (!file.exists())
        //                qDebug() << location_path;
    }
}

void iTunesLibraryParserThread::parse()
{
    LibraryTreeItemPtr parent_library_tree_item = m_library_tree_controller->liveRootLibraryTreeItem();
    LibraryTreeItemPtr base_library_tree_item;

    QFile file(m_file_path);
    if (file.exists())
    {
        base_library_tree_item = m_library_tree_controller->appendChild(parent_library_tree_item, m_display_name);

        QSettings plist(m_file_path, QSettings::NativeFormat);

        QString version_str = plist.value("Application Version").toString();
        int major_version = version_str.trimmed().split('.').at(0).toUInt();
        if (major_version >= 9)
        {
            parseTracks(plist);

            QList<QVariant> playlist_list = plist.value("Playlists").toList();
            Q_FOREACH(const QVariant &playlist, playlist_list)
            {
                if (m_cancel) break;
                QMultiMap<QString, QVariant> playlist_info = playlist.toMap();
                if (playlist_info.value("Visible", true).toBool())
                {
                    QString name = playlist_info.value("Name").toString();
                    QList<QVariant> track_dict_list = playlist_info.values("Playlist Items");
                    if (!track_dict_list.empty())
                    {
                        LibraryTreeItemPtr library_tree_item = m_library_tree_controller->appendChild(base_library_tree_item, name);
                        Q_FOREACH(const QVariant &track_dict, track_dict_list)
                        {
                            if (m_cancel) break;
                            QMultiMap<QString, QVariant> track_info = track_dict.toMap();
                            QString track_id = track_info.value("Track ID").toString().trimmed();
                            AudioMediaFileInfo audio_media_file_info = m_tracks[track_id];
                            MediaFilePtr media_file(new MediaFile(audio_media_file_info.filePath(), QLatin1String("itunes")));
                            media_file->setAudioInfo(audio_media_file_info);
                            m_library_tree_controller->appendMediaFile(library_tree_item, media_file);
                        }
                    }
                }
            }
        }
    }
}

void iTunesLibraryParserThread::run()
{
    parse();
}

iTunesLibraryParser::iTunesLibraryParser(LibraryTreeControllerPtr library_tree_controller, const QString &file_path, const QString &display_name)
{
    boost::shared_ptr<iTunesLibraryParserThread> itunes_library_parser_thread(new iTunesLibraryParserThread(this, library_tree_controller, file_path, display_name));
    m_itunes_library_parser_thread = itunes_library_parser_thread;
}

void iTunesLibraryParser::start()
{
    m_itunes_library_parser_thread->start();
}

void iTunesLibraryParser::cancel()
{
    m_itunes_library_parser_thread->cancel();
    m_itunes_library_parser_thread->wait();
}
