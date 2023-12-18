#include <iostream>
#include "MSkipList.hpp"

int main() {

    // 基本使用
    SkipList<int, std::string> skip;
    std::string tmp;

    skip.insert(101, "医用外科口罩");
    skip.insert(98, "C++标准库");
    skip.insert(12, "Hello World.");

    std::cout << "skip 元素个数：" << skip.count() << "\n";
    if (skip.search(12, tmp)) {
        std::cout << "12: " << tmp << "\n";
    }

    if (skip.search(98, tmp)) {
        std::cout << "98: " << tmp << "\n";
    }
    skip.remove(98);
    std::cout << "skip 元素个数：" << skip.count() << "\n";
    if (!skip.search(98, tmp)) {
        std::cout << "98: 未找到\n";
    }

    // 析构测试
    auto* pSkip = new SkipList<int, std::string>(12);
    pSkip->insert(101, "医用外科口罩");
    pSkip->insert(98, "C++标准库");
    pSkip->insert(12, "Hello World.");
    delete pSkip;

    return 0;
}
