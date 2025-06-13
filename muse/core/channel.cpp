#include <memory>
#include <muse/channel.hpp>
#include <muse/event_loop.hpp>

namespace muse {
    Channel::Channel(EventLoop *loop, NativeHandle native_handle) 
        : native_handle_(native_handle)
        , loop_(loop)
        , pending_events_()
    {}

    auto Channel::activate() -> void {
        flags_.fetch_or(Flags::Activated);
        loop_->add_channel(this);
    }

    auto Channel::update() -> void {
        loop_->update_channel(this);
    }

    auto Channel::remove() -> void {
        loop_->remove_channel(this);
    }

    auto Channel::handle_event_guarded(TimePoint received_time) -> void {
        flags_.fetch_or(Flags::HandlingEvent);
        auto const event = pending_events_;
        pending_events_ = Events::None;

        if (!!(event & Events::Close)) {
            if (on_close_) {
                on_close_();
            }
        }
        if (!!(event & Events::Error)) {
            if (on_error_) {
                on_error_();
            }
        }
        if (!!(event & Events::Read)) {
            if (on_read_) {
                on_read_(received_time);
            }
        }
        if (!!(event & Events::Write)) {
            if (on_write_) {
                on_write_();
            }
        }

        flags_.fetch_and(~Flags::HandlingEvent);
    }

    auto Channel::handle_event(TimePoint received_time) -> void {
        if (!is_tied()) {
            return handle_event_guarded(received_time);
        }

        auto guard = tied_object_.lock();
        return handle_event_guarded(received_time);
    }

} // namespace muse