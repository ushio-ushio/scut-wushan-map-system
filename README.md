# 校园导航数据说明（供 Map.cpp 直接读取）

本压缩包包含 1 份可直接重定向给 `Map.cpp` 的输入数据（`campus_map_input.txt`），以及若干 CSV 便于你检查/调试。

> 说明：你给的 `Map.cpp` 里数组上限 `N=1e3+10` 同时限制了「路口数量」和「边数量」都必须 ≤1000。
> 因此我在从 Overpass 导出的 GeoJSON 拟合时做了**路口聚类/合并**（约 16m 内视作同一处路口），并过滤掉明显不在校园建筑范围内的道路，
> 最终得到：  
> - 路口（Intersection）数量：360  
> - 有向边（Edge）数量：998  
> - 建筑（Building）数量：60  
> - 街道（Street）数量：44  

## 1. 你可以怎么用

假设你的可执行文件叫 `Map.exe`（或 Linux 下 `a.out`）：

```bash
./a.out < campus_map_input.txt
```

## 2. `campus_map_input.txt` 的数据格式（严格按 Map.cpp 的 `ScanMap()` 读取顺序）

下面按“段落”解释文件内容。**注意：文件里没有注释行**（因为 `cin >>` 读入时不会跳过注释）。

### (A) Street（街道表）

1) 第一行：`M`  
- `M` = 街道数量

2) 接下来 `M` 行：每行 1 个字符串  
- `edg[i].name`：街道名（已把空格替换为下划线，保证 `cin>>` 可读）

3) 再接下来 `M` 行：每行 2 个整数  
- `edg[i].length edg[i].flag`  
- `length`：该名称道路在 GeoJSON 中的总长度（米，四舍五入）  
- `flag`：是否可通行汽车（1=可，0=不可）；仅作为附加信息，你的代码目前未使用

### (B) Intersection（路口表）

4) 下一行：`InterNum`  
- 路口数量（已压到 ≤1000）

5) 接下来 `InterNum` 行：每行是 `0 0`  
- 你的代码里这里原本设计成“路口关联街道名/周边建筑名”等信息  
- 但 `Map.cpp` 这块在你给的版本里有变量/下标使用问题，且后续功能也没用到这些字段  
- 为避免读入错位，我全部填 0（表示该路口不附加任何字符串列表）

### (C) Edge（路网边）

6) 下一行：`EdgeNum`  
- 有向边数量（998）

7) 接下来 `EdgeNum` 行：每行 4 个整数  
- `u v walk_dis car_dis`  
- `u,v`：路口编号（1..InterNum）  
- `walk_dis`：步行代价（米）  
- `car_dis`：驾车代价（米）

#### “人车差异”的处理规则

- 如果该段路 **人能走、车不能走**（如 `highway=footway/path/steps/pedestrian` 等），则：
  - `walk_dis = 实际长度`
  - `car_dis = 极大值 INF`
- 如果该段路 **车能走、人不建议走/明确禁止**（如 `foot=no` 且没有其它允许步行的标记），则：
  - `car_dis = 实际长度`
  - `walk_dis = INF`
- `INF` 取值：`100000000`（1e8）
  - 这样在最短路计算里基本等价于“不可走”
  - 同时避免使用 `2e9` 造成 `int` 溢出风险
- 对于 `oneway=yes/-1` 的道路：
  - 顺行方向：`car_dis = 实际长度`
  - 逆行方向：`car_dis = INF`
  - **步行**默认仍允许双向（walk_dis 不受 oneway 影响）

### (D) Building（建筑表）

8) 下一行：`BuildingNum`  
- 建筑数量（60）

9) 接下来 `BuildingNum` 组，每组 3 行：

第 1 行：建筑名  
- `blg[i].name`（无空格）

第 2 行：`k  crossing1 crossing2 ...`  
- `k` = 该建筑可接入路网的“附近路口”数量（我取 1~3 个）  
- `crossing*` = 路口编号（1..InterNum）

> 生成方式：取建筑多边形的几何中心点，选择 120m 内最近的若干个“可步行连通”的路口；若 120m 内没有，则取全局最近的 1 个。

第 3 行：`d  token1 token2 ...`  
- `d` = 描述 token 数量  
- `token*` = 不带空格的字符串（比如 `type=university`、`osm=way/xxxx`）

## 3. 附带的 CSV 文件说明

- `streets.csv`：街道表（便于查 M 段内容）
- `intersections.csv`：路口表（包含出入度、聚类合并规模等统计）
- `edges.csv`：边表（包含 walk/car 是否可走的布尔列）
- `buildings.csv`：建筑表（建筑编号、名称、关联路口列表、描述）

## 4. 重要提示

- 本数据已满足你代码里 `N=1e3+10` 的规模限制。  
- 如果你后续把 `Intersection` 的街道/建筑列表功能补全，也可以基于 CSV 再生成更丰富的路口附加信息。
