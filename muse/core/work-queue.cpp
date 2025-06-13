#include <muse/work-queue.hpp>

#include <mutex>
#include <chrono>

namespace muse {
    auto WorkQueue::run_tasks() -> usize {
        Queue running_queue;
        {
            std::lock_guard guard{queue_lock_};
            new (&running_queue) decltype(running_queue)(std::move(tasks_));
        }

        usize const num_ran = running_queue.size();
        if (!num_ran) {
            return 0;
        }

        using namespace std::chrono;
        auto const started_time = TimePoint::clock::now();
        for (auto &task : running_queue) {
            if (!task.is_runnable()) {
                continue;
            }

            running_ = &task;
            task.execute();
        }
        auto const consumed = TimePoint::clock::now() - started_time;
        running_ = nullptr;

        std::move(recent_samples_.begin() + 1, recent_samples_.end(), recent_samples_.begin());
        recent_samples_.back() = consumed;

        last_runtime_ = started_time;
        return num_ran;
    }

    auto WorkQueue::commit_task(Task const &task) -> void {
    }

    auto WorkQueue::insert_task(Task &task) -> void {
        {
            std::lock_guard guard{queue_lock_};
            tasks_.insert(task);
        }
        num_tasks_.fetch_add(1);
    }

} // namespace muse