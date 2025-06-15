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
#ifndef MUSE_POLLER_EPOLL_HPP
#define MUSE_POLLER_EPOLL_HPP 1

#include <muse/types.hpp>
#include <muse/status.hpp>

#include <memory>
#include <vector>

namespace muse {
    class Epoll {
        using Self = Epoll;
      public:
        static auto create(EventLoop *event_loop, std::unique_ptr<Self> *out) -> MuseStatus;

        Epoll(EventLoop *event_loop, isize fd);
        ~Epoll();

        auto poll(usize timeout_ms, ActiveChannelList *active_channels) -> TimePoint;
        auto add_channel(Channel *channel) -> void;
        auto update_channel(Channel *channel) -> void;
        auto remove_channel(Channel *channel) -> void;
        auto shutdown() -> void;
      private:
        auto update_channel_internal(i32 op, Channel *channel) -> void;
        auto get_active_channels(usize num_events, ActiveChannelList *active_channels) -> void;

        struct Event;
        using EventList = std::vector<Event>;

        constexpr static auto kNumInitEvents = 16;

        isize epfd_;
        EventList events_;
    };

} // namespace muse

#endif // #ifndef MUSE_POLLER_EPOLL_HPP