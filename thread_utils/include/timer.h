#pragma once

#include <chrono>

class Timer {
public:
    void start();
    void stop();
    void pause();
    std::chrono::milliseconds chrono_elapsed_milliseconds() const;
    long elapsed_milliseconds() const { return chrono_elapsed_milliseconds().count(); }

private:
    std::chrono::time_point<std::chrono::system_clock> start_time;
    std::chrono::time_point<std::chrono::system_clock> end_time;
    bool is_running = false;
    bool was_running = false;
};
