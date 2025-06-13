#include <muse/tcp-connection.hpp>

#include <muse/acceptor.hpp>
#include <muse/channel.hpp>
#include <muse/assert.hpp>
#include <muse/base/socket.hpp>

#include <memory>

namespace muse {
    TcpConnection::TcpConnection(
        EventLoop *loop, 
        std::string const &name, 
        Socket *socket, 
        Channel *channel, 
        net::SocketAddr local, 
        net::SocketAddr peer) noexcept
        : ioloop_(loop) 
        , name_(name)
        , local_addr_(local) 
        , peer_addr_(peer)
        , socket_(socket)
        , channel_(channel)
        , status_(Status::Connecting)
    {}

    auto TcpConnection::create(EventLoop *loop, std::string const &name,
                               NativeHandle native_handle, net::SocketAddr local,
                               net::SocketAddr peer, TcpConnectionPtr *out) noexcept -> MuseStatus {
        DEBUG_ASSERT(loop);
        auto const channel = new Channel(loop, native_handle);
        if (!channel) {
            return MuseStatus::OutOfMem;
        }

        auto const socket = new Socket(native_handle);
        if (!socket) {
            return MuseStatus::OutOfMem;
        }

        auto self = new Self(loop, name, socket, channel, local, peer);
        if (!self) {
            return MuseStatus::OutOfMem;
        }

        out->reset(self);
        return MuseStatus::Ok;
    }

    auto TcpConnection::on_connected() -> void {
        DEBUG_ASSERT(Status::Connected != status_);

        status_ = Status::Connected;

        channel_->enable_reading();
        connection_handler_(shared_from_this());
    }

    auto TcpConnection::on_disconnected() -> void {
        if (Status::Connected == status_) {
            status_ = Status::Disconnected;

            channel_->disable_all();
            connection_handler_(shared_from_this());
        }
    }

    auto TcpConnection::send(char const *data, usize len) -> void {
    }

} // namespace muse