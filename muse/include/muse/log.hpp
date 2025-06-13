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
#ifndef MUSE_LOG_HPP
#define MUSE_LOG_HPP 1

#include <spdlog/spdlog.h>

namespace muse::log {
    using spdlog::error;
    using spdlog::warn;
    using spdlog::info;
    using spdlog::trace;
    using spdlog::debug;
}

#endif // #ifndef MUSE_LOG_HPP