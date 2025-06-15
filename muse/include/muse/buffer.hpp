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
#ifndef MUSE_BUFFER_HPP
#define MUSE_BUFFER_HPP 1

#include <muse/types.hpp>

#include <array>
#include <atomic>
#include <mutex>

namespace muse {
    template <typename T, usize MaxSize>
    class Buffer {
        using Self = Buffer;
      public:
        using Element = T;

      private:
        std::array<Element, MaxSize> m_elements;
        std::atomic<usize> m_front;
        std::atomic<usize> m_back;
        std::atomic<usize> m_num_used;

        std::mutex mutex_;
    };
} // namespace muse

#endif // #ifndef MUSE_BUFFER_HPP