#include <muse/channel.hpp>

namespace muse {
    Channel::Channel(EventLoop *event_loop, usize native_handle) 
        : native_handle_(native_handle),
          event_loop_(event_loop),
          pending_events_()
    {}

    auto Channel::handle_event_locked(TimePoint received_time) -> void {
        if (!!(pending_events_ & Events::Close)) {
            if (on_close_) {
                on_close_();
            }
        }
        if (!!(pending_events_ & Events::Error)) {
            if (on_error_) {
                on_error_();
            }
        }
        if (!!(pending_events_ & Events::Read)) {
            if (on_read_) {
                on_read_(received_time);
            }
        }
        if (!!(pending_events_ & Events::Write)) {
            if (on_write_) {
                on_write_();
            }
        }

        pending_events_ = Events::None;
    }

    auto Channel::handle_event(TimePoint received_time) -> void {
    }

} // namespace muse