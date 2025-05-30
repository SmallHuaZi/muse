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
#ifndef MUSE_EVENT_LOOP_THREAD_HPP
#define MUSE_EVENT_LOOP_THREAD_HPP 1

#include <thread>
#include <mutex>
#include <string>
#include <semaphore>

namespace muse {
    class EventLoop;

    class EventLoopThread {
      public:
        auto resume() -> void;

      private:
        auto worker() -> void;

        std::string name_;
        std::mutex mutex_;
        std::thread thread_;
        std::counting_semaphore<1> semaphore_;
        EventLoop *event_loop_;
    };

} // namespace muse

#endif // MUSE_EVENT_LOOP_THREAD_HPP
