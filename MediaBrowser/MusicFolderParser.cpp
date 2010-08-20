#include <QFileInfo>
#include <QUrl>
#include "MusicFolderParser.h"

MusicFolderParser::MusicFolderParser(LibraryTreeControllerPtr library_tree_controller, const QString &file_path, const QString &display_name)
        : FolderParser(library_tree_controller, file_path, display_name)
{
}

bool MusicFolderParser::filterDirectory(const QFileInfo &file_info)
{
    if (file_info.baseName() == "iTunes Library")
        return false;
    return true;
}

bool MusicFolderParser::filterFile(const QString &file_path)
{
    QFileInfo file_info(file_path);

    QString extension = file_info.suffix().toLower();

    if (extension == "mp3")
      return true;
    if (extension == "m4a" || extension == "m4p" || extension == "mp4" || extension == "3g2")
      return true;
    if (extension == "wma" || extension == "asf")
      return true;
    if (extension == "aif" || extension == "aiff")
      return true;
    if (extension == "wav")
      return true;

    return false;
}
