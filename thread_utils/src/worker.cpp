#include "thread_pool.h"

bool Worker::ready_to_work() {
    if (is_free) {
        if (thread.joinable()) {
            thread.join();
        }
        return true;
    }
    return false;
}

void Worker::start(Task task) {
    is_free = false;
    thread = std::thread([this, task]() { this->executor(task); });
}

void Worker::executor(const Task& task) {
    task.run();
    this->is_free = true;
}
