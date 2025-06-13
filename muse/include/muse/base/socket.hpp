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
#ifndef MUSE_BASE_SOCKET_HPP
#define MUSE_BASE_SOCKET_HPP 1

#include <muse/platform/socket.hpp>

namespace muse {
    class Socket {
      public:
        /// NativeHandle is namely platform specific handle of resource, such as
        /// the file descriptor on Unix-like system, and the handle object on Zircon
        /// and Windows.
        using NativeHandle = PlatformSocketTraits::NativeHandle;

        static auto create() -> Socket;

        Socket(NativeHandle);

        auto bind(net::SocketAddr addr) -> void;

        auto listen() -> void;

        auto write(char const *buf, usize len) -> void;

        auto read(char *buf, usize len) -> isize;

        auto native_handle() const -> NativeHandle {
            return native_handle_;
        }
      private:
        NativeHandle native_handle_;
    };

} // namespace muse

#endif // #ifndef MUSE_BASE_SOCKET_HPP