#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>
#include <algorithm>
#include <iomanip>
using namespace std;

// ==================== 1. 基础快速排序 ====================
int partition_basic(vector<int>& arr, int low, int high) {
    int pivot = arr[high];  // 固定选择最后一个元素作为基准
    int i = low - 1;
    
    for (int j = low; j < high; j++) {
        if (arr[j] <= pivot) {
            i++;
            swap(arr[i], arr[j]);
        }
    }
    swap(arr[i + 1], arr[high]);
    return i + 1;
}

void quickSort_basic(vector<int>& arr, int low, int high) {
    if (low < high) {
        int pi = partition_basic(arr, low, high);
        quickSort_basic(arr, low, pi - 1);
        quickSort_basic(arr, pi + 1, high);
    }
}

// ==================== 2. 随机基准优化 ====================
int partition_random(vector<int>& arr, int low, int high) {
    // 随机选择基准并交换到末尾
    int randomIndex = low + rand() % (high - low + 1);
    swap(arr[randomIndex], arr[high]);
    return partition_basic(arr, low, high);
}

void quickSort_random(vector<int>& arr, int low, int high) {
    if (low < high) {
        int pi = partition_random(arr, low, high);
        quickSort_random(arr, low, pi - 1);
        quickSort_random(arr, pi + 1, high);
    }
}

// ==================== 3. 三数取中优化 ====================
int medianOfThree(vector<int>& arr, int low, int high) {
    int mid = low + (high - low) / 2;
    
    // 对首、中、尾三个元素排序
    if (arr[mid] < arr[low]) swap(arr[low], arr[mid]);
    if (arr[high] < arr[low]) swap(arr[low], arr[high]);
    if (arr[high] < arr[mid]) swap(arr[mid], arr[high]);
    
    // 将中位数放到high-1位置
    swap(arr[mid], arr[high]);
    return high;
}

int partition_median(vector<int>& arr, int low, int high) {
    if (high - low >= 2) {
        medianOfThree(arr, low, high);
    }
    return partition_basic(arr, low, high);
}

void quickSort_median(vector<int>& arr, int low, int high) {
    if (low < high) {
        int pi = partition_median(arr, low, high);
        quickSort_median(arr, low, pi - 1);
        quickSort_median(arr, pi + 1, high);
    }
}

// ==================== 4. 插入排序（辅助函数） ====================
void insertionSort(vector<int>& arr, int low, int high) {
    for (int i = low + 1; i <= high; i++) {
        int key = arr[i];
        int j = i - 1;
        // 将key插入到已排序序列的正确位置
        while (j >= low && arr[j] > key) {
            arr[j + 1] = arr[j];
            j--;
        }
        arr[j + 1] = key;
    }
}

// ==================== 5. 混合插入排序优化 ====================
const int CUTOFF = 10;  // 阈值k，可调整测试

void quickSort_hybrid(vector<int>& arr, int low, int high) {
    // 当子数组长度小于CUTOFF时，不做任何排序就返回
    if (high - low + 1 < CUTOFF) {
        return;
    }
    
    if (low < high) {
        int pi = partition_median(arr, low, high);
        quickSort_hybrid(arr, low, pi - 1);
        quickSort_hybrid(arr, pi + 1, high);
    }
}

void hybridSort(vector<int>& arr) {
    if (arr.empty()) return;
    // 先用快排处理大块
    quickSort_hybrid(arr, 0, arr.size() - 1);
    // 最后对整个数组执行一次插入排序
    insertionSort(arr, 0, arr.size() - 1);
}

// ==================== 6. 三路划分优化（聚集相等元素） ====================
void quickSort_3way(vector<int>& arr, int low, int high) {
    if (low >= high) return;
    
    // 三数取中选择基准
    if (high - low >= 2) {
        int mid = low + (high - low) / 2;
        if (arr[mid] < arr[low]) swap(arr[low], arr[mid]);
        if (arr[high] < arr[low]) swap(arr[low], arr[high]);
        if (arr[high] < arr[mid]) swap(arr[mid], arr[high]);
        swap(arr[mid], arr[low]);  // 将中位数放到开头
    }
    
    int pivot = arr[low];
    int lt = low;      // arr[low+1..lt] < pivot
    int gt = high;     // arr[gt..high] > pivot
    int i = low + 1;   // arr[lt+1..i-1] == pivot
    
    // 三路划分：< pivot, == pivot, > pivot
    while (i <= gt) {
        if (arr[i] < pivot) {
            swap(arr[lt], arr[i]);
            lt++;
            i++;
        } else if (arr[i] > pivot) {
            swap(arr[i], arr[gt]);
            gt--;
        } else {
            i++;  // arr[i] == pivot，跳过
        }
    }
    
    // 递归排序左右两部分，相等元素不再参与
    quickSort_3way(arr, low, lt - 1);
    quickSort_3way(arr, gt + 1, high);
}

// ==================== 7. 完全优化版本 ====================
void quickSort_optimized(vector<int>& arr, int low, int high) {
    // 小数组优化：延迟到最后用插入排序
    if (high - low + 1 < CUTOFF) {
        return;
    }
    
    if (low >= high) return;
    
    // 三数取中
    if (high - low >= 2) {
        int mid = low + (high - low) / 2;
        if (arr[mid] < arr[low]) swap(arr[low], arr[mid]);
        if (arr[high] < arr[low]) swap(arr[low], arr[high]);
        if (arr[high] < arr[mid]) swap(arr[mid], arr[high]);
        swap(arr[mid], arr[low]);
    }
    
    int pivot = arr[low];
    int lt = low;
    int gt = high;
    int i = low + 1;
    
    // 三路划分
    while (i <= gt) {
        if (arr[i] < pivot) {
            swap(arr[lt], arr[i]);
            lt++;
            i++;
        } else if (arr[i] > pivot) {
            swap(arr[i], arr[gt]);
            gt--;
        } else {
            i++;
        }
    }
    
    quickSort_optimized(arr, low, lt - 1);
    quickSort_optimized(arr, gt + 1, high);
}

void fullyOptimizedSort(vector<int>& arr) {
    if (arr.empty()) return;
    quickSort_optimized(arr, 0, arr.size() - 1);
    insertionSort(arr, 0, arr.size() - 1);
}

// ==================== 文件读写 ====================
vector<int> readData(const string& filename) {
    ifstream infile(filename);
    if (!infile) {
        cerr << "无法打开文件: " << filename << endl;
        exit(1);
    }
    
    int n;
    infile >> n;
    cout << "读取数据规模: " << n << endl;
    
    vector<int> arr(n);
    for (int i = 0; i < n; i++) {
        infile >> arr[i];
    }
    infile.close();
    
    return arr;
}

void writeData(const string& filename, const vector<int>& arr) {
    ofstream outfile(filename);
    for (size_t i = 0; i < arr.size(); i++) {
        outfile << arr[i];
        if (i < arr.size() - 1) outfile << " ";
    }
    outfile.close();
}

// 验证排序是否正确
bool verifySorted(const vector<int>& arr) {
    for (size_t i = 1; i < arr.size(); i++) {
        if (arr[i] < arr[i-1]) return false;
    }
    return true;
}

// 格式化时间输出函数（精确到微秒，自动选择单位）
string formatTime(double microseconds) {
    if (microseconds < 1000) {
        return to_string((int)microseconds) + " 微秒";
    } else if (microseconds < 1000000) {
        return to_string(microseconds / 1000.0) + " 毫秒";
    } else {
        return to_string(microseconds / 1000000.0) + " 秒";
    }
}

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  快速排序算法及其优化实验" << endl;
    cout << "  精确计时（微秒级）" << endl;
    cout << "========================================" << endl << endl;
    
    // 读取数据
    vector<int> original = readData("data.txt");
    cout << "数据读取成功！" << endl << endl;
    
    cout << "请选择排序算法：" << endl;
    cout << "1. 基础快速排序（固定基准）" << endl;
    cout << "2. 随机基准优化" << endl;
    cout << "3. 三数取中优化" << endl;
    cout << "4. 混合插入排序优化（阈值=" << CUTOFF << "）" << endl;
    cout << "5. 三路划分优化（聚集相等元素）" << endl;
    cout << "6. 完全优化版本（推荐）" << endl;
    cout << "0. 运行所有算法并对比" << endl;
    cout << endl << "请输入选项: ";
    
    int choice;
    cin >> choice;
    cout << endl;
    
    if (choice == 0) {
        // 运行所有算法进行对比
        cout << "========================================" << endl;
        cout << "  性能对比测试（精确到微秒）" << endl;
        cout << "========================================" << endl << endl;
        
        vector<pair<string, double>> results;
        
        // 1. 基础快排
        {
            vector<int> arr = original;
            auto start = chrono::high_resolution_clock::now();
            quickSort_basic(arr, 0, arr.size() - 1);
            auto end = chrono::high_resolution_clock::now();
            auto duration = chrono::duration_cast<chrono::microseconds>(end - start);
            double us = duration.count();
            results.push_back({"基础快速排序", us});
            cout << "✓ 基础快速排序: " << fixed << setprecision(2) 
                 << us / 1000.0 << " 毫秒 (" << (int)us << " 微秒)"
                 << (verifySorted(arr) ? " [正确]" : " [错误]") << endl;
        }
        
        // 2. 随机基准
        {
            vector<int> arr = original;
            srand(time(0));
            auto start = chrono::high_resolution_clock::now();
            quickSort_random(arr, 0, arr.size() - 1);
            auto end = chrono::high_resolution_clock::now();
            auto duration = chrono::duration_cast<chrono::microseconds>(end - start);
            double us = duration.count();
            results.push_back({"随机基准优化", us});
            cout << "✓ 随机基准优化: " << fixed << setprecision(2) 
                 << us / 1000.0 << " 毫秒 (" << (int)us << " 微秒)"
                 << (verifySorted(arr) ? " [正确]" : " [错误]") << endl;
        }
        
        // 3. 三数取中
        {
            vector<int> arr = original;
            auto start = chrono::high_resolution_clock::now();
            quickSort_median(arr, 0, arr.size() - 1);
            auto end = chrono::high_resolution_clock::now();
            auto duration = chrono::duration_cast<chrono::microseconds>(end - start);
            double us = duration.count();
            results.push_back({"三数取中优化", us});
            cout << "✓ 三数取中优化: " << fixed << setprecision(2) 
                 << us / 1000.0 << " 毫秒 (" << (int)us << " 微秒)"
                 << (verifySorted(arr) ? " [正确]" : " [错误]") << endl;
        }
        
        // 4. 混合插入
        {
            vector<int> arr = original;
            auto start = chrono::high_resolution_clock::now();
            hybridSort(arr);
            auto end = chrono::high_resolution_clock::now();
            auto duration = chrono::duration_cast<chrono::microseconds>(end - start);
            double us = duration.count();
            results.push_back({"混合插入排序", us});
            cout << "✓ 混合插入排序: " << fixed << setprecision(2) 
                 << us / 1000.0 << " 毫秒 (" << (int)us << " 微秒)"
                 << (verifySorted(arr) ? " [正确]" : " [错误]") << endl;
        }
        
        // 5. 三路划分
        {
            vector<int> arr = original;
            auto start = chrono::high_resolution_clock::now();
            quickSort_3way(arr, 0, arr.size() - 1);
            auto end = chrono::high_resolution_clock::now();
            auto duration = chrono::duration_cast<chrono::microseconds>(end - start);
            double us = duration.count();
            results.push_back({"三路划分优化", us});
            cout << "✓ 三路划分优化: " << fixed << setprecision(2) 
                 << us / 1000.0 << " 毫秒 (" << (int)us << " 微秒)"
                 << (verifySorted(arr) ? " [正确]" : " [错误]") << endl;
        }
        
        // 6. 完全优化
        {
            vector<int> arr = original;
            auto start = chrono::high_resolution_clock::now();
            fullyOptimizedSort(arr);
            auto end = chrono::high_resolution_clock::now();
            auto duration = chrono::duration_cast<chrono::microseconds>(end - start);
            double us = duration.count();
            results.push_back({"完全优化版本", us});
            cout << "✓ 完全优化版本: " << fixed << setprecision(2) 
                 << us / 1000.0 << " 毫秒 (" << (int)us << " 微秒)"
                 << (verifySorted(arr) ? " [正确]" : " [错误]") << endl;
            
            // 使用最优算法的结果写入文件
            writeData("sorted.txt", arr);
        }
        
        // 性能分析
        cout << endl << "========================================" << endl;
        cout << "  性能分析" << endl;
        cout << "========================================" << endl;
        
        // 找出最快和最慢的算法
        auto fastest = min_element(results.begin(), results.end(),
            [](const pair<string,double>& a, const pair<string,double>& b) {
                return a.second < b.second;
            });
        auto slowest = max_element(results.begin(), results.end(),
            [](const pair<string,double>& a, const pair<string,double>& b) {
                return a.second < b.second;
            });
        
        cout << "🏆 最快算法: " << fastest->first 
             << " (" << fixed << setprecision(2) << fastest->second / 1000.0 << " 毫秒)" << endl;
        cout << "🐌 最慢算法: " << slowest->first 
             << " (" << fixed << setprecision(2) << slowest->second / 1000.0 << " 毫秒)" << endl;
        cout << "⚡ 性能提升: " << fixed << setprecision(2) 
             << (slowest->second / fastest->second) << "x" << endl;
        
        cout << endl << "详细性能排名：" << endl;
        sort(results.begin(), results.end(),
            [](const pair<string,double>& a, const pair<string,double>& b) {
                return a.second < b.second;
            });
        
        for (size_t i = 0; i < results.size(); i++) {
            cout << (i+1) << ". " << results[i].first 
                 << ": " << fixed << setprecision(2) << results[i].second / 1000.0 << " 毫秒"
                 << " (" << (int)results[i].second << " 微秒)" << endl;
        }
        cout << "========================================" << endl;
        
    } else {
        // 运行单个算法
        vector<int> arr = original;
        string algorithmName;
        
        auto start = chrono::high_resolution_clock::now();
        
        switch (choice) {
            case 1:
                quickSort_basic(arr, 0, arr.size() - 1);
                algorithmName = "基础快速排序（固定基准）";
                break;
            case 2:
                srand(time(0));
                quickSort_random(arr, 0, arr.size() - 1);
                algorithmName = "随机基准优化";
                break;
            case 3:
                quickSort_median(arr, 0, arr.size() - 1);
                algorithmName = "三数取中优化";
                break;
            case 4:
                hybridSort(arr);
                algorithmName = "混合插入排序优化";
                break;
            case 5:
                quickSort_3way(arr, 0, arr.size() - 1);
                algorithmName = "三路划分优化";
                break;
            case 6:
                fullyOptimizedSort(arr);
                algorithmName = "完全优化版本";
                break;
            default:
                cout << "无效选项！使用默认算法" << endl;
                fullyOptimizedSort(arr);
                algorithmName = "完全优化版本（默认）";
        }
        
        auto end = chrono::high_resolution_clock::now();
        auto duration_us = chrono::duration_cast<chrono::microseconds>(end - start);
        auto duration_ms = chrono::duration_cast<chrono::milliseconds>(end - start);
        
        // 写入结果
        writeData("sorted.txt", arr);
        
        // 验证排序结果
        bool correct = verifySorted(arr);
        
        cout << "========================================" << endl;
        cout << "算法名称: " << algorithmName << endl;
        cout << "排序结果: " << (correct ? "✓ 正确" : "✗ 错误") << endl;
        cout << "运行时间: " << duration_ms.count() << " 毫秒" << endl;
        cout << "精确时间: " << duration_us.count() << " 微秒" << endl;
        cout << "高精度: " << fixed << setprecision(3) 
             << duration_us.count() / 1000.0 << " 毫秒" << endl;
        cout << "========================================" << endl;
        cout << "结果已保存到 sorted.txt" << endl;
    }
    
    return 0;
}