#include <iostream>
#include <fstream>
#include <chrono>
#include <cstring>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

constexpr size_t NUM_LINES = 1'000'000;
constexpr const char* LOG_LINE = "This is a log line\n";
constexpr size_t LINE_LEN = 20; // strlen(LOG_LINE)
constexpr const char* FILE_WRITE = "log_write.txt";
constexpr const char* FILE_MMAP = "log_mmap.txt";

void benchmark_write() {
    int fd = open(FILE_WRITE, O_CREAT | O_WRONLY | O_TRUNC, 0644);
    if (fd < 0) { perror("open"); return; }

    auto start = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < NUM_LINES; ++i) {
        if (write(fd, LOG_LINE, LINE_LEN) != LINE_LEN) {
            perror("write"); break;
        }
    }
    fsync(fd); // force to disk
    close(fd);
    auto end = std::chrono::high_resolution_clock::now();

    std::cout << "write() benchmark: "
              << std::chrono::duration<double>(end - start).count()
              << " seconds\n";
}

void benchmark_mmap() {
    int fd = open(FILE_MMAP, O_CREAT | O_RDWR | O_TRUNC, 0644);
    if (fd < 0) { perror("open"); return; }

    size_t file_size = NUM_LINES * LINE_LEN;
    if (ftruncate(fd, file_size) != 0) { perror("ftruncate"); close(fd); return; }

    char* data = (char*)mmap(nullptr, file_size, PROT_WRITE, MAP_SHARED, fd, 0);
    if (data == MAP_FAILED) { perror("mmap"); close(fd); return; }

    auto start = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < NUM_LINES; ++i) {
        memcpy(data + i * LINE_LEN, LOG_LINE, LINE_LEN);
    }
    msync(data, file_size, MS_SYNC); // force to disk
    munmap(data, file_size);
    close(fd);

    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "mmap() + msync() benchmark: "
              << std::chrono::duration<double>(end - start).count()
              << " seconds\n";
}

int main() {
    benchmark_write();
    benchmark_mmap();
    return 0;
}
