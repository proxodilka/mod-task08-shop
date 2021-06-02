#pragma once

#include <thread>
#include <functional>
#include <atomic>

class CancellationTokenSource;

class CancellationToken {
private:
    const CancellationTokenSource& parent;

    CancellationToken(const CancellationTokenSource& parent) : parent(parent) {}
public:
    CancellationToken() = delete;

    bool is_cancellation_requested() const;

    friend CancellationTokenSource;
};

class CancellationTokenSource {
public:
    CancellationTokenSource() : _is_cancellation_requested(false) {}

    void cancel() {
        _is_cancellation_requested = true;
    }

    bool is_cancellation_requested() const {
        return _is_cancellation_requested;
    }

    CancellationToken get_token() const {
        return CancellationToken(*this);
    }
private:
    bool _is_cancellation_requested;
};

inline bool CancellationToken::is_cancellation_requested() const {
    return parent.is_cancellation_requested();
}

class Task {
public:
    template<class Function, class... Args>
    Task(Function f, Args&&... args) : func([f, args...]() {
        f(args...);
    }) { }

    Task(const Task& other) : func(other.func) {}
    Task(Task&& other) : func(other.func) {}
    void operator()() { run(); }
    void run() const { func(); }
private:
    std::function<void()> func;
};

class Worker {
public:
    Worker() : is_free(true) {}
    Worker(Worker&& other) : thread(std::move(other.thread)), is_free(other.is_free.load()) {}

    ~Worker() {
        if (thread.joinable()) {
            thread.detach();
        }
    }

    bool ready_to_work();
    void start(Task task);
    void join() { thread.join(); }

protected:
    void virtual executor(const Task& task);

private:
    std::thread thread;
    std::atomic<bool> is_free;
};
