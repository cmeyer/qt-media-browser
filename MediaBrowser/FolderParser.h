#ifndef MEDIA_BROWSER_FOLDER_PARSER_H
#define MEDIA_BROWSER_FOLDER_PARSER_H

#include <boost/smart_ptr.hpp>
#include "MediaParser.h"
#include "TaskGroup.h"

class QFileInfo;
class QString;

typedef boost::shared_ptr<class LibraryTreeItem> LibraryTreeItemPtr;

// parse a folder recursively. add all subfolders and images within each subfolder to the tree controller.

class FolderParser : public MediaParser
{
public:
    FolderParser(LibraryTreeControllerPtr library_tree_controller, const QString &file_path, const QString &display_name);

    // from MediaParser
    virtual void start();
    virtual void cancel();

    // from FolderPromises
    void createAndStart(LibraryTreeControllerPtr library_tree_controller, LibraryTreeItemPtr library_tree_item, const QString &file_path);

protected:
    virtual bool filterDirectory(const QFileInfo &file_info);
    virtual bool filterFile(const QString &file_path) = 0;

private:
    MediaBrowserPrivate::TaskGroupPtr m_folder_parser_task_group;

    friend class FolderParserTask;
};

#endif // MEDIA_BROWSER_FOLDER_PARSER_H
