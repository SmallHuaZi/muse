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
#ifndef MUSE_ACCEPTOR_HPP
#define MUSE_ACCEPTOR_HPP 1

#include <muse/types.hpp>
#include <muse/status.hpp>
#include <muse/base/socket.hpp>

#include <functional>

#include <net/socket_addr.hpp>

namespace muse {
    class Acceptor {
      public:
        using NativeHandle = Socket::NativeHandle;
        using NewConnectionHandler = std::function<auto (NativeHandle, net::SocketAddr const &) -> void>;

        static auto create(EventLoop *, net::SocketAddr, Acceptor *) -> MuseStatus;

        auto listen() -> void;

        auto set_new_connection_handler(NewConnectionHandler) -> void;
      private:
        Acceptor(EventLoop *event_loop);

        Socket socket_;
        EventLoop *event_loop_;
        NewConnectionHandler handler_;
    };

} // namespace muse

#endif // #ifndef MUSE_ACCEPTOR_HPP