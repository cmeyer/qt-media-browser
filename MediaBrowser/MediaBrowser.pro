# Copyright 2011. LQ Graphics, Inc. and Chris Meyer. LGPL License. See LICENSE file.

include(../config.pri)

TEMPLATE = lib
CONFIG += no_keywords stl absolute_library_soname
QT += xmlpatterns xml
INCLUDEPATH += ../boost ..
INCLUDEPATH += ../taglib/include
DEPENDPATH += ../boost
DEFINES += LQ_BUILD_QT=1

CONFIG += debug_and_release build_all

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
    SearchField.cpp \
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
    SearchField.h \
    TaskGroup.h \
    TaskPool.h \

# see http://www.qtcentre.org/threads/30564-What-Does-qtLibraryTarget()-Do
defineReplace(qtLibraryTarget) {
   unset(LIBRARY_NAME)
   LIBRARY_NAME = $$1
   mac:!static:contains(QT_CONFIG, qt_framework) {
      QMAKE_FRAMEWORK_BUNDLE_NAME = $$LIBRARY_NAME
      export(QMAKE_FRAMEWORK_BUNDLE_NAME)
   }
   contains(TEMPLATE, .*lib):CONFIG(debug, debug|release) {
      !debug_and_release|build_pass {
          mac:RET = $$member(LIBRARY_NAME, 0)_debug
          else:win32:RET = $$member(LIBRARY_NAME, 0)d
      }
   }
   isEmpty(RET):RET = $$LIBRARY_NAME
   return($$RET)
}

TARGET = $$qtLibraryTarget(MediaBrowser)

macx {
	DEFINES += TAGLIB_STATIC=1 # necessary to avoid linker errors
    DEFINES += USE_TAGLIB=0

    DEFINES += MAC_OS_X_VERSION_MIN_REQUIRED=1040
    #DEFINES += MAC_OS_X_VERSION_MAX_ALLOWED=1040

    CONFIG += lib_bundle

    CONFIG(debug, debug|release) {
        LINKLIBDIR = debug
    }
    CONFIG(release, debug|release) {
        LINKLIBDIR = release
    }

    LINKLIBS += \
        ../taglib/taglib/build/$$LINKLIBDIR/libTagLib.a

    LIBS += $$LINKLIBS -framework Cocoa
    LIBS += -framework QtMultimediaKit

    FRAMEWORK_HEADERS.version = Versions
    FRAMEWORK_HEADERS.files = MediaBrowser.h MediaBrowserView.h MusicBrowserView.h PhotoBrowserView.h
    FRAMEWORK_HEADERS.path = Headers
    QMAKE_BUNDLE_DATA += FRAMEWORK_HEADERS

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
        OBJECTS_DIR = build/release/.obj
        DESTDIR = build/lib # same DESTDIR for both debug and release to get both versions in the same directory
        PRECOMPILED_HEADER = MediaBrowser_pch.h
        QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.4
        #QMAKE_MAC_SDK=/Developer/SDKs/MacOSX10.4u.sdk
        CONFIG+=x86 ppc
    }

    CONFIG(debug, debug|release) {
        OBJECTS_DIR = build/debug/.obj
        DESTDIR = build/lib # same DESTDIR for both debug and release to get both versions in the same directory
        PRECOMPILED_HEADER = MediaBrowser_pch.h
        CONFIG += precompile_header
    }

    INCLUDEPATH += $$[QT_INSTALL_HEADERS]/QtMultimediaKit
    INCLUDEPATH += $$[QT_INSTALL_HEADERS]/QtMobility

    target.path = $${MBP_PREFIX}
    INSTALLS += target
}

win32 {
	DEFINES += USE_TAGLIB=0

    win32-msvc2008 {
        INCLUDEPATH += ../QtMobility/include/QtMultimediaKit
        INCLUDEPATH += ../QtMobility/include/QtMobility

        CONFIG += staticlib
        DEFINES += BOOST_ALL_NO_LIB
        QMAKE_CXXFLAGS_WARN_ON = -w44100
    }
}

macx {
    LOCALIZATIONS.path = Localizations
    LOCALIZATIONS.files += \
        Localizations/MediaBrowser_da.qm \
        Localizations/MediaBrowser_de.qm \
        Localizations/MediaBrowser_es.qm \
        Localizations/MediaBrowser_fi.qm \
        Localizations/MediaBrowser_fr.qm \
        Localizations/MediaBrowser_it.qm \
        Localizations/MediaBrowser_ja.qm \
        Localizations/MediaBrowser_nl.qm \
        Localizations/MediaBrowser_no.qm \
        Localizations/MediaBrowser_sv.qm \
        Localizations/MediaBrowser_zh.qm
    QMAKE_BUNDLE_DATA += LOCALIZATIONS
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
        win32:QMAKE_LRELEASE = $$[QT_INSTALL_BINS]/lrelease.exe
        else:QMAKE_LRELEASE = $$[QT_INSTALL_BINS]/lrelease
    }
    isEmpty(TS_DIR):TS_DIR = Localizations
    TSQM.name = lrelease ${QMAKE_FILE_IN}
    TSQM.input = TRANSLATIONS
    TSQM.output = $$TS_DIR/${QMAKE_FILE_BASE}.qm
    TSQM.commands = $$QMAKE_LRELEASE ${QMAKE_FILE_IN}
    TSQM.CONFIG = no_link target_predeps
    QMAKE_EXTRA_COMPILERS += TSQM
    PRE_TARGETDEPS += compiler_TSQM_make_all
    } else:message(No translation files in project)

RESOURCES += MediaBrowser.qrc
