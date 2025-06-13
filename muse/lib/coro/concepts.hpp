#ifndef CORO_CONCEPTS_HPP
#define CORO_CONCEPTS_HPP

#include <concepts>

namespace coro {
    template <typename T>
    concept Awaitable = requires (T t) {
        { t.await_ready() } -> std::same_as<bool>;
        { t.await_suspend() };
        { t.await_resume() };
    };

} // namespace coro

#endif // CORO_CONCEPTS_HPP
