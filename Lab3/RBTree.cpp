#include <algorithm>
#include <fstream>
#include <iostream>
#include <queue>
#include <vector>

using namespace std;

// 红黑树节点颜色枚举
enum Color { RED, BLACK };

// 红黑树节点结构
struct Node {
  Color color;  // 节点颜色（红/黑）
  int key;      // 节点键值
  Node *left;   // 左子节点
  Node *right;  // 右子节点
  Node *parent; // 父节点
  Node(int k)
      : key(k), color(RED), left(nullptr), right(nullptr), parent(nullptr) {}
};

class RedBlackTree {
private:
  Node *root;              // 根节点
  Node *NIL;               // 黑色虚拟叶子节点（避免空指针判断）
  vector<int> caseNumbers; // 记录修复过程中触发的修复情况编号

public:
  RedBlackTree() {
    // 初始化虚拟叶子节点（黑色）
    NIL = new Node(0);
    NIL->color = BLACK;
    NIL->left = NIL; // 虚拟叶子的左右子节点指向自己
    NIL->right = NIL;
    NIL->parent = NIL;
    root = NIL; // 初始为空树
  }

  ~RedBlackTree() {
    // 简化内存释放（实际应用需完整释放）
  }

  // 插入新节点
  void insert(int key) {
    Node *z = new Node(key);
    z->left = NIL; // 新节点左右子节点指向NIL
    z->right = NIL;
    z->parent = NIL;

    Node *y = NIL;  // 用于遍历的父节点
    Node *x = root; // 从根开始遍历
    // BST方式查找插入位置
    while (x != NIL) {
      y = x;
      if (z->key < x->key) {
        x = x->left;
      } else {
        x = x->right;
      }
    }
    z->parent = y;
    if (y == NIL) {
      root = z; // 新节点成为根
    } else if (z->key < y->key) {
      y->left = z; // 插入左子树
    } else {
      y->right = z; // 插入右子树
    }

    z->color = RED; // 新节点设为红色（违反性质4，需修复）
    insertFixup(z); // 修复红黑树性质
  }

  // 修复红黑树插入后的性质
  void insertFixup(Node *z) {
    // 当父节点是红色时（违反性质4）
    while (z->parent->color == RED) {
      // 情况1：父节点是祖父节点的左孩子
      if (z->parent == z->parent->parent->left) {
        Node *y = z->parent->parent->right; // 叔叔节点
        // 情况3：叔叔是红色（需翻色）
        if (y->color == RED) {
          z->parent->color = BLACK;       // 父节点变黑
          y->color = BLACK;               // 叔叔变黑
          z->parent->parent->color = RED; // 祖父变红
          z = z->parent->parent;          // 重点：向上检查祖父节点
          caseNumbers.push_back(3);       // 记录情况3
        } else {
          // 情况4：叔叔是黑色，且z是右孩子（需左旋）
          if (z == z->parent->right) {
            z = z->parent;            // 重点：z上移
            leftRotate(z);            // 左旋（将z转成左孩子）
            caseNumbers.push_back(4); // 记录情况4
          }
          // 情况5：叔叔是黑色，且z是左孩子（需右旋）
          z->parent->color = BLACK;       // 父节点变黑
          z->parent->parent->color = RED; // 祖父变红
          rightRotate(z->parent->parent); // 右旋
          caseNumbers.push_back(5);       // 记录情况5
        }
      }
      // 情况6：父节点是祖父节点的右孩子（对称情况）
      else {
        Node *y = z->parent->parent->left; // 叔叔节点
        if (y->color == RED) {
          z->parent->color = BLACK;
          y->color = BLACK;
          z->parent->parent->color = RED;
          z = z->parent->parent;
          caseNumbers.push_back(6); // 记录情况6
        } else {
          // 情况4（对称）：z是左孩子（需右旋）
          if (z == z->parent->left) {
            z = z->parent;
            rightRotate(z);
            caseNumbers.push_back(4); // 记录情况4
          }
          // 情况5（对称）：z是右孩子（需左旋）
          else {
            z->parent->color = BLACK;
            z->parent->parent->color = RED;
            leftRotate(z->parent->parent);
            caseNumbers.push_back(5); // 记录情况5
          }
        }
      }
    }
    root->color = BLACK; // 确保根节点为黑色
  }

  // 左旋操作（以x为支点）
  void leftRotate(Node *x) {
    Node *y = x->right; // y成为x的右孩子
    x->right = y->left; // y的左子树接给x
    if (y->left != NIL) {
      y->left->parent = x;
    }
    y->parent = x->parent; // y接替x的父节点
    if (x->parent == NIL) {
      root = y; // x是根则y成为新根
    } else if (x == x->parent->left) {
      x->parent->left = y; // x是左孩子
    } else {
      x->parent->right = y; // x是右孩子
    }
    y->left = x; // x成为y的左孩子
    x->parent = y;
  }

  // 右旋操作（以x为支点）
  void rightRotate(Node *x) {
    Node *y = x->left;  // y成为x的左孩子
    x->left = y->right; // y的右子树接给x
    if (y->right != NIL) {
      y->right->parent = x;
    }
    y->parent = x->parent; // y接替x的父节点
    if (x->parent == NIL) {
      root = y; // x是根则y成为新根
    } else if (x == x->parent->left) {
      x->parent->left = y; // x是左孩子
    } else {
      x->parent->right = y; // x是右孩子
    }
    y->right = x; // x成为y的右孩子
    x->parent = y;
  }

  // 中序遍历（左-根-右）
  void inOrder(Node *node, ofstream &out) {
    if (node != NIL) {
      inOrder(node->left, out);
      out << node->key << " " << (node->color == RED ? "red" : "black") << endl;
      inOrder(node->right, out);
    }
  }

  // 先序遍历（根-左-右）
  void preOrder(Node *node, ofstream &out) {
    if (node != NIL) {
      out << node->key << " " << (node->color == RED ? "red" : "black") << endl;
      preOrder(node->left, out);
      preOrder(node->right, out);
    }
  }

  // 层次遍历（队列实现）
  void levelOrder(ofstream &out) {
    if (root == NIL)
      return;
    queue<Node *> q;
    q.push(root);
    while (!q.empty()) {
      Node *node = q.front();
      q.pop();
      out << node->key << " " << (node->color == RED ? "red" : "black") << endl;
      if (node->left != NIL) {
        q.push(node->left);
      }
      if (node->right != NIL) {
        q.push(node->right);
      }
    }
  }

  // 保存中序遍历结果
  void saveInOrder(string filename) {
    ofstream out(filename);
    inOrder(root, out);
    out.close();
  }

  // 保存先序遍历结果
  void savePreOrder(string filename) {
    ofstream out(filename);
    preOrder(root, out);
    out.close();
  }

  // 保存层次遍历结果
  void saveLevelOrder(string filename) {
    ofstream out(filename);
    levelOrder(out);
    out.close();
  }

  // 获取修复情况编号列表
  vector<int> getCaseNumbers() const { return caseNumbers; }
};

int main() {
  RedBlackTree tree;
  ifstream infile("insert.txt");
  if (!infile) {
    cerr << "错误：无法打开输入文件" << endl;
    return 1;
  }

  int n;
  infile >> n;
  vector<int> keys;
  for (int i = 0; i < n; i++) {
    int key;
    infile >> key;
    keys.push_back(key);
  }

  // 按顺序插入所有节点
  for (int key : keys) {
    tree.insert(key);
  }

  // 输出RB_INSERT_FIXUP触发的修复情况编号
  vector<int> cases = tree.getCaseNumbers();
  for (int i = 0; i < cases.size(); i++) {
    if (i > 0)
      cout << " ";
    cout << cases[i];
  }
  cout << endl;

  // 保存三种遍历结果到文件
  tree.saveInOrder("LNR.txt");
  tree.savePreOrder("NLR.txt");
  tree.saveLevelOrder("LOT.txt");

  return 0;
}