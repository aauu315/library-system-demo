/**
 * @file    StatisticsManager.h
 * @brief   统计分析业务逻辑 —— 排行榜、库存统计、超期统计、当前借阅统计
 *
 * 职责：
 *   - 借阅排行榜（快速排序按 borrowCount 降序，支持 Top N）
 *   - 库存统计（总书目数、总册数、在库册数、借出册数）
 *   - 超期统计（未归还且已超期的借阅记录）
 *   - 当前借阅统计（活跃借阅总数、各读者/各图书借出数）
 *
 * 依赖：
 *   - BookRepository   的引用（只读）
 *   - ReaderRepository 的引用（只读）
 *   - LoanRepository   的引用（只读）
 *
 * 注意：
 *   - 本类只读访问数据，不修改任何数据
 *   - 排序使用 SortAlgorithms::quickSort
 *   - 排序前复制数据，不修改仓库原始数据
 */

#ifndef STATISTICS_MANAGER_H
#define STATISTICS_MANAGER_H

#include <string>
#include <vector>
#include "../models/Book.h"
#include "../models/Loan.h"
#include "../repositories/BookRepository.h"
#include "../repositories/ReaderRepository.h"
#include "../repositories/LoanRepository.h"

class StatisticsManager {
public:
    // ========================================================================
    //  统计结果结构体
    // ========================================================================

    /** @brief 库存统计结果 */
    struct InventoryStats {
        int totalTitles;       // 总书目数
        int totalCopies;       // 总册数（totalStock 之和）
        int availableCopies;   // 在库册数（currentStock 之和）
        int borrowedCopies;    // 借出册数（totalStock - currentStock 之和）
    };

    // ========================================================================
    //  构造函数
    // ========================================================================

    StatisticsManager(BookRepository& bookRepo,
                      ReaderRepository& readerRepo,
                      LoanRepository& loanRepo);

    // ========================================================================
    //  借阅排行榜
    // ========================================================================

    /** @brief 借阅排行榜（全部） —— 复制数据 → 快速排序降序 → 返回完整列表 */
    std::vector<Book> getBorrowRanking();

    /** @brief 借阅排行榜（Top N） —— 同上 + 截取前 topN（<=0 表示全部） */
    std::vector<Book> getBorrowRanking(int topN);

    // ========================================================================
    //  库存统计
    // ========================================================================

    /** @brief 库存统计 —— 遍历全部图书汇总 totalStock / currentStock */
    InventoryStats getInventoryStats();

    // ========================================================================
    //  超期统计
    // ========================================================================

    /** @brief 超期统计 —— 筛选未归还 且 当前日期 > dueDate 的记录 */
    std::vector<Loan*> getOverdueLoans();

    // ========================================================================
    //  当前借阅统计
    // ========================================================================

    /** @brief 当前借阅统计 —— 活跃借阅总数 + 各读者/各图书借出数（格式化文本） */
    std::string getCurrentLoanStats();

private:
    BookRepository&   bookRepo;
    ReaderRepository& readerRepo;
    LoanRepository&   loanRepo;
};

#endif // STATISTICS_MANAGER_H
