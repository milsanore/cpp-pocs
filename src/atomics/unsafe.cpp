#include <thread>
#include <iostream>

int data = 0;
bool flag = false;

void producer() {
    data = 42;     // Step 1: write data
    flag = true;   // Step 2: signal that data is ready
}

void consumer() {
    while (!flag); // busy-wait until flag is true
    std::cout << data << std::endl;
}

///////////////////////////////////////////
// run multiple times and you may observe 42 OR 0
///////////////////////////////////////////

int main() {
    std::thread t1(producer);
    std::thread t2(consumer);
    t1.join();
    t2.join();
}
