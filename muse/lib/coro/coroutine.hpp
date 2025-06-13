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
#ifndef CORO_COROUTINE_HPP
#define CORO_COROUTINE_HPP 1

#include <coroutine>

namespace coro {
    template <typename T>
    class Promise;

    class Task: public std::coroutine_handle<Promise<Task>> {
        typedef Task    Self;
        typedef std::coroutine_handle<Promise<Task>>    Handle;
      public:
    };

    template <typename T>
    class Promise {
        typedef T   Element;
      public:
        auto get_return_object() -> Task { 
            return Task::from_promise(*this);
        }

        auto initial_suspend() noexcept -> std::suspend_always {
            return {};
        }

        auto final_suspend() noexcept -> std::suspend_always { 
            return {};
        }

        // co_yield
        auto yield_value(Element element) noexcept -> std::suspend_always { return {}; }

        // co_return
        auto return_void() -> void {}

        // 
        auto unhandled_exception() -> void {}
      private:
        Element element_;
    };

} // namespace coro

#endif // #ifndef CORO_COROUTINE_HPP