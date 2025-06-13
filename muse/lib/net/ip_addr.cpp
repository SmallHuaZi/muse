#include <net/ip_addr.hpp>

#include <charconv>
#include <optional>

namespace net {
    template <typename IpAddr, char Delimiter>
    auto from_str(char const *str) -> std::optional<IpAddr> {
        constexpr auto kNumFields = sizeof(IpAddr);

        auto const num_chars = std::strlen(str);
        if (num_chars > kNumFields * 3) {
            return std::nullopt;
        }

        auto first = str;
        auto const last = str + num_chars;
        IpAddr ipaddr;
        for (auto i = 0; i < kNumFields - 1; ++i) {
            auto end = std::find(first, last, Delimiter);
            if (std::errc() != std::from_chars(first, end, ipaddr.octets[i]).ec) {
                return std::nullopt;
            }
            first = end;
        }
        if (std::errc() != std::from_chars(first, last, ipaddr.octets[kNumFields - 1]).ec) {
            return std::nullopt;
        }

        return std::make_optional(ipaddr);
    }

    auto IpV4Addr::from(char const *addr) -> std::optional<Self> {
        return from_str<Self, '.'>(addr);
    }

    auto IpV6Addr::from(char const *addr) -> std::optional<Self> {
        return from_str<Self, ':'>(addr);
    }

} // namespace net