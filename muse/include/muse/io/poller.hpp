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
#ifndef MUSE_POLLER_HPP
#define MUSE_POLLER_HPP 1

#define USING_POLLER(TYPE) \
    namespace muse {\
        using Poller = TYPE;\
    } // namespace muse

#if defined(__linux__)
#   include <muse/io/epoll.hpp>
    USING_POLLER(Epoll);
#elif defined(__WIN__)
#   include <muse/io/iocp.hpp>
    USING_POLLER(Iocp);
#endif

#endif // #ifndef MUSE_POLLER_HPP