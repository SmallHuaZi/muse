#include <coro/scheduler.hpp>
#include <coro/thread-pool.hpp>

#include <memory>

namespace coro {
    auto Scheduler::create_shared(Options const &option) -> std::shared_ptr<Self> {
        auto self = std::make_shared<Self>();

        auto const need_thread_pool = !!(option.mode & WorkMode::ThreadPool);
        if (need_thread_pool) {
            self->thread_pool_ = ThreadPool::create_shared(option.worker);
        }

        auto const need_event_driver = !(option.mode & WorkMode::Manually);
        if (need_event_driver) {
            auto raw_self = self.get();
            self->event_driver_ = std::thread([raw_self] () {
                raw_self->handle_event();
            });
        }

        self->scheduled_tasks_.reserve(kInitialNumTask);
        return std::move(self);
    }

    auto Scheduler::do_schedule(std::coroutine_handle<> handle) -> void {
        if (!(options_.mode & WorkMode::ThreadPool)) {
            thread_pool_->resume(handle);
        } else {
            scheduled_tasks_.push_back(handle);

            // Trigger the driver thread.
            event_trigger_.release(1);
        }
    }

    auto Scheduler::handle_event_thread() -> void {
        while (true) {
            event_trigger_.acquire();
        }
    }

} // namespace coro