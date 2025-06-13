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
#ifndef UTIL_UUID_HPP
#define UTIL_UUID_HPP 1

namespace util {
    class Uuid {
      public:
        Uuid() = default;
        ~Uuid() = default;

        Uuid(int base)
            : base_(base)
        {}

        auto gen() -> int {
            return base_++;
        }

      private:
        int base_;
    };

} // namespace util

#endif // #ifndef UTIL_UUID_HPP