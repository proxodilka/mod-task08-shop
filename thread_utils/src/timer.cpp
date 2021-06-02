#include "timer.h"

void Timer::start() {
    was_running = true;
    start_time = std::chrono::system_clock::now();
    is_running = true;
}

void Timer::stop() {
    end_time = std::chrono::system_clock::now();
    is_running = false;
}

std::chrono::milliseconds Timer::chrono_elapsed_milliseconds() const {
    if (!was_running) {
        return std::chrono::milliseconds(0);
    }

    std::chrono::time_point<std::chrono::system_clock> _end_time;

    if (is_running) {
        _end_time = std::chrono::system_clock::now();
    }
    else {
        _end_time = end_time;
    }

    return std::chrono::duration_cast<std::chrono::milliseconds>(_end_time - start_time);
}
