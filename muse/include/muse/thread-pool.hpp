/// Copyright(C) 2024 smallhuazi

#ifndef INCLUDE_MUSE_THREAD_POOL_HPP
#define INCLUDE_MUSE_THREAD_POOL_HPP
///
/// This program is free software; you can redistribute it and/or modify
/// it under the terms of the GNU General Public License as published
/// by the Free Software Foundation; either version 2 of the License, or
/// (at your option) any later version.
///
/// For additional information, please refer to the following website:
/// https://opensource.org/license/gpl-2-0
///
#ifndef CORO_THREAD_POOL_HPP
#define CORO_THREAD_POOL_HPP 1

#include <coro/concepts.hpp>
#include <coro/task.hpp>

#include <cstdint>

#include <memory>
#include <atomic>
#include <thread>
#include <vector>
#include <deque>
#include <mutex>
#include <condition_variable>
#include <coroutine>
#include <functional>

#include <enumbits/atomic.hpp>

namespace coro {
    class ThreadPool: public std::enable_shared_from_this<ThreadPool> {
        using Self = ThreadPool;
        using Base = std::enable_shared_from_this<ThreadPool>;
      public:
        struct Options {
            uint32_t num_workers = std::thread::hardware_concurrency();
            std::function<auto () -> void> on_thread_start{};
            std::function<auto () -> void> on_thread_exit{};
        };

        static auto create_shared(Options const &option) -> std::shared_ptr<Self>;

        struct ScheduleOperation {
            /// Schedule operations always pause so the executing thread can be switched.
            auto await_ready() noexcept -> bool { 
                return false;
            }

            /// Suspending always returns to the caller (using void return of await_suspend()) and
            /// stores the coroutine internally for the executing thread to resume from.
            auto await_suspend(std::coroutine_handle<> awaiting_coroutine) noexcept -> void;

            auto await_resume() noexcept -> void {
                // Do nothing.
            }
          private:
            friend ThreadPool;

            ScheduleOperation(ThreadPool &tpool)
                : tpool_(tpool)
            {}

            ThreadPool &tpool_;
        };
        static_assert(concepts::Awaitable<ScheduleOperation>);

        /// Schedules the currently executing coroutine to be run on this thread pool.
        ///
        /// Assumptions:
        ///     1). Called from within the coroutines function.
        [[nodiscard]]
        auto schedule() -> ScheduleOperation;

        /// Schedules the given coroutine |handle| to be run on this thread pool.
        template <typename Result>
        [[nodiscard]]
        auto schedule(coro::Task<Result> task) -> coro::Task<Result> {
            // Suspend current execution flow and switch to work thread.
            co_await schedule();

            // Now we are on another thread, so actually execute the given task.
            co_return co_await task;
        }

        auto resume(std::coroutine_handle<> handle) noexcept -> bool;

        /// Yield current tasks and places it at the end of runqueue.
        [[nodiscard]]
        auto yield() -> ScheduleOperation {
            return schedule();
        }

        /// Return the number of executor threads.
        auto num_thread() const noexcept -> size_t {
            return threads_.size();
        }

        /// Internal routine exported for std::make_shared<Self>()
        ThreadPool(Options const &);

        ThreadPool(Self const &) = delete;
        ThreadPool(Self &&) = delete;
        auto operator =(Self const &) -> Self & = delete;
        auto operator =(Self &&) -> Self & = delete;

        ~ThreadPool();
      private: 
        enum class Flags: uint32_t {
            Shutdown = BIT(0),
        };
        MUSE_ENABLE_ENUMBITS_INNER(Flags);

        /// Internal routine for threads
        auto process_task() -> void;

        auto do_schedule(std::coroutine_handle<> handle) -> void;

        Options options_;
        std::vector<std::thread> threads_;
        std::mutex wait_mutex_;
        std::condition_variable wait_cv_;

        /// Similar to traditional callbacks or functional object, but it is acutally
        /// a suspended coroutine handle.
        ///
        /// All of worker threads attempt to preempt a task from this queue.
        std::deque<std::coroutine_handle<>> waiting_tasks_;

        std::atomic_uint32_t num_running_;
        enumbits::Atomic<Flags> flags_;
    };
} // namespace coro

#endif // #ifndef CORO_THREAD_POOL_HPP


#endif // INCLUDE_MUSE_THREAD_POOL_HPP
