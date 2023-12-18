# MSkipList
一个根据LevelDb源码修改的直接可用的跳表，纯标C++11准库使用，跨平台。

# 压力测试

参考[Skiplist-CPP](https://github.com/youngyangyang04/Skiplist-CPP)的压力测试代码：

测得在 Visual Studio 2022 x64 release环境下（Windows 10 Pro）：

- 随机写（10次均值）：0.05002037，即约20w条/s。
- 随机读（10次均值）：0.369919，即约27w条/s。