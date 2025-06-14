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
#ifndef MUSE_PLATFORM_TIMER_HPP
#define MUSE_PLATFORM_TIMER_HPP 1

#include <sys/timerfd.h>

namespace muse {
    struct PlatformTimerTratis {
        using NativeHandle = decltype(timerfd_create(0, 0));
    };

} // namespace muse

#endif // #ifndef MUSE_PLATFORM_TIMER_HPP