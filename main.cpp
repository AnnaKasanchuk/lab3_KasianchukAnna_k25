// Compiler: Bundled MinGW 14.0 w64 (GCC 14), CLion toolchain
// Build system: CMake 4.3.1 + Ninja
// Language standard: C++20

#include <chrono>
#include <iostream>
#include <latch>
#include <map>
#include <memory>
#include <string>
#include <syncstream>
#include <thread>
#include <vector>

struct Task {
    char set;
    int id;
};

void f(char x, int i) {
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::osyncstream(std::cout) << "From set " << x << " task " << i << " completed.\n";
}

int main() {
    constexpr int nt = 4;

    const std::map<char, int> counts = {
        {'a', 4}, {'b', 8}, {'c', 6}, {'d', 6}, {'e', 7}, {'f', 7},
        {'g', 6}, {'h', 7}, {'i', 5}, {'j', 9}, {'k', 3}
    };

    const std::map<char, std::string> deps = {
        {'d', "bc"}, {'e', "a"}, {'g', "a"}, {'i', "e"},
        {'j', "e"}, {'h', "dg"}, {'k', "i"}, {'f', "hj"}
    };

    std::map<char, std::unique_ptr<std::latch>> done;
    for (const auto& [name, n] : counts)
        done[name] = std::make_unique<std::latch>(n);

    const std::vector<std::vector<Task>> schedules = {
        { {'a',1}, {'b',1}, {'b',5}, {'c',1}, {'c',5}, {'d',1}, {'d',5}, {'e',5}, {'g',2}, {'g',6}, {'h',1}, {'h',5}, {'i',5}, {'j',4}, {'j',8}, {'k',3}, {'f',4} },
        { {'a',2}, {'b',2}, {'b',6}, {'c',2}, {'c',6}, {'d',2}, {'d',6}, {'e',6}, {'g',3}, {'i',1}, {'h',2}, {'h',6}, {'j',1}, {'j',5}, {'j',9}, {'f',1}, {'f',5} },
        { {'a',3}, {'b',3}, {'b',7}, {'c',3}, {'e',1}, {'d',3}, {'e',3}, {'e',7}, {'g',4}, {'i',2}, {'h',3}, {'h',7}, {'j',2}, {'j',6}, {'k',1}, {'f',2}, {'f',6} },
        { {'a',4}, {'b',4}, {'b',8}, {'c',4}, {'e',2}, {'d',4}, {'e',4}, {'g',1}, {'g',5}, {'i',3}, {'h',4}, {'i',4}, {'j',3}, {'j',7}, {'k',2}, {'f',3}, {'f',7} }
    };

    auto worker = [&](int id) {
        for (const auto& [set, num] : schedules[id]) {
            if (auto it = deps.find(set); it != deps.end())
                for (char d : it->second)
                    done.at(d)->wait();
            f(set, num);
            done.at(set)->count_down();
        }
    };

    std::osyncstream(std::cout) << "Computation started.\n";
    const auto start = std::chrono::steady_clock::now();
    {
        std::vector<std::jthread> threads;
        for (int i = 0; i < nt; ++i)
            threads.emplace_back(worker, i);
    }
    const auto sec = std::chrono::duration_cast<std::chrono::seconds>(
        std::chrono::steady_clock::now() - start).count();

    std::osyncstream(std::cout) << "Computation completed. Duration " << sec << " seconds.\n";
}