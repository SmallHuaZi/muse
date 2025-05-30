#include <muse/event_loop.hpp>

namespace muse {
    auto EventLoop::process_pending_tasks() -> void {
        decltype(pending_tasks_) to_process;
        {
            std::lock_guard guard{mutex_};
            // Guess this should be the optimal way
            new (&to_process) decltype(to_process)(std::move(pending_tasks_));
        }

        for (auto &task : to_process) {
            task();
        }
    }

    auto EventLoop::start() -> void {
        while (!shutdown_.load()) {
            auto const received_time = poller_->poll(0, &active_channels_);

            num_iterations_ += 1;
            if (!active_channels_.empty()) {
                flags_ |= Flags::Handling;
                for (auto &channel : active_channels_) {
                    current_active_channel_ = &channel;
                    channel.handle_event(received_time);
                }
                flags_ &= ~Flags::Handling;
                current_active_channel_ = 0;
            }

            process_pending_tasks();
        }

        flags_ &= ~Flags::Running;
    }

    auto EventLoop::run_in_loop(Task const &task) -> void {
        if (is_in_loop_thread()) {
            task();
        } else {
            enqueue_task(task);
        }
    }

    auto EventLoop::enqueue_task(Task const &task) -> void {
        std::lock_guard guard{mutex_};
        pending_tasks_.push_back(task);
    }

} // namespace muse