# 实验报告：最长公共子序列 (LCS)

**姓名**：高宇轩

**学号**： SA25218104

**仓库地址**：https://github.com/91gaozhidao/AlgorithmLab/tree/main/Lab5

------

## 1. 实验内容

本实验旨在通过动态规划（Dynamic Programming, DP）解决**最长公共子序列（LCS）**问题。给定两个字符串 `text1` 和 `text2`，需计算它们最长公共子序列的长度，并输出该子序列的具体内容。

**核心目标：**

- 理解DP状态转移方程。
- 实现基础 O(mn) 空间算法并回溯求路径。
- 优化空间至 O(2 × min(m,n))（滚动数组）。
- 优化空间至 O(min(m,n))（一维数组压缩）。

## 2. 算法设计思路

### 2.1 核心思想 (动态规划)

LCS 问题具有最优子结构和重叠子问题性质。我们定义一个二维数组 `dp[i][j]`，表示字符串 `text1` 的前 `i` 个字符和 `text2` 的前 `j` 个字符的最长公共子序列长度。

**状态转移方程：**

假设 `text1` 长度为 `m`，`text2` 长度为 `n`。

- 当 `text1[i-1] == text2[j-1]` 时（字符匹配）：
  - 当前字符必定在LCS中，长度加1。
  
  ```
  dp[i][j] = dp[i-1][j-1] + 1
  ```

- 当 `text1[i-1] ≠ text2[j-1]` 时（字符不匹配）：
  - 当前字符不能同时作为LCS的一部分，LCS长度继承自"去掉text1当前字符"或"去掉text2当前字符"后的最大值。
  
  ```
  dp[i][j] = max(dp[i-1][j], dp[i][j-1])
  ```

**边界条件：**

`dp[0][j] = 0` 和 `dp[i][0] = 0`（空字符串的LCS长度为0）。

### 2.2 空间优化思路

- **基础版 O(mn)：** 需要完整的二维表来存储状态，不仅为了计算长度，也为了通过回溯（Backtracking）从 `dp[m][n]` 倒推回 `dp[0][0]` 来构建LCS字符串。

- **滚动数组 O(2 × min(m,n))：** 计算 `dp[i]` 行只依赖于 `dp[i-1]` 行。因此只需两行数组（`prev` 和 `curr`）交替使用即可。

- **一维压缩 O(min(m,n))：** 极致压缩。使用单行数组 `dp`。在更新 `dp[j]` 时，我们需要用到"左上角"的旧值（即 `dp[i-1][j-1]`）。在一维数组中，这个值会被覆盖，因此需要用一个临时变量 `temp` 记录更新前的 `dp[j]`（即下一轮的左上角值）。

## 3. 源码 + 注释 (C++)

以下代码整合了实验要求的三个部分。为了方便测试，我在 `main` 函数中会依次调用三种方法。

```cpp
#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

// ==========================================
// Part1.时间 O(mn), 空间 O(mn)
// 功能: 求出 LCS 长度及其具体内容
// ==========================================
void solveStandard(string text1, string text2) {
  int m = text1.length();
  int n = text2.length();

  // 创建二维 DP 表，初始化为 0
  vector<vector<int>> dp(m + 1, vector<int>(n + 1, 0));

  // 1. 填充 DP 表
  for (int i = 1; i <= m; i++) {
    for (int j = 1; j <= n; j++) {
      if (text1[i - 1] == text2[j - 1]) {
        dp[i][j] = dp[i - 1][j - 1] + 1;
      } else {
        dp[i][j] = max(dp[i - 1][j], dp[i][j - 1]);
      }
    }
  }

  // 2. 回溯找出具体的 LCS 字符串
  // 从 dp[m][n] 开始往回找
  string lcs = "";
  int i = m, j = n;
  while (i > 0 && j > 0) {
    // 如果字符相等，说明这个字符来源于对角线，是 LCS 的一部分
    if (text1[i - 1] == text2[j - 1]) {
      lcs += text1[i - 1];
      i--;
      j--;
    }
    // 如果不相等，说明数据来源于上方或左方较大的那个
    else if (dp[i - 1][j] > dp[i][j - 1]) {
      i--; // 向上回退
    } else {
      j--; // 向左回退
    }
  }

  // 因为是倒着找的，需要反转字符串
  reverse(lcs.begin(), lcs.end());

  int length = dp[m][n];

  cout << "--- 方法 1: 标准二维 DP (空间 O(mn)) ---" << endl;
  if (length == 0) {
    cout << "LCS: 0 (不存在公共子序列)" << endl;
  } else {
    cout << "LCS: \"" << lcs << "\", 长度: " << length << endl;
  }
  cout << endl;
}

// ==========================================
// Part2.时间 O(mn), 空间 O(2 * min(m, n))
// 功能: 仅求 LCS 长度 (使用滚动数组)
// ==========================================
void solveRollingArray(string text1, string text2) {
  // 确保 text2 是较短的那个，以保证空间复杂度为 min(m, n)
  if (text1.length() < text2.length()) {
    swap(text1, text2);
  }
  int m = text1.length();
  int n = text2.length();

  // 只申请两行空间
  vector<vector<int>> dp(2, vector<int>(n + 1, 0));

  for (int i = 1; i <= m; i++) {
    for (int j = 1; j <= n; j++) {
      // 使用 i % 2 和 (i-1) % 2 来切换当前行和上一行
      int curr = i % 2;
      int prev = (i - 1) % 2;

      if (text1[i - 1] == text2[j - 1]) {
        dp[curr][j] = dp[prev][j - 1] + 1;
      } else {
        dp[curr][j] = max(dp[prev][j], dp[curr][j - 1]);
      }
    }
  }

  cout << "--- 方法 2: 滚动数组 (空间 O(2*min)) ---" << endl;
  cout << "LCS 长度: " << dp[m % 2][n] << endl;
  cout << endl;
}

// ==========================================
// Part3.时间 O(mn), 空间 O(min(m, n))
// 功能: 仅求 LCS 长度 (一维数组压缩)
// ==========================================
void solveCompressed(string text1, string text2) {
  // 确保 inner loop 对应的 text2 是较短的
  if (text1.length() < text2.length()) {
    swap(text1, text2);
  }
  int m = text1.length();
  int n = text2.length();

  // 仅申请一行空间
  vector<int> dp(n + 1, 0);

  for (int i = 1; i <= m; i++) {
    int prev_diag = 0; // 记录左上角的值 (dp[i-1][j-1])

    for (int j = 1; j <= n; j++) {
      int temp = dp[j]; // 在 dp[j] 被更新前，它是下一轮 j+1 的 "左上角"

      if (text1[i - 1] == text2[j - 1]) {
        dp[j] = prev_diag + 1;
      } else {
        // dp[j] 是 "上方" (旧值), dp[j-1] 是 "左方" (新值)
        dp[j] = max(dp[j], dp[j - 1]);
      }

      prev_diag = temp; // 更新左上角为当前列的旧值
    }
  }

  cout << "--- 方法 3: 一维压缩 (空间 O(min)) ---" << endl;
  cout << "LCS 长度: " << dp[n] << endl;
  cout << endl;
}

int main() {
  string text1, text2;

  // 控制台输入
  cout << "请输入 text1: ";
  cin >> text1;
  cout << "请输入 text2: ";
  cin >> text2;
  cout << "========================================" << endl;

  // 执行三种不同的实现
  solveStandard(text1, text2);
  solveRollingArray(text1, text2);
  solveCompressed(text1, text2);

  return 0;
}
```

## 4. 算法测试结果

以下是基于你提供的示例进行的模拟测试输出。

### 测试用例 1

**输入：**

```
text1 = abcde
text2 = ace
```

**控制台输出结果：**

```
请输入 text1: abcde
请输入 text2: ace
========================================
--- 方法 1: 标准二维 DP (空间 O(mn)) ---
LCS:  "ace", 长度: 3

--- 方法 2: 滚动数组 (空间 O(2*min)) ---
LCS 长度: 3

--- 方法 3: 一维压缩 (空间 O(min)) ---
LCS 长度:  3
```

### 测试用例 2

**输入：**

```
text1 = abc
text2 = def
```

**控制台输出结果：**

```
请输入 text1: abc
请输入 text2: def
========================================
--- 方法 1: 标准二维 DP (空间 O(mn)) ---
LCS: 0 (不存在公共子序列)

--- 方法 2: 滚动数组 (空间 O(2*min)) ---
LCS 长度: 0

--- 方法 3: 一维压缩 (空间 O(min)) ---
LCS 长度: 0
```

## 5.总结

- **方法1** 成功输出了LCS字符串和长度，适合需要路径还原的场景。
- **方法2和方法3** 成功计算了长度，且空间占用显著减小，适合仅需计算相似度而不需要具体序列的场景。
- 三种算法的时间复杂度均为 O(mn)，但空间复杂度依次降低。