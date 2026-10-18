/**
 * @file    BookRepository.h
 * @brief   图书数据仓库 —— 管理全部图书的内存存储，提供增删改查接口
 *
 * 核心数据结构：
 *   - vector<Book> books     : 图书主存储（顺序遍历、快速迭代、排序）
 *   - BST<Book>    bookIndex : 图书编号索引（O(log n) 按 ID 查找）
 *
 * 职责边界：
 *   - 只负责内存数据的增删改查和数据结构维护
 *   - 不包含任何业务规则判断
 *   - 不进行文件 IO（由 FileIO 处理）
 *
 * 同步要求：
 *   - add/remove 操作必须同步维护 vector 和 BST 的一致性
 */

#ifndef BOOK_REPOSITORY_H
#define BOOK_REPOSITORY_H

#include <string>
#include <vector>
#include "../models/Book.h"
#include "../utils/BST.h"

class BookRepository {
public:
    // ========================================================================
    //  构造函数
    // ========================================================================

    BookRepository();

    // ========================================================================
    //  增删改查接口
    // ========================================================================

    /**
     * @brief 新增图书 —— 同步添加到 vector 和 BST 索引
     * @param  book 图书对象
     * @return 成功返回 true，重复 bookId 返回 false
     */
    bool add(const Book& book);

    /**
     * @brief 删除图书 —— 同步从 vector 和 BST 索引中移除
     * @param  bookId 图书编号
     * @return 成功返回 true，不存在返回 false
     */
    bool remove(const std::string& bookId);

    /**
     * @brief 按编号查找图书（通过 BST 索引 O(log n) 查找）
     * @param  bookId 图书编号
     * @return 找到返回 Book 指针，未找到返回 nullptr
     */
    Book* findById(const std::string& bookId);

    /**
     * @brief 获取全部图书的引用（只读遍历或通过 update 修改）
     * @return vector<Book> 引用
     */
    std::vector<Book>& getAll();

    /**
     * @brief 更新图书信息 —— 同时更新 vector 和 BST 中的对应元素
     * @param  book 更新后的图书对象（以 bookId 定位）
     * @return 成功返回 true，不存在返回 false
     */
    bool update(const Book& book);

    /** @brief 检查图书是否存在 */
    bool exists(const std::string& bookId);

private:
    // ========================================================================
    //  内部辅助方法
    // ========================================================================

    /** @brief 在 vector 中按 bookId 线性查找，返回下标 */
    int findIndexInVector(const std::string& bookId) const;

    /** @brief 同步 BST 索引：从 BST 中移除旧 bookId 并插入新编码 */
    void syncBST(const Book& oldBook, const Book& newBook);

    // ========================================================================
    //  成员变量
    // ========================================================================

    std::vector<Book> books;      // 图书主存储（有序遍历、快速排序）
    BST<Book>         bookIndex;  // 图书编号索引（O(log n) 按 ID 查找）
};

#endif // BOOK_REPOSITORY_H
