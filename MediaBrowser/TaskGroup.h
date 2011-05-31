#ifndef MEDIA_BROWSER_TASK_GROUP_H
#define MEDIA_BROWSER_TASK_GROUP_H

#include "TaskPool.h"

namespace MediaBrowserPrivate {
    
typedef boost::shared_ptr<class TaskGroup> TaskGroupPtr;

class TaskGroup // : boost::noncopyable, using Q_DISABLE_COPY instead
{
public:
    TaskGroup(TaskPool *task_pool, bool delayed = false);
    ~TaskGroup();
    
    Q_DISABLE_COPY(TaskGroup)

    void start();

    void addTask(TaskPtr task);
    void cancelPendingTasks();
    void cancelAllTasks();

private:
    bool m_started;
    TaskPool *m_task_pool;
    TaskList m_tasks;
};

} /* MediaBrowserPrivate */

#endif  //  MEDIA_BROWSER_TASK_GROUP_H
