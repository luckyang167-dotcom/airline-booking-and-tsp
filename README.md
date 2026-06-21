# ✈️ Airline Booking & TSP Solver

> **航线订票系统与旅行商问题求解器** —— 一个集航班管理、在线订票与路径优化于一体的 C 语言综合项目。

---

## 📋 项目简介

本项目由两个核心子系统组成：

1. **航线订票系统 (Airline Booking System)** — 模拟航空公司票务管理，支持航班查询、座位预订、订单取消等完整业务流程。
2. **TSP 求解器 (Traveling Salesman Problem Solver)** — 针对旅行商问题提供三种不同策略的求解算法：穷举法、最近邻贪心算法、遗传算法。

该项目以 C 语言实现，通过友好的命令行交互菜单串联两大功能模块，可用于学习数据结构、算法设计与系统集成。

---

## 🛠️ 技术栈

| 技术 | 说明 |
|------|------|
| **C11 (C99+)** | 项目主体语言，遵循 C11 标准 |
| **GCC** | 编译器，启用 `-Wall -Wextra -O2` 编译检查 |
| **Make** | 构建工具，支持 `make` / `make clean` / `make run` |
| **CSV 文件 I/O** | 航班数据与订票数据的持久化存储 |
| **算法设计** | 包含穷举法、贪心算法、遗传算法 |

---

## 🏗️ 系统架构

### 模块划分

```
┌─────────────────────────────────────────────────────────┐
│                     main.c (主入口)                       │
│               CLI 交互菜单 + 数据初始化                     │
└────────┬────────────┬────────────────────┬──────────────┘
         │            │                    │
         ▼            ▼                    ▼
┌─────────────────┐ ┌─────────────────┐ ┌─────────────────┐
│  flight.h/c     │ │  booking.h/c    │ │  tsp.h/c         │
│  航班数据库管理   │ │  订票系统管理     │ │  TSP 算法求解    │
│                 │ │                 │ │                  │
│ • 航班增删查     │ │ • 创建/取消订单  │ │ • 穷举法 BF     │
│ • 路线搜索       │ │ • 座位管理      │ │ • 最近邻 NN     │
│ • CSV 文件读写   │ │ • 订单记录持久化  │ │ • 遗传算法 GA   │
│ • 航班列表展示   │ │ • 乘客订单查询   │ │ • 城市管理      │
└─────────────────┘ └─────────────────┘ └─────────────────┘
         │                    │                    │
         ▼                    ▼                    ▼
    data/flights.txt    data/bookings.txt    data/cities.txt
```

### 文件结构

```
airline-booking-and-tsp/
├── include/
│   ├── flight.h       # 航班数据结构与函数声明
│   ├── booking.h      # 订票数据结构与函数声明
│   └── tsp.h          # TSP 数据结构与函数声明
├── src/
│   ├── main.c         # 主入口、CLI 界面、数据初始化
│   ├── flight.c       # 航班数据库管理实现
│   ├── booking.c      # 订票系统实现
│   └── tsp.c          # TSP 算法实现
├── data/
│   └── cities.txt     # 城市坐标样本数据
├── output/            # 算法结果输出目录
├── Makefile           # 构建脚本
├── README.md          # 项目文档（本文件）
└── CLAUDE.md          # 项目开发规范
```

---

## 🧠 核心算法详解

### 1️⃣ 穷举法 (Brute Force)

**原理**：枚举所有可能的城市排列顺序，计算每条路径的总距离，取最小值。

**时间复杂度**：O(n!) — 仅适用于城市数量较少的场景（≤10 个城市）。

**代码路径**：`src/tsp.c` → `solveTSPBruteForce()`

### 2️⃣ 最近邻贪心算法 (Nearest Neighbor)

**原理**：从起点城市出发，每一步选择距离当前城市最近的未访问城市，直至所有城市都被访问。

**时间复杂度**：O(n²) — 适用于大规模城市。

**特点**：速度快但解的质量一般，通常作为其他算法的初始解或对比基准。

**代码路径**：`src/tsp.c` → `solveTSPNearestNeighbor()`

### 3️⃣ 遗传算法 (Genetic Algorithm)

**原理**：模拟自然进化过程，通过选择、交叉（有序交叉 OX）、变异（交换变异）不断优化路径。

**算法流程**：
```
① 初始化种群（随机排列）
② 评估适应度（路径越短适应度越高）
③ 锦标赛选择
④ 有序交叉 (OX) 产生子代
⑤ 交换变异（15% 概率）
⑥ 重复 ②~⑤ 直到达到指定代数
```

**时间复杂度**：O(popSize × generations × n)

**特点**：在解的质量和计算时间之间取得良好平衡，适合中等规模城市（10~100 个）。

**代码路径**：`src/tsp.c` → `solveTSPGenetic()`

### 算法对比

| 算法 | 时间复杂度 | 解的质量 | 适用规模 |
|------|-----------|---------|---------|
| 穷举法 | O(n!) | 最优解 | ≤ 10 城市 |
| 最近邻 | O(n²) | 近似解 | 任意 |
| 遗传算法 | O(p·g·n) | 近似最优 | ≤ 100 城市 |

---

## 🚀 编译与运行

### 环境要求
- GCC 编译器 (支持 C11)
- GNU Make
- Linux / macOS / WSL

### 编译

```bash
# 克隆项目
git clone <your-repo-url>
cd airline-booking-and-tsp

# 编译
make clean && make
```

### 运行

```bash
# 方式一：通过 Make
make run

# 方式二：直接运行
./airline_tsp
```

### 清理

```bash
make clean
```

---

## 💻 运行示例

### 主菜单
```
  =============================================
  ||      Airline Booking & TSP Solver       ||
  ||         C Language Project              ||
  =============================================
1. Booking System
2. TSP Solver
0. Exit
Choice:
```

### 航班查询
```
Code       Airline         Departure       -> Arrival         Dep    Arr     Min     Price  Seats
CA1234     Air China       Beijing         -> Shanghai        07:00  09:15   135   1200.00  180/180
MU5678     China Eastern   Shanghai        -> Guangzhou       10:00  12:30   150    980.00  200/200
...
```

### TSP 求解（遗传算法）
```
Solving with Genetic Algorithm (pop=100, gen=500)...

Optimized Route:
  Beijing -> Xi'an -> Chengdu -> Kunming -> Shenzhen -> Guangzhou -> Wuhan -> Shanghai -> Beijing
Total distance: 5172.34

Result saved to output/tsp_result.txt
```

---

## 📝 许可

本项目仅供学习和交流使用。
