/// Copyright(C) 2024 smallhuazi
///
/// This program is free software; you can redistribute it and/or modify
/// it under the terms of the GNU General Public License as published
/// by the Free Software Foundation; either version 2 of the License, or
/// (at your option) any later version.
///
/// For additional information, please refer to the following website:
/// https://opensource.org/license/gpl-2-0
///
#ifndef RUNTIME_SCHED_SCHEDULER_HPP
#define RUNTIME_SCHED_SCHEDULER_HPP 1

#include <muse/runtime/task.hpp>

#include <memory>
#include <concepts>
#include <coroutine>
#include <type_traits>

#include <boost/intrusive/list.hpp>

namespace muse::runtime {
    class Task;

    class Scheduler: public std::enable_shared_from_this<Scheduler> {
      public:
        auto schedule(Task &curr) -> void;

        auto unblock(Task &task) -> void;

        auto block(Task &curr) -> void;
      private:
        using TaskQueueState = boost::intrusive::member_hook<Task,
            boost::intrusive::list_member_hook<>, &Task::m_rqhook>;
        using RunQueue = boost::intrusive::list<Task, TaskQueueState>;
        RunQueue m_runqueue;
    };

} // namespace muse::runtime

#endif // #ifndef RUNTIME_SCHED_SCHEDULER_HPP