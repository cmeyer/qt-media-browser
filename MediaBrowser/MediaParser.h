// Copyright 2011. LQ Graphics, Inc. and Chris Meyer. LGPL License. See LICENSE file.

#ifndef MEDIA_BROWSER_MEDIA_PARSER_H
#define MEDIA_BROWSER_MEDIA_PARSER_H

#include <boost/smart_ptr.hpp>

typedef boost::shared_ptr<class LibraryTreeController> LibraryTreeControllerPtr;

// the media parser is an abstract class to parse a specific library such as a folder or an iphoto library.

// it starts a thread to parse a the library. it adds each item it finds to the library
// tree controller using the append media file method.

// the media parser submits all of its data to the live tree of the library tree controller,
// which tracks changes to its model. when sync is called on the library tree controller, it replays
// each change from the live model to the display model.

// start is separate from the constructor so that it can use virtual methods and shared_from_this.

// since the media parser will typically be threaded, subclasses should cancel as soon as possible
// and then return.

class MediaParser : public boost::enable_shared_from_this<MediaParser>
{
public:
    MediaParser();
    virtual ~MediaParser();

    // start the parser. separate from constructor so it can use virtual methods and shared_from_this.
    virtual void start() = 0;

    // cancel the parser. stop as quickly as possible and then return.
    virtual void cancel() = 0;
};

typedef boost::shared_ptr<class MediaParser> MediaParserPtr;

#endif // MEDIA_BROWSER_MEDIA_PARSER_H
