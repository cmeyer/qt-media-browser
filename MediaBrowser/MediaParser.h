#ifndef MEDIA_BROWSER_MEDIA_PARSER_H
#define MEDIA_BROWSER_MEDIA_PARSER_H

#include <boost/smart_ptr.hpp>

typedef boost::shared_ptr<class LibraryTreeController> LibraryTreeControllerPtr;

// the media parser starts a thread to parse a specific library (such as a folder or an image database).
// it adds each item it finds to a LibraryTreeController using the appendMediaFile method.
//
// the library tree controller object is passed into the constructor for the media parser. the thread that the media
// parser starts funnels through the api of the library tree controller. the library tree controller tracks changes
// to its model. periodically the user thread can synchronize in a thread safe manner via the sync method.
//
// start is separate from the constructor so that it can use virtual methods and shared_from_this.
//
// since the media parser will typically be threaded, the cancel method should cancel as soon as possible
// and then return.

class MediaParser : public boost::enable_shared_from_this<MediaParser>
{
public:
    MediaParser(LibraryTreeControllerPtr library_tree_controller);
    virtual ~MediaParser();

    // start the parser. separate from constructor so it can use virtual methods and shared_from_this.
    virtual void start() = 0;

    // cancel the parser. stop as quickly as possible and then return.
    virtual void cancel() = 0;

    // accessors
    LibraryTreeControllerPtr libraryTreeController() const { return m_library_tree_controller; }

private:
    LibraryTreeControllerPtr m_library_tree_controller;
};

typedef boost::shared_ptr<class MediaParser> MediaParserPtr;

#endif // MEDIA_BROWSER_MEDIA_PARSER_H
