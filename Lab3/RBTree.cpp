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
        TNode *y = z->p->p->right; // 叔叔节点

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
          // Case 3 (注意：Case 2 处理完后通常会流转到 Case 3，或者直接进入 Case
          // 3)
          cout << "3 "; // 输出 Case 3
          z->p->color = BLACK;
          z->p->p->color = RED;
          rightRotate(z->p->p);
        }
      } else { // 对称情况 (else part omitted in image, mapped to 4, 5, 6)
        TNode *y = z->p->p->left; // Uncle

        // Case 4
        if (y->color == RED) {
          cout << "4 ";
          z->p->color = BLACK;
          y->color = BLACK;
          z->p->p->color = RED;
          z = z->p->p;
        } else {
          // Case 5
          if (z == z->p->left) {
            cout << "5 ";
            z = z->p;
            rightRotate(z);
          }
          // Case 6
          cout << "6 ";
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