/**
 * @file    LoanManager.cpp
 * @brief   借阅管理业务逻辑实现 —— 借书、还书、续借、罚款计算、自动封禁
 *
 * 设计原则：
 *   - 每个步骤拆分为独立的私有函数，函数名即文档
 *   - 公开方法（borrowBook / returnBook / renewBook）仅做顺序编排
 *   - 每个函数只做一件事，不超过 10 行
 */

#include "../../include/managers/LoanManager.h"
#include "../../include/utils/DateUtils.h"
#include <algorithm>  // std::min

// odr-use 要求静态常量有外部定义（std::min 会取引用）
constexpr double LoanManager::MAX_FINE;

// ============================================================================
//  构造函数
// ============================================================================

LoanManager::LoanManager(BookRepository& bookRepo,
                         ReaderRepository& readerRepo,
                         LoanRepository& loanRepo)
    : bookRepo(bookRepo)
    , readerRepo(readerRepo)
    , loanRepo(loanRepo)
{
}

// ============================================================================
//  错误信息
// ============================================================================

std::string LoanManager::getLastError() const {
    return lastError;
}

void LoanManager::setError(const std::string& msg) {
    lastError = msg;
}

// ============================================================================
//  辅助：根据身份获取最大借阅数
// ============================================================================

int LoanManager::getMaxBorrow(const Reader* reader) const {
    if (reader->identity == "教师") {
        return MAX_BORROW_TEACHER;
    }
    return MAX_BORROW_STUDENT;
}

// ============================================================================
//  借书 —— 步骤函数
// ============================================================================

Reader* LoanManager::checkReaderExists(const std::string& cardId) {
    Reader* reader = readerRepo.findById(cardId);
    if (reader == nullptr) {
        setError("借书证号 [" + cardId + "] 不存在");
    }
    return reader;
}

bool LoanManager::checkReaderNotBanned(const Reader* reader) {
    if (reader->isBanned) {
        setError("读者 [" + reader->name + "] 已被封禁，无法借书");
        return false;
    }
    return true;
}

bool LoanManager::checkBorrowLimit(const Reader* reader) {
    int maxBorrow = getMaxBorrow(reader);
    int activeCount = loanRepo.countActiveByReaderId(reader->cardId);
    if (activeCount >= maxBorrow) {
        setError("借阅数量已达上限（" + reader->identity +
                 "最多借 " + std::to_string(maxBorrow) +
                 " 本，当前已借 " + std::to_string(activeCount) + " 本）");
        return false;
    }
    return true;
}

Book* LoanManager::checkBookExists(const std::string& bookId) {
    Book* book = bookRepo.findById(bookId);
    if (book == nullptr) {
        setError("图书编号 [" + bookId + "] 不存在");
    }
    return book;
}

bool LoanManager::checkBookStock(const Book* book) {
    if (book->currentStock <= 0) {
        setError("图书 [" + book->title + "] 库存不足（当前库存：" +
                 std::to_string(book->currentStock) + "）");
        return false;
    }
    return true;
}

void LoanManager::createLoanRecord(const std::string& bookId,
                                   const std::string& cardId) {
    std::string today = DateUtils::getToday();
    std::string dueDate = DateUtils::addDays(today, BORROW_DAYS);
    Loan loan(bookId, cardId, today, dueDate, false, false, 0.0);
    loanRepo.add(loan);
}

void LoanManager::decrementBookStock(Book* book) {
    book->currentStock--;
    // BST 已通过指针原地更新，此处只同步 vector
    // 不调用 bookRepo.update() 以避免 syncBST remove+insert 破坏 BST 结构
    for (auto& b : bookRepo.getAll()) {
        if (b.bookId == book->bookId) {
            b.currentStock--;
            break;
        }
    }
}

void LoanManager::incrementBookBorrowCount(Book* book) {
    book->borrowCount++;
    // BST 已通过指针原地更新，此处只同步 vector
    for (auto& b : bookRepo.getAll()) {
        if (b.bookId == book->bookId) {
            b.borrowCount++;
            break;
        }
    }
}

void LoanManager::incrementReaderBorrowCount(Reader* reader) {
    reader->currentBorrow++;
    readerRepo.update(*reader);
}

// ============================================================================
//  借书 —— 公开入口
// ============================================================================

bool LoanManager::borrowBook(const std::string& bookId, const std::string& cardId) {
    // 步骤1：检查读者是否存在
    Reader* reader = checkReaderExists(cardId);
    if (reader == nullptr) return false;

    // 步骤2：检查读者是否被封禁
    if (!checkReaderNotBanned(reader)) return false;

    // 步骤3：检查借阅数量是否已达上限
    if (!checkBorrowLimit(reader)) return false;

    // 步骤4：检查图书是否存在
    Book* book = checkBookExists(bookId);
    if (book == nullptr) return false;

    // 步骤5：检查图书库存
    if (!checkBookStock(book)) return false;

    // 步骤6：创建 Loan 记录
    createLoanRecord(bookId, cardId);

    // 步骤7：库存 -1
    decrementBookStock(book);

    // 步骤8：借阅次数 +1
    incrementBookBorrowCount(book);

    // 步骤9：读者借阅数 +1
    incrementReaderBorrowCount(reader);

    return true;
}

// ============================================================================
//  还书 —— 步骤函数
// ============================================================================

Loan* LoanManager::findActiveLoanRecord(const std::string& bookId,
                                        const std::string& cardId) {
    Loan* loan = loanRepo.findActive(bookId, cardId);
    if (loan == nullptr) {
        setError("未找到图书 [" + bookId + "] 和读者 [" + cardId + "] 的活跃借阅记录");
    }
    return loan;
}

void LoanManager::restoreBookStock(Book* book) {
    book->currentStock++;
    // BST 已通过指针原地更新，此处只同步 vector
    for (auto& b : bookRepo.getAll()) {
        if (b.bookId == book->bookId) {
            b.currentStock++;
            break;
        }
    }
}

void LoanManager::decrementReaderBorrowCount(Reader* reader) {
    reader->currentBorrow--;
    readerRepo.update(*reader);
}

void LoanManager::markLoanReturned(Loan* loan, double fine) {
    loan->isReturned = true;
    loan->fine = fine;
}

// ============================================================================
//  还书 —— 公开入口
// ============================================================================

bool LoanManager::returnBook(const std::string& bookId, const std::string& cardId) {
    // 步骤1：查找活跃借阅记录
    Loan* loan = findActiveLoanRecord(bookId, cardId);
    if (loan == nullptr) return false;

    // 步骤2：计算超期罚款
    std::string today = DateUtils::getToday();
    double fine = calculateFine(today, loan->dueDate);

    // 步骤3：标记为已归还
    markLoanReturned(loan, fine);

    // 步骤4：恢复库存
    Book* book = bookRepo.findById(bookId);
    if (book != nullptr) {
        restoreBookStock(book);
    }

    // 步骤5：减少读者借阅数量
    Reader* reader = readerRepo.findById(cardId);
    if (reader != nullptr) {
        decrementReaderBorrowCount(reader);
    }

    // 步骤6：检查是否需要自动封禁
    checkAndAutoBan(cardId);

    return true;
}

// ============================================================================
//  续借 —— 步骤函数
// ============================================================================

bool LoanManager::checkNotAlreadyRenewed(const Loan* loan) {
    if (loan->isRenewed) {
        setError("该借阅记录已经续借过一次，不能再次续借");
        return false;
    }
    return true;
}

bool LoanManager::checkWithinRenewWindow(const Loan* loan) {
    std::string today = DateUtils::getToday();
    int daysRemaining = DateUtils::daysBetween(today, loan->dueDate);
    // daysRemaining = dueDate - today，为负表示已超期
    if (daysRemaining < 0) {
        setError("该借阅记录已超期，不能续借");
        return false;
    }
    if (daysRemaining > RENEW_MIN_DAYS) {
        setError("距离到期日还有 " + std::to_string(daysRemaining) +
                 " 天，须在到期前 " + std::to_string(RENEW_MIN_DAYS) +
                 " 天内才能续借");
        return false;
    }
    return true;
}

void LoanManager::extendDueDate(Loan* loan) {
    loan->dueDate = DateUtils::addDays(loan->dueDate, RENEW_DAYS);
}

void LoanManager::markLoanRenewed(Loan* loan) {
    loan->isRenewed = true;
}

// ============================================================================
//  续借 —— 公开入口
// ============================================================================

bool LoanManager::renewBook(const std::string& bookId, const std::string& cardId) {
    // 步骤1：查找活跃借阅记录
    Loan* loan = findActiveLoanRecord(bookId, cardId);
    if (loan == nullptr) return false;

    // 步骤2：检查是否已续借过
    if (!checkNotAlreadyRenewed(loan)) return false;

    // 步骤3：检查是否在续借窗口内
    if (!checkWithinRenewWindow(loan)) return false;

    // 步骤4：延期 60 天
    extendDueDate(loan);

    // 步骤5：标记为已续借
    markLoanRenewed(loan);

    return true;
}

// ============================================================================
//  罚款计算
// ============================================================================

double LoanManager::calculateFine(const std::string& returnDate,
                                  const std::string& dueDate) const {
    // overdueDays = returnDate - dueDate，负值表示提前归还
    int overdueDays = DateUtils::daysBetween(dueDate, returnDate);
    if (overdueDays <= 0) {
        return 0.0;  // 未超期，无罚款
    }
    // 每天 1 元，最高 10 元
    double fine = static_cast<double>(overdueDays) * FINE_PER_DAY;
    return std::min(fine, MAX_FINE);
}

// ============================================================================
//  自动封禁
// ============================================================================

void LoanManager::checkAndAutoBan(const std::string& cardId) {
    // 获取读者累计罚款总额
    double totalFine = loanRepo.getTotalFineByReaderId(cardId);
    if (totalFine < MAX_FINE) {
        return;  // 未达封禁门槛
    }

    // 累计罚款 >= 10 元，自动封禁
    Reader* reader = readerRepo.findById(cardId);
    if (reader != nullptr && !reader->isBanned) {
        reader->isBanned = true;
        readerRepo.update(*reader);
    }
}

// ============================================================================
//  罚款查询
// ============================================================================

double LoanManager::getTotalFine(const std::string& cardId) const {
    return loanRepo.getTotalFineByReaderId(cardId);
}
