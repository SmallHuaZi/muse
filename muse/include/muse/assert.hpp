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
#ifndef MUSE_ASSERT_HPP
#define MUSE_ASSERT_HPP 1

#include <cassert>

#define ASSERT(EXPR, ...) \
    if (!(EXPR)) {\
        assert((EXPR));\
    }

#define DEBUG_ASSERT(EXPR, ...) \
    if constexpr (MUSE_DEBUG) {\
        ASSERT(EXPR, __VA_ARGS__);\
    }

#endif // #ifndef MUSE_ASSERT_HPP