#include <muse/poller.hpp>
#include <muse/event_loop.hpp>

#include <cassert>

namespace muse {
    auto Poller::assert_in_loop_thread() const -> void {
        assert(loop_);
        assert(loop_->in_loop_thread());
    }

} // namespace muse