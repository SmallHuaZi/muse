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
#ifndef MUSE_PLATFORM_SOCKET_HPP
#define MUSE_PLATFORM_SOCKET_HPP 1

#include <type_traits>

#include <sys/socket.h>

#include <net/socket_addr.hpp>

namespace muse {
    struct PlatformSocketTraits {
        using NativeHandle = std::invoke_result_t<decltype(socket), int, int, int>;

        static auto create_socket(net::SocketAddr addr) -> NativeHandle;
    };

} // namespace muse

#endif // #ifndef MUSE_PLATFORM_SOCKET_HPP