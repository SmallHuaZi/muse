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
#ifndef MUSE_EVENT_LOOP_THREAD_POOL_HPP
#define MUSE_EVENT_LOOP_THREAD_POOL_HPP 1

#include <muse/types.hpp>

namespace muse {
    class EventLoopThreadPool {
        using Self = EventLoopThreadPool;
      public:
        EventLoopThreadPool();
        ~EventLoopThreadPool() = default;

        auto start() -> void;

        auto set_num_thread(usize num_thread) -> void {
            num_threads_ = num_thread;
        }
      private:
        u32 num_threads_;
    };

} // namespace muse

#endif // #ifndef MUSE_EVENT_LOOP_THREAD_POOL_HPP