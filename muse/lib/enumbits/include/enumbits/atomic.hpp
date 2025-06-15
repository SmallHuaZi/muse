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
#ifndef ENUMBITS_ATOMIC_HPP
#define ENUMBITS_ATOMIC_HPP 1

#include <enumbits/enumbits.hpp>

#include <atomic>
#include <type_traits>

namespace enumbits {
    template <typename T>
    class Atomic {
        static_assert(std::is_enum_v<T>, "Template parameter 'T' is not an enum type");
        static_assert(MuseEnumBitsEnabledV<T>, "Template parameter 'T' is not enabled MuseEnumBits");

        typedef Atomic  Self;
      public:
        typedef T   EnumType;
        typedef std::underlying_type_t<EnumType>   UnderlyingType;
      
        auto fetch_and(EnumType flags, std::memory_order mo = std::memory_order::seq_cst) -> EnumType {
            return static_cast<EnumType>(inner_.fetch_and(static_cast<UnderlyingType>(flags), mo));
        }

        auto fetch_or(EnumType flags, std::memory_order mo = std::memory_order::seq_cst) -> EnumType {
            return static_cast<EnumType>(inner_.fetch_or(static_cast<UnderlyingType>(flags), mo));
        }

        auto fetch_test(EnumType flags, std::memory_order mo = std::memory_order::seq_cst) const -> bool {
            UnderlyingType const clone{inner_.load(mo)};
            return !!(clone & static_cast<UnderlyingType>(flags));
        }

        auto wait(EnumType flags, std::memory_order mo = std::memory_order::seq_cst) const -> void {
            auto const expected = static_cast<UnderlyingType>(flags);

            for (auto val = inner_.load(mo); (val & expected) != expected; val = inner_.load(mo)) {
                inner_.wait(val);
            }
        }
      private:
        std::atomic_flag x;
        std::atomic<UnderlyingType> inner_;
    };

} // namespace enumbits

#endif // #ifndef ENUMBITS_ATOMIC_HPP