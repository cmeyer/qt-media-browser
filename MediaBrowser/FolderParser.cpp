// Copyright 2011. LQ Graphics, Inc. and Chris Meyer. LGPL License. See LICENSE file.

#include <QDebug>
#include <QDirIterator>
#include <QFileInfo>
#include "FolderParser.h"
#include "LibraryTreeItem.h"
#include "LibraryTreeController.h"
#include "TaskPool.h"

// http://www.devarticles.com/c/a/Cplusplus/C-plus-plus-In-Theory-The-Singleton-Pattern-Part-I/1/
class FolderParserTaskPool : public MediaBrowserPrivate::TaskPool
{
public:
    static FolderParserTaskPool *instance();

private:
    FolderParserTaskPool(); // ctor hidden
    FolderParserTaskPool(FolderParserTaskPool const&); // copy ctor hidden
    FolderParserTaskPool& operator=(FolderParserTaskPool const&); // assign op. hidden
    ~FolderParserTaskPool(); // dtor hidden

    static FolderParserTaskPool *m_instance;
};

FolderParserTaskPool *FolderParserTaskPool::m_instance = NULL;

FolderParserTaskPool *FolderParserTaskPool::instance()
{
    static QMutex mutex;

    if (!m_instance)
    {
        mutex.lock();

        if (!m_instance)
            m_instance = new FolderParserTaskPool();

        mutex.unlock();
    }

    return m_instance;
}

FolderParserTaskPool::FolderParserTaskPool()
    : MediaBrowserPrivate::TaskPool(2)
{
}

FolderParserTaskPool::FolderParserTaskPool(FolderParserTaskPool const &)
    : MediaBrowserPrivate::TaskPool(2)
{
}

FolderParserTaskPool& FolderParserTaskPool::operator=(FolderParserTaskPool const &)
{
    return *this;
}

FolderParserTaskPool::~FolderParserTaskPool()
{
}

class FolderParserTask : public MediaBrowserPrivate::Task
{
public:
    FolderParserTask(FolderParser *folder_parser, LibraryTreeControllerPtr library_tree_controller, LibraryTreeItemPtr parent_library_tree_item, const QString &file_path, const QString &display_name, bool run_alternate = false);

    virtual void run();

private:
    FolderParser *m_folder_parser;
    LibraryTreeControllerPtr m_library_tree_controller;
    LibraryTreeItemPtr m_parent_library_tree_item;
    QString m_file_path;
    QString m_display_name;
    bool m_run_alternate;

    bool isFilteredDirectory(const QString &directory_path);
    void parseDirectory(const QString &directory_path, const QString &display_name, LibraryTreeItemPtr parent_library_tree_item, unsigned level);
    void parse2(const QString &directory_path, LibraryTreeItemPtr library_tree_item);
};

FolderParserTask::FolderParserTask(FolderParser *folder_parser, LibraryTreeControllerPtr library_tree_controller, LibraryTreeItemPtr parent_library_tree_item, const QString &file_path, const QString &display_name, bool run_alternate)
    : m_folder_parser(folder_parser)
    , m_library_tree_controller(library_tree_controller)
    , m_parent_library_tree_item(parent_library_tree_item)
    , m_file_path(file_path)
    , m_display_name(display_name)
    , m_run_alternate(run_alternate)
{
}

bool FolderParserTask::isFilteredDirectory(const QString &directory_path)
{
    QFileInfo file_info(directory_path);

    if (file_info.baseName() == "")
        return true;
    if (file_info.baseName() == ".")
        return true;
    if (file_info.baseName() == "..")
        return true;
    if (!m_folder_parser->filterDirectory(file_info))
        return true;

    return false;
}

typedef boost::shared_ptr<class FolderPromise> FolderPromisePtr;

class FolderPromise : public LibraryTreeItemPromise
{
public:
    FolderPromise(FolderParser *folder_parser, const QString &directory)
        : m_folder_parser(folder_parser)
        , m_directory(directory)
    {
    }

    void setLibraryTreeItem(LibraryTreeItemPtr library_tree_item)
    {
        m_library_tree_item = library_tree_item;
    }

    virtual void populate(LibraryTreeControllerPtr library_tree_controller)
    {
        m_folder_parser->createAndStart(library_tree_controller, m_library_tree_item, m_directory);
    }

private:
    FolderParser *m_folder_parser;
    LibraryTreeItemPtr m_library_tree_item;
    QString m_directory;
};

void FolderParserTask::parseDirectory(const QString &directory_path, const QString &display_name, LibraryTreeItemPtr parent_library_tree_item, unsigned level)
{
    QFileInfo file_info(directory_path);

    if (file_info.exists() && file_info.isDir())
    {
        QString name = display_name.isEmpty() ? file_info.baseName() : display_name;

        if (level > 0)
        {
            LibraryTreeItemPtr library_tree_item = m_library_tree_controller->appendChild(parent_library_tree_item, name);

            QDirIterator iter(directory_path, QDirIterator::NoIteratorFlags);
            while (iter.hasNext() && !cancelFlag())
            {
                QString file_path(iter.next());
                if (isFilteredDirectory(file_path)) // filter directories here to avoid displaying empty directories
                    continue;

                parseDirectory(file_path, QString(), library_tree_item, level-1);
            }
        }
        else
        {
            FolderPromisePtr folder_promise(new FolderPromise(m_folder_parser, directory_path));

            LibraryTreeItemPtr library_tree_item = m_library_tree_controller->appendChild(parent_library_tree_item, name, folder_promise);

            folder_promise->setLibraryTreeItem(library_tree_item);
        }
    }
    else
    {
        if (m_folder_parser->filterFile(directory_path))
        {
            m_library_tree_controller->appendFile(parent_library_tree_item, directory_path, QLatin1String("folder"));
        }
    }
}

void FolderParserTask::parse2(const QString &directory_path, LibraryTreeItemPtr library_tree_item)
{
    QDirIterator iter(directory_path, QDirIterator::NoIteratorFlags);
    while (iter.hasNext() && !cancelFlag())
    {
        QString file_path(iter.next());
        if (isFilteredDirectory(file_path)) // filter directories here to avoid displaying empty directories
            continue;

        parseDirectory(file_path, QString(), library_tree_item, 0);
    }
}

void FolderParserTask::run()
{
    if (!m_run_alternate)
        parseDirectory(m_file_path, m_display_name, m_parent_library_tree_item, 1);
    else
        parse2(m_file_path, m_parent_library_tree_item);
}

FolderParser::FolderParser(LibraryTreeControllerPtr library_tree_controller, const QString &file_path, const QString &display_name)
    : m_folder_parser_task_group(new MediaBrowserPrivate::TaskGroup(FolderParserTaskPool::instance(), true))
{
    MediaBrowserPrivate::TaskPtr folder_parser_task(new FolderParserTask(this, library_tree_controller, library_tree_controller->liveRootLibraryTreeItem(), file_path, display_name));
    m_folder_parser_task_group->addTask(folder_parser_task);
}

void FolderParser::start()
{
    m_folder_parser_task_group->start();
}

void FolderParser::createAndStart(LibraryTreeControllerPtr library_tree_controller, LibraryTreeItemPtr library_tree_item, const QString &file_path)
{
    MediaBrowserPrivate::TaskPtr folder_parser_task(new FolderParserTask(this, library_tree_controller, library_tree_item, file_path, QString(), true));
    m_folder_parser_task_group->addTask(folder_parser_task);
}

void FolderParser::cancel()
{
    m_folder_parser_task_group->cancelAllTasks();
}

bool FolderParser::filterDirectory(const QFileInfo &file_info)
{
    return true;
}
