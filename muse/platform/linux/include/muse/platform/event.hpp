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
#ifndef MUSE_PLATFORM_EVENT_HPP
#define MUSE_PLATFORM_EVENT_HPP 1

#include <type_traits>

#include <sys/eventfd.h>

namespace muse {
    struct PlatformEventTraits {
        using Handle = std::invoke_result_t<decltype(eventfd), int, int>;
        using Value = uint_fast64_t; 

        static auto create_event() -> Handle {
            return ::eventfd(0, 0);
        }

        static auto read_event(Handle handle, Value *value) {
            return ::eventfd_read(handle, value);
        }

        static auto write_event(Handle handle, Value value)  {
            return ::eventfd_write(handle, value);
        }
    };

} // namespace muse

#endif // #ifndef MUSE_PLATFORM_EVENT_HPP