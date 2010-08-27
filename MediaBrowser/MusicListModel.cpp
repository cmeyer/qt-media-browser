#include <math.h>
#include <QDebug>
#include <QFileInfo>
#include "MusicListModel.h"
#include "LibraryTreeItem.h"
#include "MediaLoader.h"
#include "MediaFile.h"
#include "TaskGroup.h"

static QString DurationAsSeconds(unsigned duration_as_seconds)
{
    int hours = (int) (duration_as_seconds/3600);
    int minutes = (int) ((duration_as_seconds - hours*3600)/60);
    int seconds = (int) (duration_as_seconds - hours*3600 - minutes*60);
    if ( hours > 0 )
    {
        QString result;
        QTextStream ts(&result);
        ts.setPadChar('0');
        ts << hours
           << ":" << qSetFieldWidth(2) << minutes << qSetFieldWidth(0)
           << ":" << qSetFieldWidth(2) << seconds << qSetFieldWidth(0);
        return result;
    }
    else
    {
        QString result;
        QTextStream ts(&result);
        ts.setPadChar('0');
        ts << minutes << ":" << qSetFieldWidth(2) << seconds << qSetFieldWidth(0);
        return result;
    }
}

MusicListModel::MusicListModel(QObject *parent)
    : MediaListModel(parent)
    , m_task_group(new MediaBrowserPrivate::TaskGroup(MediaFileTaskPool::instance()))
{
    MediaLoaderPtr media_loader(new MediaLoader());
    m_media_loader = media_loader;

    // connect the media loader signal mediaLoaded to the method in this class.
    // this allows this class to redisplay the icon when it becomes available.
    connect(m_media_loader.get(), SIGNAL(mediaLoaded(MediaFilePtr)), this, SLOT(mediaLoaded(MediaFilePtr)));
}

MusicListModel::~MusicListModel()
{
    disconnect(m_media_loader.get(), SIGNAL(mediaLoaded(MediaFilePtr)), this, SLOT(mediaLoaded(MediaFilePtr)));
}

int MusicListModel::dataColumnCount() const
{
    return 3;
}

QVariant MusicListModel::displayFromMediaFile(MediaFilePtr media_file, int column) const
{
    // asking the media loader for an icon may return a placeholder.
    // if the placeholder is returned, the media loader will signal
    // mediaLoaded when the icon becomes available. this class needs
    // to be prepared to handle those signals.
    AudioMediaFileInfo audio_info = m_media_loader->mediaFileAudioInfo(media_file, m_task_group);

    switch (column)
    {
        case 0: return audio_info.title();
        case 1: return audio_info.artist();
        case 2: return DurationAsSeconds(audio_info.duration());
    }

    return QVariant();
}

QVariant MusicListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    switch (section)
    {
        case 0: return tr("Title");
        case 1: return tr("Artist");
        case 2: return tr("Duration");
    }

    return QVariant();
}

QVariant MusicListModel::sortValueFromMediaFile(MediaFilePtr media_file, int column) const
{
    if (column == 2)
    {
        AudioMediaFileInfo audio_info = m_media_loader->mediaFileAudioInfo(media_file, m_task_group);
        return audio_info.duration();
    }
    else
    {
        return displayFromMediaFile(media_file, column);
    }
}

void MusicListModel::resetTasks(const QModelIndexList &index_list)
{
    // first remove everything from the queue
    m_task_group->cancelAllTasks();

    // now prioritize the display items first
    Q_FOREACH(const QModelIndex &index, index_list)
    {
        data(index, Qt::DisplayRole);
    }

    // now the rest of the items
    for(int i=0; i<rowCount(); i++)
    {
        QModelIndex index = this->index(i,0);
        data(index, Qt::DisplayRole);
    }
}
