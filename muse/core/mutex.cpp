#include <atomic>
#include <coro/mutex.hpp>
#include <coroutine>

namespace coro {
namespace details {
    auto LockOperationBase::await_ready() noexcept -> bool {

    }

    auto LockOperationBase::await_suspend(std::coroutine_handle<> handle) noexcept -> bool {
        auto &wq_head = mutex_.wq_head_;
        auto curr = wq_head.load();

        auto const unlocked_state = mutex_.unlocked_state();
        while (true) {
            if (curr == unlocked_state) {
                // Here no any one coroutine holding this lock, try to acquire it.
                if (wq_head.compare_exchange_weak(curr, nullptr, std::memory_order::acq_rel, std::memory_order::release)) {
                    waiting_coro_ = nullptr;

                    // Now we held this lock and then run.
                    return false;
                }
            } else {
                // curr->waiter->...->owner->nullptr.
                next_= curr;
                if (wq_head.compare_exchange_weak(curr, this, std::memory_order::acq_rel, std::memory_order::release)) {
                    return true;
                }
            }
        }
    }

} // namespace coro::details
} // namespace coro