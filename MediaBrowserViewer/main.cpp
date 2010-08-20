#include <QtGui/QApplication>
#include "MainWindow.h"
#include <MediaBrowser/MediaBrowser.h>

#if defined(Q_OS_MAC)
#include <Foundation/NSAutoreleasePool.h>
#endif

int main(int argc, char *argv[])
{
#if defined(Q_OS_MAC)
    /* NSAutoreleasePool *pool = */ [[NSAutoreleasePool alloc] init];
#endif

    QApplication a(argc, argv);
    Q_INIT_RESOURCE(MediaBrowser);
    MainWindow w;
    w.show();
    return a.exec();
}
