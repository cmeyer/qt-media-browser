#include "TaskPool.h"


#include <QQueue>
#include <QMutex>
#include <QMutexLocker>
#include <QDebug>

#if defined(Q_OS_MAC)
#include <Foundation/NSAutoreleasePool.h>
#endif

namespace MediaBrowserPrivate {

    AutoMemoryPool::AutoMemoryPool()
    {
#if defined(Q_OS_MAC)
        m_pool = [[NSAutoreleasePool alloc] init];
#endif
    }

    AutoMemoryPool::~AutoMemoryPool()
    {
#if defined(Q_OS_MAC)
        [(id)m_pool drain];
#endif
    }

    class TaskPoolThread : public QThread
    {
    public:
        TaskPoolThread(TaskPool *task_pool) : m_task_pool(task_pool) { }
        void run();
    private:
        TaskPool *m_task_pool;
    };

    void TaskPoolThread::run()
    {
        AutoMemoryPool pool;

        m_task_pool->processTasksInThread();
    }

    TaskPool::TaskPool(unsigned thread_count)
        : m_thread_count(thread_count), m_break_count(0)
    {
        for (unsigned i=0; i<m_thread_count; ++i)
        {
            QThread *thread = new TaskPoolThread(this);
            m_threads.push_back(thread);
            thread->start();
        }
    }

    TaskPool::~TaskPool()
    {
        {
            QMutexLocker lock(&m_queue_lock);
            m_break_count = m_thread_count;
            m_queue_condition.wakeAll();
        }

        Q_FOREACH(QThread *thread, m_threads)
        {
            thread->wait();
        }
    }

    void TaskPool::addTask(TaskPtr task)
    {
        QMutexLocker lock(&m_queue_lock);

        m_queue.push_back(task);

        m_queue_condition.wakeAll();
    }

    void TaskPool::removeTasks(TaskList tasks)
    {
        QMutexLocker lock(&m_queue_lock);

        Q_FOREACH(TaskPtr task, tasks)
        {
            TaskQueue::iterator iter = qFind(m_queue.begin(), m_queue.end(), task);

            if (iter != m_queue.end())
            {
                m_queue.erase(iter);

                task->wasRemoved();
            }
        }

        m_queue_condition.wakeAll();
    }

    void TaskPool::removeTask(TaskPtr task)
    {
        QMutexLocker lock(&m_queue_lock);

        TaskQueue::iterator iter = qFind(m_queue.begin(), m_queue.end(), task);

        if (iter != m_queue.end())
        {
            m_queue.erase(iter);

            task->wasRemoved();
        }

        m_queue_condition.wakeAll();
    }

    void TaskPool::processTasksInThread()
    {
        while (true)
        {
            TaskPtr next_task;

            try
            {
                QMutexLocker lock(&m_queue_lock);

                if (m_break_count > 0)
                {
                    m_break_count--;
                    m_queue_condition.wakeAll();
                    break;
                }

                if (m_queue.empty())
                {
                    m_queue_condition.wait(&m_queue_lock);
                    continue;
                }

                next_task = m_queue.dequeue();

                if (next_task->cancelFlag())
                    next_task.reset();
            }
            catch (...)
            {
                //LOG_EXCEPTION("TaskPool::processTasksInThread (queue)");
            }
            
            try
            {
                if (next_task != NULL)
                {
                    next_task->run();
                    next_task->finished();
                }
            }
            catch (...)
            {
                //LOG_EXCEPTION("TaskPool::processTasksInThread");
            }
        }
    }

    // will wait for any tasks in progress to finish
    void TaskPool::cancelTasks(TaskList tasks)
    {
        TaskList tasks_to_wait;

        // first remove the tasks that haven't started
        {
            QMutexLocker lock(&m_queue_lock);

            Q_FOREACH(TaskPtr task, tasks)
            {
                TaskQueue::iterator iter = qFind(m_queue.begin(), m_queue.end(), task);

                if (iter != m_queue.end())
                {
                    m_queue.erase(iter);

                    task->wasRemoved();
                }
                else
                {
                    tasks_to_wait.push_back(task);
                }
            }

            m_queue_condition.wakeAll();
        }

        // next cancel the running tasks
        Q_FOREACH(TaskPtr task, tasks_to_wait)
        {
            task->setCancelFlag(true);
        }

        // and wait for them
        Q_FOREACH(TaskPtr task, tasks_to_wait)
        {
            task->wait();
        }
    }

    void Task::finished()
    {
        QMutexLocker lock(&m_finished_lock);

        m_finished = true;

        m_finished_condition.wakeAll();
    }

    void Task::wait()
    {
        QMutexLocker lock(&m_finished_lock);

        while (!m_finished)
            m_finished_condition.wait(&m_finished_lock);
    }

    void Task::wasRemoved()
    {
    }

} /* MediaBrowserPrivate */

//extern "C" void tss_cleanup_implemented(void)
//{
//}
