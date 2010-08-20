#ifndef MEDIA_BROWSER_IPHOTO_LIBRARY_PARSER_H
#define MEDIA_BROWSER_IPHOTO_LIBRARY_PARSER_H

#include "FolderParser.h"

class iPhotoLibraryParserThread;

class iPhotoLibraryParser : public MediaParser
{
public:
    iPhotoLibraryParser(LibraryTreeControllerPtr library_tree_controller, const QString &file_path, const QString &display_name);

    // from MediaParser
    virtual void start();
    virtual void cancel();

private:
    boost::shared_ptr<iPhotoLibraryParserThread> m_iphoto_library_parser_thread;

    friend class iPhotoLibraryParserThread;
};

#endif // MEDIA_BROWSER_IPHOTO_LIBRARY_PARSER_H
