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
#ifndef MUSE_STATUS_HPP
#define MUSE_STATUS_HPP 1

namespace muse {
    enum class MuseStatus {
        Ok,
        Unimplemented,
        Unsupported,
        Unreachable,
        Error,
        Fail,
        OutOfMem,
        OutOfRange,
        InvalidArguments,
        InternalError,
        BadState,
        TimeOut,
        NotFound,
        ShouldWait,
        ShouldRetry,
        MisAligned,
        AlreadyExists,
        NoResource,
        NoCapability,
        MaxCount,
    };

} // namespace muse

#endif // #ifndef MUSE_STATUS_HPP