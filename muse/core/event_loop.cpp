#include <muse/event_loop.hpp>
#include <muse/log.hpp>

#include <muse/platform/socket.hpp>

#include <thread>

namespace muse {
    EventLoop::EventLoop()
        : flags_()
        , tid_(std::this_thread::get_id())
        , workqueue_(this)
        , timerqueue_(this)
        , wakeup_channel_()
        , current_active_channel_(0)
        , active_channels_() {
        DEBUG_ASSERT(!per_thread_loop, "Exists multiple active instances in one thread");

        if (per_thread_loop) {
            log::error("There are simoutaneously two loops in one thread.");
            return;
        }

        wakeup_channel_->set_reading_handler(std::bind(&Self::on_woken, this));
        wakeup_channel_->enable_reading();

        per_thread_loop = this;
    }

    EventLoop::~EventLoop() {
        if (this == per_thread_loop) {
            per_thread_loop = 0;
        }
    }

    auto EventLoop::start() -> void {
        DEBUG_ASSERT(per_thread_loop == this, "Duplicated calls");

        while (!is_quited()) {
            auto const received_time = poller_->poll(kPollingTimeoutMs, &active_channels_);

            num_iterations_ += 1;
            if (!active_channels_.empty()) {
                flags_.fetch_or(Flags::Handling);

                for (auto &channel : active_channels_) {
                    current_active_channel_ = &channel;
                    channel.handle_event(received_time);
                }

                flags_.fetch_and(~Flags::Handling);
            }
            current_active_channel_ = 0;

            // Give pending tasks a chance to execute.
            workqueue_.run_tasks();
        }

        flags_.fetch_and(~Flags::Running);

        // Waiting until EventLoop::quit() finished.
        flags_.wait(Flags::ShutDown);
    }

    auto EventLoop::quit() -> void {
        // Before to quit, acquires the |shutdown_| to prevent accessing an invalid object
        // in following wakeup() call.
        flags_.fetch_or(Flags::Quited);

        if (!in_loop_thread()) {
            wakeup();
        }

        flags_.fetch_or(Flags::ShutDown);
    }

    auto EventLoop::wakeup() -> void {
        usize tag = 1;
    }

    auto EventLoop::on_woken() -> void {
        usize tag = 1;
    }

} // namespace muse