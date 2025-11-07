#include <algorithm>
#include <chrono>
#include <fstream>
#include <future>
#include <iomanip>
#include <iostream>
#include <vector>

using namespace std;

const int THRESHOLD = 10000;

// 三数取中法选择枢轴
int medianOfThree(vector<int> &arr, int left, int right) {
  int mid = left + (right - left) / 2;

  if (arr[left] > arr[mid])
    swap(arr[left], arr[mid]);
  if (arr[left] > arr[right])
    swap(arr[left], arr[right]);
  if (arr[mid] > arr[right])
    swap(arr[mid], arr[right]);

  return mid;
}

// 分区函数
int partition(vector<int> &arr, int left, int right) {
  int pivotIdx = medianOfThree(arr, left, right);
  int pivot = arr[pivotIdx];
  swap(arr[pivotIdx], arr[right]);

  int i = left - 1;
  for (int j = left; j < right; j++) {
    if (arr[j] <= pivot) {
      i++;
      swap(arr[i], arr[j]);
    }
  }
  swap(arr[i + 1], arr[right]);
  return i + 1;
}

// 单线程快速排序
void quicksortSingle(vector<int> &arr, int left, int right) {
  if (left < right) {
    int pi = partition(arr, left, right);
    quicksortSingle(arr, left, pi - 1);
    quicksortSingle(arr, pi + 1, right);
  }
}

// 多线程快速排序
void quicksortParallel(vector<int> &arr, int left, int right, int depth = 0) {
  if (left >= right)
    return;

  if (right - left < THRESHOLD || depth > 10) {
    quicksortSingle(arr, left, right);
    return;
  }

  int pi = partition(arr, left, right);

  auto leftFuture = async(launch::async, [&]() {
    quicksortParallel(arr, left, pi - 1, depth + 1);
  });

  quicksortParallel(arr, pi + 1, right, depth + 1);

  leftFuture.wait();
}

// 读取数据
bool readData(const string &filename, vector<int> &data) {
  ifstream infile(filename);
  if (!infile.is_open()) {
    cerr << "无法打开文件: " << filename << endl;
    return false;
  }

  int n;
  infile >> n;
  data.resize(n);

  for (int i = 0; i < n; i++) {
    infile >> data[i];
  }

  infile.close();
  return true;
}

// 写入数据
bool writeData(const string &filename, const vector<int> &data) {
  ofstream outfile(filename);
  if (!outfile.is_open()) {
    cerr << "无法创建文件: " << filename << endl;
    return false;
  }

  for (size_t i = 0; i < data.size(); i++) {
    outfile << data[i];
    if (i < data.size() - 1) {
      outfile << " ";
    }
  }

  outfile.close();
  return true;
}

int main() {
  vector<int> data;

  // 读取数据
  if (!readData("data.txt", data)) {
    return 1;
  }

  // 多线程快速排序
  vector<int> parallelData = data;
  auto start = chrono::high_resolution_clock::now();
  quicksortParallel(parallelData, 0, parallelData.size() - 1);
  auto end = chrono::high_resolution_clock::now();
  auto parallelDuration =
      chrono::duration_cast<chrono::microseconds>(end - start).count() / 1000.0;

  // 写入结果
  if (!writeData("sorted.txt", parallelData)) {
    return 1;
  }

  // STL sort 对比
  vector<int> stlData = data;
  start = chrono::high_resolution_clock::now();
  sort(stlData.begin(), stlData.end());
  end = chrono::high_resolution_clock::now();
  auto stlDuration =
      chrono::duration_cast<chrono::microseconds>(end - start).count() / 1000.0;

  // 输出性能对比
  cout << "========== 性能对比 ==========" << endl;
  cout << "STL sort 运行时间: " << fixed << setprecision(2) << stlDuration
       << " 毫秒" << endl;
  cout << "多线程快速排序运行时间: " << fixed << setprecision(2)
       << parallelDuration << " 毫秒" << endl;

  double speedup = stlDuration / parallelDuration;
  if (parallelDuration < stlDuration) {
    cout << "加速比: " << fixed << setprecision(2) << speedup << "x (快 "
         << fixed << setprecision(2) << ((speedup - 1) * 100) << "%)" << endl;
  } else {
    cout << "相对STL sort: " << fixed << setprecision(2)
         << (parallelDuration / stlDuration) << "x (慢 " << fixed
         << setprecision(2) << ((parallelDuration / stlDuration - 1) * 100)
         << "%)" << endl;
  }

  return 0;
}