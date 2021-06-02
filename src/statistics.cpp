#include "statistics.h"

WorkerStatistic::WorkerStatistic() : Worker() {
    timer_total.start();
}

WorkerStatistic::WorkerStatistic(WorkerStatistic&& other) : Worker(std::move(other)) {
    timer_total.start();
}

long WorkerStatistic::get_plain_time() const {
    return timer_total.elapsed_milliseconds() - work_time;
}

double WorkerStatistic::get_plain_probability() const {
    auto total_time = timer_total.elapsed_milliseconds();
    if (total_time == 0) {
        return 1;
    }
    return (total_time - work_time) / (double)total_time;
}

long WorkerStatistic::get_number_of_processed() const {
    return number_of_processed;
}

void WorkerStatistic::executor(const Task& task) {
    timer_work.start();
    Worker::executor(task);
    timer_work.stop();
    work_time += timer_work.elapsed_milliseconds();
    number_of_processed += 1;
}

ThreadPoolStatistic::ThreadPoolStatistic(size_t pool_size, size_t max_queue_size) 
    : ThreadPool(0, max_queue_size) {
    for (int i = 0; i < pool_size; i++) {
        pool.push_back(std::make_shared<WorkerStatistic>());
    }
}

void ThreadPoolStatistic::listen() {
    ThreadPool::listen();
    queue_length_monitoring_thread = std::thread([this]() { this->queue_monitoring(); });
    queue_length_monitoring_thread.detach();
}

double ThreadPoolStatistic::get_mean_queue_size() {
    double result;

    lock_queue_size.lock();
    result = (double)queue_size_accum / queue_size_n_observations;
    lock_queue_size.unlock();

    return result;
}

double ThreadPoolStatistic::get_plain_probability() {
    double accum = 0;
    for (auto worker : pool) {
        WorkerStatistic* wrk = static_cast<WorkerStatistic*>(worker.get());
        accum += wrk->get_plain_probability();
    }
    return accum / pool.size();
}

std::vector<std::pair<int, double>> ThreadPoolStatistic::get_workers_statistic() {
    std::vector<std::pair<int, double>> result;
    result.reserve(pool.size());
    for (auto worker : pool) {
        WorkerStatistic* wrk = static_cast<WorkerStatistic*>(worker.get());
        result.emplace_back(wrk->get_number_of_processed(), wrk->get_plain_probability());
    }
    return result;
}

void ThreadPoolStatistic::queue_monitoring() {
    while (!listen_cancellation.is_cancellation_requested()) {
        lock_requests_queue.lock();
        long queue_size = requests_queue.size();
        lock_requests_queue.unlock();

        lock_queue_size.lock();
        queue_size_accum += queue_size;
        queue_size_n_observations += 1;
        lock_queue_size.unlock();

        std::this_thread::sleep_for(refresh_frequency);
    }
}
