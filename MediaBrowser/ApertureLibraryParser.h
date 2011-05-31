// Copyright 2011. LQ Graphics, Inc. and Chris Meyer. LGPL License. See LICENSE file.

#ifndef MEDIA_BROWSER_APERTURE_LIBRARY_PARSER_H
#define MEDIA_BROWSER_APERTURE_LIBRARY_PARSER_H

#include "FolderParser.h"

typedef boost::shared_ptr<class ApertureLibraryParserThread> ApertureLibraryParserThreadPtr;

// parses an Aperture library located at file_path and adds nodes to library tree controller

class ApertureLibraryParser : public MediaParser
{
public:
    ApertureLibraryParser(LibraryTreeControllerPtr library_tree_controller, const QString &file_path, const QString &display_name);

    // from MediaParser
    virtual void start();
    virtual void cancel();

private:
    ApertureLibraryParserThreadPtr m_aperture_library_parser_thread;

    friend class ApertureLibraryParserThread;
};

#endif // MEDIA_BROWSER_APERTURE_LIBRARY_PARSER_H
