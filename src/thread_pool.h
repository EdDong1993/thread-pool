#pragma once

#include <condition_variable>
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <queue>
#include <stdexcept>
#include <thread>
#include <vector>

class thread_pool {
public:
    thread_pool(const size_t threads = 4);
    thread_pool(const thread_pool &) = delete;
    thread_pool(thread_pool &&) = delete;
    thread_pool &operator=(const thread_pool &) = delete;
    thread_pool &operator=(thread_pool &&) = delete;
    ~thread_pool();

    template <typename F, typename... Args>
    auto push(F &&f, Args &&...args);

private:
    // working threads
    std::vector<std::thread> m_threads;

    // queue of tasks
    std::queue<std::function<void()>> m_tasks;

    // synchronization
    std::mutex m_mutex;
    std::condition_variable m_condition;
    bool m_stop;
};

inline thread_pool::thread_pool(size_t threads) : m_stop(false) {
    auto thread = [this] {
        for (;;) {
            std::function<void()> task;
            {
                std::unique_lock<std::mutex> lock(this->m_mutex);
                this->m_condition.wait(lock, [this] { return this->m_stop || !this->m_tasks.empty(); });
                if (this->m_stop && this->m_tasks.empty()) {
                    return;
                }
                task = std::move(this->m_tasks.front());
                this->m_tasks.pop();
            }
            task();
        }
    };
    for (size_t i = 0; i < threads; ++i) {
        m_threads.emplace_back(thread);
    }
}

template <typename F, typename... Args>
auto thread_pool::push(F &&f, Args &&...args) {
    // make packaged_task
    auto task = std::make_shared<std::packaged_task<typename std::invoke_result<F, Args...>::type()>>(std::bind(std::forward<F>(f), std::forward<Args>(args)...));
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        if (m_stop) {
            throw std::runtime_error("pushing a task to the stopped thread_pool");
        }
        // enqueue callable lambda which will execute task
        m_tasks.emplace([task]() { (*task)(); });
    }
    m_condition.notify_one();
    return task->get_future();
}

inline thread_pool::~thread_pool() {
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_stop = true;
    }
    m_condition.notify_all();
    for (std::thread &thread : m_threads) {
        thread.join();
    }
}
