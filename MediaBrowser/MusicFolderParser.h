// Copyright 2011. LQ Graphics, Inc. and Chris Meyer. LGPL License. See LICENSE file.

#ifndef MEDIA_BROWSER_MUSIC_FOLDER_PARSER_H
#define MEDIA_BROWSER_MUSIC_FOLDER_PARSER_H

#include "FolderParser.h"

// parse a music folder and add items to the live portion of the libary tree controller.

class MusicFolderParser : public FolderParser
{
public:
    MusicFolderParser(LibraryTreeControllerPtr library_tree_controller, const QString &file_path, const QString &display_name);

protected:
    virtual bool filterDirectory(const QFileInfo &file_info);
    virtual bool filterFile(const QString &file_path);
};

#endif // MEDIA_BROWSER_MUSIC_FOLDER_PARSER_H
