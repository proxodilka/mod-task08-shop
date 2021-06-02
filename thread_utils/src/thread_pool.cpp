#include "thread_pool.h"
#include <iostream>

void ThreadPool::listen() {
    listen_cancellation = CancellationTokenSource();
    accepting_tasks_cancellation = CancellationTokenSource();
    auto cancellation_token = listen_cancellation.get_token();
    auto accepting_tasks_token = accepting_tasks_cancellation.get_token();
    listen_thread = std::thread(
        [cancellation_token, accepting_tasks_token, this]() {
            while (!accepting_tasks_token.is_cancellation_requested() || !requests_queue.empty()) {
                if (cancellation_token.is_cancellation_requested()) {
                    break;
                }
                for (auto& worker : pool) {
                    if (worker->ready_to_work()) {
                        lock_requests_queue.lock();
                        if (!requests_queue.empty()) {
                            Task t = requests_queue.front();
                            requests_queue.pop();
                            worker->start(t);
                        }
                        lock_requests_queue.unlock();
                        break;
                    }
                }
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
            this->listen_cancellation.cancel();
        }
    );
}

void ThreadPool::stop_listening() {
    listen_cancellation.cancel();
}

void ThreadPool::drain_call_queue() {
    accepting_tasks_cancellation.cancel();
    if (listen_thread.joinable()) {
        listen_thread.join();
    }
}

