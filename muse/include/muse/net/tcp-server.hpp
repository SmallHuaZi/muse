/// Copyright(C) 2024 smallhuazi

#ifndef MUSE_NET_TCP_SERVER_HPP
#define MUSE_NET_TCP_SERVER_HPP
///
/// This program is free software; you can redistribute it and/or modify
/// it under the terms of the GNU General Public License as published
/// by the Free Software Foundation; either version 2 of the License, or
/// (at your option) any later version.
///
/// For additional information, please refer to the following website:
/// https://opensource.org/license/gpl-2-0
///
#ifndef MUSE_TCP_SERVER_HPP
#define MUSE_TCP_SERVER_HPP 1

#include <muse/base/socket.hpp>
#include <muse/tcp-connection.hpp>
#include <muse/event_loop_thread_pool.hpp>

#include <memory>
#include <string>
#include <map>

#include <net/socket_addr.hpp>
#include <boost/intrusive/set.hpp>
#include <enumbits/atomic.hpp>
#include <util/uuid.hpp>

namespace muse {
    class TcpServer {
        using Self = TcpServer;
      public:
        using NativeHandle = Socket::NativeHandle;
        static auto create(EventLoop *, std::string const &, net::SocketAddr const &, Self *) -> MuseStatus;

      protected:
        TcpServer(EventLoop *, net::SocketAddr const &, std::string const &, Acceptor *, EventLoopThreadPool *);

        auto start() -> void;

        auto new_connection(NativeHandle handle, net::SocketAddr const &addr) -> void;
        auto remove_connection(TcpConnectionPtr const &connection) -> void;
      private:
        enum class Flags: u32 {
            None = 0,
            Running = BIT(0),
        };
        MUSE_ENABLE_ENUMBITS_INNER(Flags);

        EventLoop *loop_;
        net::SocketAddr addr_;
        std::unique_ptr<Acceptor> acceptor_;
        std::unique_ptr<EventLoopThreadPool> thread_pool_;

        util::Uuid cid_gen_;
        enumbits::Atomic<Flags> flags_;

        // For pretty printion.
        std::string name_;
        std::string addr_str_;

        // The callback specified by user to handle the message received. 
        TcpConnection::MessageHandler on_connection_message_;

        TcpConnection::ConnectionHandler on_connection_established_or_destroied_;

        using ConnectionMap = std::map<std::string, TcpConnectionPtr>;
        ConnectionMap connections_;
    };

} // namespace muse

#endif // #ifndef MUSE_TCP_SERVER_HPP


#endif // MUSE_NET_TCP_SERVER_HPP
