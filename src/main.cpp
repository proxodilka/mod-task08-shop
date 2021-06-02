#include <thread>
#include <vector>
#include <queue>
#include <functional>
#include <iostream>
#include <mutex>
#include "task.h"

int main() {
    ShopEmulator emulator(
        /*n_threads=*/8,
        /*customers_intensity=*/40,
        /*service_intensity=*/10,
        /*mean_cart_size=*/10,
        /*queue_capacity=*/10
    );
    emulator.start(std::chrono::milliseconds(12000));
    return 0;
}

