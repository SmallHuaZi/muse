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
#ifndef MUSE_EVENT_HPP
#define MUSE_EVENT_HPP 1

#include <muse/types.hpp>

#include <lib/enumbits/enumbits.hpp>

namespace muse {
    enum class Events {
        None,
        Connection = BIT(0),
        Close = BIT(1),

        Read = BIT(2),
        Write = BIT(3),

        Error = BIT(4),

        MaxNumEvents,
    };
    MUSE_ENABLE_ENUMBITS(Events);

    template <typename PlatformEvents>
    auto from_platform_events(PlatformEvents events) -> Events;

} // namespace muse

#endif // #ifndef MUSE_EVENT_HPP