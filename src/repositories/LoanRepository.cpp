/**
 * @file    LoanRepository.cpp
 * @brief   借阅记录数据仓库实现 —— list 存储，遍历查找
 */

#include "../../include/repositories/LoanRepository.h"
#include "../../include/utils/DateUtils.h"

// ============================================================================
//  构造函数
// ============================================================================

LoanRepository::LoanRepository() {
}

// ============================================================================
//  新增
// ============================================================================

bool LoanRepository::add(const Loan& loan) {
    loans.push_back(loan);
    return true;
}

// ============================================================================
//  删除（按 bookId + cardId 匹配第一条）
// ============================================================================

bool LoanRepository::remove(const Loan& loan) {
    for (auto it = loans.begin(); it != loans.end(); ++it) {
        if (it->bookId == loan.bookId && it->cardId == loan.cardId) {
            loans.erase(it);
            return true;
        }
    }
    return false;
}

// ============================================================================
//  获取全部
// ============================================================================

std::list<Loan>& LoanRepository::getAll() {
    return loans;
}

// ============================================================================
//  查找活跃借阅记录
// ============================================================================

Loan* LoanRepository::findActive(const std::string& bookId,
                                  const std::string& cardId) {
    for (auto& loan : loans) {
        if (loan.bookId == bookId &&
            loan.cardId == cardId &&
            !loan.isReturned) {
            return &loan;
        }
    }
    return nullptr;
}

// ============================================================================
//  获取某读者的全部借阅记录
// ============================================================================

std::vector<Loan*> LoanRepository::findByReaderId(const std::string& cardId) {
    std::vector<Loan*> result;
    for (auto& loan : loans) {
        if (loan.cardId == cardId) {
            result.push_back(&loan);
        }
    }
    return result;
}

// ============================================================================
//  获取某读者的活跃借阅记录
// ============================================================================

std::vector<Loan*> LoanRepository::findActiveByReaderId(const std::string& cardId) {
    std::vector<Loan*> result;
    for (auto& loan : loans) {
        if (loan.cardId == cardId && !loan.isReturned) {
            result.push_back(&loan);
        }
    }
    return result;
}

// ============================================================================
//  获取某图书的全部借阅记录
// ============================================================================

std::vector<Loan*> LoanRepository::findByBookId(const std::string& bookId) {
    std::vector<Loan*> result;
    for (auto& loan : loans) {
        if (loan.bookId == bookId) {
            result.push_back(&loan);
        }
    }
    return result;
}

// ============================================================================
//  全部超期记录（未归还且到期日 < 今天）
// ============================================================================

std::vector<Loan*> LoanRepository::findOverdue() {
    std::vector<Loan*> result;
    std::string today = DateUtils::getToday();
    for (auto& loan : loans) {
        if (!loan.isReturned && DateUtils::isAfter(today, loan.dueDate)) {
            result.push_back(&loan);
        }
    }
    return result;
}

// ============================================================================
//  统计某读者的活跃借阅数
// ============================================================================

int LoanRepository::countActiveByReaderId(const std::string& cardId) {
    int count = 0;
    for (const auto& loan : loans) {
        if (loan.cardId == cardId && !loan.isReturned) {
            count++;
        }
    }
    return count;
}

// ============================================================================
//  计算某读者的累计罚款总额
// ============================================================================

double LoanRepository::getTotalFineByReaderId(const std::string& cardId) {
    double total = 0.0;
    for (const auto& loan : loans) {
        if (loan.cardId == cardId) {
            total += loan.fine;
        }
    }
    return total;
}
