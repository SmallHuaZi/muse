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
#ifndef MUSE_CHANNEL_HPP
#define MUSE_CHANNEL_HPP 1

#include <muse/event.hpp>
#include <muse/assert.hpp>
#include <muse/event_handler.hpp>
#include <muse/base/socket.hpp>

#include <memory>
#include <atomic>
#include <type_traits>

#include <enumbits/atomic.hpp>

#include <boost/intrusive/set.hpp>
#include <boost/intrusive/list.hpp>

namespace muse {
    class Channel {
        using Self = Channel;
      public:
        using NativeHandle = Socket::NativeHandle;

        Channel(EventLoop *event_loop, NativeHandle native_handle);

        auto activate() -> void;

        auto handle_event(TimePoint received_time) -> void;

        template <typename T>
        auto tie(std::shared_ptr<T> const &obj) -> void {
            flags_.fetch_and(Flags::Tied);
            tied_object_ = obj;
        }

        auto enable_reading() -> void {
            DEBUG_ASSERT(is_activated());
            capabilities_ |= Events::Read;
            update();
        }

        auto disable_reading() -> void {
            DEBUG_ASSERT(is_activated());
            capabilities_ &= ~Events::Read;
            update();
        }

        auto enable_writing() -> void {
            DEBUG_ASSERT(is_activated());
            capabilities_ |= Events::Write;
            update();
        }

        auto disable_writing() -> void {
            DEBUG_ASSERT(is_activated());
            capabilities_ &= ~Events::Write;
            update();
        }

        auto disable_all() -> void {
            DEBUG_ASSERT(is_activated());
            capabilities_ = Events::None;
            update();
        }

        auto set_reading_handler(ReadHandler const &handler) -> void {
            on_read_ = handler;
        }

        auto set_writing_handler(WriteHandler const &handler) -> void {
            on_write_ = handler;
        }

        auto set_error_handler(ErrorHandler const &handler) -> void {
            on_error_ = handler;
        }

        auto set_close_handler(CloseHandler const &handler) -> void {
            on_close_ = handler;
        }

        auto add_pending_events(Events events) -> void {
            pending_events_ |=  events;
        }

        auto events() const -> Events {
            return capabilities_;
        }

        auto native_handle() const -> NativeHandle {
            return native_handle_;
        }

        auto is_activated() const -> bool {
            return flags_.fetch_test(Flags::Activated);
        }

        auto is_tied() const -> bool {
            return flags_.fetch_test(Flags::Tied);
        }
      private:
        auto update() -> void;

        auto remove() -> void;

        auto handle_event_guarded(TimePoint received_time) -> void;

        enum Flags: u16 {
            Tied = BIT(0),
            Activated = BIT(1),
            HandlingEvent = BIT(2),
        };
        MUSE_ENABLE_ENUMBITS_INNER(Flags);

        NativeHandle native_handle_;
        EventLoop *loop_;

        std::weak_ptr<void> tied_object_;
        i32 platform_events_;
        i16 priority_;

        enumbits::Atomic<Flags> flags_;
        Events pending_events_;
        ReadHandler on_read_;
        WriteHandler on_write_;
        CloseHandler on_close_;
        ErrorHandler on_error_;

        Events capabilities_;

        boost::intrusive::set_member_hook<> active_channels_hook_;
        boost::intrusive::set_member_hook<> per_poller_hook_;
      public:
        struct NodeTratis {
            // Key type
            typedef i8 type;

            // Comparator
            constexpr auto operator()(type x, type y) const -> bool {
                return x < y;
            }

            // Getting key from value
            constexpr auto operator()(Self const &x) const -> type {
                return x.priority_;
            }
        };

        template <typename HookState>
        using List = boost::intrusive::set<Self, HookState,
                        boost::intrusive::compare<NodeTratis>,
                        boost::intrusive::key_of_value<NodeTratis>>;

        using ActiveChannelHookState = boost::intrusive::member_hook<Self, decltype(Self::active_channels_hook_), &Self::active_channels_hook_>;
        using PerPollerHookState = boost::intrusive::member_hook<Self, decltype(Self::per_poller_hook_), &Self::per_poller_hook_>;
    };
    using ActiveChannelList = Channel::List<Channel::ActiveChannelHookState>;
    using ChannelList = Channel::List<Channel::PerPollerHookState>;

} // namespace muse

#endif // #ifndef MUSE_CHANNEL_HPP