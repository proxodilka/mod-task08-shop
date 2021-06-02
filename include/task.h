#include "statistics.h"

#include <random>
#include <iostream>

const int TIME_TICK = 200;

class ShopEmulator {
public:
    ShopEmulator(int n_cashboxes, int customers_intensity, int service_intensity, int mean_cart_size, int queue_capacity)
        : cashboxes(n_cashboxes, queue_capacity), customers_intensity(customers_intensity),
        mean_cart_size(mean_cart_size), processing_duration(service_intensity), timeout(TIME_TICK / customers_intensity)
    {}
    void start(std::chrono::milliseconds duration);
    void print_statistics();

private:
    ThreadPoolStatistic cashboxes;
    std::chrono::milliseconds timeout;
    const std::chrono::milliseconds CONSOLE_REFRESH_RATE = std::chrono::milliseconds(100);
    Timer emulation_timer;

    int customers_intensity, mean_cart_size, processing_duration;

    long time_in_queue_accum = 0;
    long execution_time_accum = 0;
    long number_of_customers = 0;

    void clear_console();
    void drawer(CancellationToken token);
    void task_completed(long time_in_queue, long execution_time);

    class DummyTask {
        long duration_ms = 0;
        mutable Timer time_in_queue;
        ShopEmulator* statistics_owner;

    public:
        DummyTask(long duration_ms, ShopEmulator* statistics_owner = nullptr);
        void operator()() const;
    };
};
