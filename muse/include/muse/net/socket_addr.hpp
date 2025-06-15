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
#ifndef MUSE_NET_SOCKET_ADDR_HPP
#define MUSE_NET_SOCKET_ADDR_HPP 1

#include <muse/net/ip_addr.hpp>

#include <string>

namespace muse::net {
    class SocketAddr {
      public:
        SocketAddr(IpAddr ip, usize port)
            : ipaddr_(ip), port_(port)
        {}

        auto to_string() const -> std::string {
            return {};
        }

      private:
        IpAddr ipaddr_;
        usize port_;
    };

} // namespace muse::net

#endif // #ifndef MUSE_NET_SOCKET_ADDR_HPP