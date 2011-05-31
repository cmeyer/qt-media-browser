#ifndef MEDIA_BROWSER_TASK_POOL_H
#define MEDIA_BROWSER_TASK_POOL_H

#include <boost/noncopyable.hpp>
#include <boost/smart_ptr.hpp>

#include <deque>

#include <QList>
#include <QMutex>
#include <QThread>
#include <QWaitCondition>

namespace MediaBrowserPrivate {

    class AutoMemoryPool
    {
    public:
        AutoMemoryPool();
        ~AutoMemoryPool();

    private:
#if defined(Q_OS_MAC)
        void *m_pool;
#endif
    };
    
    typedef AutoMemoryPool ThreadInitializer;
    
    typedef boost::shared_ptr<class Task> TaskPtr;
    typedef std::list<TaskPtr> TaskList;
    
    class Task : boost::noncopyable
    {
    public:
        Task() : m_cancel_flag(false), m_finished(false) { }
        virtual ~Task() { }

        virtual void run() = 0;

        virtual void wasRemoved();

        bool cancelFlag() const { return m_cancel_flag; }
        void setCancelFlag(bool cancel_flag) { m_cancel_flag = cancel_flag; }

        void wait();

    private:
        bool m_cancel_flag;

        QMutex m_finished_lock;
        QWaitCondition m_finished_condition;
        bool m_finished;

        void finished();

        friend class TaskPool;
    };

    // TODO: add constant for 'auto thread count' to maximize processor usage
    
    typedef boost::shared_ptr<class TaskPool> TaskPoolPtr;
    
    class TaskPool : boost::noncopyable
    {
    public:
        TaskPool(unsigned thread_count);
        ~TaskPool();

        void addTask(TaskPtr task);

        void removeTasks(TaskList tasks);   // will not remove tasks already in progress
        void removeTask(TaskPtr task);      // will not remove tasks already in progress
        void removeAllTasks();              // will not remove tasks already in progress

        void cancelTasks(TaskList tasks);   // will wait for any tasks in progress to finish
        void cancelTask(TaskPtr task);      // will wait for any tasks in progress to finish
        void cancelAllTasks();

    private:
        void processTasksInThread();

        unsigned m_thread_count;
        unsigned m_break_count;

        std::deque<TaskPtr> m_queue;
        QMutex m_queue_lock;
        QWaitCondition m_queue_condition;
        QList<QThread *> m_threads;

        friend class TaskPoolThread;
    };

} /* MediaBrowserPrivate */

#endif // MEDIA_BROWSER_TASK_POOL_H
