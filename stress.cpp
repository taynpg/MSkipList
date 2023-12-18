#include <iostream>
#include <thread>
#include <chrono>
#include <string>
#include <mutex>
#include <random>
#include <climits>

#include "MSkipList.hpp"

#define THREAD_COUNT 1
#define TEST_COUNT 100000

std::random_device g_rd{};
std::mt19937       g_gen(g_rd());
std::uniform_int_distribution<int> g_dis(0, std::numeric_limits<int>::max());

std::mutex g_mutex{};
SkipList<int, std::string> skip(18);

void insertFunc()
{
    for (int i  = 0; i < TEST_COUNT; ++i) {
        g_mutex.lock();
        skip.insert(g_dis(g_gen), "Hello World");
        g_mutex.unlock();
    }
}

void searchFunc()
{
    std::string tmp{};
    for (int i  = 0; i < TEST_COUNT; ++i) {
        g_mutex.lock();
        skip.search(g_dis(g_gen), tmp);
        g_mutex.unlock();
    }
}

void insertTime()
{
    std::thread thread_insert[THREAD_COUNT];
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < THREAD_COUNT; ++i) {
        thread_insert[i] = std::thread(insertFunc);
    }
    for (int i = 0; i < THREAD_COUNT; ++i) {
        thread_insert[i].join();
    }
    auto finish = std::chrono::high_resolution_clock::now(); 
    std::chrono::duration<double> elapsed = finish - start;
    std::cout << "insert elapsed:" << elapsed.count() << std::endl;
}

void getTime()
{
    std::thread thread_search[THREAD_COUNT];
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < THREAD_COUNT; ++i) {
        thread_search[i] = std::thread(searchFunc);
    }
        for (int i = 0; i < THREAD_COUNT; ++i) {
        thread_search[i].join();
    }
    auto finish = std::chrono::high_resolution_clock::now(); 
    std::chrono::duration<double> elapsed = finish - start;
    std::cout << "get elapsed:" << elapsed.count() << std::endl;
}

int main()
{
    insertTime();
    getTime();
    return 0;
}