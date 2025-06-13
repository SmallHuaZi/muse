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
#ifndef MUSE_EVENT_LOOP_HPP
#define MUSE_EVENT_LOOP_HPP 1

#include <muse/types.hpp>
#include <muse/status.hpp>
#include <muse/poller.hpp>
#include <muse/channel.hpp>
#include <muse/work-queue.hpp>
#include <muse/timer-queue.hpp>

#include <atomic>
#include <thread>
#include <memory>
#include <semaphore>

#include <enumbits/atomic.hpp>

namespace muse {
    class EventLoop {
        using Self = EventLoop;
      public:
        static auto current() -> Self * {
            return per_thread_loop;
        }

        static auto create(EventLoop *out) -> MuseStatus;

        EventLoop();
        ~EventLoop();

        auto start() -> void;

        auto quit() -> void;

        auto in_loop_thread() const -> bool {
            return tid_ == std::this_thread::get_id();
        }

        auto add_channel(Channel *channel) -> void {
            DEBUG_ASSERT(in_loop_thread());
            poller_->add_channel(channel);
        }

        auto update_channel(Channel *channel) -> void {
            DEBUG_ASSERT(in_loop_thread());
            poller_->update_channel(channel);
        }

        auto remove_channel(Channel *channel) -> void {
            DEBUG_ASSERT(in_loop_thread());
            poller_->remove_channel(channel);
        }

        auto work_queue() -> WorkQueue & {
            return workqueue_;
        }

        auto timer_queue() -> TimerQueue & {
            return timerqueue_;
        }

        auto is_quited() const -> bool {
            return flags_.fetch_test(Flags::Quited);
        }
      private:
        auto wakeup() -> void;

        auto on_woken() -> void;

        enum class Flags: u32 {
            Running = BIT(0),
            Waiting = BIT(1),
            Handling = BIT(2),
            HandlingPendingTasks = BIT(3),
            Quited = BIT(4),
            ShutDown = BIT(4),
        };
        MUSE_ENABLE_ENUMBITS_INNER(Flags);

        // 10 seconds.
        static usize constexpr kPollingTimeoutMs = 10000;

        // Do not modifies the following layout without a good enough reason.
        enumbits::Atomic<Flags> flags_;
        u32 num_iterations_;

        std::thread::id tid_;
        std::unique_ptr<Poller> poller_;

        WorkQueue workqueue_;
        TimerQueue timerqueue_;

        std::unique_ptr<Channel> wakeup_channel_;
        Channel *current_active_channel_;
        ActiveChannelList active_channels_;

        static inline thread_local Self *per_thread_loop;
    };

} // namespace muse

#endif // MUSE_EVENT_LOOP_HPP