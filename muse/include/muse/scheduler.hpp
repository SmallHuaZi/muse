/// Copyright(C) 2024 smallhuazi

#ifndef INCLUDE_MUSE_SCHEDULER_HPP
#define INCLUDE_MUSE_SCHEDULER_HPP
///
/// This program is free software; you can redistribute it and/or modify
/// it under the terms of the GNU General Public License as published
/// by the Free Software Foundation; either version 2 of the License, or
/// (at your option) any later version.
///
/// For additional information, please refer to the following website:
/// https://opensource.org/license/gpl-2-0
///
#ifndef CORO_SCHEDULER_HPP
#define CORO_SCHEDULER_HPP 1

#include <coro/task.hpp>
#include <coro/concepts.hpp>
#include <coro/thread-pool.hpp>

#include <memory>
#include <vector>
#include <atomic>
#include <thread>
#include <semaphore>
#include <coroutine>

#include <enumbits/enumbits.hpp>

namespace coro {
    class Scheduler: public std::enable_shared_from_this<Scheduler> {
        using Self = Scheduler;
      public:
        /// This is usually used on startup of scheduler, so make it a bitflags enum, 
        enum class WorkMode {
            // Thread mode[Basic]: delegate the task of handling pending events to |io_worker|
            Basic = 0,

            // Thread mode[ThreadPool]: Use thread pool to handle pending events.
            ThreadPool = BIT(0),

            /// Drivring mode[Manually]: 
            /// If unset, spawnes a background thread that handle those pending events.
            /// Else, requires user to call handle_event().
            Manually = BIT(1),
        };
        MUSE_ENABLE_ENUMBITS_INNER(WorkMode);

        struct Options {
            WorkMode mode;
            ThreadPool::Options worker;
        };

        static auto create_shared(Options const &options = {
            .mode = WorkMode::Basic,
            .worker = {
                .num_workers = 1,
                .on_thread_start = {},
                .on_thread_exit = {},
            }
        }) -> std::shared_ptr<Self>;

        class ScheduleOperation {
          public:
            auto await_ready() noexcept -> bool {
                // Directly suspend caller.
                return false;
            }

            auto await_suspend(std::coroutine_handle<> handle) -> void {
                scheduler_.do_schedule(handle);
            }

            auto await_resume() noexcept -> void {}
          private:
            friend class Scheduler;
            ScheduleOperation(Scheduler &scheduler)
                : scheduler_(scheduler)
            {}

            Scheduler &scheduler_;
        };
        static_assert(concepts::Awaitable<ScheduleOperation>);

        [[nodiscard]]
        auto schedule() -> ScheduleOperation {
            return ScheduleOperation(*this);
        }

        template <typename Result>
        auto schedule(coro::Task<Result> task) -> void;

        auto handle_event() -> void;
      private:
        friend ScheduleOperation;

        auto do_schedule(std::coroutine_handle<>) -> void;

        auto handle_event_thread() -> void;

        static constexpr auto kInitialNumTask = 16;

        Options options_;
        std::mutex scheduled_tasks_mutex_;
        std::vector<std::coroutine_handle<>> scheduled_tasks_;
        std::atomic_size_t num_tasks_;

        /// We need semaphore implement a lightway event trigger.
        std::counting_semaphore<> event_trigger_;
        std::thread event_driver_;
        std::shared_ptr<ThreadPool> thread_pool_;
    };

} // namespace coro

#endif // #ifnde CORO_SCHEDULER_HPP


#endif // INCLUDE_MUSE_SCHEDULER_HPP
