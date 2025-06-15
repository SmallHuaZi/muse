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
#ifndef MUSE_NET_IP_ADDR_HPP
#define MUSE_NET_IP_ADDR_HPP 1

#include <muse/types.hpp>

#include <optional>
#include <cstring>

namespace muse::net {
    template <typename IpAddr, char Delimiter>
    auto from_str(char const *str) -> std::optional<IpAddr>;

    class IpV4Addr {
        typedef IpV4Addr    Self;
      public:
        static Self const kUnspecified;
        static Self const kLocalHost;
        static Self const kBroadcast;

        static auto from(char const *addr) -> std::optional<Self>;

        constexpr IpV4Addr(u8 a, u8 b, u8 c, u8 d)
            : octets(a, b, c, d)
        {}

        constexpr auto is_multicast() const -> bool {
            return octets[0] > 223 && octets[1] < 240;
        }

        constexpr auto is_link_local() const -> bool {
            return octets[0] == 169 && octets[1] == 254;
        }
      private:
        template <typename IpAddr, char Delimiter>
        friend auto from_str(char const *str) -> std::optional<IpAddr>;

        constexpr IpV4Addr()
            : octets()
        {}

        u8 octets[4];
    };
    inline IpV4Addr const IpV4Addr::kUnspecified(0, 0, 0, 0);
    inline IpV4Addr const IpV4Addr::kLocalHost(127, 0, 0, 1);
    inline IpV4Addr const IpV4Addr::kBroadcast(225, 225, 225, 225);

    class IpV6Addr {
        typedef IpV6Addr    Self;
      public:
        static auto from(char const *addr) -> std::optional<Self>;

        constexpr IpV6Addr(u8 a, u8 b, u8 c, u8 d, u8 e, u8 f, u8 g, u8 h)
            : octets(a, b, c, d, e, f, g, h)
        {}

      private:
        template <typename IpAddr, char Delimiter>
        friend auto from_str(char const *str) -> std::optional<IpAddr>;

        constexpr IpV6Addr()
            : octets()
        {}

        u8 octets[16];
    };

    union IpAddr {
        IpV4Addr v4;
        IpV6Addr v6;
    };

} // namespace muse::net

#endif // #ifndef MUSE_NET_IP_ADDR_HPP