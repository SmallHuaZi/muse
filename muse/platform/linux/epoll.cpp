#include <muse/poller/epoll.hpp>
#include <muse/channel.hpp>
#include <muse/assert.hpp>

#define MUSE_LOG_TARGET "(EPOLL) "
#include <muse/log.hpp>

#include <cassert>
#include <memory>
#include <chrono>

#include <poll.h>
#include <sys/epoll.h>
#include <unistd.h>

namespace muse {
    typedef decltype(epoll_event::events)   EpollEvents;

    static auto to_events(EpollEvents events) -> Events {
        Events result{};
        if (events & EPOLLIN) {
            result |= Events::Read;
        }

        if (events & EPOLLOUT) {
            result |= Events::Write;
        }

        if (events & EPOLLERR) {
            result |= Events::Error;
        }

        return result;
    }

    static auto to_epoll_events(Events events) -> EpollEvents {
        EpollEvents result{};
        if (!!(events & Events::Read)) {
            result |= EPOLLIN;
        }

        if (!!(events & Events::Write)) {
            result |= EPOLLOUT;
        }

        if (!!(events & Events::Error)) {
            result |= EPOLLERR;
        }

        return result;
    }

    struct Epoll::Event: epoll_event {};

    Epoll::Epoll(EventLoop *event_loop, isize epfd) 
        : Base(event_loop),
          epfd_(epfd),
          events_(kNumInitEvents)
    {}

    Epoll::~Epoll() {
        shutdown();
    }

    /// 
    auto Epoll::create(EventLoop *event_loop, std::unique_ptr<Self> *out) -> MuseStatus {
        isize const epfd = epoll_create1(EPOLL_CLOEXEC);
        if (epfd == -1) {
            return MuseStatus::InternalError;
        }

        auto self = new Self(event_loop, epfd);
        if (!self) {
            return MuseStatus::OutOfMem;
        }

        out->reset(self);
        return MuseStatus::Ok;
    }

    auto Epoll::get_active_channels(usize num_events, ActiveChannelList *active_channels) -> void {
        for (auto i = 0; i < num_events; ++i) {
            auto const channel = static_cast<Channel *>(events_[i].data.ptr);
            channel->add_pending_events(to_events(events_[i].events));
            active_channels->push_back(*channel);
        }
    }

    auto Epoll::poll(usize timeout_ms, ActiveChannelList *active_channels) -> TimePoint {
        auto const num_events = ::epoll_wait(epfd_, &events_.front(), events_.size(), timeout_ms);
        // Imediatedly collects timestamp.
        auto const received_time = TimePoint::clock::now();

        if (num_events > 0) {
            get_active_channels(num_events, active_channels);
        } else if (!num_events) {
            log::trace("Did not receive any events");
        } else {
            auto const error = errno;
            log::error("Error={}", error);
        }

        return received_time;
    }

    static inline auto operation_to_string(i32 op) -> char const * {
        switch (op) {
            case EPOLL_CTL_ADD:
                return "ADD";
            case EPOLL_CTL_DEL:
                return "DEL";
            case EPOLL_CTL_MOD:
                return "MOD";
            default:
                assert(false && "ERROR op");
                return "Unknown Operation";
        }
    }

    auto Epoll::update_channel_internal(i32 op, Channel *channel) -> void {
        epoll_event event;
        memset(&event, 0, sizeof(event));

        event.events = to_epoll_events(channel->events());
        event.data.ptr = channel;
        auto const fd = channel->native_handle();

        if (::epoll_ctl(epfd_, op, fd, &event) < 0) {
            log::error("epoll_ctl() fd={} op={}", fd, operation_to_string(op));
        }
    }

    auto Epoll::add_channel(Channel *channel) -> void {
        assert_in_loop_thread();
        DEBUG_ASSERT(!has_channel(channel));

        channels_.insert(*channel);
        update_channel_internal(EPOLL_CTL_ADD, channel);
    }

    auto Epoll::update_channel(Channel *channel) -> void {
        assert_in_loop_thread();
        update_channel_internal(EPOLL_CTL_MOD, channel);
    }

    auto Epoll::remove_channel(Channel *channel) -> void {
        assert_in_loop_thread();
        DEBUG_ASSERT(has_channel(channel));

        channels_.erase(channels_.iterator_to(*channel));
        update_channel_internal(EPOLL_CTL_DEL, channel);
    }

    auto Epoll::shutdown() -> void {
        close(epfd_);
    }

} // namespace muse