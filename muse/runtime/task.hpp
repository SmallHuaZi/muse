#ifndef MUSE_RUNTIME_TASK_HPP
#define MUSE_RUNTIME_TASK_HPP

#include <functional>
#include <muse/types.hpp>

#include <concepts>
#include <memory>
#include <coroutine>
#include <type_traits>

#include <boost/intrusive/list_hook.hpp>

#include <enumbits/atomic.hpp>
#include <util/uuid.hpp>
#include <utility>

namespace muse::runtime {
    class Task;
    class Scheduler;

    template <typename T>
    class Promise;

    template <typename T>
    class JoinHandle;

    class Task: public std::enable_shared_from_this<Task> {
        using Self = Task;
        using Base = boost::intrusive::list_base_hook<>;
      public:
        /// Spawns a new task and place it onto runqueue of this CPU.
        template <typename F, typename... Args>
            requires std::invocable<F, Args...>
        static auto spawn(F &&f, Args &&...args) -> JoinHandle<std::invoke_result_t<F, Args...>>;

        enum class State {
            /// Task is being run.
            Running = BIT(0),

            /// Task is waiting to be woken.
            Waiting = BIT(1),

            Complete = BIT(2),
        };
        MUSE_ENABLE_ENUMBITS_INNER(State);

        struct Id {
            static auto next() -> usize {
                return tid_gen.gen();
            }

            static inline util::Uuid tid_gen;
        };
      protected:
        friend Scheduler;

        template <typename T>
        friend class JoinHandle;

        static auto create(std::coroutine_handle<>) -> std::shared_ptr<Self>;

        usize m_id;
        enumbits::Atomic<State> m_state;
        std::coroutine_handle<> m_coroutine;
        std::shared_ptr<Scheduler> m_scheduler;

        boost::intrusive::list_member_hook<> m_rqhook;
    };

    template <typename F, typename... Args>
        requires std::invocable<F, Args...>
    auto Task::spawn(F &&f, Args &&...args) -> JoinHandle<std::invoke_result_t<F, Args...>> {
        auto routine = std::bind(std::forward<F>(f), std::forward<Args>(args)...);
        struct SpawnOperation {
            using Result = std::invoke_result_t<F, Args...>;
            using Handle = std::coroutine_handle<Promise<Result>>;

            auto await_ready() const noexcept -> bool {
                return false;
            }

            auto await_suspend(Handle handle) -> Handle {
                m_task = Task::create(handle);
                std::invoke(m_routine);
                return handle;
            }

            auto await_resume() noexcept -> JoinHandle<Result> {
                return JoinHandle<Result>(*m_task);
            }

            Task *m_task;
            decltype(routine) m_routine;
        };
        co_return co_await SpawnOperation(std::forward<F>(f), std::forward<Args>(args)...);
    }

    template <typename T>
    class Promise {
      public:
        using Result = T;

        auto get_return_object() -> Task { return { Task::from_promise(*this)}; }
        auto initial_suspend()  const -> std::suspend_always {}
        auto final_suspend() const -> std::suspend_always {}
        auto unhandled_exception() -> void {}

        auto return_value(Result const &result) -> void {
            m_result = result;
        }

        auto return_value(Result &&result) -> void {
            m_result = std::move(result);
        }

        auto return_void() -> void {}

        auto result() -> Result {
            return m_result;
        }
      private:
        Task m_task;
        Result m_result;
    };

    template <typename T>
    class JoinHandle {
        using Self = JoinHandle;
        using Promise = Promise<T>;
        using CoroutineHandle = std::coroutine_handle<Promise>;
      public:
        using Result = T;

        struct JoinOperation {
            auto await_ready() const -> bool {
                return m_task.m_state.fetch_test(Task::State::Complete);
            }

            auto await_suspend(CoroutineHandle handle) const -> void {
                if (!m_task.m_state.fetch_test(Task::State::Complete)) {
                    m_task.m_scheduler->schedule(handle);
                }
            }

            auto await_resume() -> Result {
                if constexpr (!std::is_void_v<Result>) {
                    return CoroutineHandle::from_address(m_task.m_coroutine.address())
                          .promise().result();
                }
            }

            Task const &m_task;
        };

        auto join() -> JoinOperation {
            return JoinOperation(this->m_task);
        }
      private:
        friend Task;

        Task const &m_task;
    };
} // namespace muse::runtime

#endif // MUSE_RUNTIME_TASK_HPP
