/**
 * @file    BST.h
 * @brief   二叉搜索树（Binary Search Tree）模板类
 *
 * 泛型 BST，支持插入、查找、删除、中序遍历。
 * 用于 BookRepository 按图书编号索引，实现 O(log n) 查找。
 * 不支持自平衡（AVL/红黑树），作为数据结构课程的基础实现。
 *
 * 注意事项：
 *   - 模板类所有实现将与声明放在同一头文件中（模板类特性）
 *   - 当前阶段仅声明接口，不实现函数体
 *   - T 类型必须支持 operator== 和 operator< 比较
 */

#ifndef BST_H
#define BST_H

#include <vector>

template<typename T>
class BST {
private:
    // ========================================================================
    //  内部节点结构
    // ========================================================================

    /** @brief BST 节点 —— 包含数据域和左右子树指针 */
    struct Node {
        T     data;   // 节点存储的数据
        Node* left;   // 左子树指针
        Node* right;  // 右子树指针

        /** @brief 节点构造函数 */
        Node(const T& d);
    };

    // ========================================================================
    //  私有递归辅助方法
    // ========================================================================

    /** @brief 递归插入辅助 —— 向以 node 为根的子树中插入 data */
    Node* insertHelper(Node* node, const T& data);

    /** @brief 递归查找辅助 —— 在以 node 为根的子树中查找 key */
    Node* searchHelper(Node* node, const T& key) const;

    /** @brief 递归删除辅助 —— 在以 node 为根的子树中删除 key */
    Node* removeHelper(Node* node, const T& key);

    /** @brief 查找最小节点 —— 在以 node 为根的子树中找最左节点 */
    Node* findMin(Node* node) const;

    /** @brief 递归中序遍历辅助 —— 结果存入 result */
    void inorderHelper(Node* node, std::vector<T>& result) const;

    /** @brief 递归销毁整棵树 —— 释放所有节点内存 */
    void destroyTree(Node* node);

    // ========================================================================
    //  成员变量
    // ========================================================================

    Node* root;  // 根节点指针

public:
    // ========================================================================
    //  构造与析构
    // ========================================================================

    /** @brief 构造函数 —— 初始化空树 */
    BST();

    /** @brief 析构函数 —— 释放整棵树的内存 */
    ~BST();

    // ========================================================================
    //  核心操作
    // ========================================================================

    /** @brief 插入节点 —— 向 BST 中插入一个数据元素 */
    void insert(const T& data);

    /** @brief 查找节点 —— 返回指向匹配数据的指针，未找到返回 nullptr */
    T* search(const T& key);

    /** @brief 删除节点 —— 按比较器匹配并删除，成功返回 true */
    bool remove(const T& key);

    /** @brief 中序遍历 —— 将 BST 中所有元素按升序存入 vector */
    void inorderTraversal(std::vector<T>& result) const;

    /** @brief 判空 —— 树为空时返回 true */
    bool isEmpty() const;
};

#endif // BST_H

// ================================================================================
//  模板实现（模板类必须将实现放在头文件中）
// ================================================================================

// ————————————————————————————————————————————————————————————————————————————————
//  Node 构造函数
// ————————————————————————————————————————————————————————————————————————————————

template<typename T>
BST<T>::Node::Node(const T& d)
    : data(d)
    , left(nullptr)
    , right(nullptr)
{
}

// ————————————————————————————————————————————————————————————————————————————————
//  构造函数 —— 初始化空树
// ————————————————————————————————————————————————————————————————————————————————

template<typename T>
BST<T>::BST()
    : root(nullptr)
{
}

// ————————————————————————————————————————————————————————————————————————————————
//  析构函数 —— 释放整棵树
// ————————————————————————————————————————————————————————————————————————————————

template<typename T>
BST<T>::~BST() {
    destroyTree(root);
    root = nullptr;
}

// ————————————————————————————————————————————————————————————————————————————————
//  判空
// ————————————————————————————————————————————————————————————————————————————————

template<typename T>
bool BST<T>::isEmpty() const {
    return root == nullptr;
}

// ————————————————————————————————————————————————————————————————————————————————
//  插入（公开接口）
// ————————————————————————————————————————————————————————————————————————————————

template<typename T>
void BST<T>::insert(const T& data) {
    root = insertHelper(root, data);
}

// ————————————————————————————————————————————————————————————————————————————————
//  插入辅助（递归）
//  规则：data < node.data → 插入左子树
//        data > node.data → 插入右子树
//        data == node.data → 不插入（BST 不允许重复键）
// ————————————————————————————————————————————————————————————————————————————————

template<typename T>
typename BST<T>::Node* BST<T>::insertHelper(Node* node, const T& data) {
    // 到达空位 → 创建新节点
    if (node == nullptr) {
        return new Node(data);
    }

    // data < 当前节点 → 递归插入左子树
    if (data < node->data) {
        node->left = insertHelper(node->left, data);
    }
    // data > 当前节点 → 递归插入右子树
    else if (node->data < data) {
        node->right = insertHelper(node->right, data);
    }
    // 相等：不插入（BST 不允许重复键），直接返回原节点

    return node;
}

// ————————————————————————————————————————————————————————————————————————————————
//  查找（公开接口）
//  返回指向树中匹配数据的指针，未找到返回 nullptr
// ————————————————————————————————————————————————————————————————————————————————

template<typename T>
T* BST<T>::search(const T& key) {
    Node* found = searchHelper(root, key);
    if (found != nullptr) {
        return &(found->data);
    }
    return nullptr;
}

// ————————————————————————————————————————————————————————————————————————————————
//  查找辅助（递归）
//  二分查找：key < node.data → 左子树
//            key > node.data → 右子树
//            key == node.data → 找到
// ————————————————————————————————————————————————————————————————————————————————

template<typename T>
typename BST<T>::Node* BST<T>::searchHelper(Node* node, const T& key) const {
    if (node == nullptr) {
        return nullptr;       // 到底了，没找到
    }
    if (key < node->data) {
        return searchHelper(node->left, key);   // 去左子树找
    }
    if (node->data < key) {
        return searchHelper(node->right, key);  // 去右子树找
    }
    return node;  // 找到了
}

// ————————————————————————————————————————————————————————————————————————————————
//  删除（公开接口）
//  先查找确认存在 → 调用递归删除
// ————————————————————————————————————————————————————————————————————————————————

template<typename T>
bool BST<T>::remove(const T& key) {
    // 先确认要删除的节点存在
    Node* found = searchHelper(root, key);
    if (found == nullptr) {
        return false;  // 不存在
    }
    root = removeHelper(root, key);
    return true;
}

// ————————————————————————————————————————————————————————————————————————————————
//  删除辅助（递归）
//  三种情况：
//    Case 1：叶子节点 → 直接删除
//    Case 2：只有一个子节点 → 用子节点替换
//    Case 3：有两个子节点 → 找右子树最小节点（中序后继）替换，然后删除后继
// ————————————————————————————————————————————————————————————————————————————————

template<typename T>
typename BST<T>::Node* BST<T>::removeHelper(Node* node, const T& key) {
    if (node == nullptr) {
        return nullptr;
    }

    // 在左子树中删除
    if (key < node->data) {
        node->left = removeHelper(node->left, key);
    }
    // 在右子树中删除
    else if (node->data < key) {
        node->right = removeHelper(node->right, key);
    }
    // 找到了要删除的节点
    else {
        // Case 1 + Case 2（左为空/右为空）
        if (node->left == nullptr) {
            Node* rightChild = node->right;
            delete node;
            return rightChild;
        }
        if (node->right == nullptr) {
            Node* leftChild = node->left;
            delete node;
            return leftChild;
        }

        // Case 3：两个子节点都非空
        // 找到右子树的最小节点（中序后继）
        Node* successor = findMin(node->right);
        // 用后继节点的值替换当前节点
        node->data = successor->data;
        // 在右子树中删除后继节点
        node->right = removeHelper(node->right, successor->data);
    }

    return node;
}

// ————————————————————————————————————————————————————————————————————————————————
//  查找最小节点 —— 一直向左走到底
// ————————————————————————————————————————————————————————————————————————————————

template<typename T>
typename BST<T>::Node* BST<T>::findMin(Node* node) const {
    if (node == nullptr) {
        return nullptr;
    }
    while (node->left != nullptr) {
        node = node->left;  // 最左节点即最小
    }
    return node;
}

// ————————————————————————————————————————————————————————————————————————————————
//  中序遍历（公开接口）
//  左 → 根 → 右，结果按升序存入 vector
// ————————————————————————————————————————————————————————————————————————————————

template<typename T>
void BST<T>::inorderTraversal(std::vector<T>& result) const {
    inorderHelper(root, result);
}

// ————————————————————————————————————————————————————————————————————————————————
//  中序遍历辅助（递归）
// ————————————————————————————————————————————————————————————————————————————————

template<typename T>
void BST<T>::inorderHelper(Node* node, std::vector<T>& result) const {
    if (node == nullptr) {
        return;
    }
    inorderHelper(node->left, result);   // 左
    result.push_back(node->data);         // 根
    inorderHelper(node->right, result);   // 右
}

// ————————————————————————————————————————————————————————————————————————————————
//  销毁整棵树（递归后序释放，避免访问已释放内存）
// ————————————————————————————————————————————————————————————————————————————————

template<typename T>
void BST<T>::destroyTree(Node* node) {
    if (node == nullptr) {
        return;
    }
    destroyTree(node->left);   // 先销毁左子树
    destroyTree(node->right);  // 再销毁右子树
    delete node;               // 最后销毁当前节点
}
