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
#ifndef MUSE_TYPES_HPP
#define MUSE_TYPES_HPP 1

#include <cstddef>
#include <cstdint>

#include <chrono>

namespace muse {
    using i8 = std::int8_t;
    using i16 = std::int16_t;
    using i32 = std::int32_t;
    using i64 = std::int64_t;

    using u8 = std::uint8_t;
    using u16 = std::uint16_t;
    using u32 = std::uint32_t;
    using u64 = std::uint64_t;

    using usize = std::size_t;
    using isize = std::ptrdiff_t;

    class Poller;
    class Channel;
    class Acceptor;
    class EventLoop;
    class EventLoopThread;

    using TimePoint = std::chrono::time_point<std::chrono::milliseconds>;

} // namespace muse

#endif // #ifndef MUSE_TYPES_HPP