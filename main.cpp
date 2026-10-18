/**
 * @file    main.cpp
 * @brief   图书管理系统 —— 程序入口
 *
 * 职责（仅此三件事）：
 *   1. 初始化：创建 Repository → 加载数据 → 创建 Manager
 *   2. 菜单循环：显示菜单 → 获取输入 → 调用 Manager
 *   3. 退出保存：将数据写回 txt 文件
 *
 * 禁止：任何业务逻辑、数据校验、直接操作 Repository
 */

#include <iostream>
#include <iomanip>
#include <limits>
#include <clocale>
#ifdef _WIN32
#include <windows.h>
#endif
#include "include/utils/DataManager.h"
#include "include/repositories/BookRepository.h"
#include "include/repositories/ReaderRepository.h"
#include "include/repositories/LoanRepository.h"
#include "include/managers/BookManager.h"
#include "include/managers/ReaderManager.h"
#include "include/managers/LoanManager.h"
#include "include/managers/StatisticsManager.h"
#include "include/models/Reservation.h"
#include "include/repositories/ReservationRepository.h"
#include "include/managers/ReservationManager.h"

// ============================================================================
//  前向声明 —— 各模块子菜单
// ============================================================================
void menuBookManagement(BookManager& bookMgr);
void menuReaderManagement(ReaderManager& readerMgr);
void menuLoanManagement(LoanManager& loanMgr, ReservationManager& reservationMgr);
void menuStatistics(StatisticsManager& statsMgr);

// ============================================================================
//  输入辅助
// ============================================================================

/** @brief 读取一个整数，输入非法时重新提示（使用 getline + stoi，避免 cin>> + ignore 的 unget 失败问题） */
static int readInt(const char* prompt) {
    while (true) {
        std::cout << prompt;
        std::string line;
        if (!std::getline(std::cin, line)) {
            if (std::cin.eof()) {
                std::cin.clear();
                std::cout << std::endl << "检测到输入流结束，程序退出" << std::endl;
                exit(0);
            }
            std::cin.clear();
            std::cout << "输入无效，请重新输入" << std::endl;
            continue;
        }
        if (line.empty()) {
            std::cout << "输入无效，请重新输入" << std::endl;
            continue;
        }
        try {
            std::size_t pos = 0;
            int value = std::stoi(line, &pos);
            // 检查尾部是否只有空白字符
            bool trailingGarbage = false;
            for (std::size_t i = pos; i < line.size(); ++i) {
                if (line[i] != ' ' && line[i] != '\t' && line[i] != '\r') {
                    trailingGarbage = true;
                    break;
                }
            }
            if (trailingGarbage) {
                std::cout << "输入无效，请重新输入" << std::endl;
            } else {
                return value;
            }
        } catch (...) {
            std::cout << "输入无效，请重新输入" << std::endl;
        }
    }
}

/** @brief 暂停等待用户按回车（使用 getline 替代 cin.get()，避免 unget 失败导致的遗留问题） */
static void pause() {
    std::cout << std::endl << "按回车键继续...";
    std::string dummy;
    std::getline(std::cin, dummy);
}

/** @brief 清屏（输出足够多的空行使旧内容滚出视野） */
static void clearScreen() {
    for (int i = 0; i < 40; ++i) {
        std::cout << std::endl;
    }
}

// ============================================================================
//  主函数
// ============================================================================

int main() {
    // ———— 解决中文乱码 ————
    // 步骤1：先设置 CRT locale 为 ".UTF-8"（避免 MinGW 产生 nul 文件）
    // 步骤2：再通过 Windows API 设置控制台 code page 为 UTF-8
    // 关键：必须先 setlocale 再 SetConsoleOutputCP，反序或不调 setlocale 都会产生 nul 文件
#ifdef _WIN32
    setlocale(LC_ALL, ".UTF-8");
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#else
    // 非 Windows 平台切换 C 运行时 locale
    setlocale(LC_ALL, ".UTF-8");
#endif

    // ================================================================
    //  第1部分：初始化
    // ================================================================

    // 1.1 创建数据管理器并加载 txt 文件
    DataManager dataMgr("data/");
    std::vector<Book>   loadedBooks   = dataMgr.loadBooks();
    std::vector<Reader> loadedReaders = dataMgr.loadReaders();
    std::list<Loan>     loadedLoans   = dataMgr.loadLoans();
    std::vector<Reservation> loadedReservations = dataMgr.loadReservations();

    // 1.2 创建仓库并填充数据
    BookRepository   bookRepo;
    for (const auto& b : loadedBooks)   { bookRepo.add(b); }

    ReaderRepository readerRepo;
    for (const auto& r : loadedReaders) { readerRepo.add(r); }

    LoanRepository   loanRepo;
    for (const auto& l : loadedLoans)   { loanRepo.add(l); }

    ReservationRepository reservationRepo;
    for (const auto& r : loadedReservations) { reservationRepo.add(r); }

    // 1.3 创建 Manager（传入仓库引用）
    BookManager       bookMgr(bookRepo);
    ReaderManager     readerMgr(readerRepo);
    LoanManager       loanMgr(bookRepo, readerRepo, loanRepo);
    StatisticsManager statsMgr(bookRepo, readerRepo, loanRepo);
    ReservationManager reservationMgr(bookRepo, readerRepo, loanRepo, reservationRepo);

    // ================================================================
    //  第2部分：主循环
    // ================================================================

    bool running = true;
    while (running) {
        clearScreen();

        std::cout << "==================================================" << std::endl;
        std::cout << "             图书管理系统" << std::endl;
        std::cout << "==================================================" << std::endl;
        std::cout << "  1. 图书管理" << std::endl;
        std::cout << "  2. 读者管理" << std::endl;
        std::cout << "  3. 借阅管理" << std::endl;
        std::cout << "  4. 统计分析" << std::endl;
        std::cout << "  5. 退出系统" << std::endl;
        std::cout << "==================================================" << std::endl;

        int choice = readInt("请选择 [1-5]：");

        switch (choice) {
            case 1: menuBookManagement(bookMgr);       break;
            case 2: menuReaderManagement(readerMgr);   break;
            case 3: menuLoanManagement(loanMgr, reservationMgr); break;
            case 4: menuStatistics(statsMgr);           break;
            case 5: running = false;                    break;
            default:
                std::cout << "无效选择，请重新输入" << std::endl;
                pause();
                break;
        }
    }

    // ================================================================
    //  第3部分：退出保存
    // ================================================================

    dataMgr.overwriteBooks(bookRepo.getAll());
    dataMgr.overwriteReaders(readerRepo.getAll());
    dataMgr.overwriteLoans(loanRepo.getAll());
    dataMgr.overwriteReservations(reservationRepo.getAll());

    std::cout << "数据已保存，系统退出。" << std::endl;
    return 0;
}

// ============================================================================
//  图书管理子菜单
// ============================================================================

void menuBookManagement(BookManager& bookMgr) {
    bool back = false;
    while (!back) {
        clearScreen();
        std::cout << "==================================================" << std::endl;
        std::cout << "                 图书管理" << std::endl;
        std::cout << "==================================================" << std::endl;
        std::cout << "  1. 新增图书      2. 删除图书" << std::endl;
        std::cout << "  3. 修改图书      4. 查找图书" << std::endl;
        std::cout << "  5. 显示全部      6. 入库" << std::endl;
        std::cout << "  7. 出库          8. 返回主菜单" << std::endl;
        std::cout << "==================================================" << std::endl;

        int choice = readInt("请选择 [1-8]：");

        switch (choice) {
            case 1: {
                // ———— 新增图书 ————
                Book book;
                book.input();               // 从控制台输入（Model 层提供）
                if (bookMgr.addBook(book)) {
                    std::cout << "新增成功" << std::endl;
                } else {
                    std::cout << "新增失败：" << bookMgr.getLastError() << std::endl;
                }
                pause();
                break;
            }
            case 2: {
                // ———— 删除图书 ————
                std::string bookId;
                std::cout << "请输入要删除的图书编号：";
                std::getline(std::cin, bookId);
                if (bookMgr.removeBook(bookId)) {
                    std::cout << "删除成功" << std::endl;
                } else {
                    std::cout << "删除失败：" << bookMgr.getLastError() << std::endl;
                }
                pause();
                break;
            }
            case 3: {
                // ———— 修改图书 ————
                std::string bookId;
                std::cout << "请输入要修改的图书编号：";
                std::getline(std::cin, bookId);
                Book* existing = bookMgr.findBook(bookId);
                if (!existing) {
                    std::cout << "修改失败：图书编号 [" << bookId << "] 不存在" << std::endl;
                    pause();
                    break;
                }
                Book newInfo = *existing;
                std::cout << "请输入新的图书信息（直接回车保留原值）：" << std::endl;
                newInfo.modify();
                if (bookMgr.updateBook(bookId, newInfo)) {
                    std::cout << "修改成功" << std::endl;
                } else {
                    std::cout << "修改失败：" << bookMgr.getLastError() << std::endl;
                }
                pause();
                break;
            }
            case 4: {
                // ———— 查找图书 ————
                std::string keyword;
                std::cout << "请输入书名或作者关键字：";
                std::getline(std::cin, keyword);
                std::vector<Book> results = bookMgr.searchBooks(keyword);
                if (results.empty()) {
                    std::cout << "未找到匹配的图书" << std::endl;
                } else {
                    std::cout << "找到 " << results.size() << " 本图书：" << std::endl;
                    for (const auto& b : results) {
                        b.print();
                    }
                }
                pause();
                break;
            }
            case 5: {
                // ———— 显示全部 ————
                std::vector<Book> all = bookMgr.listAllBooks();
                if (all.empty()) {
                    std::cout << "暂无图书记录" << std::endl;
                } else {
                    std::cout << "共 " << all.size() << " 本图书：" << std::endl;
                    for (const auto& b : all) {
                        b.print();
                    }
                }
                pause();
                break;
            }
            case 6: {
                // ———— 入库 ————
                std::string bookId;
                std::cout << "请输入图书编号：";
                std::getline(std::cin, bookId);
                int count = readInt("请输入入库数量：");
                if (bookMgr.increaseStock(bookId, count)) {
                    std::cout << "入库成功" << std::endl;
                } else {
                    std::cout << "入库失败：" << bookMgr.getLastError() << std::endl;
                }
                pause();
                break;
            }
            case 7: {
                // ———— 出库 ————
                std::string bookId;
                std::cout << "请输入图书编号：";
                std::getline(std::cin, bookId);
                int count = readInt("请输入出库数量：");
                if (bookMgr.decreaseStock(bookId, count)) {
                    std::cout << "出库成功" << std::endl;
                } else {
                    std::cout << "出库失败：" << bookMgr.getLastError() << std::endl;
                }
                pause();
                break;
            }
            case 8: back = true; break;
            default:
                std::cout << "无效选择" << std::endl;
                pause();
                break;
        }
    }
}

// ============================================================================
//  读者管理子菜单
// ============================================================================

void menuReaderManagement(ReaderManager& readerMgr) {
    bool back = false;
    while (!back) {
        clearScreen();
        std::cout << "==================================================" << std::endl;
        std::cout << "                 读者管理" << std::endl;
        std::cout << "==================================================" << std::endl;
        std::cout << "  1. 新增读者      2. 删除读者" << std::endl;
        std::cout << "  3. 修改读者      4. 查找读者" << std::endl;
        std::cout << "  5. 显示全部      6. 封禁读者" << std::endl;
        std::cout << "  7. 解除封禁      8. 返回主菜单" << std::endl;
        std::cout << "==================================================" << std::endl;

        int choice = readInt("请选择 [1-8]：");

        switch (choice) {
            case 1: {
                // ———— 新增读者 ————
                Reader reader;
                reader.input();
                if (readerMgr.addReader(reader)) {
                    std::cout << "新增成功" << std::endl;
                } else {
                    std::cout << "新增失败：" << readerMgr.getLastError() << std::endl;
                }
                pause();
                break;
            }
            case 2: {
                // ———— 删除读者 ————
                std::string cardId;
                std::cout << "请输入要删除的借书证号：";
                std::getline(std::cin, cardId);
                if (readerMgr.removeReader(cardId)) {
                    std::cout << "删除成功" << std::endl;
                } else {
                    std::cout << "删除失败：" << readerMgr.getLastError() << std::endl;
                }
                pause();
                break;
            }
            case 3: {
                // ———— 修改读者 ————
                std::string cardId;
                std::cout << "请输入要修改的借书证号：";
                std::getline(std::cin, cardId);
                Reader* existing = readerMgr.findReader(cardId);
                if (!existing) {
                    std::cout << "修改失败：借书证号 [" << cardId << "] 不存在" << std::endl;
                    pause();
                    break;
                }
                Reader newInfo = *existing;
                std::cout << "请输入新的读者信息（直接回车保留原值）：" << std::endl;
                newInfo.modify();
                if (readerMgr.updateReader(cardId, newInfo)) {
                    std::cout << "修改成功" << std::endl;
                } else {
                    std::cout << "修改失败：" << readerMgr.getLastError() << std::endl;
                }
                pause();
                break;
            }
            case 4: {
                // ———— 查找读者 ————
                std::string cardId;
                std::cout << "请输入借书证号：";
                std::getline(std::cin, cardId);
                Reader* reader = readerMgr.findReader(cardId);
                if (reader != nullptr) {
                    reader->print();
                } else {
                    std::cout << "查找失败：" << readerMgr.getLastError() << std::endl;
                }
                pause();
                break;
            }
            case 5: {
                // ———— 显示全部 ————
                std::vector<Reader> all = readerMgr.listAllReaders();
                if (all.empty()) {
                    std::cout << "暂无读者记录" << std::endl;
                } else {
                    std::cout << "共 " << all.size() << " 位读者：" << std::endl;
                    for (const auto& r : all) {
                        r.print();
                    }
                }
                pause();
                break;
            }
            case 6: {
                // ———— 封禁读者 ————
                std::string cardId;
                std::cout << "请输入要封禁的借书证号：";
                std::getline(std::cin, cardId);
                if (readerMgr.banReader(cardId)) {
                    std::cout << "封禁成功" << std::endl;
                } else {
                    std::cout << "封禁失败：" << readerMgr.getLastError() << std::endl;
                }
                pause();
                break;
            }
            case 7: {
                // ———— 解除封禁 ————
                std::string cardId;
                std::cout << "请输入要解封的借书证号：";
                std::getline(std::cin, cardId);
                if (readerMgr.unbanReader(cardId)) {
                    std::cout << "解封成功" << std::endl;
                } else {
                    std::cout << "解封失败：" << readerMgr.getLastError() << std::endl;
                }
                pause();
                break;
            }
            case 8: back = true; break;
            default:
                std::cout << "无效选择" << std::endl;
                pause();
                break;
        }
    }
}

// ============================================================================
//  借阅管理子菜单
// ============================================================================

void menuLoanManagement(LoanManager& loanMgr, ReservationManager& reservationMgr) {
    bool back = false;
    while (!back) {
        clearScreen();
        std::cout << "==================================================" << std::endl;
        std::cout << "                 借阅管理" << std::endl;
        std::cout << "==================================================" << std::endl;
        std::cout << "  1. 借书          2. 还书" << std::endl;
        std::cout << "  3. 续借          4. 预约图书" << std::endl;
        std::cout << "  5. 取消预约      6. 查看预约队列" << std::endl;
        std::cout << "  7. 返回主菜单" << std::endl;
        std::cout << "==================================================" << std::endl;

        int choice = readInt("请选择 [1-7]：");

        switch (choice) {
            case 1: {
                // ———— 借书 ————
                std::string bookId, cardId;
                std::cout << "请输入图书编号：";
                std::getline(std::cin, bookId);
                std::cout << "请输入借书证号：";
                std::getline(std::cin, cardId);
                if (loanMgr.borrowBook(bookId, cardId)) {
                    std::cout << "借书成功" << std::endl;
                } else {
                    std::cout << "借书失败：" << loanMgr.getLastError() << std::endl;
                }
                pause();
                break;
            }
            case 2: {
                // ———— 还书 ————
                std::string bookId, cardId;
                std::cout << "请输入图书编号：";
                std::getline(std::cin, bookId);
                std::cout << "请输入借书证号：";
                std::getline(std::cin, cardId);
                if (loanMgr.returnBook(bookId, cardId)) {
                    std::cout << "还书成功" << std::endl;
                    reservationMgr.processReturnedBook(bookId);
                } else {
                    std::cout << "还书失败：" << loanMgr.getLastError() << std::endl;
                }
                pause();
                break;
            }
            case 3: {
                // ———— 续借 ————
                std::string bookId, cardId;
                std::cout << "请输入图书编号：";
                std::getline(std::cin, bookId);
                std::cout << "请输入借书证号：";
                std::getline(std::cin, cardId);
                if (loanMgr.renewBook(bookId, cardId)) {
                    std::cout << "续借成功" << std::endl;
                } else {
                    std::cout << "续借失败：" << loanMgr.getLastError() << std::endl;
                }
                pause();
                break;
            }
            case 4: {
                // ———— 预约图书 ————
                std::string bookId, cardId;
                std::cout << "请输入图书编号：";
                std::getline(std::cin, bookId);
                std::cout << "请输入借书证号：";
                std::getline(std::cin, cardId);
                if (reservationMgr.reserveBook(bookId, cardId)) {
                    std::cout << "预约成功" << std::endl;
                } else {
                    std::cout << "预约失败：" << reservationMgr.getLastError() << std::endl;
                }
                pause();
                break;
            }
            case 5: {
                // ———— 取消预约 ————
                std::string bookId, cardId;
                std::cout << "请输入图书编号：";
                std::getline(std::cin, bookId);
                std::cout << "请输入借书证号：";
                std::getline(std::cin, cardId);
                if (reservationMgr.cancelReservation(bookId, cardId)) {
                    std::cout << "取消预约成功" << std::endl;
                } else {
                    std::cout << "取消预约失败：" << reservationMgr.getLastError() << std::endl;
                }
                pause();
                break;
            }
            case 6: {
                // ———— 查看预约队列 ————
                std::string bookId;
                std::cout << "请输入图书编号（直接回车查看全部预约）：";
                std::getline(std::cin, bookId);
                if (bookId.empty()) {
                    std::vector<Reservation> all = reservationMgr.listReservations();
                    if (all.empty()) {
                        std::cout << "暂无预约记录" << std::endl;
                    } else {
                        std::cout << "共 " << all.size() << " 条预约记录：" << std::endl;
                        for (size_t i = 0; i < all.size(); ++i) {
                            std::cout << "  " << (i + 1) << ". ";
                            all[i].print();
                        }
                    }
                } else {
                    const std::deque<Reservation>* queue = reservationMgr.getQueueForBook(bookId);
                    if (queue == nullptr || queue->empty()) {
                        std::cout << "图书 [" << bookId << "] 暂无预约记录" << std::endl;
                    } else {
                        std::cout << "图书 [" << bookId << "] 预约队列（共 " << queue->size() << " 人）：" << std::endl;
                        int pos = 1;
                        for (const auto& res : *queue) {
                            std::cout << "  " << pos++ << ". ";
                            res.print();
                        }
                    }
                }
                pause();
                break;
            }
            case 7: back = true; break;
            default:
                std::cout << "无效选择" << std::endl;
                pause();
                break;
        }
    }
}

// ============================================================================
//  统计分析子菜单
// ============================================================================

void menuStatistics(StatisticsManager& statsMgr) {
    bool back = false;
    while (!back) {
        clearScreen();
        std::cout << "==================================================" << std::endl;
        std::cout << "                 统计分析" << std::endl;
        std::cout << "==================================================" << std::endl;
        std::cout << "  1. 借阅排行榜" << std::endl;
        std::cout << "  2. 库存统计" << std::endl;
        std::cout << "  3. 超期统计" << std::endl;
        std::cout << "  4. 当前借阅统计" << std::endl;
        std::cout << "  5. 返回主菜单" << std::endl;
        std::cout << "==================================================" << std::endl;

        int choice = readInt("请选择 [1-5]：");

        switch (choice) {
            case 1: {
                // ———— 借阅排行榜 ————
                int topN = readInt("请输入要显示的排名数量（0=全部）：");
                std::vector<Book> ranking = statsMgr.getBorrowRanking(topN);
                if (ranking.empty()) {
                    std::cout << "暂无图书记录" << std::endl;
                } else {
                    std::cout << "借阅排行榜（Top " << ranking.size() << "）：" << std::endl;
                    for (size_t i = 0; i < ranking.size(); ++i) {
                        std::cout << "  No." << (i + 1) << "  "
                                  << ranking[i].bookId << "  "
                                  << ranking[i].title << "  ──  "
                                  << ranking[i].borrowCount << " 次"
                                  << std::endl;
                    }
                }
                pause();
                break;
            }
            case 2: {
                // ———— 库存统计 ————
                auto stats = statsMgr.getInventoryStats();
                std::cout << "┌───────────────────────────────────┐" << std::endl;
                std::cout << "│  总书目数：" << std::setw(24) << stats.totalTitles    << "│" << std::endl;
                std::cout << "│  总 册 数：" << std::setw(24) << stats.totalCopies    << "│" << std::endl;
                std::cout << "│  在库册数：" << std::setw(24) << stats.availableCopies<< "│" << std::endl;
                std::cout << "│  借出册数：" << std::setw(24) << stats.borrowedCopies << "│" << std::endl;
                std::cout << "└───────────────────────────────────┘" << std::endl;
                pause();
                break;
            }
            case 3: {
                // ———— 超期统计 ————
                std::vector<Loan*> overdue = statsMgr.getOverdueLoans();
                if (overdue.empty()) {
                    std::cout << "暂无超期记录" << std::endl;
                } else {
                    std::cout << "超期记录共 " << overdue.size() << " 条：" << std::endl;
                    for (const auto& loan : overdue) {
                        std::cout << "  图书 " << loan->bookId
                                  << " / 读者 " << loan->cardId
                                  << " / 到期 " << loan->dueDate
                                  << std::endl;
                    }
                }
                pause();
                break;
            }
            case 4: {
                // ———— 当前借阅统计 ————
                std::string report = statsMgr.getCurrentLoanStats();
                std::cout << report;
                pause();
                break;
            }
            case 5: back = true; break;
            default:
                std::cout << "无效选择" << std::endl;
                pause();
                break;
        }
    }
}
