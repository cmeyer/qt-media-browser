// Copyright 2011. LQ Graphics, Inc. and Chris Meyer. LGPL License. See LICENSE file.

#ifndef MEDIA_BROWSER_ITUNES_LIBRARY_PARSER_H
#define MEDIA_BROWSER_ITUNES_LIBRARY_PARSER_H

#include "FolderParser.h"

typedef boost::shared_ptr<class iTunesLibraryParserThread> iTunesLibraryParserThreadPtr;

class iTunesLibraryParser : public MediaParser
{
public:
    iTunesLibraryParser(LibraryTreeControllerPtr library_tree_controller, const QString &file_path, const QString &display_name);

    // from MediaParser
    virtual void start();
    virtual void cancel();

private:
    iTunesLibraryParserThreadPtr m_itunes_library_parser_thread;

    friend class iTunesLibraryParserThread;
};

#endif // MEDIA_BROWSER_ITUNES_LIBRARY_PARSER_H
