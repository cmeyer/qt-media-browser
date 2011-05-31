// Copyright 2011. LQ Graphics, Inc. and Chris Meyer. LGPL License. See LICENSE file.

#include <AppKit/AppKit.h>
#include <Foundation/Foundation.h>
#include <QDebug>
#include <QString>
#include <QStringList>
#include <QUrl>
#include <QVarLengthArray>

#include "MacUtility.h"

QString PathForIMovie()
{
    NSString *imovie_path = [[NSWorkspace sharedWorkspace] absolutePathForAppBundleWithIdentifier:@"com.apple.iMovie"];
    NSString *app_path = [imovie_path stringByAppendingPathComponent:@"/Contents/Resources/Sound Effects/"];
    if (app_path != NULL)
        return MediaBrowserPrivate::CFStringToQString((CFStringRef)app_path);
    return QString();
}

QStringList recentDatabasePaths(NSString *key)
{
    NSArray *libraries = [(NSArray *)CFPreferencesCopyAppValue((CFStringRef)key,(CFStringRef)@"com.apple.iApps") autorelease];
    QStringList paths;

    NSEnumerator *enumerator = [libraries objectEnumerator];
    NSString *url_string;
    while (url_string = [enumerator nextObject])
    {
        QUrl url(MediaBrowserPrivate::CFStringToQString((CFStringRef)url_string));
        QString path = url.toLocalFile();
        path.remove("//localhost");
        if (!path.isEmpty())
            paths.push_back(path);
    }

    return paths;
}

QStringList iPhotoRecentDatabasePaths()
{
    return recentDatabasePaths(@"iPhotoRecentDatabases");
}

QStringList iTunesRecentDatabasePaths()
{
    return recentDatabasePaths(@"iTunesRecentDatabases");
}

QStringList ApertureRecentDatabasePaths()
{
    return recentDatabasePaths(@"ApertureLibraries");
}


// Will resolve an alias into a path.. this code was taken from
// see http://cocoa.karelia.com/Foundation_Categories/
// see http://developer.apple.com/documentation/Cocoa/Conceptual/LowLevelFileMgmt/Tasks/ResolvingAliases.html
QString PathResolved(const QString &path_q)
{
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];

    NSString *path = [NSString stringWithCharacters:(const unichar *)path_q.unicode() length:path_q.length()];

    NSString *resolvedPath = NULL;

    CFURLRef url = CFURLCreateWithFileSystemPath(NULL /*allocator*/, (CFStringRef)path, kCFURLPOSIXPathStyle, NO /*isDirectory*/);
    if (url != NULL)
    {
        FSRef fsRef;
        if (CFURLGetFSRef(url, &fsRef))
        {
            Boolean targetIsFolder, wasAliased;
            if (FSResolveAliasFile (&fsRef, true /*resolveAliasChains*/,
                                    &targetIsFolder, &wasAliased) == noErr && wasAliased)
            {
                CFURLRef resolvedUrl = CFURLCreateFromFSRef(NULL, &fsRef);
                if (resolvedUrl != NULL)
                {
                    resolvedPath = (NSString *)CFURLCopyFileSystemPath(resolvedUrl, kCFURLPOSIXPathStyle);
                    CFRelease(resolvedUrl);
                    resolvedPath = [resolvedPath autorelease];
                }
            }
        }
        CFRelease(url);
    }

    if ( resolvedPath == NULL )
        resolvedPath = [[path copy] autorelease];

    QString q_resolved_path = MediaBrowserPrivate::CFStringToQString((CFStringRef)resolvedPath);

    [pool release];

    return q_resolved_path;
}

namespace MediaBrowserPrivate {

QString CFStringToQString(CFStringRef cf_string)
{
    QString string;
    if (cf_string != NULL)
    {
        CFIndex cf_string_length = CFStringGetLength(cf_string);
        QVarLengthArray<UniChar> cf_string_buffer(cf_string_length);
        CFStringGetCharacters(cf_string, CFRangeMake(0, cf_string_length), cf_string_buffer.data());
        string = QString(reinterpret_cast<const QChar *>(cf_string_buffer.constData()), cf_string_length);
    }
    return string;
}

}
