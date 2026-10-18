/**
 * @file    BookManager.h
 * @brief   图书管理业务逻辑 —— 图书 CRUD 与库存管理
 *
 * 职责：
 *   - 校验图书数据的合法性
 *   - 封装图书增删改查的完整业务流程
 *   - 库存入库/出库管理
 *
 * 依赖：
 *   - BookRepository 的引用（仅操作图书容器）
 *
 * 禁止：
 *   - 涉及 Reader、Loan 相关操作
 *   - 直接操作 BookRepository 的内部数据结构
 *   - 包含 UI 相关代码
 */

#ifndef BOOK_MANAGER_H
#define BOOK_MANAGER_H

#include <string>
#include <vector>
#include "../models/Book.h"
#include "../repositories/BookRepository.h"

class BookManager {
public:
    // ========================================================================
    //  构造函数
    // ========================================================================

    /** @brief 构造函数 —— 绑定图书数据仓库 */
    explicit BookManager(BookRepository& bookRepo);

    // ========================================================================
    //  新增图书
    // ========================================================================

    /**
     * @brief 新增图书
     * @details 校验数据合法性 → 检查 bookId 不重复 → 调用 repo.add()
     * @param  book 图书对象
     * @return 成功返回 true，失败时可通过 getLastError() 获取原因
     */
    bool addBook(const Book& book);

    // ========================================================================
    //  删除图书
    // ========================================================================

    /**
     * @brief 删除图书
     * @details 检查图书是否存在 → 调用 repo.remove()
     * @param  bookId 图书编号
     * @return 成功返回 true
     */
    bool removeBook(const std::string& bookId);

    // ========================================================================
    //  修改图书
    // ========================================================================

    /**
     * @brief 修改图书信息
     * @details 校验新数据 → 检查原图书存在 → 调用 repo.update()
     * @param  bookId  要修改的图书编号
     * @param  newInfo 新的图书信息
     * @return 成功返回 true
     */
    bool updateBook(const std::string& bookId, const Book& newInfo);

    // ========================================================================
    //  查询图书
    // ========================================================================

    /**
     * @brief 按编号精确查找图书
     * @param  bookId 图书编号
     * @return 找到返回 Book 指针，未找到返回 nullptr
     */
    Book* findBook(const std::string& bookId);

    /**
     * @brief 获取全部图书列表
     * @return 全部图书的 vector 副本
     */
    std::vector<Book> listAllBooks() const;

    /**
     * @brief 按关键字模糊搜索图书（匹配书名或作者，不区分大小写）
     * @param  keyword 搜索关键字
     * @return 匹配的图书 vector
     */
    std::vector<Book> searchBooks(const std::string& keyword) const;

    // ========================================================================
    //  库存增加
    // ========================================================================

    /**
     * @brief 入库 —— 增加图书的总库存和当前库存
     * @details 检查图书存在 → 检查数量合法 → totalStock += n, currentStock += n
     * @param  bookId 图书编号
     * @param  count  入库数量（必须 > 0）
     * @return 成功返回 true
     */
    bool increaseStock(const std::string& bookId, int count);

    // ========================================================================
    //  库存减少
    // ========================================================================

    /**
     * @brief 出库 —— 减少图书的总库存和当前库存
     * @details 检查图书存在 → 检查数量合法 → 检查库存充足 → totalStock -= n, currentStock -= n
     * @param  bookId 图书编号
     * @param  count  出库数量（必须 > 0，且不能导致库存 < 0）
     * @return 成功返回 true
     */
    bool decreaseStock(const std::string& bookId, int count);

    // ========================================================================
    //  错误信息
    // ========================================================================

    /** @brief 获取最近一次操作的错误信息 */
    std::string getLastError() const;

private:
    // ========================================================================
    //  业务校验（每个方法职责单一）
    // ========================================================================

    /** @brief 校验 Book 数据的合法性（字段非空、库存合理性等） */
    bool validateBook(const Book& book);

    /** @brief 设置错误信息（内部辅助） */
    void setError(const std::string& msg);

    // ========================================================================
    //  成员变量
    // ========================================================================

    BookRepository& bookRepo;    // 图书数据仓库引用
    std::string     lastError;   // 最近一次错误信息
};

#endif // BOOK_MANAGER_H
