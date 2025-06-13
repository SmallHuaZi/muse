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
#ifndef MUSE_TIMER_QUEUE_HPP
#define MUSE_TIMER_QUEUE_HPP 1

#include <muse/types.hpp>
#include <muse/platform/timer.hpp>

namespace muse {
    class TimerQueue {
        typedef TimerQueue  Self;
      public:
        using NativeHandle = PlatformTimerTratis::NativeHandle;

        TimerQueue(EventLoop *loop);

      private:
        EventLoop *loop_;
        NativeHandle native_handle_;
        TimePoint next_timer_deadline_;
    };

} // namespace muse

#endif // #ifndef MUSE_TIMER-QUEUE_HPP