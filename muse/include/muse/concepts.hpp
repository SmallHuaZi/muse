/// Copyright(C) 2024 smallhuazi

#ifndef INCLUDE_MUSE_CONCEPTS_HPP
#define INCLUDE_MUSE_CONCEPTS_HPP
///
/// This program is free software; you can redistribute it and/or modify
/// it under the terms of the GNU General Public License as published
/// by the Free Software Foundation; either version 2 of the License, or
/// (at your option) any later version.
///
/// For additional information, please refer to the following website:
/// https://opensource.org/license/gpl-2-0
///
#ifndef CORO_CONCEPTS_HPP
#define CORO_CONCEPTS_HPP 1

#include <concepts>
#include <coroutine>

namespace coro::concepts {
    template <typename T, typename... Ts>
    concept SameAs = (std::same_as<T, Ts> || ...);

    /// Concept defining requirements for an awaitable type
    /// Used by co_await expression to control coroutine suspension/resumption
    template <typename T>
    concept Awaitable = requires (T t) {
        /// Called first when evaluating co_await expression
        /// - Return true: skip suspension, immediately continue to await_resume()
        /// - Return false: proceed with suspension flow
        { t.await_ready() } -> SameAs<bool>;

        /// Called after await_ready() returns false, BEFORE coroutine suspends
        /// - Receives handle to current coroutine
        /// - May return:
        ///   * void: always suspend
        ///   * bool: true=suspend, false=don't kuspend
        ///   * coroutine_handle: suspend and resume specified handle (symmetric transfer)
        { t.await_suspend(std::coroutine_handle<>()) } -> SameAs<void, bool, std::coroutine_handle<>>;

        /// Called after coroutine resumes (if suspended)
        /// - Return value becomes co_await expression result
        /// - May throw exceptions which propagate to coroutine
        { t.await_resume() };
    };

    /// Concept defining requirements for a coroutine promise type
    /// Controls coroutine behavior and lifecycle
    template <typename T>
    concept Promise = requires (T t) {
        /// Called immediately when coroutine is first called
        /// - suspend_always: coroutine starts suspended (lazy)
        /// - suspend_never: coroutine starts executing immediately (eager)
        { t.initial_suspend() } -> SameAs<std::suspend_always, std::suspend_never>;

        /// Called after coroutine body completes (via co_return or uncaught exception)
        /// BUT BEFORE coroutine frame is destroyed
        /// - suspend_always: coroutine remains suspended (manual destruction required)
        /// - suspend_never: coroutine automatically destroys itself
        { t.final_suspend() } -> SameAs<std::suspend_always, std::suspend_never>;

        /// Exactly one of these must be implemented:
        /// - return_void(): called when co_return; or falling off end of coroutine
        /// - return_value(): called when co_return expr; is used
        (requires {{ t.return_void() } -> SameAs<void>; }) || (requires {{ t.return_value() }; });
    };

} // namespace coro

#endif // CORO_CONCEPTS_HPP


#endif // INCLUDE_MUSE_CONCEPTS_HPP
