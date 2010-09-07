#ifndef MEDIA_BROWSER_APERTURE_LIBRARY_PARSER_H
#define MEDIA_BROWSER_APERTURE_LIBRARY_PARSER_H

#include "FolderParser.h"

class ApertureLibraryParserThread;

// parses an Aperture library located at file_path and adds nodes to library tree controller
class ApertureLibraryParser : public MediaParser
{
public:
    ApertureLibraryParser(LibraryTreeControllerPtr library_tree_controller, const QString &file_path, const QString &display_name);

    // from MediaParser
    virtual void start();
    virtual void cancel();

private:
    boost::shared_ptr<ApertureLibraryParserThread> m_aperture_library_parser_thread;

    friend class ApertureLibraryParserThread;
};

#endif // MEDIA_BROWSER_APERTURE_LIBRARY_PARSER_H
