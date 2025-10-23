#include <thread>
#include <iostream>
#include <chrono>
#include <cstdint>

constexpr uint64_t ITER = 1'000'000'000;


// False sharing
struct DataFalseSharing {
    volatile uint64_t x;
    volatile uint64_t y;
};

// Avoid false sharing
struct alignas(64) DataNoSharing {
    volatile uint64_t x;
    char pad1[56];
    volatile uint64_t y;
    char pad2[56];
};

template <typename T>
double run_test(T& data) {
    auto start = std::chrono::high_resolution_clock::now();

    std::thread t1([&]() {
        for (uint64_t i = 0; i < ITER; ++i)
            data.x++;
    });

    std::thread t2([&]() {
        for (uint64_t i = 0; i < ITER; ++i)
            data.y++;
    });

    t1.join();
    t2.join();

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> diff = end - start;
    return diff.count();
}

int main() {
    DataFalseSharing dfs{0, 0};
    DataNoSharing dns{0, 0};

    double t1 = run_test(dfs);
    std::cout << "False sharing time: " << t1 << " s\n";

    double t2 = run_test(dns);
    std::cout << "No sharing time: " << t2 << " s\n";
}
