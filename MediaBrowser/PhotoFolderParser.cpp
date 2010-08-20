#include <QDir>
#include <QFileInfo>
#include <QImageReader>
#include "PhotoFolderParser.h"

PhotoFolderParser::PhotoFolderParser(LibraryTreeControllerPtr library_tree_controller, const QString &file_path, const QString &display_name)
        : FolderParser(library_tree_controller, file_path, display_name)
{
}

bool PhotoFolderParser::filterDirectory(const QFileInfo &file_info)
{
    QString suffix = file_info.suffix();
    QDir dir = file_info.dir();

    // lightroom data
    if (suffix == "lrdata")
        return false;

#if defined(Q_OS_MAC)
    // aperture libraries
    if (suffix == "aplibrary")
        return false;
#endif

#if defined(Q_OS_MAC)
    // filter out iPhoto libraries
    if (QFileInfo(dir.absoluteFilePath("Library.iPhoto")).exists())
        return false;
    if (QFileInfo(dir.absoluteFilePath("Library6.iPhoto")).exists())
        return false;
    if (QFileInfo(dir.absoluteFilePath("iPhotoMain.db")).exists())
        return false;
#endif

    return true;
}

bool PhotoFolderParser::filterFile(const QString &file_path)
{
    QImageReader image_reader(file_path);
    return image_reader.canRead();
}
