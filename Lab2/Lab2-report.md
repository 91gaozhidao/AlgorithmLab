### **最近点对问题求解实验报告**

**姓名：** 高宇轩

**学号： **SA25218104

**仓库地址**：[AlgorithmLab/Lab2 at main · 91gaozhidao/AlgorithmLab](https://github.com/91gaozhidao/AlgorithmLab/tree/main/Lab2)

------

#### **1. 实验内容**

本次实验旨在解决最近点对问题。具体任务是，在一个包含大量二维平面上的点的集合中，找到距离最近的两个点。

为完成此任务，我们采用两种不同的算法进行实现和比较：

1. **朴素暴力算法 (Naive Brute-Force Algorithm)**：通过遍历所有可能的点对来计算它们之间的距离，从而找到最小值。
2. **分治算法 (Divide-and-Conquer Algorithm)**：一种更高效的算法，通过将问题分解为更小的子问题来递归求解。

------

#### **2. 算法设计思路**

##### **2.1 朴素暴力算法**

这是解决该问题最直观的方法。其思路非常简单：

1. **遍历**：使用两层嵌套循环，遍历集合中所有独一无二的点对 `(p_i, p_j)`。
2. **计算距离**：对于每一个点对，计算它们之间的欧几里得距离。
3. **寻找最小距离**：维护一个变量来记录当前找到的最小距离。如果在遍历过程中发现一个更小的距离，就更新这个最小值以及对应的点对。
4. **返回结果**：遍历完成后，记录的即是全局最近点对及其距离。

该算法的时间复杂度为 **O(n²)**，因为需要对 n 个点进行两两比较。对于大规模数据集，这种方法效率极低。

##### **2.2 分治算法**

分治算法是解决此问题的高效方法，其核心思想是“分而治之”。

1. **预排序 (Preprocessing)**：
   - 为了提高效率，首先将所有点分别按 `x` 坐标和 `y` 坐标进行排序，得到两个数组 `points_by_x` 和 `points_by_y`。这一步的时间复杂度为 **O(n log n)**。
2. **分解 (Divide)**：
   - 根据按 `x` 坐标排序的数组 `points_by_x`，找到中点 `mid_point`，并以此为界将点集垂直划分为左右两个大小大致相等的子集：`Left` 和 `Right`。
3. **解决 (Conquer)**：
   - 递归地对 `Left` 和 `Right` 两个子集调用分治算法，分别找到它们内部的最近点对，其距离为 `d_L` 和 `d_R`。
   - 取 `delta = min(d_L, d_R)` 作为当前已知的最小距离。
4. **合并 (Combine)**：
   - 最近点对可能存在于 `Left` 内部，或 `Right` 内部，也可能一个点在 `Left`，另一个点在 `Right`（我们称之为“跨越”中线的点对）。
   - 对于跨越中线的点对，它们必然位于以中线为中心、宽度为 `2 * delta` 的垂直带状区域（`strip`）内。因为如果两点距离小于 `delta`，它们的 `x` 坐标之差也必然小于 `delta`。
   - 我们从按 `y` 坐标排序的数组 `points_by_y` 中筛选出所有位于此带状区域的点，并存入新数组 `strip_points`。
   - 遍历 `strip_points` 中的每个点 `p`。对于每个 `p`，我们只需检查其后最多 7 个点（这是一个已知的理论界限）。因为这些点按 `y` 坐标排序，如果某个点 `q` 与 `p` 的 `y` 坐标之差大于 `delta`，那么它们之间的距离也必然大于 `delta`，无需再比较。
   - 如果在带状区域内找到了比 `delta` 更小的距离，则更新最小距离。
5. **基准情况 (Base Case)**：
   - 当递归的子问题规模足够小（例如，点数小于等于 3）时，直接使用暴力算法求解，并返回结果。

------

#### **3. 源码 + 注释**

```c++
#include <algorithm>
#include <chrono>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <vector>

using namespace std;
using namespace chrono;

// 表示一个带ID的二维空间点。
struct Point {
  int id;
  double x, y;
};

// 表示一对点以及它们之间的距离。
struct PointPair {
  Point p1, p2;
  double distance = numeric_limits<double>::max();
};

// 计算两点之间的欧几里得距离。
double calculateDistance(const Point &p1, const Point &p2) {
  return sqrt(pow(p1.x - p2.x, 2) + pow(p1.y - p2.y, 2));
}

// 暴力算法：遍历所有点对，找到距离最小的一对。
// 时间复杂度: O(n^2)
PointPair bruteForceClosestPair(const vector<Point> &points) {
  PointPair best_pair;
  for (size_t i = 0; i < points.size(); ++i) {
    for (size_t j = i + 1; j < points.size(); ++j) {
      double dist = calculateDistance(points[i], points[j]);
      if (dist < best_pair.distance) {
        best_pair.distance = dist;
        best_pair.p1 = points[i];
        best_pair.p2 = points[j];
      }
    }
  }
  return best_pair;
}

// 暴力算法的入口函数，用于对比测试。
PointPair naiveClosestPair(const vector<Point> &points) {
  return bruteForceClosestPair(points);
}

// 分治算法的主递归函数。
// 时间复杂度: O(n log n)
PointPair closestPairRecursive(const vector<Point> &points_by_x,
                               const vector<Point> &points_by_y) {
  // 基准情况：如果点的数量很少(<=3)，直接使用暴力法。
  if (points_by_x.size() <= 3) {
    return bruteForceClosestPair(points_by_x);
  }

  // 1. 分解(Divide): 将点集按x坐标分为左右两半。
  size_t mid_index = points_by_x.size() / 2;
  Point mid_point = points_by_x[mid_index];

  vector<Point> left_half_x(points_by_x.begin(),
                            points_by_x.begin() + mid_index);
  vector<Point> right_half_x(points_by_x.begin() + mid_index,
                             points_by_x.end());

  vector<Point> left_half_y, right_half_y;
  // 通过遍历y排序数组来高效地(O(n))构建左右两半的y排序数组。
  for (const auto &p : points_by_y) {
    if (p.x < mid_point.x || (p.x == mid_point.x && p.y < mid_point.y)) {
      left_half_y.push_back(p);
    } else {
      right_half_y.push_back(p);
    }
  }

  // 2. 解决(Conquer): 递归地在每一半中找到最近点对。
  PointPair left_pair = closestPairRecursive(left_half_x, left_half_y);
  PointPair right_pair = closestPairRecursive(right_half_x, right_half_y);

  PointPair best_pair =
      (left_pair.distance < right_pair.distance) ? left_pair : right_pair;
  double delta = best_pair.distance;

  // 3. 合并(Combine): 寻找跨越中线的更近点对。
  //    只考虑距离中线小于delta的带状区域内的点。
  vector<Point> strip_points;
  for (const auto &p : points_by_y) {
    if (abs(p.x - mid_point.x) < delta) {
      strip_points.push_back(p);
    }
  }

  // 遍历带状区域内的点，对每个点，只需检查其后有限个点。
  for (size_t i = 0; i < strip_points.size(); ++i) {
    for (size_t j = i + 1; j < strip_points.size() &&
                           (strip_points[j].y - strip_points[i].y) < delta;
         ++j) {
      double dist = calculateDistance(strip_points[i], strip_points[j]);
      if (dist < best_pair.distance) {
        best_pair.distance = dist;
        best_pair.p1 = strip_points[i];
        best_pair.p2 = strip_points[j];
        delta = dist; // 发现更近的距离，更新delta以缩小搜索范围。
      }
    }
  }

  return best_pair;
}

int main() {
  locale::global(locale("")); // 支持中文输出

  // 从文件读取数据
  ifstream data_file("data.txt");
  if (!data_file) {
    cerr << "错误：无法打开 data.txt" << endl;
    return 1;
  }

  vector<Point> points;
  Point temp_point;
  while (data_file >> temp_point.id >> temp_point.x >> temp_point.y) {
    points.push_back(temp_point);
  }
  data_file.close();

  if (points.size() < 2) {
    cerr << "错误：没有足够的点来寻找点对。" << endl;
    return 1;
  }

  // --- 分治算法测试 ---
  auto start_dc = high_resolution_clock::now();

  vector<Point> points_by_x = points;
  vector<Point> points_by_y = points;

  // 预排序
  sort(points_by_x.begin(), points_by_x.end(),
       [](const Point &a, const Point &b) { return a.x < b.x; });
  sort(points_by_y.begin(), points_by_y.end(),
       [](const Point &a, const Point &b) { return a.y < b.y; });

  PointPair closest_pair = closestPairRecursive(points_by_x, points_by_y);

  auto end_dc = high_resolution_clock::now();
  duration<double, milli> duration_dc = end_dc - start_dc;

  // --- 朴素暴力算法测试 ---
  auto start_naive = high_resolution_clock::now();
  PointPair naive_pair = naiveClosestPair(points);
  auto end_naive = high_resolution_clock::now();
  duration<double, milli> duration_naive = end_naive - start_naive;

  // --- 输出结果 ---
  cout << "--- 分治算法结果 ---" << endl;
  cout << "最近点对: " << closest_pair.p1.id << " 和 " << closest_pair.p2.id
       << endl;
  cout << "距离: " << fixed << setprecision(6) << closest_pair.distance << endl;
  cout << "耗时: " << duration_dc.count() << " 毫秒" << endl;

  cout << "\n--- 朴素暴力算法结果 ---" << endl;
  cout << "最近点对: " << naive_pair.p1.id << " 和 " << naive_pair.p2.id
       << endl;
  cout << "距离: " << fixed << setprecision(6) << naive_pair.distance << endl;
  cout << "耗时: " << duration_naive.count() << " 毫秒" << endl;

  return 0;
}
```



------

#### **4. 算法测试结果与分析**

##### **测试结果**

根据提供的运行输出：

- **分治算法结果:**
  - **最近点对:** 7119 和 5826
  - **距离:** 2.807526
  - **耗时:** 20.376223 毫秒
- **朴素暴力算法结果:**
  - **最近点对:** 5826 和 7119
  - **距离:** 2.807526
  - **耗时:** 2052.208841 毫秒

##### **结果分析**

1. **正确性分析**： 从结果可以看出，两种算法找到了相同的最近点对，并且计算出的最小距离完全一致。

2. **性能分析**：

   - **分治算法**耗时约为 **20.38 毫秒**。
   - **朴素暴力算法**耗时约为 **2052.21 毫秒**。

   通过计算可以得出，朴素暴力算法的运行时间大约是分治算法的 **100倍** 。

   这个巨大的性能差异完全符合我们的理论预期。假设 `data.txt` 中包含 `n` 个点，两种算法的时间复杂度分别为：

   - 分治算法: **O(n log n)**
   - 朴素算法: **O(n²)**

   当 `n` 的值很大时，`n²` 的增长速度远快于 `n log n`。这导致了朴素算法的运行时间随着数据规模的增大而急剧增加，而分治算法则能保持在一个相对可接受的范围内。

##### **结论**

本次实验成功地实现了朴素暴力算法和分治算法来解决最近点对问题。测试结果表明，两种算法都能得到正确的结果，但在性能上存在天壤之别。分治算法凭借其 O(n log n) 的时间复杂度，在处理大规模数据集时展现出巨大的优势，其效率远超 O(n²) 的暴力算法。