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
#ifndef MUSE_TCP_CONNECTION_HPP
#define MUSE_TCP_CONNECTION_HPP 1

#include <muse/types.hpp>
#include <muse/status.hpp>

#include <muse/base/socket.hpp>

#include <memory>
#include <string>
#include <iterator>
#include <functional>

#include <net/socket_addr.hpp>
#include <boost/intrusive/set_hook.hpp>

namespace muse {
    class TcpConnection;
    using TcpConnectionPtr = std::shared_ptr<TcpConnection>;

    class TcpConnection: public std::enable_shared_from_this<TcpConnection> {
        using Self = TcpConnection;
      public:
        using NativeHandle = PlatformSocketTraits::NativeHandle;
        using CloseHandler = std::function<auto (TcpConnectionPtr const &) -> void>;
        using MessageHandler = std::function<auto (TcpConnectionPtr const &) -> void>;
        using ConnectionHandler = std::function<auto (TcpConnectionPtr const &) -> void>;

        static auto create(EventLoop *loop, std::string const &name,
                           NativeHandle native_handle, net::SocketAddr local,
                           net::SocketAddr peer, TcpConnectionPtr *out) noexcept -> MuseStatus;

        // Conveniency way for standard library
        template <typename T>
        auto send(T const &t) -> void {
            send(std::begin(t), std::size(t));
        }

        auto send(char const *data, usize len) -> void;

        auto on_connected() -> void;

        auto on_disconnected() -> void;

        auto set_close_handler(CloseHandler const &handler)  noexcept-> void {
            on_close_ = handler;
        }

        auto set_message_handler(MessageHandler const &handler) noexcept -> void {
            on_message_ = handler;
        }

        auto set_connection_handler(ConnectionHandler const &handler) noexcept -> void {
        }
      protected:
        enum class Status {
            Connecting,
            Connected,
            Disconnected,
        };
        
        // Prohibit users to construct a connection object on stack.
        TcpConnection(EventLoop *, std::string const &, Socket *, Channel *,
                      net::SocketAddr, net::SocketAddr) noexcept;
        
        ~TcpConnection() = default;

        EventLoop *ioloop_;
        std::string name_;
        net::SocketAddr local_addr_;
        net::SocketAddr peer_addr_;

        std::unique_ptr<Socket> socket_;
        std::unique_ptr<Acceptor> acceptor_;
        std::unique_ptr<Channel> channel_;
      private:
        Status status_;
        CloseHandler on_close_;
        MessageHandler on_message_;
        ConnectionHandler connection_handler_;
    };

} // namespace muse

#endif // #ifndef MUSE_TCP_CONNECTION_HPP