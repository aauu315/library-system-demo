/**
 * @file    LoanManager.h
 * @brief   借阅管理业务逻辑 —— 借书、还书、续借、罚款计算、自动封禁
 *
 * 职责：
 *   - 借书：校验读者与图书状态 → 创建借阅记录 → 更新库存与借阅数
 *   - 还书：查找活跃记录 → 计算超期罚款 → 标记归还 → 更新库存与借阅数
 *   - 续借：校验续借条件 → 延长到期日期
 *   - 自动封禁：罚款总额达上限时自动封禁读者账号
 *
 * 依赖：
 *   - BookRepository   的引用
 *   - ReaderRepository 的引用
 *   - LoanRepository   的引用
 *
 * 业务规则：
 *   - 借阅期限 60 天
 *   - 距离到期日 <= 15 天可续借，每本书限续借一次，续借延期 60 天
 *   - 超期每天罚款 1 元，封顶 10 元
 *   - 累计罚款达 10 元自动封禁读者账号
 *   - 学生最多借 3 本，教师最多借 5 本
 */

#ifndef LOAN_MANAGER_H
#define LOAN_MANAGER_H

#include <string>
#include "../repositories/BookRepository.h"
#include "../repositories/ReaderRepository.h"
#include "../repositories/LoanRepository.h"

class LoanManager {
public:
    // ========================================================================
    //  业务常量
    // ========================================================================

    static constexpr int    BORROW_DAYS        = 60;    // 借阅期限（天）
    static constexpr int    RENEW_DAYS         = 60;    // 续借延期（天）
    static constexpr int    RENEW_MIN_DAYS     = 15;    // 距到期日多少天内可续借
    static constexpr double FINE_PER_DAY       = 1.0;   // 每天罚款金额（元）
    static constexpr double MAX_FINE           = 10.0;  // 最高罚款金额（元）
    static constexpr int    MAX_BORROW_STUDENT = 3;     // 学生最大借阅数
    static constexpr int    MAX_BORROW_TEACHER = 5;     // 教师最大借阅数

    // ========================================================================
    //  构造函数
    // ========================================================================

    /** @brief 构造函数 —— 绑定三个数据仓库引用 */
    LoanManager(BookRepository& bookRepo,
                ReaderRepository& readerRepo,
                LoanRepository& loanRepo);

    // ========================================================================
    //  借书
    // ========================================================================

    /**
     * @brief 借书
     * @details 依次调用各步骤函数，任意步骤失败即终止
     * @param  bookId 图书编号
     * @param  cardId 借书证号
     * @return 成功返回 true，失败可通过 getLastError() 获取原因
     */
    bool borrowBook(const std::string& bookId, const std::string& cardId);

    // ========================================================================
    //  还书
    // ========================================================================

    /**
     * @brief 还书
     * @details 依次调用各步骤函数，任意步骤失败即终止
     * @param  bookId 图书编号
     * @param  cardId 借书证号
     * @return 成功返回 true，失败可通过 getLastError() 获取原因
     */
    bool returnBook(const std::string& bookId, const std::string& cardId);

    // ========================================================================
    //  续借
    // ========================================================================

    /**
     * @brief 续借
     * @details 依次调用各步骤函数，任意步骤失败即终止
     * @param  bookId 图书编号
     * @param  cardId 借书证号
     * @return 成功返回 true，失败可通过 getLastError() 获取原因
     */
    bool renewBook(const std::string& bookId, const std::string& cardId);

    // ========================================================================
    //  罚款查询
    // ========================================================================

    /** @brief 获取某读者的累计罚款总额 */
    double getTotalFine(const std::string& cardId) const;

    // ========================================================================
    //  错误信息
    // ========================================================================

    /** @brief 获取最近一次操作的错误信息 */
    std::string getLastError() const;

private:
    // ========================================================================
    //  借书 —— 步骤函数（每个函数只做一件事）
    // ========================================================================

    /** @brief 检查读者是否存在，返回 Reader 指针 */
    Reader* checkReaderExists(const std::string& cardId);

    /** @brief 检查读者是否被封禁 */
    bool checkReaderNotBanned(const Reader* reader);

    /** @brief 检查读者借阅数量是否已达上限 */
    bool checkBorrowLimit(const Reader* reader);

    /** @brief 检查图书是否存在，返回 Book 指针 */
    Book* checkBookExists(const std::string& bookId);

    /** @brief 检查图书当前库存是否 > 0 */
    bool checkBookStock(const Book* book);

    /** @brief 创建 Loan 并添加到 LoanRepository */
    void createLoanRecord(const std::string& bookId, const std::string& cardId);

    /** @brief 库存 -1 */
    void decrementBookStock(Book* book);

    /** @brief 借阅次数 +1 */
    void incrementBookBorrowCount(Book* book);

    /** @brief 读者借阅数量 +1 */
    void incrementReaderBorrowCount(Reader* reader);

    // ========================================================================
    //  还书 —— 步骤函数
    // ========================================================================

    /** @brief 查找指定的活跃借阅记录，返回 Loan 指针 */
    Loan* findActiveLoanRecord(const std::string& bookId, const std::string& cardId);

    /** @brief 库存 +1 */
    void restoreBookStock(Book* book);

    /** @brief 读者借阅数量 -1 */
    void decrementReaderBorrowCount(Reader* reader);

    /** @brief 将借阅记录标记为已归还并设置罚款 */
    void markLoanReturned(Loan* loan, double fine);

    // ========================================================================
    //  续借 —— 步骤函数
    // ========================================================================

    /** @brief 检查是否已续借过 */
    bool checkNotAlreadyRenewed(const Loan* loan);

    /** @brief 检查当前日期距离到期日是否 <= 15 天 */
    bool checkWithinRenewWindow(const Loan* loan);

    /** @brief 到期日期延长 60 天 */
    void extendDueDate(Loan* loan);

    /** @brief 标记为已续借 */
    void markLoanRenewed(Loan* loan);

    // ========================================================================
    //  罚款与封禁
    // ========================================================================

    /** @brief 计算超期罚款 = min(超期天数 × 1.0, 10.0) */
    double calculateFine(const std::string& returnDate,
                         const std::string& dueDate) const;

    /** @brief 检查读者累计罚款是否 >= 10 元，是则自动封禁 */
    void checkAndAutoBan(const std::string& cardId);

    // ========================================================================
    //  辅助
    // ========================================================================

    /** @brief 设置错误信息 */
    void setError(const std::string& msg);

    /** @brief 根据读者身份返回最大借阅数量 */
    int getMaxBorrow(const Reader* reader) const;

    // ========================================================================
    //  成员变量
    // ========================================================================

    BookRepository&   bookRepo;    // 图书数据仓库引用
    ReaderRepository& readerRepo;  // 读者数据仓库引用
    LoanRepository&   loanRepo;    // 借阅记录数据仓库引用
    std::string       lastError;   // 最近一次错误信息
};

#endif // LOAN_MANAGER_H
