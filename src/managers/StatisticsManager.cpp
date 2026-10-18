/**
 * @file    StatisticsManager.cpp
 * @brief   统计分析业务逻辑实现 —— 排行榜、库存统计、超期统计、当前借阅统计
 *
 * 设计原则：
 *   - 只读不写：所有方法不修改仓库中的数据
 *   - 复制排序：排行榜先复制再排序，原始数据不变
 *   - 单一职责：每个函数只负责一种统计
 */

#include "../../include/managers/StatisticsManager.h"
#include "../../include/utils/SortAlgorithms.h"
#include "../../include/utils/DateUtils.h"
#include <sstream>
#include <map>

// ============================================================================
//  构造函数
// ============================================================================

StatisticsManager::StatisticsManager(BookRepository& bookRepo,
                                     ReaderRepository& readerRepo,
                                     LoanRepository& loanRepo)
    : bookRepo(bookRepo)
    , readerRepo(readerRepo)
    , loanRepo(loanRepo)
{
}

// ============================================================================
//  借阅排行榜（全部）
// ============================================================================

std::vector<Book> StatisticsManager::getBorrowRanking() {
    // topN <= 0 表示返回全部
    return getBorrowRanking(0);
}

// ============================================================================
//  借阅排行榜（Top N）
// ============================================================================

std::vector<Book> StatisticsManager::getBorrowRanking(int topN) {
    // 步骤1：从仓库复制全部图书（不修改原始数据）
    std::vector<Book> books = bookRepo.getAll();

    // 步骤2：快速排序，按 borrowCount 降序
    SortAlgorithms::quickSort<Book>(
        books,
        [](const Book& a, const Book& b) {
            // 借阅次数多的排前面
            return a.borrowCount > b.borrowCount;
        }
    );

    // 步骤3：截取 Top N
    if (topN > 0 && topN < static_cast<int>(books.size())) {
        books.resize(topN);
    }

    return books;
}

// ============================================================================
//  库存统计
// ============================================================================

StatisticsManager::InventoryStats StatisticsManager::getInventoryStats() {
    InventoryStats stats = {0, 0, 0, 0};

    const std::vector<Book>& books = bookRepo.getAll();
    stats.totalTitles = static_cast<int>(books.size());

    for (const auto& book : books) {
        stats.totalCopies     += book.totalStock;
        stats.availableCopies += book.currentStock;
    }
    stats.borrowedCopies = stats.totalCopies - stats.availableCopies;

    return stats;
}

// ============================================================================
//  超期统计
// ============================================================================

std::vector<Loan*> StatisticsManager::getOverdueLoans() {
    std::vector<Loan*> overdueList;
    std::string today = DateUtils::getToday();

    // 遍历全部借阅记录，筛选未归还且已超期的
    std::list<Loan>& allLoans = loanRepo.getAll();
    for (auto& loan : allLoans) {
        if (loan.isReturned) {
            continue;
        }
        if (DateUtils::isAfter(today, loan.dueDate)) {
            overdueList.push_back(&loan);
        }
    }

    return overdueList;
}

// ============================================================================
//  当前借阅统计（格式化文本报告）
// ============================================================================

std::string StatisticsManager::getCurrentLoanStats() {
    std::ostringstream report;

    int activeCount = 0;                           // 活跃借阅总数
    std::map<std::string, int> perReader;          // 各读者借阅数
    std::map<std::string, int> perBook;            // 各图书借出数

    // 遍历全部借阅记录，统计活跃（未归还）记录
    const std::list<Loan>& allLoans = loanRepo.getAll();
    for (const auto& loan : allLoans) {
        if (loan.isReturned) {
            continue;
        }
        activeCount++;
        perReader[loan.cardId]++;
        perBook[loan.bookId]++;
    }

    // ———— 构建报告 ————

    report << "═══════════════════════════════════════════════\n";
    report << "           当前借阅统计报告\n";
    report << "═══════════════════════════════════════════════\n\n";

    // 活跃借阅总数
    report << "▶ 活跃借阅总数：" << activeCount << " 条\n\n";

    // 各读者当前借阅数
    report << "▶ 各读者当前借阅数量：\n";
    if (perReader.empty()) {
        report << "  （无）\n";
    } else {
        for (const auto& item : perReader) {
            Reader* reader = readerRepo.findById(item.first);
            std::string name = (reader != nullptr) ? reader->name : "未知";
            report << "  " << item.first << " (" << name << ")"
                   << "：" << item.second << " 本\n";
        }
    }
    report << "\n";

    // 各图书当前借出数
    report << "▶ 各图书当前借出数量：\n";
    if (perBook.empty()) {
        report << "  （无）\n";
    } else {
        for (const auto& item : perBook) {
            Book* book = bookRepo.findById(item.first);
            std::string title = (book != nullptr) ? book->title : "未知";
            report << "  " << item.first << " (" << title << ")"
                   << "：" << item.second << " 本\n";
        }
    }
    report << "\n";

    report << "═══════════════════════════════════════════════\n";

    return report.str();
}
