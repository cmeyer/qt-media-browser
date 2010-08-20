#ifndef MEDIA_BROWSER_MEDIA_PARSER_H
#define MEDIA_BROWSER_MEDIA_PARSER_H

#include <boost/smart_ptr.hpp>

typedef boost::shared_ptr<class LibraryTreeController> LibraryTreeControllerPtr;

class MediaParser : public boost::enable_shared_from_this<MediaParser>
{
public:
    MediaParser(LibraryTreeControllerPtr library_tree_controller);
    virtual ~MediaParser();

    // start the parser
    virtual void start() = 0;

    // cancel the parser
    virtual void cancel() = 0;

    // accessors
    LibraryTreeControllerPtr libraryTreeController() const { return m_library_tree_controller; }

private:
    LibraryTreeControllerPtr m_library_tree_controller;
};

typedef boost::shared_ptr<class MediaParser> MediaParserPtr;

#endif // MEDIA_BROWSER_MEDIA_PARSER_H
