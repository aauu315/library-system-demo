/**
 * @file    BookManager.cpp
 * @brief   图书管理业务逻辑实现 —— 增删改查 + 库存管理
 *
 * 每个方法职责单一：
 *   - 调用方传入参数
 *   - 执行业务校验
 *   - 通过 BookRepository 操作数据
 *   - 返回成功/失败
 *
 * 不涉及 Reader、Loan，只操作 BookRepository。
 */

#include "../../include/managers/BookManager.h"
#include <algorithm>  // std::transform
#include <cctype>     // std::tolower

// ============================================================================
//  构造函数
// ============================================================================

/** @brief 构造函数 —— 绑定图书数据仓库引用 */
BookManager::BookManager(BookRepository& bookRepo)
    : bookRepo(bookRepo)
{
}

// ============================================================================
//  错误信息
// ============================================================================

/** @brief 获取最近一次操作的错误信息 */
std::string BookManager::getLastError() const {
    return lastError;
}

/** @brief 设置错误信息（内部辅助） */
void BookManager::setError(const std::string& msg) {
    lastError = msg;
}

// ============================================================================
//  业务校验
// ============================================================================

/**
 * @brief 校验 Book 数据的合法性
 * @details 校验规则：
 *          - bookId、title、author 不能为空
 *          - category 只允许 "书籍" / "期刊" / "报刊"
 *          - totalStock >= 0
 *          - currentStock >= 0 且 <= totalStock
 *          - borrowCount >= 0
 * @param  book 待校验的图书对象
 * @return 合法返回 true
 */
bool BookManager::validateBook(const Book& book) {
    // 图书编号不能为空
    if (book.bookId.empty()) {
        setError("图书编号不能为空");
        return false;
    }

    // 书名不能为空
    if (book.title.empty()) {
        setError("书名不能为空");
        return false;
    }

    // 作者不能为空
    if (book.author.empty()) {
        setError("作者不能为空");
        return false;
    }

    // 类别只允许三种
    if (book.category != "书籍" &&
        book.category != "期刊" &&
        book.category != "报刊") {
        setError("图书类别必须为：书籍 / 期刊 / 报刊");
        return false;
    }

    // 总库存不能为负数
    if (book.totalStock < 0) {
        setError("总库存不能为负数");
        return false;
    }

    // 当前库存 >= 0 且 <= 总库存
    if (book.currentStock < 0) {
        setError("当前库存不能为负数");
        return false;
    }
    if (book.currentStock > book.totalStock) {
        setError("当前库存不能超过总库存");
        return false;
    }

    // 借阅次数 >= 0
    if (book.borrowCount < 0) {
        setError("借阅次数不能为负数");
        return false;
    }

    return true;
}

// ============================================================================
//  新增图书
// ============================================================================

/**
 * @brief 新增图书
 * @details 1. 校验数据合法性
 *          2. 检查 bookId 是否已存在
 *          3. 调用 BookRepository.add()
 */
bool BookManager::addBook(const Book& book) {
    // 步骤1：校验数据
    if (!validateBook(book)) {
        // validateBook 内部已设置 lastError
        return false;
    }

    // 步骤2：检查图书编号是否重复
    if (bookRepo.exists(book.bookId)) {
        setError("图书编号 [" + book.bookId + "] 已存在，不能重复添加");
        return false;
    }

    // 步骤3：添加到数据仓库
    if (!bookRepo.add(book)) {
        setError("添加图书失败");
        return false;
    }

    return true;
}

// ============================================================================
//  删除图书
// ============================================================================

/**
 * @brief 删除图书
 * @details 1. 检查图书是否存在
 *          2. 调用 BookRepository.remove()
 */
bool BookManager::removeBook(const std::string& bookId) {
    // 步骤1：检查图书是否存在
    if (!bookRepo.exists(bookId)) {
        setError("图书编号 [" + bookId + "] 不存在");
        return false;
    }

    // 步骤2：从数据仓库删除
    if (!bookRepo.remove(bookId)) {
        setError("删除图书失败");
        return false;
    }

    return true;
}

// ============================================================================
//  修改图书
// ============================================================================

/**
 * @brief 修改图书信息
 * @details 1. 校验新数据合法性
 *          2. 检查原图书是否存在
 *          3. 确保 bookId 不变（如果需要修改编号，需额外检查）
 *          4. 调用 BookRepository.update()
 */
bool BookManager::updateBook(const std::string& bookId, const Book& newInfo) {
    // 步骤1：校验新数据
    if (!validateBook(newInfo)) {
        return false;
    }

    // 步骤2：检查原图书是否存在
    if (!bookRepo.exists(bookId)) {
        setError("图书编号 [" + bookId + "] 不存在");
        return false;
    }

    // 步骤3：如果修改了 bookId，检查新编号是否与已有图书冲突
    if (bookId != newInfo.bookId && bookRepo.exists(newInfo.bookId)) {
        setError("新图书编号 [" + newInfo.bookId + "] 已存在");
        return false;
    }

    // 步骤4：更新数据仓库
    if (!bookRepo.update(newInfo)) {
        setError("修改图书信息失败");
        return false;
    }

    return true;
}

// ============================================================================
//  查询图书
// ============================================================================

/**
 * @brief 按编号精确查找图书
 * @details 通过 BookRepository.findById() 查找
 */
Book* BookManager::findBook(const std::string& bookId) {
    Book* book = bookRepo.findById(bookId);
    if (book == nullptr) {
        setError("图书编号 [" + bookId + "] 不存在");
    }
    return book;
}

/**
 * @brief 获取全部图书列表
 * @details 返回 BookRepository 中全部图书的副本
 */
std::vector<Book> BookManager::listAllBooks() const {
    // 返回副本，避免外部直接修改仓库数据
    return bookRepo.getAll();
}

/**
 * @brief 按关键字模糊搜索图书
 * @details 对书名和作者字段进行不区分大小写的子串匹配
 * @param  keyword 搜索关键字
 * @return 匹配的图书列表（可能为空）
 */
std::vector<Book> BookManager::searchBooks(const std::string& keyword) const {
    std::vector<Book> result;

    // 空关键字返回空结果
    if (keyword.empty()) {
        return result;
    }

    // 将关键字转为小写，用于不区分大小写的比较
    std::string lowerKeyword = keyword;
    std::transform(lowerKeyword.begin(), lowerKeyword.end(),
                   lowerKeyword.begin(), ::tolower);

    // 遍历全部图书，匹配书名或作者
    const std::vector<Book>& allBooks = bookRepo.getAll();
    for (const auto& book : allBooks) {
        // 书名转小写
        std::string lowerTitle = book.title;
        std::transform(lowerTitle.begin(), lowerTitle.end(),
                       lowerTitle.begin(), ::tolower);

        // 作者转小写
        std::string lowerAuthor = book.author;
        std::transform(lowerAuthor.begin(), lowerAuthor.end(),
                       lowerAuthor.begin(), ::tolower);

        // 子串匹配
        if (lowerTitle.find(lowerKeyword) != std::string::npos ||
            lowerAuthor.find(lowerKeyword) != std::string::npos) {
            result.push_back(book);
        }
    }

    if (result.empty()) {
        // 注意：这里不设置 lastError，因为 searchBooks 是 const 方法，
        // 搜索无结果不是错误，调用方通过返回空 vector 自行判断
    }

    return result;
}

// ============================================================================
//  库存增加（入库）
// ============================================================================

/**
 * @brief 入库 —— 增加图书的总库存和当前库存
 * @details 1. 检查图书是否存在
 *          2. 检查入库数量 > 0
 *          3. 直接修改 BST 节点数据（通过 findById 返回的指针，BST 自动同步）
 *          4. 同步更新 vector 中的数据
 */
bool BookManager::increaseStock(const std::string& bookId, int count) {
    // 步骤1：检查图书是否存在（返回 BST 节点数据指针）
    Book* book = bookRepo.findById(bookId);
    if (book == nullptr) {
        setError("图书编号 [" + bookId + "] 不存在");
        return false;
    }

    // 步骤2：检查数量合法性
    if (count <= 0) {
        setError("入库数量必须大于 0");
        return false;
    }

    // 步骤3：更新 BST 节点数据（直接通过 findById 返回的指针原地修改）
    book->totalStock   += count;
    book->currentStock += count;

    // 步骤4：同步更新 vector 中的数据
    // 注意：不调用 bookRepo.update()，因为 update() 内部的 syncBST()
    // 会执行 BST::remove() + BST::insert()，其中 remove() 的 Case 3
    //（successor 拷贝替换）会破坏已被原地修改的 BST 节点数据。
    // BST 已通过步骤3的指针原地修改保持最新，此处只同步 vector。
    std::vector<Book>& allBooks = bookRepo.getAll();
    int savedTotal   = book->totalStock;
    int savedCurrent = book->currentStock;
    bool vectorUpdated = false;
    for (auto& b : allBooks) {
        if (b.bookId == bookId) {
            b.totalStock   += count;
            b.currentStock += count;
            vectorUpdated = true;
            break;
        }
    }
    if (!vectorUpdated) {
        // vector 与 BST 不一致（不应发生），回滚 BST 修改
        book->totalStock   = savedTotal   - count;
        book->currentStock = savedCurrent - count;
        setError("入库操作失败：内部数据不一致");
        return false;
    }

    return true;
}

// ============================================================================
//  库存减少（出库）
// ============================================================================

/**
 * @brief 出库 —— 减少图书的总库存和当前库存
 * @details 1. 检查图书是否存在
 *          2. 检查出库数量 > 0
 *          3. 检查库存充足（currentStock >= count 且 totalStock >= count）
 *          4. 直接修改 BST 节点数据（通过 findById 返回的指针）
 *          5. 同步更新 vector 中的数据
 */
bool BookManager::decreaseStock(const std::string& bookId, int count) {
    // 步骤1：检查图书是否存在
    Book* book = bookRepo.findById(bookId);
    if (book == nullptr) {
        setError("图书编号 [" + bookId + "] 不存在");
        return false;
    }

    // 步骤2：检查数量合法性
    if (count <= 0) {
        setError("出库数量必须大于 0");
        return false;
    }

    // 步骤3：检查库存是否充足
    if (book->totalStock < count) {
        setError("总库存不足（当前总库存：" +
                 std::to_string(book->totalStock) +
                 "，需要：" + std::to_string(count) + "）");
        return false;
    }
    if (book->currentStock < count) {
        setError("当前库存不足（当前可借：" +
                 std::to_string(book->currentStock) +
                 "，需要：" + std::to_string(count) + "）");
        return false;
    }

    // 步骤4：更新 BST 节点数据（直接原地修改）
    book->totalStock   -= count;
    book->currentStock -= count;

    // 步骤5：同步更新 vector 中的数据（不调用 bookRepo.update()，原因同 increaseStock）
    std::vector<Book>& allBooks = bookRepo.getAll();
    int savedTotal   = book->totalStock;
    int savedCurrent = book->currentStock;
    bool vectorUpdated = false;
    for (auto& b : allBooks) {
        if (b.bookId == bookId) {
            b.totalStock   -= count;
            b.currentStock -= count;
            vectorUpdated = true;
            break;
        }
    }
    if (!vectorUpdated) {
        // vector 与 BST 不一致（不应发生），回滚 BST 修改
        book->totalStock   = savedTotal   + count;
        book->currentStock = savedCurrent + count;
        setError("出库操作失败：内部数据不一致");
        return false;
    }

    return true;
}
