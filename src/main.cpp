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
	
	void run() const {
		std::this_thread::sleep_for(time);
		std::cout << "Im dead" << std::endl;
	}
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

	bool is_free() {
		if (this->_is_free) {
			return true;
		}
		return false;
	}
	void serve(Task task) {
		if (!_is_free) {
            // wtf?
			throw std::exception();
		}
		_is_free = false;
		if (worker.joinable()) {
			worker.join();
		}
		worker = std::thread([this, task]() {
			task.run();
			this->_is_free = true;
		});
	}
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

	bool add_to_queue(Task task) {
		bool is_success = false;
		queue_lock.lock();
		if (clients.size() < max_queue_len) {
			clients.push(task);
			is_success = true;
		}
		queue_lock.unlock();
		return is_success;
	}

	void start_listening() {
		has_stopped = false;
		listen_thread = std::thread([this]() {
			while (!this->has_stopped || !this->clients.empty()) {
				for (int i=0; i<this->cashboxes.size(); i++) {
					if (this->cashboxes[i].is_free()) {
						this->queue_lock.lock();
						if (!this->clients.empty()) {
							Task task = this->clients.front();
							this->clients.pop();
							this->cashboxes[i].serve(task);
						}
						this->queue_lock.unlock();
					}
				}
				std::this_thread::sleep_for(std::chrono::milliseconds(1));
			}
		});
	}

	void stop_listening() {
		has_stopped = true;
		if (listen_thread.joinable()) {
			listen_thread.join();
		}
	}
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

	void run(int n_clients) {
		int tick = 200;
		std::chrono::milliseconds delay(tick / clients_intensity);
		for (int i = 0; i < n_clients; i++) {
			int n_products = mean_cart_size;
			bool res = server.add_to_queue(
				Task(std::chrono::milliseconds(n_products * product_time))
			);
			if (res) {
				accepted++;
			}
			else {
				declined++;
			}
			std::this_thread::sleep_for(delay);
		}
		server.stop_listening();
		std::cout << "Total clients: " << n_clients << " | Served: " << accepted << " | " << "Not served: " << declined << std::endl;
		std::cout << "Decline probability: " << declined / (double)n_clients << std::endl;
		std::cout << "Relative throughput: " << 1 - (declined / (double)n_clients) << std::endl;
		std::cout << "Absolute throughput: " << clients_intensity * (1 - (declined / (double)n_clients)) << std::endl;
	}
};

int main() {
	Store st(6, 10, 10, 5);
	st.run(100);
	return 0;
}
