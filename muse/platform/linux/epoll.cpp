#include <muse/poller/epoll.hpp>
#include <muse/channel.hpp>

#include <cassert>
#include <memory>

#include <poll.h>
#include <sys/epoll.h>
#include <unistd.h>

namespace muse {
    static auto from_epoll_events(decltype(epoll_event::events) events) -> Events {
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
    }

    struct Epoll::Event: epoll_event {};

    Epoll::Epoll(isize epfd) 
        : epfd_(epfd),
          events_(kNumInitEvents)
    {}

    /// 
    auto Epoll::create(std::unique_ptr<Self> *out) -> MuseStatus {
        isize const epfd = epoll_create1(EPOLL_CLOEXEC);
        if (epfd == -1) {
            return MuseStatus::InternalError;
        }

        auto self = new Self(epfd);
        if (!self) {
            return MuseStatus::OutOfMem;
        }

        out->reset(self);
        return MuseStatus::Ok;
    }

    auto Epoll::get_active_channels(usize num_events, ChannelList *active_channels) -> void {
        for (auto i = 0; i < num_events; ++i) {
            auto const channel = static_cast<Channel *>(events_[i].data.ptr);
            channel->add_pending_events(from_epoll_events(events_[i].events));
            active_channels->push_back(*channel);
        }
    }

    auto Epoll::poll(usize timeout_ms, ChannelList *active_channels) -> TimePoint {
        auto const num_events = epoll_wait(epfd_, &events_.front(), events_.size(), timeout_ms);
        if (num_events > 0) {
            get_active_channels(num_events, active_channels);
        }
    }

    auto Epoll::update_channel(Channel *channel) -> void {

    }

    auto Epoll::remove_channel(Channel *channel) -> void {
    }

    auto Epoll::shutdown() -> void {
        close(epfd_);
    }

} // namespace muse