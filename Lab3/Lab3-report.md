# 红黑树插入算法实验报告

**姓名：** 高宇轩

**学号： **SA25218104

**仓库地址**：https://github.com/91gaozhidao/AlgorithmLab/tree/main/Lab3

## 1. 实验内容

实现红黑树的插入操作，包括：

- 按照BST规则插入新节点（初始为红色）
- 修复插入后违反的红黑树性质
- 实现三种遍历：中序（LNR）、先序（NLR）、层次遍历（LOT）
- 从文件 `insert.txt` 读取输入数据，输出修复情况编号到控制台，并生成 `LNR.txt`、`NLR.txt`、`LOT.txt` 三个遍历结果文件

## 2. 实验目的

- 理解红黑树的5条核心性质（根黑、红节点子节点黑、路径黑节点数同、NIL为黑、无连续红节点）
- 掌握插入操作的修复算法（RB-INSERT-FIXUP）及6种情况的处理逻辑
- 验证红黑树的平衡性与正确性
- 熟悉树的遍历实现（递归中序/先序、队列层次遍历）

## 3. 算法设计思路

### 核心设计

1. **红黑树性质保障**

   - 使用 `NIL` 节点（黑色）替代空指针，简化边界条件
   - 插入新节点初始化为红色
   - 修复过程通过 `insertFixup` 确保所有性质

2. **插入修复逻辑（RB-INSERT-FIXUP）**

   **Case 1, 2, 3**：完全对应左子节点情况

   - `Case 1`：叔叔节点是红色。
   - `Case 2`：叔叔是黑色，且当前节点是右孩子（会左旋变为 Case 3）。
   - `Case 3`：叔叔是黑色，且当前节点是左孩子（右旋并变色）。

   **Case 4, 5, 6**：对应 `else` 分支（右子节点情况）。

   - `Case 4`：对称的 Case 1。
   - `Case 5`：对称的 Case 2（右孩子 -> 右旋）。
   - `Case 6`：对称的 Case 3（左孩子 -> 左旋并变色）。

3. **遍历实现**

   - **中序（LNR）**：递归左-根-右（输出升序序列）
   - **先序（NLR）**：递归根-左-右
   - **层次遍历（LOT）**：队列实现（层序输出）

4. **使用内存池优化**

   - **解决系统开销大的问题**：减少调用 `new/malloc` 等昂贵操作的次数。

## 4. 源码 + 注释

```cpp
#include <fstream>
#include <iostream>
#include <queue>
using namespace std;
enum Color { RED, BLACK }; // 颜色枚举
struct TNode {
  int key;
  Color color;
  TNode *left;
  TNode *right;
  TNode *p; // Parent
};
class RedBlackTree {
private:
  TNode *root;
  TNode *nil;

  // 左旋
  void leftRotate(TNode *x) {
    TNode *y = x->right;
    x->right = y->left;
    if (y->left != nil) {
      y->left->p = x;
    }
    y->p = x->p;
    if (x->p == nil) {
      root = y;
    } else if (x == x->p->left) {
      x->p->left = y;
    } else {
      x->p->right = y;
    }
    y->left = x;
    x->p = y;
  }

  // 右旋
  void rightRotate(TNode *y) {
    TNode *x = y->left;
    y->left = x->right;
    if (x->right != nil) {
      x->right->p = y;
    }
    x->p = y->p;
    if (y->p == nil) {
      root = x;
    } else if (y == y->p->right) {
      y->p->right = x;
    } else {
      y->p->left = x;
    }
    x->right = y;
    y->p = x;
  }

  // 插入修复
  void rbInsertFixup(TNode *z) {
    while (z->p->color == RED) {
      if (z->p == z->p->p->left) {
        TNode *y = z->p->p->right; // 右叔叔节点

        // Case 1
        if (y->color == RED) {
          cout << "1 "; // 输出 Case 1
          z->p->color = BLACK;
          y->color = BLACK;
          z->p->p->color = RED;
          z = z->p->p;
        } else {
          // Case 2
          if (z == z->p->right) {
            cout << "2 "; // 输出 Case 2
            z = z->p;
            leftRotate(z);
          }
          // Case 3 
          cout << "3 "; // 输出 Case 3
          z->p->color = BLACK;
          z->p->p->color = RED;
          rightRotate(z->p->p);
        }
      } else { // 对称情况
        TNode *y = z->p->p->left; // 左叔叔节点

        // Case 4
        if (y->color == RED) {
          cout << "4 ";// 输出 Case 4
          z->p->color = BLACK;
          y->color = BLACK;
          z->p->p->color = RED;
          z = z->p->p;
        } else {
          // Case 5
          if (z == z->p->left) {
            cout << "5 ";// 输出 Case 5
            z = z->p;
            rightRotate(z);
          }
          // Case 6
          cout << "6 ";// 输出 Case 6
          z->p->color = BLACK;
          z->p->p->color = RED;
          leftRotate(z->p->p);
        }
      }
    }
    root->color = BLACK;
  }

  // 先序遍历递归
  void preOrderHelper(TNode *node, ofstream &ofs) {
    if (node != nil) {
      ofs << node->key << " " << (node->color == RED ? "RED" : "BLACK") << endl;
      preOrderHelper(node->left, ofs);
      preOrderHelper(node->right, ofs);
    }
  }

  // 中序遍历递归
  void inOrderHelper(TNode *node, ofstream &ofs) {
    if (node != nil) {
      inOrderHelper(node->left, ofs);
      ofs << node->key << " " << (node->color == RED ? "RED" : "BLACK") << endl;
      inOrderHelper(node->right, ofs);
    }
  }

public:
  RedBlackTree() {
    nil = new TNode;
    nil->color = BLACK;                        // T.nil 是黑色的
    nil->left = nil->right = nil->p = nullptr; // 或者是自身，只要不访问即可
    root = nil;
  }

  // 插入函数
  void insert(int key) {
    TNode *z = new TNode;
    z->key = key;
    TNode *y = nil;
    TNode *x = root;
    while (x != nil) {
      y = x;
      if (z->key < x->key) {
        x = x->left;
      } else {
        x = x->right;
      }
    }
    z->p = y;
    if (y == nil) {
      root = z;
    } else if (z->key < y->key) {
      y->left = z;
    } else {
      y->right = z;
    }
    z->left = nil;
    z->right = nil;
    z->color = RED;
    rbInsertFixup(z);
  }

  // 生成文件输出
  void generateOutputs() {
    // 1. 中序遍历 (LNR)
    ofstream lnrFile("LNR.txt");
    if (lnrFile.is_open()) {
      inOrderHelper(root, lnrFile);
      lnrFile.close();
    }
    // 2. 先序遍历 (NLR)
    ofstream nlrFile("NLR.txt");
    if (nlrFile.is_open()) {
      preOrderHelper(root, nlrFile);
      nlrFile.close();
    }
    // 3. 层次遍历 (LOT)
    ofstream lotFile("LOT.txt");
    if (lotFile.is_open()) {
      if (root != nil) {
        queue<TNode *> q;
        q.push(root);
        while (!q.empty()) {
          TNode *current = q.front();
          q.pop();
          lotFile << current->key << " "
                  << (current->color == RED ? "RED" : "BLACK") << endl;
          if (current->left != nil)
            q.push(current->left);
          if (current->right != nil)
            q.push(current->right);
        }
      }
      lotFile.close();
    }
  }
};
int main() {
  ifstream inputFile("insert.txt");
  if (!inputFile.is_open()) {
    cerr << "Error: did not found insert.txt" << endl;
    return 1;
  }
  int n;
  inputFile >> n; // 读取第一行：数据个数
  RedBlackTree rbt;
  int val;
  cout << "红黑树修复cases: ";
  // 读取第二行：n个数据
  for (int i = 0; i < n; ++i) {
    inputFile >> val;
    // 每次插入可能会触发多次 Case 1, 4，或者一次性的 Case 2,3,5,6
    rbt.insert(val);
  }
  cout << endl;
  inputFile.close();
  rbt.generateOutputs(); // 生成三个输出文件
  return 0;
}
```

```cpp

  // =================内存池相关变量与函数 =================
  const int BLOCK_SIZE = 100;   // 每个内存块能存多少个节点
  vector<TNode *> memoryBlocks; // 存储所有申请的大内存块
  int freeIndex;                // 当前内存块用到第几个位置了

  // 从内存池分配节点的辅助函数
  TNode *allocateNode(int key) {
    // 如果当前块用完了（或者还没申请过块），就申请一个新的大块
    if (freeIndex >= BLOCK_SIZE || memoryBlocks.empty()) {
      TNode *newBlock = new TNode[BLOCK_SIZE];
      memoryBlocks.push_back(newBlock);
      freeIndex = 0; // 重置游标
    }

    // 从当前块中拿出一个空闲节点
    TNode *node = &memoryBlocks.back()[freeIndex++];

    // 预先初始化基础数据
    node->key = key;
    node->color = RED; // 默认为红色
    node->left = nil;
    node->right = nil;
    node->p = nil;

    return node;
  }
```



## 5. 算法正确性测试

### 测试数据

`insert.txt` 内容：

```text
20
12 1 9 2 0 11 7 19 4 15 18 5 14 13 10 16 6 3 8 17
```

### 测试结果

1. **修复情况编号验证**

   - 控制台输出：`2 3 1 4 5 6 4 2 3 4 1 1 3 4 5 6 2 3 4 6 6 `
   - **验证**：符合红黑树插入修复逻辑

2. **遍历结果验证**

   - **中序遍历（LNR.txt）**：

     ```text
     0 BLACK
     1 RED
     2 BLACK
     3 RED
     4 BLACK
     ...
     ```
     
   - **先序遍历（NLR.txt）**：
   
     ```text
     9 BLACK
     4 BLACK
     1 RED
     0 BLACK
     2 BLACK
     ...
     ```
   
   - **层次遍历（LOT.txt）**：
   
     ```text
     9 BLACK
     4 BLACK
     14 BLACK
     1 RED
     6 RED
     ...
     ```
   
3. **红黑树性质验证**

   - 根节点始终为黑色（`root->color = BLACK`）
   - 无连续红色节点
   - 每条路径黑节点数相同

## 6. 实验过程中遇到的困难及收获

### 困难

1. **旋转逻辑混淆**
   - 问题：情况4（左旋）和情况5（右旋）的旋转方向易混淆。
   - 解决：绘制旋转示意图，明确 `leftRotate` 使右孩子上移，`rightRotate` 使左孩子上移。
2. **对称情况处理**
   - 问题：父节点是祖父右孩子时（情况6），代码逻辑与左孩子分支重复。
   - 解决：将对称分支独立处理，用 `else` 分支统一管理，避免冗余。
3. **NIL节点边界处理**
   - 问题：空指针导致崩溃（如 `x->left` 为 `nullptr`）。
   - 解决：用 `NIL` 节点替代空指针，统一处理所有边界。

### 收获

1. **深入理解了红黑树**
