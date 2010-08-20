/*
 *  Threading.h
 *  LQFoundation
 *
 *  Created by Chris Meyer on 4/26/09.
 *  Copyright 2009 LQ Graphics, Inc. All rights reserved.
 *
 */

#ifndef MEDIA_BROWSER_TASK_POOL_H
#define MEDIA_BROWSER_TASK_POOL_H

#include <boost/noncopyable.hpp>

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

    class TaskPool;

    typedef boost::shared_ptr<TaskPool> TaskPoolPtr;

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

    typedef boost::shared_ptr<Task> TaskPtr;
    typedef std::list<TaskPtr> TaskList;

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
