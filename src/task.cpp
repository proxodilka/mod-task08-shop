#include "task.h"

void ShopEmulator::start(std::chrono::milliseconds duration) {

    std::random_device rd{};
    std::mt19937 gen{ rd() };
    std::normal_distribution random((double)mean_cart_size, 8.0);
    time_in_queue_accum = execution_time_accum = number_of_customers = 0;

    CancellationTokenSource draw_cancellation;

    cashboxes.listen();
    auto draw_thread = std::thread([this, &draw_cancellation]() { this->drawer(draw_cancellation.get_token()); });

    emulation_timer.start();
    while (emulation_timer.chrono_elapsed_milliseconds() < duration) {
        int cart_size = std::max((int)random(gen), 1);
        cashboxes.add_call_to_queue(DummyTask(cart_size * processing_duration, this));
        std::this_thread::sleep_for(timeout);
    }
    emulation_timer.stop();
    cashboxes.drain_call_queue();

    draw_cancellation.cancel();
    draw_thread.join();

    clear_console();
    print_statistics();
}

void ShopEmulator::print_statistics() {
    std::cout << "================== Shop statistics ==================" << std::endl;
    std::cout << "Total number of customers: " << cashboxes.get_number_of_recieved() << " | "
        << "Number of served: " << cashboxes.get_number_of_accepted() << " | "
        << "Number of declined: " << cashboxes.get_number_of_declined() << std::endl;
    std::cout << "Per thread statistic:" << std::endl;
    auto workers = cashboxes.get_workers_statistic();
    for (auto& stats : workers) {
        std::cout << "\tNumber of processed: " << stats.first << " | " << "Plain probability: " << stats.second << std::endl;
    }

    double decline_probability = cashboxes.get_decline_probability();
    double mean_time_in_queue = (double)time_in_queue_accum / number_of_customers;
    double relative_throughput = 1 - cashboxes.get_decline_probability();
    double absolute_throughput =
        ((double)cashboxes.get_number_of_recieved() / ((double)emulation_timer.elapsed_milliseconds() / TIME_TICK)) * relative_throughput;
    std::cout << "Decline probability: " << decline_probability << std::endl;
    std::cout << "Mean queue size: " << cashboxes.get_mean_queue_size() << std::endl;
    std::cout << "Mean time in queue: " << mean_time_in_queue << "ms" << std::endl;
    std::cout << "Relative throughput: " << relative_throughput << std::endl;
    std::cout << "Absolute throughput: " << absolute_throughput << std::endl;
}

void ShopEmulator::clear_console() {
#if defined _WIN32
    system("cls");
#elif defined (__LINUX__) || defined(__gnu_linux__) || defined(__linux__)
    system("clear");
#elif defined (__APPLE__)
    system("clear");
#endif
}

void ShopEmulator::drawer(CancellationToken token) {
    while (!token.is_cancellation_requested()) {
        clear_console();
        print_statistics();
        std::this_thread::sleep_for(CONSOLE_REFRESH_RATE);
    }
}

void ShopEmulator::task_completed(long time_in_queue, long execution_time) {
    time_in_queue_accum += time_in_queue;
    execution_time_accum += execution_time;
    number_of_customers += 1;
}

ShopEmulator::DummyTask::DummyTask(long duration_ms, ShopEmulator* statistics_owner)
    : duration_ms(duration_ms), statistics_owner(statistics_owner) {
    time_in_queue.start();
}

void ShopEmulator::DummyTask::operator()() const {
    time_in_queue.stop();
    std::this_thread::sleep_for(std::chrono::milliseconds(duration_ms));
    if (statistics_owner) {
        statistics_owner->task_completed(time_in_queue.elapsed_milliseconds(), duration_ms);
    }
}
