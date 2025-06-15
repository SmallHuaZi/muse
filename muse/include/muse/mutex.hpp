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
#ifndef MUSE_MUTEX_HPP
#define MUSE_MUTEX_HPP 1

#include <coro/concepts.hpp>
#include <coro/details/wait-queue.hpp>

#include <atomic>
#include <coroutine>

namespace coro {
    class Mutex;

namespace details {
    class LockOperationBase {
        using Self = LockOperationBase;
      public:
        LockOperationBase(coro::Mutex &mutex)
            : mutex_(mutex)
        {}

        auto await_ready() noexcept -> bool;
        auto await_suspend(std::coroutine_handle<>) noexcept -> bool;
      private:
        void *next_;
        coro::Mutex &mutex_;
        std::coroutine_handle<> waiting_coro_;
    };

    template <typename ResultType>
    class LockOperation: public LockOperationBase {
        using Self = LockOperation;
        using Base = LockOperationBase;
        
        static_assert(concepts::Awaitable<Self>);
      public:
        using Result = ResultType;

        auto await_resume() noexcept -> Result {
        }
    };
} // namespace coro::details

    /// Designed to replace std::mutex which will triggeres eager rescheduling of current
    /// thread when failed to acquire an owned lock. This class also do the similar things
    /// on coroutine level as is:
    ///     1) Coroutine A acquires the mutex. If it has been owned by other coroutine 
    ///        then do 2), else do 3).
    ///     2) Owned the mutex and then run.
    ///     3) Suspend and switch out current coroutine, until Mutex::unlock() invoked to
    ///        wake up an waiting coroutine.
    class Mutex {
        using Self = Mutex;
      public:
        auto lock() noexcept -> void;

        auto try_lock() noexcept -> void;

        auto unlock() noexcept -> void;

        auto is_locked() const noexcept -> bool;
      private:
        auto unlocked_state() -> void * {
            return this;
        }
        friend details::LockOperationBase;

        std::atomic<void *> wq_head_;
        details::WaitQueue waiters_;
    };

} // namespace coro

#endif // #ifndef MUSE_MUTEX_HPP