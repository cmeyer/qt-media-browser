#ifndef MEDIA_BROWSER_ITUNES_LIBRARY_PARSER_H
#define MEDIA_BROWSER_ITUNES_LIBRARY_PARSER_H

#include "FolderParser.h"

class iTunesLibraryParserThread;

class iTunesLibraryParser : public MediaParser
{
public:
    iTunesLibraryParser(LibraryTreeControllerPtr library_tree_controller, const QString &file_path, const QString &display_name);

    // from MediaParser
    virtual void start();
    virtual void cancel();

private:
    boost::shared_ptr<iTunesLibraryParserThread> m_itunes_library_parser_thread;

    friend class iTunesLibraryParserThread;
};

#endif // MEDIA_BROWSER_ITUNES_LIBRARY_PARSER_H
