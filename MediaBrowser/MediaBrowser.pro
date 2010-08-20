TEMPLATE = lib
CONFIG += staticlib no_keywords stl
QT += xmlpatterns xml
INCLUDEPATH += ../boost ..
INCLUDEPATH += ../taglib/include
DEPENDPATH += ../boost
DEFINES += TAGLIB_STATIC=1 # necessary to avoid linker errors
DEFINES += USE_TAGLIB=1
DEFINES += LQ_BUILD_QT=1

SOURCES += \
    FolderParser.cpp \
    LibraryTreeItem.cpp \
    LibraryTreeController.cpp \
    LibraryTreeModel.cpp \
    MediaBrowserView.cpp \
    MediaFile.cpp \
    MediaListModel.cpp \
    MediaLoader.cpp \
    MediaParser.cpp \
    MusicBrowserView.cpp \
    MusicFolderParser.cpp \
    MusicListModel.cpp \
    PhotoBrowserView.cpp \
    PhotoFolderParser.cpp \
    PhotoListModel.cpp \
    TaskGroup.cpp \
    TaskPool.cpp \

HEADERS += \
    MediaBrowser.h \
    FolderParser.h \
    LibraryTreeItem.h \
    LibraryTreeController.h \
    LibraryTreeModel.h \
    MediaBrowser_pch.h \
    MediaBrowserView.h \
    MediaFile.h \
    MediaListModel.h \
    MediaLoader.h \
    MediaParser.h \
    MusicBrowserView.h \
    MusicFolderParser.h \
    MusicListModel.h \
    PhotoBrowserView.h \
    PhotoFolderParser.h \
    PhotoListModel.h \
    TaskGroup.h \
    TaskPool.h \

macx {
    DEFINES += MAC_OS_X_VERSION_MIN_REQUIRED=1040
    #DEFINES += MAC_OS_X_VERSION_MAX_ALLOWED=1040

    OBJECTIVE_SOURCES = MacUtility.mm
    SOURCES += \
        ApertureLibraryParser.cpp \
        iPhotoLibraryParser.cpp \
        iTunesLibraryParser.cpp
    HEADERS += \
        ApertureLibraryParser.h \
        iPhotoLibraryParser.h \
        iTunesLibraryParser.h \
        MacUtility.h

    QMAKE_CFLAGS_WARN_ON = -Wreturn-type -Wunused-variable
    QMAKE_CXXFLAGS_WARN_ON = -Wreturn-type -Wunused-variable
    QMAKE_CXXFLAGS += -x objective-c++
    QMAKE_CFLAGS += -x objective-c++

    CONFIG(release, debug|release) {
        OBJECTS_DIR = obj/release
        DESTDIR = build/release
        QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.4
        #QMAKE_MAC_SDK=/Developer/SDKs/MacOSX10.4u.sdk
        CONFIG+=x86 ppc
    }

    CONFIG(debug, debug|release) {
        OBJECTS_DIR = obj/debug
        DESTDIR = build/debug
        PRECOMPILED_HEADER = MediaBrowser_pch.h
        CONFIG += precompile_header
    }

    INCLUDEPATH += /Users/cmeyer/Developer/Qt/qt-mobility/install/include/QtMultimediaKit
    INCLUDEPATH += /Users/cmeyer/Developer/Qt/qt-mobility/install/include/QtMobility
}

win32 {
    win32-msvc2008 {
        DEFINES += BOOST_ALL_NO_LIB
        QMAKE_CXXFLAGS_WARN_ON = -w44100
    }
}

TRANSLATIONS += \
    Localizations/MediaBrowser_da.ts \
    Localizations/MediaBrowser_de.ts \
    Localizations/MediaBrowser_es.ts \
    Localizations/MediaBrowser_fi.ts \
    Localizations/MediaBrowser_fr.ts \
    Localizations/MediaBrowser_it.ts \
    Localizations/MediaBrowser_ja.ts \
    Localizations/MediaBrowser_nl.ts \
    Localizations/MediaBrowser_no.ts \
    Localizations/MediaBrowser_sv.ts \
    Localizations/MediaBrowser_zh.ts

!isEmpty(TRANSLATIONS) {
    isEmpty(QMAKE_LRELEASE) {
        win32:QMAKE_LRELEASE = $$[QT_INSTALL_BINS]\lrelease.exe
        else:QMAKE_LRELEASE = $$[QT_INSTALL_BINS]/lrelease
    }
    isEmpty(TS_DIR):TS_DIR = Localizations
    TSQM.name = lrelease ${QMAKE_FILE_IN}
    TSQM.input = TRANSLATIONS
    TSQM.output = $$TS_DIR/${QMAKE_FILE_BASE}.qm
    TSQM.commands = $$QMAKE_LRELEASE ${QMAKE_FILE_IN}
    TSQM.CONFIG = no_link
    QMAKE_EXTRA_COMPILERS += TSQM
    PRE_TARGETDEPS += compiler_TSQM_make_all
    } else:message(No translation files in project)

RESOURCES += MediaBrowser.qrc
