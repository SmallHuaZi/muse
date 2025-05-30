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

namespace muse {
    class Acceptor {
      public:
        virtual auto listen() -> void = 0;
      private:
        EventLoop *event_loop_;
    };

} // namespace muse

#endif // #ifndef MUSE_ACCEPTOR_HPP