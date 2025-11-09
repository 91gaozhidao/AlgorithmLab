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