# SCUT 五山校区地图导航（数据结构课程设计）

本项目为小组数据结构课程设计作业：针对 **华南理工大学五山校区** 的地图数据，提供 **建筑信息查询** 与 **建筑间最短路径规划**（步行/驾车）等功能。

---

## 功能概览

- **建筑信息查询**
  - 输入建筑名称，支持四级匹配策略：精确匹配 → 名称子串 → 描述关键词 → LCS 模糊匹配
  - 输出该建筑的描述信息
- **路径规划**
  - 输入起点建筑、终点建筑
  - 分别计算 **驾车最短路径**（`car_dis` 权重）和 **步行最短路径**（`walk_dis` 权重）
  - 输出经过的路口序列及总距离
- **其他特性**
  - Windows 控制台 UTF-8 支持（中文正常显示）
  - 自动加载同目录下的 `map.txt`（也可通过命令行参数指定）
  - 最短路预计算时显示进度

---

## 项目结构

```
main.cpp              -- 源代码（唯一）
map.txt               -- 运行时地图数据（程序自动读取）
data/                 -- 参考数据（OSM 导出，不参与运行）
  buildings.csv         -- 建筑数据
  edges.csv             -- 边数据
  intersections.csv     -- 路口数据
  streets.csv           -- 道路数据
.vscode/              -- VS Code 配置
```

---

## 编译与运行

```bash
# 使用 MSYS2 UCRT64 终端或 VS Code (Ctrl+Shift+B)
g++ -std=c++17 -o main.exe main.cpp

# 运行（自动加载同目录的 map.txt）
./main.exe

# 或指定地图文件
./main.exe custom_map.txt
```

程序启动后显示菜单：

- 输入 `1`：查询建筑信息
- 输入 `2`：规划路径
- 输入 `0`：退出

---

## 输入数据格式

程序通过 `ScanMap()` 读入地图数据（所有字符串用 `cin >>` 读取，**不能包含空格**）。

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

- `M`：道路数量
- `name_i`：道路名
- `length_i`：道路长度
- `flag_i`：道路标记

### 2）路口（Intersection）

```
K
x1  b11 b12 ... b1x1
y1  s11 s12 ... s1y1
x2  ...
y2  ...
```

- `K`：路口数量
- 每个路口先读 `x` 个建筑名，再读 `y` 个道路名

### 3）路口之间的有向边

```
E
u1 v1 walk1 car1
u2 v2 walk2 car2
...
```

- `E`：边数
- 每条有向边 `u → v` 带步行权重和驾车权重

### 4）建筑（Building）

```
B
building_name_1
t  c1 c2 ... ct
d  desc1 desc2 ... descd
building_name_2
...
```

- `B`：建筑数量
- 每个建筑包含名称、关联路口编号列表、描述字符串列表

---

## 算法与实现要点

- **最短路预处理**：对每个路口运行 SPFA，得到 `car_dist[s][*]` 和 `walk_dist[s][*]`
- **建筑名匹配**（四级策略）：
  1. 精确匹配：输入名与建筑名完全一致
  2. 名称子串：输入名是建筑名的子串
  3. 描述匹配：输入名出现在建筑描述中
  4. LCS 模糊匹配：取最长公共子序列相似度最高的建筑
- **起终点选择**：一个建筑可关联多个路口，程序在候选路口组合中选取距离最短的一对

---

## 数据来源与获取

地图数据来源于 **OpenStreetMap**，使用 **Overpass Turbo** 查询与导出：

1. 打开 [Overpass Turbo](https://overpass-turbo.eu/)
2. 缩放/框选五山校区范围
3. 编写 Overpass QL 查询并导出
4. 将导出数据转换为本项目所需的输入格式

---

## 致谢

- OpenStreetMap Contributors（地图数据）
- Overpass API / Overpass Turbo（数据查询与导出）
