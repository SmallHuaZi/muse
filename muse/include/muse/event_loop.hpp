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
#include <muse/poller.hpp>
#include <muse/channel.hpp>

#include <vector>
#include <atomic>
#include <thread>
#include <memory>
#include <functional>

#include <lib/enumbits/enumbits.hpp>

namespace muse {
    class EventLoop {
      public:
        using Task = std::function<auto () -> void>;

        auto start() -> void;

        auto run_in_loop(Task const &) -> void;
      private:
        enum class Flags: u16 {
            Running = BIT(0),
            Waiting = BIT(1),
            Handling = BIT(2),
            HandlingPendingTasks = BIT(3),
        };
        MUSE_ENABLE_INNER_ENUMBITS(Flags);

        auto process_pending_tasks() -> void;

        auto enqueue_task(Task const &task) -> void;

        auto is_in_loop_thread() const -> bool {
            return tid_ == std::this_thread::get_id();
        }

        // Do not modifies the following layout without a good enough reason.
        u32 num_iterations_;
        std::atomic_uint8_t shutdown_;
        std::atomic_uint8_t padding_;
        Flags flags_;

        std::thread::id tid_;
        std::unique_ptr<Poller> poller_;

        std::mutex mutex_;
        std::vector<Task> pending_tasks_;

        Channel *current_active_channel_;
        ChannelList active_channels_;
    };

} // namespace muse

#endif // MUSE_EVENT_LOOP_HPP