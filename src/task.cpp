#include "task.h"
	
void Task::run() const {
    std::this_thread::sleep_for(time);
    std::cout << "Im dead" << std::endl;
}

bool Cashbox::is_free() {
    if (this->_is_free) {
        return true;
    }
    return false;
}

void Cashbox::serve(Task task) {
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

bool Server::add_to_queue(Task task) {
    bool is_success = false;
    queue_lock.lock();
    if (clients.size() < max_queue_len) {
        clients.push(task);
        is_success = true;
    }
    queue_lock.unlock();
    return is_success;
}

void Server::start_listening() {
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

void Server::stop_listening() {
    has_stopped = true;
    if (listen_thread.joinable()) {
        listen_thread.join();
    }
}

void Store::run(int n_clients) {
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
