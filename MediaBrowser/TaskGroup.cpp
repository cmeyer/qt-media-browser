// Copyright 2011. LQ Graphics, Inc. and Chris Meyer. LGPL License. See LICENSE file.

#include "TaskGroup.h"

namespace MediaBrowserPrivate {

TaskGroup::TaskGroup(TaskPool *task_pool, bool delayed)
    : m_task_pool(task_pool)
    , m_started(!delayed)
{
}

TaskGroup::~TaskGroup()
{
    cancelAllTasks();
}

void TaskGroup::start()
{
    if (!m_started)
    {
        Q_FOREACH(TaskPtr task, m_tasks)
        {
            m_task_pool->addTask(task);
        }

        m_started = true;
    }
}

void TaskGroup::addTask(TaskPtr task)
{
    m_tasks.push_back(task);
    if (m_started)
        m_task_pool->addTask(task);
}

void TaskGroup::cancelPendingTasks()
{
    if (m_started)
    {
        m_task_pool->removeTasks(m_tasks);
    }
    m_tasks.clear();
}

void TaskGroup::cancelAllTasks()
{
    if (m_started)
    {
        m_task_pool->cancelTasks(m_tasks);
    }
    m_tasks.clear();
}

} /* MediaBrowserPrivate */
