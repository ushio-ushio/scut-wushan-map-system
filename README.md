# SCUT 五山校区地图导航（数据结构课程设计）

本项目为小组数据结构课程设计作业：针对 **华南理工大学五山校区** 的地图数据，提供 **建筑信息查询** 与 **建筑间最短路径规划**（步行/驾车）等功能。citeturn1view0

---

## 目录

- [功能概览](#功能概览)
- [项目结构](#项目结构)
- [环境与编译](#环境与编译)
- [运行方式](#运行方式)
- [输入数据格式](#输入数据格式)
- [数据来源与获取](#数据来源与获取)
- [算法与实现要点](#算法与实现要点)
- [注意事项（重要）](#注意事项重要)
- [常见问题](#常见问题)
- [致谢](#致谢)

---

## 功能概览

- **建筑信息查询**
  - 输入建筑名称（支持一定程度的模糊匹配）
  - 输出该建筑的描述信息（多行文本）
- **路径规划**
  - 输入起点建筑、终点建筑
  - 计算并输出两类方案：
    - **驾车最短路径**（基于 `car_dis` 权重）
    - **步行最短路径**（基于 `walk_dis` 权重）
  - 输出路径经过的路口序列（Intersection ID）
- **交互式命令行菜单**
  - 运行后按提示选择：查询 / 规划 / 退出

---

## 项目结构

当前仓库主分支包含 1 个核心源码文件：

```
.
└── Map.cpp        # 主程序（读取数据 → 预处理最短路 → 菜单交互）
```

建议你后续补充（可选但强烈推荐）：

```
data/
  data.txt         # 可直接运行的示例输入
docs/
  report.pdf       # 课程设计报告
tools/
  convert.py       # 从 OSM/GeoJSON 转换到程序输入格式的脚本
```

---

## 环境与编译

### 方式 A：g++ / clang++（推荐，最通用）

> 需要支持 C++17 的编译器

```bash
g++ -std=c++17 -O2 -o scut Map.cpp
```

运行：

```bash
./scut < data/data.txt
```

### 方式 B：Windows + VS Code（你当前的环境）

1. 安装 **MinGW-w64** 或 **MSYS2**（确保 `g++` 可用）
2. VS Code 安装扩展：
   - C/C++（Microsoft）
3. 打开项目文件夹 → `Terminal` 里执行：

```powershell
g++ -std=c++17 -O2 -o scut.exe Map.cpp
.\scut.exe < data\data.txt
```

> 如果你没有准备 `data.txt`，程序会等待你从键盘输入完整数据。

---

## 运行方式

程序启动后会显示菜单（英文提示）：

- 输入 `1`：查询建筑信息
- 输入 `2`：规划路径
- 输入 `0`：退出

路径规划模式下：

1. 输入起点建筑名
2. 输入终点建筑名
3. 程序会分别给出驾车/步行最短方案（若数据与代码逻辑完整）

---

## 输入数据格式

> **重要：** 程序在 `main()` 中先 `ScanMap()` 从标准输入读入全部地图数据，再 `Init()` 做最短路预处理，然后进入交互菜单。

下面按 `ScanMap()` 的读取顺序说明（所有字符串使用 `cin >>` 读取，所以 **不能包含空格**）。

### 1）道路（Street）

```
M
name_1
name_2
...
name_M
length_1 flag_1
length_2 flag_2
...
length_M flag_M
```

- `M`：道路数量（int）
- `name_i`：道路名（string）
- `length_i`：道路长度（int）
- `flag_i`：道路标记（int，含义由你们数据定义）

---

### 2）路口（Intersection）及其关联信息

```
K
x1  b11 b12 ... b1x1
y1  s11 s12 ... s1y1
x2  ...
y2  ...
...
xK  ...
yK  ...
```

- `K`：路口数量（int）
- 每个路口读两段列表：
  - 先读 `x` 个字符串
  - 再读 `y` 个字符串

> 说明：当前代码会把两段字符串都存进 `ints[i].buildings`。若你们原意是分别存“建筑名/道路名”，建议把第二段改为 `ints[i].streetname.push_back(...)`，并修正循环变量命名。

---

### 3）路口之间的有向边（Intersection Edge）

```
E
u1 v1 walk1 car1
u2 v2 walk2 car2
...
uE vE walkE carE
```

- `E`：边数（int）
- 每条边是有向边 `u -> v`
- `walk`：步行权重（存入 `walk_dis`）
- `car`：驾车权重（存入 `car_dis`）

---

### 4）建筑（Building）

```
B
building_name_1
t  c1 c2 ... ct
d  desc1 desc2 ... descd
building_name_2
...
```

- `B`：建筑数量（int）
- 对每个建筑：
  - `building_name`：建筑名（string）
  - `t`：该建筑关联的路口数量（int）
  - 后面跟 `t` 个路口编号（int）
  - `d`：描述条目数量（int）
  - 后面跟 `d` 个描述字符串（string）

> 说明：描述字符串同样用 `cin >>` 读取，因此默认不支持带空格的长句；如需支持，请改为 `getline` 或改为用分隔符/引号包裹。

---

## 数据来源与获取

地图数据来源于 **OpenStreetMap**。本项目使用 **Overpass Turbo** 作为查询与导出工具：

- Overpass Turbo 是一个 **Web 端 OSM 数据挖掘工具**，可运行 Overpass API 查询并在交互地图上展示结果。citeturn4search0turn4search1
- Overpass API 是一个 **只读 API**，可以按查询条件返回 OSM 地图数据的子集。citeturn4search3

### 推荐获取流程（概览）

1. 打开 Overpass Turbo：`https://overpass-turbo.eu/`
2. 缩放/框选五山校区范围
3. 编写 Overpass QL 查询（示例，按需求调整标签）：

```ql
[out:json][timeout:25];
(
  way["highway"]({{bbox}});
  way["building"]({{bbox}});
);
out body;
>;
out skel qt;
```

4. `Export` 导出为 GeoJSON / OSM XML / JSON
5. 将导出的要素 **整理/转换** 为本项目 `ScanMap()` 所需的输入格式（建议写一个 `tools/convert.py` 固化流程）

---

## 算法与实现要点

- **最短路预处理**
  - 对每个路口 `s` 运行一次 SPFA，得到 `car[s][*]` 与 `walk[s][*]`
- **建筑名模糊匹配**
  - 使用 LCS（最长公共子序列）相似度，从 `blg[]` 中选择“最接近”的建筑名
- **起终点路口选择**
  - 一个建筑可能关联多个路口（`crossing`）
  - 程序在起点候选路口集合 × 终点候选路口集合中，选择距离最短的一对路口作为最终路径端点

---

## 注意事项（重要）

### 1）退出选项里包含系统重启命令（建议立即移除）

当前代码在菜单选择 `0` 并输入 `N` 时，会执行：

```cpp
system("shutdown -r -t 0");
```

这会在 Windows 上 **立刻重启电脑**。建议你在公开仓库中直接删除或注释掉这段逻辑，以免误触造成影响。

---

### 2）路径回溯函数需要完善（否则可能不输出路径）

`findpath()` 当前没有“到达终点就返回”的终止条件（函数参数里也没有传入 `T`），实际使用时可能无法正确打印路径。

建议做法：增加 `T` 参数并在 `u == T` 时返回 `true`，同时记录前驱节点（`pre[]`）会更稳。

---

### 3）Walking/Driving 输出文案可能对调

`GetPath()` 内部打印：

- `flag == 0`（实际上是 car 权重）却打印 `Walking solution`
- `flag == 1`（实际上是 walk 权重）却打印 `Driving solution`

如需修正，请交换两段文案即可。

---

## 常见问题

### Q1：为什么输入建筑名找不到？

程序用 `cin >>` 读入建筑名，因此输入里不能有空格；并且模糊匹配基于 LCS，相似度较低时可能匹配到其他建筑。

---

## 致谢

- OpenStreetMap Contributors（地图数据）
- Overpass API / Overpass Turbo（数据查询与导出）citeturn4search0turn4search3
