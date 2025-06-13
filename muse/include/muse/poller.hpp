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
#ifndef MUSE_POLLER_HPP
#define MUSE_POLLER_HPP 1

#include <muse/types.hpp>
#include <muse/event.hpp>
#include <muse/channel.hpp>

#include <util/enumbits.hpp>

namespace muse {
    class Poller {
      public:
        enum class Feature {
            
        };
        MUSE_ENABLE_ENUMBITS_INNER(Feature);

        Poller(EventLoop *loop) 
            : loop_(loop), channels_()
        {}

        virtual ~Poller() = default;

        virtual auto poll(usize timeout_ms, ActiveChannelList *channels) -> TimePoint = 0;
        virtual auto add_channel(Channel *channel) -> void = 0;
        virtual auto update_channel(Channel *channel) -> void = 0;
        virtual auto remove_channel(Channel *channel) -> void = 0;
        virtual auto shutdown() -> void = 0;

        auto has_channel(Channel *channel) const -> bool {
            assert_in_loop_thread();
            return channels_.find(channel->native_handle()) != channels_.end();
        }
      protected:
        auto assert_in_loop_thread() const -> void;
        EventLoop *loop_;
        Feature feature_;
        ChannelList channels_;
    };

} // namespace muse

#endif // #ifndef MUSE_POLLER_HPP