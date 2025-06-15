#include <coro/thread-pool.hpp>

#include <atomic>
#include <functional>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <utility>

namespace coro {
    auto ThreadPool::ScheduleOperation::await_suspend(std::coroutine_handle<> handle) noexcept -> void {
        tpool_.do_schedule(handle);
    }

    auto ThreadPool::create_shared(Options const &option) -> std::shared_ptr<Self> {
        auto self = std::make_shared<Self>(option);
        auto const raw_self = self.get();

        self->threads_.reserve(option.num_workers);
        for (auto i = option.num_workers; i != 0; --i) {

            self->threads_.emplace_back(std::thread([raw_self] () {
                raw_self->process_task();
            }));
        }

        // TODO(SmallHuaZi): There is a warning reported by clang. It tell us moving a local object 
        // in a return statement prevents copy elision. If this statement is work, please ignoring it
        // since to std::shared_ptr<> the move constructor is faster than copy constructor which has
        // the additional counting operation.
        return std::move(self);
    }

    auto ThreadPool::schedule() -> ScheduleOperation {
        if (flags_.fetch_test(Flags::Shutdown, std::memory_order_acquire)) {
            throw std::runtime_error("Attempt to schedule a task when thread pool was shutdown");
        }

        num_running_.fetch_add(1, std::memory_order_release);
        return ScheduleOperation(*this);
    }

    auto ThreadPool::resume(std::coroutine_handle<> handle) noexcept -> bool {
        if (!handle && handle.done()) {
            return false;
        }

        num_running_.fetch_add(1, std::memory_order_release);
        if (flags_.fetch_test(Flags::Shutdown, std::memory_order_acquire)) {
            num_running_.fetch_sub(1, std::memory_order_release);
            return false;
        }

        do_schedule(handle);
        return true;
    }

    /// Internal method to schedule a coroutine through given |handle|.
    ///
    /// It add the |handle| onto this pool's queue, and wake a worker 
    /// thread up to resume it.
    ///
    /// Assumptions:
    ///     1) The |wait_mutex_| has not been held by caller.
    auto ThreadPool::do_schedule(std::coroutine_handle<> handle) -> void {
        if (!handle || handle.done()) {
            return;
        }

        {
            std::lock_guard guard{wait_mutex_};
            waiting_tasks_.push_back(handle);
            wait_cv_.notify_one();
        }
    }

    /// Internal method for a worker thread to resume those tasks scheduled.
    auto ThreadPool::process_task() -> void {
        if (options_.on_thread_start) {
            options_.on_thread_start();
        }

        auto const is_shutdown_requested = [this] () {
            return flags_.fetch_test(Flags::Shutdown, std::memory_order_acquire);
        };

        while (is_shutdown_requested()) {
            std::unique_lock lock{wait_mutex_};
            wait_cv_.wait(lock, [this, is_shutdown_requested] () {
                return !waiting_tasks_.empty() || is_shutdown_requested();
            });

            // Now we held the |wait_mutex_|.
            auto task = waiting_tasks_.front();
            waiting_tasks_.pop_front();

            // A task was stolen by us so drop |wait_mutex_|.
            lock.unlock();

            task.resume();
            num_running_.fetch_sub(1, std::memory_order_release);
        }

        while (num_running_.load(std::memory_order_acquire)) {
            std::unique_lock lock{wait_mutex_};
            if (!waiting_tasks_.empty()) {
                break;
            }

            // Now we held the |wait_mutex_|.
            auto task = waiting_tasks_.front();
            waiting_tasks_.pop_front();

            // A task was stolen by us so drop |wait_mutex_|.
            lock.unlock();

            task.resume();
            num_running_.fetch_sub(1, std::memory_order_release);
        }

        if (options_.on_thread_exit) {
            options_.on_thread_exit();
        }
    }

} // namespace coro