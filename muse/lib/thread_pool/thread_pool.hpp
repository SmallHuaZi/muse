#ifndef LIB_THREADPOOL_THREAD_POOL_HPP
#define LIB_THREADPOOL_THREAD_POOL_HPP

#include <vector>

template <typename T>
concept Thread = requires (T t) {
    { t.resume() };
};

template <typename Thread>
class ThreadPool {
  public:

  private:
    std::vector<Thread *> threads_;
};

#endif // LIB_THREADPOOL_THREAD_POOL_HPP