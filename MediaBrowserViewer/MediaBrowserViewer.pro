TARGET = MediaBrowserViewer
TEMPLATE = app

LOCALIZATIONS.files = \
    ../MediaBrowser/Localizations/MediaBrowser_da.qm \
    ../MediaBrowser/Localizations/MediaBrowser_de.qm \
    ../MediaBrowser/Localizations/MediaBrowser_es.qm \
    ../MediaBrowser/Localizations/MediaBrowser_fi.qm \
    ../MediaBrowser/Localizations/MediaBrowser_fr.qm \
    ../MediaBrowser/Localizations/MediaBrowser_it.qm \
    ../MediaBrowser/Localizations/MediaBrowser_ja.qm \
    ../MediaBrowser/Localizations/MediaBrowser_nl.qm \
    ../MediaBrowser/Localizations/MediaBrowser_no.qm \
    ../MediaBrowser/Localizations/MediaBrowser_sv.qm \
    ../MediaBrowser/Localizations/MediaBrowser_zh.qm

win32 {
    win32-g++ {
        CONFIG(debug, debug|release) {
            LINKLIBS += \
                ../MediaBrowser/debug/libMediaBrowser.a \
#                ../taglib/taglib/build/debug/libTagLib.a
        }
        CONFIG(release, debug|release) {
            LINKLIBS += \
                ../MediaBrowser/release/libMediaBrowser.a \
#                ../taglib/taglib/build/release/libTagLib.a
        }
        LINKLIBS += \
            "../QuickTime SDK/Libraries/QTMLClient.lib"
    }
    win32-msvc2008 {
        DEFINES += BOOST_ALL_NO_LIB
        QMAKE_CXXFLAGS_WARN_ON = -w44100
        CONFIG(debug, debug|release) {
            LINKLIBS += \
                C:/QtMobility/lib/QtMultimediaKitd1.lib \
                ../MediaBrowser/debug/MediaBrowserd.lib \
#                ../taglib/taglib/build/debug/TagLib.lib
            # See http://msdn.microsoft.com/en-us/library/aa267384%28VS.60%29.aspx?ppud=4
            # Use /MDd option Multithreaded using DLL (msvcrt.lib)
            QMAKE_LFLAGS += /VERBOSE:LIB /NODEFAULTLIB:libc.lib /NODEFAULTLIB:libcmt.lib /NODEFAULTLIB:libcd.lib /NODEFAULTLIB:libcmtd.lib /NODEFAULTLIB:msvcrt.lib /NODEFAULTLIB:msvcprt.lib
            #QMAKE_LFLAGS += /VERBOSE:LIB
        }
        CONFIG(release, debug|release) {
            LINKLIBS += \
                C:/QtMobility/lib/QtMultimediaKit1.lib \
                ../MediaBrowser/release/MediaBrowser.lib \
#                ../taglib/taglib/build/release/TagLib.lib
            # See http://msdn.microsoft.com/en-us/library/aa267384%28VS.60%29.aspx?ppud=4
            # Use /MD option Multithreaded using DLL (msvcrt.lib)
            QMAKE_LFLAGS += /NODEFAULTLIB:libc.lib /NODEFAULTLIB:libcmt.lib /NODEFAULTLIB:libcd.lib /NODEFAULTLIB:libcmtd.lib /NODEFAULTLIB:msvcrtd.lib /NODEFAULTLIB:msvcprtd.lib
            #QMAKE_LFLAGS += /VERBOSE:LIB
        }
        LINKLIBS += \
            "$$(DXSDK_DIR)/Lib/x86/dxguid.lib" \
            "$$(DXSDK_DIR)/../../Program Files/Microsoft SDKs/Windows/v7.0/Lib/dmoguids.lib" \
            "$$(DXSDK_DIR)/../../Program Files/Microsoft SDKs/Windows/v7.0/Lib/msdmo.lib" \
            "$$(DXSDK_DIR)/../../Program Files/Microsoft SDKs/Windows/v7.0/Lib/Ole32.lib" \
            "$$(DXSDK_DIR)/../../Program Files/Microsoft SDKs/Windows/v7.0/Lib/strmiids.lib" \
            "$$(DXSDK_DIR)/../../Program Files/Microsoft SDKs/Windows/v7.0/Lib/WinMM.lib" \
            "$$(DXSDK_DIR)/../../Program Files/Microsoft SDKs/Windows/v7.0/Lib/Wmvcore.lib" \
            "$$(DXSDK_DIR)/../../Program Files/Microsoft SDKs/Windows/v7.0/Lib/Rpcrt4.lib"
    }
    system( $$QMAKE_MKDIR \"$$DLLDIR\Localizations\" )
    message( $$QMAKE_MKDIR \"$$DLLDIR\Localizations\" )
    for(LOCALIZATION,LOCALIZATIONS.files) {
        message( copying \"$$replace(LOCALIZATION, /,$$DIR_SEPARATOR)\" -> \"$$DLLDIR\Localizations\" ... )
        system( $$QMAKE_COPY \"$$replace(LOCALIZATION, /,$$DIR_SEPARATOR)\" \"$$DLLDIR\Localizations\" )
    }
    POST_TARGETDEPS += $$LINKLIBS
    WINLIBS += -lkernel32 -luser32 -lgdi32 -lwinspool -lcomdlg32 -ladvapi32 -lshell32 -lole32 -loleaut32 -luuid -lodbc32 -lodbccp32
    LIBS += $$LINKLIBS $$WINLIBS -lopengl32 -lglu32
    INCLUDEPATH += \
                "../QuickTime SDK/CIncludes" \
                ../DirectShowAudio
    win32-g++ {
        QMAKE_CXXFLAGS += -x c++
        QMAKE_CXXFLAGS_WARN_ON = -Wno-multichar
    }
    #RC_FILE = Application.rc
}
macx {

    QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.6

    CONFIG(debug, debug|release) {
        LINKLIBDIR = debug
    }
    CONFIG(release, debug|release) {
        LINKLIBDIR = release
    }
#    LINKLIBS += \
#        ../taglib/taglib/build/$$LINKLIBDIR/libTagLib.a
    POST_TARGETDEPS += $$LINKLIBS
    LIBS += -F../MediaBrowser/build/lib -framework MediaBrowser -framework QtMultimediaKit
    LIBS += $$LINKLIBS -framework QuickTime -framework Carbon -framework Cocoa -framework CoreFoundation
    QMAKE_CFLAGS_WARN_ON = -Wreturn-type  -Wunused-variable
    QMAKE_CXXFLAGS_WARN_ON = -Wreturn-type -Wunused-variable
    QMAKE_CXXFLAGS += -x objective-c++
    QMAKE_CFLAGS += -x objective-c++
    QMAKE_INFO_PLIST = Info.plist
    LOCALIZATIONS.path = Contents/Resources/Localizations
    QMAKE_BUNDLE_DATA += LOCALIZATIONS
    CONFIG(debug, debug|release) {
        OBJECTS_DIR = obj/debug
        DESTDIR = build/debug
    }
    CONFIG(release, debug|release) {
        OBJECTS_DIR = obj/release
        DESTDIR = build/release
        CONFIG += x86 ppc x86_64
    }
}

SOURCES += \
        main.cpp \
        MainWindow.cpp

HEADERS += MainWindow.h

INCLUDEPATH += ../boost .. ../taglib/include
DEPENDPATH += ../boost .. ../taglib/include
