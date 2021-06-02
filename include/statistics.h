#include "thread_utils.h"
#include "thread_pool.h"
#include "timer.h"

class WorkerStatistic : public Worker {
public:
	WorkerStatistic();
	WorkerStatistic(WorkerStatistic&& other);
	long get_plain_time() const;
	double get_plain_probability() const;
	long get_number_of_processed() const;

protected:
	void executor(const Task& task) override;

	Timer timer_total;
	Timer timer_work;
	std::atomic<long> work_time = 0;
	std::atomic<long> number_of_processed = 0;
};

class ThreadPoolStatistic : public ThreadPool {
public:
	ThreadPoolStatistic(size_t pool_size, size_t max_queue_size = INFINITE);

	template<class Function, class... Args>
	bool add_call_to_queue(Function&& f, Args&&... args) {
		recieved += 1;
		bool result = ThreadPool::add_call_to_queue(f, args...);
		if (result) {
			accepted += 1;
		}
		return result;
	}

	void listen();
	long get_number_of_recieved() { return recieved; }
	long get_number_of_declined() { return recieved - accepted; }
	long get_number_of_accepted() { return accepted; }
	double get_decline_probability() { return get_number_of_declined() / (double)recieved; }
	double get_mean_queue_size();
	double get_plain_probability();
	std::vector<std::pair<int, double>> get_workers_statistic();

protected:
    void queue_monitoring();

	long accepted = 0;
	long recieved = 0;
	long queue_size_accum = 0;
	long queue_size_n_observations = 0;

	std::thread queue_length_monitoring_thread;
	std::chrono::milliseconds refresh_frequency = std::chrono::milliseconds(50);
	std::mutex lock_queue_size;
};
