#ifndef MEDIA_BROWSER_PHOTO_FOLDER_PARSER_H
#define MEDIA_BROWSER_PHOTO_FOLDER_PARSER_H

#include "FolderParser.h"

class PhotoFolderParser : public FolderParser
{
public:
    PhotoFolderParser(LibraryTreeControllerPtr library_tree_controller, const QString &file_path, const QString &display_name);

protected:
    virtual bool filterDirectory(const QFileInfo &file_info);
    virtual bool filterFile(const QString &file_path);
};

#endif // MEDIA_BROWSER_PHOTO_FOLDER_PARSER_H
