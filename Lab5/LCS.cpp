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