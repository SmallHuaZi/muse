#include <muse/event_loop_thread.hpp>
#include <muse/event_loop.hpp>

namespace muse {
    auto EventLoopThread::resume() -> void {
        new (&thread_) decltype(thread_)([this] () { worker(); });
    }

    auto EventLoopThread::worker() -> void {
        EventLoop event_loop;

        {
            semaphore_.acquire();
            event_loop_ = &event_loop;
            semaphore_.release();
        }

        event_loop.start();
    }

} // namespace muse