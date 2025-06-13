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
#ifndef MUSE_EVENT_HANDLER_HPP
#define MUSE_EVENT_HANDLER_HPP 1

#include <muse/types.hpp>

#include <functional>

namespace muse {
    using ReadHandler = std::function<auto (TimePoint) -> void>;
    using WriteHandler = std::function<auto () -> void>;
    using ErrorHandler = std::function<auto () -> void>;
    using CloseHandler = std::function<auto () -> void>;

    class TcpConnection;
    using TcpConnectionPtr = std::shared_ptr<TcpConnection>;

    using MessageHandler = std::function<auto (TcpConnectionPtr const &) -> void>;
    using MessageHandler = std::function<auto (TcpConnectionPtr const &) -> void>;
    using MessageHandler = std::function<auto (TcpConnectionPtr const &) -> void>;

} // namespace muse

#endif // #ifndef MUSE_EVENT_HANDLER_HPP