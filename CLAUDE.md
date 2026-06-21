# CLAUDE.md — Airline Booking & TSP Solver

## 项目概述

一个用 C 语言开发的综合命令行工具，集成航班订票系统和旅行商问题（TSP）求解器。旨在通过实际项目展示 C 语言的数据结构设计、文件 I/O、经典算法实现能力。

## 代码风格规范

- **语言标准**: C11 (C99+ with `//` comments, `snprintf`, `strncpy`)
- **命名约定**: 驼峰命名，结构体类型用 `typedef`，函数前缀如 `init_`/`add_`/`find_`/`list_`
- **注释**: 中文注释，函数前简要说明功能
- **头文件**: 使用 `#ifndef / #define / #endif` 防止重复包含
- **错误处理**: 函数返回 `-1` 表示失败，打印 `fprintf(stderr, "Error: ...")`
- **内存管理**: 动态分配的内存需配对 `free()`，`calloc` 用于安全初始化

## 构建指令

```bash
make        # 编译项目，生成 airline_tsp
make run    # 编译并运行
make clean  # 清理编译产物
```

编译器使用 `gcc -Wall -Wextra -O2 -Iinclude -lm`。

## 项目结构

```
airline-booking-and-tsp/
├── src/          # 源代码
├── include/      # 头文件
├── data/         # 数据文件（CSV 格式）
├── output/       # 算法输出结果
├── docs/         # 文档（可选）
├── Makefile      # 构建脚本
├── README.md     # 项目文档
└── CLAUDE.md     # 本文件
```

## 关键设计决策

1. **CSV 文件持久化**: 航班和订票数据以 CSV 格式存储，便于手动编辑和调试
2. **三种 TSP 算法**: 穷举法保证最优解但限小规模；最近邻快速但次优；遗传算法平衡速度与质量
3. **遗传算法参数**: 种群大小 100、迭代 500 代、变异率 0.15，可在 `main.c` 的 `tspMenu()` 中调整
4. **预置样本数据**: 10 条航班覆盖中国主要城市航线，8 个城市供 TSP 求解
5. **RNG 种子**: 使用 `srand(time(NULL))`，每次运行遗传算法结果不同

## 注意事项

- `data/flights.txt` 和 `data/bookings.txt` 在程序退出时自动保存/覆盖
- 城市坐标使用简化经纬度，距离为欧氏距离（非实际球面距离）
- 订票取消退还 90% 金额（模拟退票手续费）
