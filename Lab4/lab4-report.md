# 实验四：区间树上重叠区间的查找算法

**姓名**：高宇轩

**学号**： SA25218104

**仓库地址**：https://github.com/91gaozhidao/AlgorithmLab/tree/main/Lab4

------

## 一、实验内容

本实验要求对红黑树进行修改，使其成为一棵区间树，并实现区间树上的重叠区间查找算法。

**具体要求：**

1. 实现基于红黑树的区间树数据结构
2. 实现区间插入操作，维护红黑树性质
3. 实现重叠区间查找算法

------

## 二、实验目的

1. **理解红黑树的基本性质**：掌握红黑树的5个性质及其平衡机制
2. **掌握数据结构扩展技术**：学习如何在基础数据结构上增加额外信息以支持新操作
3. **实现高效查找算法**：理解区间树如何在O(log n)时间内查找重叠区间

------

## 三、区间树的数据结构

### 3.1 理论基础

区间树是一种基于红黑树的增强数据结构，用于维护动态区间集合，并支持高效的重叠区间查询。

**区间树的核心特征：**

| 特征         | 说明                                                      |
| ------------ | --------------------------------------------------------- |
| **基础结构** | 红黑树（保证O(log n)的操作时间复杂度）                    |
| **排序键**   | 区间的下界（low值）                                       |
| **增强信息** | 每个节点维护max值：以该节点为根的子树中所有区间的最大上界 |
| **查询效率** | O(log n)时间复杂度                                        |

### 3.2 数据结构设计

#### 3.2.1 区间结构

```c++
struct Interval {
    int low;   // 区间下界
    int high;  // 区间上界
    
    // 判断两个区间是否重叠
    // 数学条件：[a,b]和[c,d]重叠 ⟺ a ≤ d 且 c ≤ b
    bool overlaps(const Interval& other) const {
        return low <= other.high && other.low <= high;
    }
};
```

#### 3.2.2 区间树节点结构

```c++
struct IntervalNode {
    Interval interval;      // 存储的区间 [low, high]
    int max;               // 子树中所有区间的最大上界
    Color color;           // 红黑树颜色（RED/BLACK）
    IntervalNode* left;    // 左子节点指针
    IntervalNode* right;   // 右子节点指针
    IntervalNode* parent;  // 父节点指针
};
```

#### 3.2.3 max值的维护规则

对于任意节点x，其max值满足：

```C++
x.max = max(x.interval.high, x.left.max, x.right.max)
```

这个值在以下情况需要更新：

- 插入新节点后，沿着插入路径向上更新
- 执行旋转操作后，更新受影响的节点

### 3.3 红黑树性质

区间树保持以下红黑树性质：

1. **性质1**：每个节点要么是红色，要么是黑色
2. **性质2**：根节点是黑色
3. **性质3**：所有叶子节点（NIL）是黑色
4. **性质4**：红色节点的两个子节点都是黑色（不存在连续的红色节点）
5. **性质5**：从任一节点到其每个叶子的所有路径都包含相同数目的黑色节点

------

## 四、核心算法设计

### 4.1 插入算法

**算法流程：**

```
INSERT(T, interval)
1. 创建新节点z，设置z.interval = interval
2. 按照interval. low作为键，执行BST插入
3.  设置z的颜色为RED
4. 从z向根更新所有祖先节点的max值
5. 调用INSERT-FIXUP(T, z)修复红黑树性质
```

**时间复杂度：** O(log n)

### 4.2 旋转操作

#### 4.2.1 左旋（Left Rotate）

```
     x              y
    / \            / \
   α   y    →     x   γ
      / \        / \
     β   γ      α   β
```

**关键点：** 旋转后必须更新x和y的max值

#### 4.2.2 右旋（Right Rotate）

```
     y              x
    / \            / \
   x   γ    →     α   y
  / \                / \
 α   β              β   γ
```

### 4.3 重叠区间查找算法

#### 4.3.1 查找单个重叠区间

```
INTERVAL-SEARCH(T, i)
1. x = T.root
2. while x ≠ NIL and i不与x.interval重叠
3.      if x.left ≠ NIL and x.left.max ≥ i.low
4.         x = x.left          // 左子树可能有重叠
5.     else
6.         x = x. right         // 只可能在右子树
7. return x
```

**算法正确性证明：**

- **情况1**：如果左子树的max ≥ i.low，则左子树可能包含重叠区间
- **情况2**：如果左子树的max < i.low，则左子树中所有区间的high < i.low，不可能重叠，只需搜索右子树

**时间复杂度：** O(log n)

#### 4.3.2 查找所有重叠区间

```
INTERVAL-SEARCH-ALL(x, i, result)
1. if x = NIL
2.     return
3. if i与x.interval重叠
4.     将x. interval加入result
5.  if x.left ≠ NIL and x.left.max ≥ i. low
6.     INTERVAL-SEARCH-ALL(x.left, i, result)
7.  if x.right ≠ NIL and x.interval.low ≤ i.high
8.     INTERVAL-SEARCH-ALL(x.right, i, result)
```

**时间复杂度：** O(min(n, k log n))，其中k是重叠区间的数量

------

## 五、源码实现

### 5.1 完整源代码

```c++
#include <algorithm>
#include <fstream>
#include <iostream>
#include <vector>
using namespace std;

// 颜色枚举
enum Color { RED, BLACK };

// 区间结构
struct Interval {
  int low;  // 区间下界
  int high; // 区间上界

  Interval(int l = 0, int h = 0) : low(l), high(h) {}

  // 判断两个区间是否重叠
  bool overlaps(const Interval &other) const {
    return low <= other.high && other.low <= high;
  }
};

// 区间树节点
struct IntervalNode {
  Interval interval;    // 节点存储的区间
  int max;              // 以该节点为根的子树中所有区间的最大上界
  Color color;          // 节点颜色
  IntervalNode *left;   // 左子节点
  IntervalNode *right;  // 右子节点
  IntervalNode *parent; // 父节点

  IntervalNode(Interval i)
      : interval(i), max(i.high), color(RED), left(nullptr), right(nullptr),
        parent(nullptr) {}
};

// 区间树类
class IntervalTree {
private:
  IntervalNode *root;
  IntervalNode *NIL; // 哨兵节点，代表空节点

  // 左旋操作
  void leftRotate(IntervalNode *x) {
    IntervalNode *y = x->right;
    x->right = y->left;

    if (y->left != NIL) {
      y->left->parent = x;
    }

    y->parent = x->parent;

    if (x->parent == NIL) {
      root = y;
    } else if (x == x->parent->left) {
      x->parent->left = y;
    } else {
      x->parent->right = y;
    }

    y->left = x;
    x->parent = y;

    // 更新max值
    updateMax(x);
    updateMax(y);
  }

  // 右旋操作
  void rightRotate(IntervalNode *y) {
    IntervalNode *x = y->left;
    y->left = x->right;

    if (x->right != NIL) {
      x->right->parent = y;
    }

    x->parent = y->parent;

    if (y->parent == NIL) {
      root = x;
    } else if (y == y->parent->right) {
      y->parent->right = x;
    } else {
      y->parent->left = x;
    }

    x->right = y;
    y->parent = x;

    // 更新max值
    updateMax(y);
    updateMax(x);
  }

  // 更新节点的max值
  void updateMax(IntervalNode *node) {
    if (node == NIL)
      return;

    node->max = node->interval.high;
    if (node->left != NIL) {
      node->max = max(node->max, node->left->max);
    }
    if (node->right != NIL) {
      node->max = max(node->max, node->right->max);
    }
  }

  // 插入后修复红黑树性质
  void insertFixup(IntervalNode *z) {
    while (z->parent->color == RED) {
      if (z->parent == z->parent->parent->left) {
        IntervalNode *y = z->parent->parent->right; // 叔节点

        if (y->color == RED) { // 情况1：叔节点是红色
          z->parent->color = BLACK;
          y->color = BLACK;
          z->parent->parent->color = RED;
          z = z->parent->parent;
        } else {
          if (z == z->parent->right) { // 情况2：z是右孩子
            z = z->parent;
            leftRotate(z);
          }
          // 情况3：z是左孩子
          z->parent->color = BLACK;
          z->parent->parent->color = RED;
          rightRotate(z->parent->parent);
        }
      } else { // 对称情况
        IntervalNode *y = z->parent->parent->left;

        if (y->color == RED) {
          z->parent->color = BLACK;
          y->color = BLACK;
          z->parent->parent->color = RED;
          z = z->parent->parent;
        } else {
          if (z == z->parent->left) {
            z = z->parent;
            rightRotate(z);
          }
          z->parent->color = BLACK;
          z->parent->parent->color = RED;
          leftRotate(z->parent->parent);
        }
      }
    }
    root->color = BLACK;
  }

  // 中序遍历辅助函数（用于调试）
  void inorderHelper(IntervalNode *node) {
    if (node != NIL) {
      inorderHelper(node->left);
      cout << "[" << node->interval.low << ", " << node->interval.high
           << "] max=" << node->max
           << " color=" << (node->color == RED ? "RED" : "BLACK") << endl;
      inorderHelper(node->right);
    }
  }

  // 更新从节点到根的路径上所有节点的max值
  void updateMaxToRoot(IntervalNode *node) {
    while (node != NIL) {
      updateMax(node);
      node = node->parent;
    }
  }

public:
  // 构造函数
  IntervalTree() {
    NIL = new IntervalNode(Interval(0, 0));
    NIL->color = BLACK;
    NIL->left = NIL->right = NIL->parent = NIL;
    root = NIL;
  }

  // 插入区间
  void insert(Interval interval) {
    IntervalNode *z = new IntervalNode(interval);
    z->left = NIL;
    z->right = NIL;

    IntervalNode *y = NIL;
    IntervalNode *x = root;

    // 按照区间的下界进行二叉搜索树插入
    while (x != NIL) {
      y = x;
      if (z->interval.low < x->interval.low) {
        x = x->left;
      } else {
        x = x->right;
      }
    }

    z->parent = y;

    if (y == NIL) {
      root = z;
    } else if (z->interval.low < y->interval.low) {
      y->left = z;
    } else {
      y->right = z;
    }

    z->color = RED;

    // 更新从插入节点到根的路径上所有节点的max值
    updateMaxToRoot(z);

    // 修复红黑树性质
    insertFixup(z);
  }

  // 查找与给定区间重叠的区间
  IntervalNode *searchOverlap(Interval i) {
    return searchOverlapHelper(root, i);
  }

  // 查找所有与给定区间重叠的区间
  vector<Interval> searchAllOverlaps(Interval i) {
    vector<Interval> result;
    searchAllOverlapsHelper(root, i, result);
    return result;
  }

private:
  // 查找重叠区间的辅助函数（返回第一个找到的）
  IntervalNode *searchOverlapHelper(IntervalNode *x, Interval i) {
    while (x != NIL && !i.overlaps(x->interval)) {
      // 如果左子树的max值大于等于查询区间的下界，则左子树可能包含重叠区间
      if (x->left != NIL && x->left->max >= i.low) {
        x = x->left;
      } else {
        x = x->right;
      }
    }
    return x;
  }

  // 查找所有重叠区间的辅助函数
  void searchAllOverlapsHelper(IntervalNode *x, Interval i,
                               vector<Interval> &result) {
    if (x == NIL)
      return;

    // 检查当前节点
    if (i.overlaps(x->interval)) {
      result.push_back(x->interval);
    }

    // 如果左子树可能包含重叠区间，则搜索左子树
    if (x->left != NIL && x->left->max >= i.low) {
      searchAllOverlapsHelper(x->left, i, result);
    }

    // 如果当前节点的下界小于等于查询区间的上界，则搜索右子树
    if (x->right != NIL && x->interval.low <= i.high) {
      searchAllOverlapsHelper(x->right, i, result);
    }
  }

public:
  // 中序遍历（用于调试）
  void inorder() { inorderHelper(root); }

  // 从文件读取并构建区间树
  bool buildFromFile(const string &filename) {
    ifstream file(filename);
    if (!file.is_open()) {
      cout << "无法打开文件: " << filename << endl;
      return false;
    }

    int n;
    file >> n;

    cout << "开始插入 " << n << " 个区间..." << endl;
    for (int i = 0; i < n; i++) {
      int low, high;
      file >> low >> high;
      cout << "插入区间 [" << low << ", " << high << "]" << endl;
      insert(Interval(low, high));
    }

    file.close();
    return true;
  }
};

int main() {
  cout << "========================================" << endl;
  cout << "    区间树重叠区间查找算法实验" << endl;
  cout << "========================================" << endl << endl;

  IntervalTree tree;

  // 从文件构建区间树
  //   cout << "从文件读取区间并构建区间树" << endl;
  //   cout << "----------------------------------------" << endl;
  if (!tree.buildFromFile("insert.txt")) {
    return 1;
  }

  cout << "\n区间树构建完成！" << endl << endl;

  //   // 可选：显示树的中序遍历（用于调试）
  //   cout << "区间树的中序遍历（按区间下界排序）" << endl;
  //   cout << "----------------------------------------" << endl;
  //   tree.inorder();

  while (true) {
    cout << "\n请输入要查询的区间（格式：low high，输入 -1 -1 退出）：";
    int low, high;
    cin >> low >> high;

    if (low == -1 && high == -1) {
      break;
    }

    Interval queryInterval(low, high);

    cout << "\n查询区间: [" << low << ", " << high << "]" << endl;

    // 查找所有重叠区间
    vector<Interval> overlaps = tree.searchAllOverlaps(queryInterval);

    if (overlaps.empty()) {
      cout << "结果：未找到重叠区间" << endl;
    } else {
      cout << "结果：找到 " << overlaps.size() << " 个重叠区间：" << endl;
      for (const auto &interval : overlaps) {
        cout << "  [" << interval.low << ", " << interval.high << "]" << endl;
      }
    }
  }
  return 0;
}
```

------

## 六、算法测试结果

### 6. 1 测试环境

- **操作系统**: Linux
- **编译器**: g++ 
- **测试数据**: insert.txt 

### 6.2 测试数据集

```
测试数据文件：insert.txt
区间数量：30
区间列表：
[18,23] [45,46] [30,34] [2,17] [13,18] [9,18] [32,43] [43,45] [21,24] [6,15]
[49,50] [14,22] [11,22] [38,43] [15,16] [19,24] [36,42] [8,13] [7,14] [42,50]
[48,49] [0,1] [4,15] [24,25] [34,43] [10,17] [31,40] [17,18] [16,25] [33,37]
```

### 6.3 测试用例及结果

#### 测试用例1：查询 [22, 25]

**执行过程：**

```
查询区间: [22, 25]
结果：找到 7 个重叠区间：
  [11, 22]
  [18, 23]
  [14, 22]
  [16, 25]
  [21, 24]
  [19, 24]
  [24, 25]
```

#### 测试用例2：查询 [10, 15]

**执行过程：**

```
查询区间: [10, 15]
结果：找到 11 个重叠区间：
  [13, 18]
  [6, 15]
  [2, 17]
  [4, 15]
  [9, 18]
  [8, 13]
  [7, 14]
  [11, 22]
  [10, 17]
  [15, 16]
  [14, 22]
```

#### 测试用例3：查询 [100, 200]

**执行过程：**

```
查询区间: [100, 200]
结果：未找到重叠区间
```

## 七、实验过程中遇到的困难及收获

### 7.1 max值更新时机和顺序错误

**问题描述：**
在实现旋转操作时，最初忘记更新max值，导致查找算法总是返回错误结果。后来添加了max值更新，但更新顺序错误（先更新父节点再更新子节点），仍然出现错误。

**问题原因：**

- 旋转操作改变了树的结构，必须更新受影响节点的max值
- max值的计算依赖于子节点的max值，必须按照"子节点 → 父节点"的顺序更新

**解决过程：**

1. 第一次尝试：在旋转函数末尾添加 `updateMax(y); updateMax(x);`
2. 发现仍然错误：通过打印调试发现更新顺序有问题
3. 最终解决：在左旋中先更新x再更新y，在右旋中先更新y再更新x

------

### 7.2 查找所有重叠区间的剪枝策略

**问题描述：**
最初实现的查找所有重叠区间的算法没有使用剪枝，导致性能很差，需要设计合理的剪枝策略来提高效率。

**问题原因：**

- 暴力遍历所有节点，没有利用区间树的特性
- 不理解max值在查找中的作用
- 没有利用BST的排序性质

**解决过程：**

1. **分析剪枝条件**：
   - 左子树剪枝：如果 `x.left.max < i. low`，则左子树中所有区间的high都小于i.low，不可能重叠
   - 右子树剪枝：如果 `x.interval.low > i.high`，则右子树中所有区间的low都大于i.high，不可能重叠
2. **实现剪枝**：

```c++
// 左子树剪枝
if (x->left != NIL && x->left->max >= i.low) {
    searchAllOverlapsHelper(x->left, i, result);
}

// 右子树剪枝
if (x->right != NIL && x->interval.low <= i.high) {
    searchAllOverlapsHelper(x->right, i, result);
}
```

**收获：**

- 理解了如何利用数据结构的性质进行优化
- 学会了通过剪枝减少不必要的搜索
- 认识到理论分析与实际实现的差距



