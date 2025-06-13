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
#ifndef MUSE_WORK_QUEUE_HPP
#define MUSE_WORK_QUEUE_HPP 1

#include <muse/types.hpp>

#include <array>
#include <mutex>
#include <atomic>
#include <chrono>
#include <functional>

#include <boost/intrusive/set.hpp>
#include <boost/intrusive/list.hpp>

#include <util/enumbits.hpp>

namespace muse {
    using TaskBase = boost::intrusive::set_base_hook<boost::intrusive::link_mode<boost::intrusive::auto_unlink>>;

    class Task: public TaskBase {
        typedef Task  Self;
      public:
        enum class Flags: u8 {
            // Only allow given tasks running on current event loop.
            // Unable to assign or migrate it onto other loops for load
            // balance.
            OnlyThisLoop,
        };
        MUSE_ENABLE_ENUMBITS_INNER(Flags);

        using Handler = std::function<auto () ->void>;

        Task(Handler, i8 priority);

        auto defer(std::chrono::milliseconds timeout) -> void;

        auto start() -> void;

        auto is_deferred() const -> bool {
            return deferred_;
        }

        auto is_runnable() const -> bool {
            return !(cancelled_ || deferred_);
        }
      private:
        auto execute() -> void;

        friend class WorkQueue;
        friend class TimerQueue;
        Handler handler_;
        u32 priority_;
        Flags flags_;
        bool cancelled_;
        bool deferred_;
    };

    class WorkQueue: public boost::intrusive::list_base_hook<> {
        using Self = WorkQueue;
        using Base = boost::intrusive::list_base_hook<>;
      public:
        WorkQueue(EventLoop *loop);

        static auto commit_task(Task const &task) -> void;

        static auto commit_task(Task::Handler const &handler, usize priority) -> void;

        auto run_tasks() -> usize;

        auto num_tasks() const -> usize {
            return num_tasks_.load(std::memory_order_acquire);
        }
      private:
        friend class Task;
        auto insert_task(Task &task) -> void;

        struct TaskNodeTraits {
            constexpr auto operator()(Task const &x, Task const &y) const -> bool {
                return x.priority_ < y.priority_;
            }
        };

        using Queue = boost::intrusive::set<Task, 
                          boost::intrusive::constant_time_size<false>,
                          boost::intrusive::compare<TaskNodeTraits>>;
        Queue tasks_;
        std::atomic_size_t num_tasks_;
        std::mutex queue_lock_;
        Task *running_;
        EventLoop *loop_;

        TimePoint last_runtime_;

        static constexpr auto kNumSamples = 4;
        std::array<decltype(last_runtime_)::duration, kNumSamples> recent_samples_;

        static inline boost::intrusive::list<Self> global_workqueues;
    };

} // namespace muse

#endif // #ifndef MUSE_WORK-QUEUE_HPP