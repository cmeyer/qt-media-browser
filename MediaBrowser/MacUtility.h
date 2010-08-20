#ifndef MEDIA_BROWSER_MAC_UTILITY_H
#define MEDIA_BROWSER_MAC_UTILITY_H

#include <CoreFoundation/CFString.h>
#include <QString>

QString PathForIMovie();
QStringList iPhotoRecentDatabasePaths();
QStringList iTunesRecentDatabasePaths();
QStringList ApertureRecentDatabasePaths();
QString PathResolved(const QString &path_q);

namespace MediaBrowserPrivate {
    QString CFStringToQString(CFStringRef cf_string);
}

#endif // MEDIA_BROWSER_MAC_UTILITY_H
