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
#include "muse/event_loop.hpp"
#ifndef MUSE_CHANNEL_HPP
#define MUSE_CHANNEL_HPP 1

#include <muse/event.hpp>
#include <muse/event_handler.hpp>

#include <lib/enumbits/enumbits.hpp>

#include <boost/intrusive/set.hpp>
#include <boost/intrusive/list.hpp>

namespace muse {
    class Channel {
        using Self = Channel;
      public:
        Channel(EventLoop *event_loop, usize native_handle);

        auto handle_event(TimePoint received_time) -> void;

        auto add_pending_events(Events events) -> void {
            pending_events_ |=  events;
        }
      private:
        auto handle_event_locked(TimePoint received_time) -> void;

        usize native_handle_;
        EventLoop *event_loop_;

        Events pending_events_;

        ReadHandler on_read_;
        WriteHandler on_write_;
        CloseHandler on_close_;
        ErrorHandler on_error_;

        boost::intrusive::list_member_hook<> list_hook_;
        boost::intrusive::set_member_hook<> set_hook_;
      public:
        using ListHookState = boost::intrusive::member_hook<Self, decltype(Self::list_hook_), &Self::list_hook_>;
        using SetHookState = boost::intrusive::member_hook<Self, decltype(Self::set_hook_), &Self::set_hook_>;
    };
    using ChannelList = boost::intrusive::list<Channel, Channel::ListHookState>;
    using ChannelSet = boost::intrusive::set<Channel, Channel::SetHookState>;

} // namespace muse

#endif // #ifndef MUSE_CHANNEL_HPP