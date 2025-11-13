#include <atomic>
#include <thread>
#include <iostream>

std::atomic<bool> flag{false};
int data = 0;

void producer() {
    data = 42;
    flag.store(true, std::memory_order_release);  // Release barrier
}

void consumer() {
    while (!flag.load(std::memory_order_acquire));  // Acquire barrier
    std::cout << data << std::endl;
}

///////////////////////////////////////////
// safe version, guarnateed to print 42
///////////////////////////////////////////

int main() {
    std::thread t1(producer);
    std::thread t2(consumer);
    t1.join();
    t2.join();
}
