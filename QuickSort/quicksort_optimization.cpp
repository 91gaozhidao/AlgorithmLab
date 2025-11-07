#include <algorithm>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <random>
#include <vector>

using namespace std;

// ==================== 插入排序 ====================
void insertionSort(vector<int> &arr, int left, int right) {
  for (int i = left + 1; i <= right; i++) {
    int key = arr[i];
    int j = i - 1;
    while (j >= left && arr[j] > key) {
      arr[j + 1] = arr[j];
      j--;
    }
    arr[j + 1] = key;
  }
}

// ==================== 第一部分：基准选择策略 ====================

// 1. 固定基准（选择最后一个元素）
int partitionFixed(vector<int> &arr, int left, int right) {
  int pivot = arr[right];
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

void quicksortFixed(vector<int> &arr, int left, int right) {
  if (left < right) {
    int pi = partitionFixed(arr, left, right);
    quicksortFixed(arr, left, pi - 1);
    quicksortFixed(arr, pi + 1, right);
  }
}

// 2. 随机基准
random_device rd;
mt19937 gen(rd());

int partitionRandom(vector<int> &arr, int left, int right) {
  uniform_int_distribution<> dis(left, right);
  int randomIndex = dis(gen);
  swap(arr[randomIndex], arr[right]);
  return partitionFixed(arr, left, right);
}

void quicksortRandom(vector<int> &arr, int left, int right) {
  if (left < right) {
    int pi = partitionRandom(arr, left, right);
    quicksortRandom(arr, left, pi - 1);
    quicksortRandom(arr, pi + 1, right);
  }
}

// 3. 三数取中
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

int partitionMedian(vector<int> &arr, int left, int right) {
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

void quicksortMedian(vector<int> &arr, int left, int right) {
  if (left < right) {
    int pi = partitionMedian(arr, left, right);
    quicksortMedian(arr, left, pi - 1);
    quicksortMedian(arr, pi + 1, right);
  }
}

// ==================== 第二部分：混合优化（带参数K） ====================

void quicksortHybrid(vector<int> &arr, int left, int right, int k) {
  if (left < right) {
    // 当子数组长度小于k时，直接使用插入排序
    if (right - left < k) {
      insertionSort(arr, left, right);
      return;
    }

    int pi = partitionMedian(arr, left, right);
    quicksortHybrid(arr, left, pi - 1, k);
    quicksortHybrid(arr, pi + 1, right, k);
  }
}

// ==================== 辅助函数 ====================

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

double measureTime(vector<int> data,
                   void (*sortFunc)(vector<int> &, int, int)) {
  auto start = chrono::high_resolution_clock::now();
  sortFunc(data, 0, data.size() - 1);
  auto end = chrono::high_resolution_clock::now();
  return chrono::duration_cast<chrono::microseconds>(end - start).count() /
         1000.0;
}

double measureTimeHybrid(vector<int> data, int k) {
  auto start = chrono::high_resolution_clock::now();
  quicksortHybrid(data, 0, data.size() - 1, k);
  auto end = chrono::high_resolution_clock::now();
  return chrono::duration_cast<chrono::microseconds>(end - start).count() /
         1000.0;
}

// ==================== 主程序 ====================

int main() {
  vector<int> data;

  // 读取数据
  if (!readData("data.txt", data)) {
    return 1;
  }

  // 第一部分：不同基准选择策略对比
  cout << "【第一部分：基准选择策略】" << endl;

  vector<int> data1 = data;
  double time1 = measureTime(data1, quicksortFixed);
  cout << "1) 固定基准: " << fixed << setprecision(2) << time1 << " 毫秒"
       << endl;

  vector<int> data2 = data;
  double time2 = measureTime(data2, quicksortRandom);
  cout << "2) 随机基准: " << fixed << setprecision(2) << time2 << " 毫秒"
       << endl;

  vector<int> data3 = data;
  double time3 = measureTime(data3, quicksortMedian);
  cout << "3) 三数取中: " << fixed << setprecision(2) << time3 << " 毫秒"
       << endl;

  cout << endl;

  // 第二部分：混合优化（测试不同的K值）
  cout << "【第二部分：三数取中 + 插入排序混合优化】" << endl;

  vector<int> k_values = {5, 10, 15, 20, 30};
  vector<pair<int, double>> results;

  for (int k : k_values) {
    vector<int> dataK = data;
    double timeK = measureTimeHybrid(dataK, k);
    results.push_back({k, timeK});
    cout << "K = " << setw(2) << k << ": " << fixed << setprecision(2) << timeK
         << " 毫秒" << endl;
  }

  cout << endl;

  // 性能对比
  cout << "========== 性能对比 ==========" << endl;

  // 找出最优的K值
  auto bestK =
      min_element(results.begin(), results.end(),
                  [](const pair<int, double> &a, const pair<int, double> &b) {
                    return a.second < b.second;
                  });

  cout << "基准选择最优: 三数取中 (" << fixed << setprecision(2) << time3
       << " 毫秒)" << endl;
  cout << "混合优化最优: K = " << bestK->first << " (" << fixed
       << setprecision(2) << bestK->second << " 毫秒)" << endl;
  cout << "优化提升: " << fixed << setprecision(2)
       << ((time3 - bestK->second) / time3 * 100) << "%" << endl;

  // 使用最优K值排序并保存结果
  vector<int> finalData = data;
  quicksortHybrid(finalData, 0, finalData.size() - 1, bestK->first);
  writeData("sorted.txt", finalData);

  return 0;
}