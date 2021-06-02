#pragma once

#include <thread>
#include <vector>
#include <functional>
#include <atomic>
#include <queue>
#include <mutex>
#include <memory>
#include "thread_utils.h"

class ThreadPool {
protected:
    static const size_t INFINITE = -1;
    std::vector<std::shared_ptr<Worker>> pool;
    std::queue<Task> requests_queue;
    std::mutex lock_requests_queue;

    std::thread listen_thread;

    const size_t max_queue_size;
    CancellationTokenSource listen_cancellation, accepting_tasks_cancellation;

public:
    ThreadPool(size_t pool_size, size_t max_queue_size=INFINITE): max_queue_size(max_queue_size) {
        for (int i = 0; i < pool_size; i++) {
            pool.push_back(std::make_shared<Worker>());
        }
    }
    ~ThreadPool() {
        listen_cancellation.cancel();
        if (listen_thread.joinable()) {
            listen_thread.join();
        }
    }

    void drain_call_queue();
    void listen();
    void stop_listening();

    template<class Function, class... Args>
    bool add_call_to_queue(Function&& f, Args&&... args) {
        if (accepting_tasks_cancellation.is_cancellation_requested()) {
            throw std::runtime_error("Can't add new task: ThreadPool is draining its call queue.");
        }
        bool status = false;
        lock_requests_queue.lock();
        if (requests_queue.size() < max_queue_size) {
            requests_queue.emplace(f, args...);
            status = true;
        }
        lock_requests_queue.unlock();
        return status;
    }
};
