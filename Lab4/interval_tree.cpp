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