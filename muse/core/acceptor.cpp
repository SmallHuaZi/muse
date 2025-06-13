#include <muse/acceptor.hpp>
#include <muse/assert.hpp>

namespace muse {
    auto Acceptor::create(EventLoop *loop, net::SocketAddr addr, Acceptor *out) -> MuseStatus {
        DEBUG_ASSERT(loop && out);
    }

    auto Acceptor::listen() -> void {
        
    }

} // namespace muse