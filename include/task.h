#include <iostream>
#include <thread>
#include <queue>
#include <vector>
#include <memory>
#include <mutex>
#include <atomic>

class Task {
	std::chrono::milliseconds time;
	
public:
	Task(std::chrono::milliseconds time): time(time) {}
	void run() const;
};

class Cashbox {
	std::thread worker;
	std::atomic<bool> _is_free = true;
public:
	Cashbox() {}
	Cashbox(Cashbox&& other): worker(std::move(other.worker)), _is_free(other._is_free.load()) {}
	~Cashbox() {
		if (worker.joinable()) {
			worker.join();
		}
	}

	bool is_free();
	void serve(Task task);
};

class Server {
private:
	std::queue<Task> clients;
	std::vector<Cashbox> cashboxes;
	std::mutex queue_lock;
	std::thread listen_thread;

	int max_queue_len;
	std::atomic<bool> has_stopped = false;

public:
	Server(int n_cashboxes, int max_queue_len): max_queue_len(max_queue_len) {
		cashboxes.reserve(n_cashboxes);
		for (int i = 0; i < n_cashboxes; i++) {
			cashboxes.emplace_back(Cashbox());
		}
	}

	~Server() {
		stop_listening();
		if (listen_thread.joinable()) {
			listen_thread.join();
		}
	}

	bool add_to_queue(Task task);
	void start_listening();
	void stop_listening();
};

class Store {
	Server server;
	int mean_cart_size;
	int product_time;
	int clients_intensity;

	int accepted = 0, declined = 0;

public:
	Store(int n_cashboxes, int max_queue_size, int mean_cart_size, int clients_intensity)
		: server(n_cashboxes, max_queue_size), mean_cart_size(mean_cart_size), clients_intensity(clients_intensity) {
	}
	void run(int n_clients);
};
