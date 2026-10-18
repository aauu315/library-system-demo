/**
 * @file    DataManager.h
 * @brief   数据文件管理器 —— 负责 books.txt / readers.txt / loans.txt 的读写
 *
 * 本类封装了三个持久化数据文件的所有 I/O 操作：
 *   - 读取（加载全部数据）
 *   - 覆盖保存（重写整个文件）
 *   - 追加（在文件末尾追加单条记录）
 *
 * 数据目录默认为 "data/"。
 * 序列化/反序列化委托给各个 Model 类的 toFileString() / fromFileString()。
 *
 * 注意：本类不包含任何业务逻辑，仅负责文件读写。
 */

#ifndef DATA_MANAGER_H
#define DATA_MANAGER_H

#include <string>
#include <vector>
#include <list>
#include "../models/Book.h"
#include "../models/Reader.h"
#include "../models/Loan.h"
#include "../models/Reservation.h"

class DataManager {
public:
    // ========================================================================
    //  构造函数
    // ========================================================================

    /** @brief 默认构造函数 —— 数据目录为 "data/" */
    DataManager();

    /** @brief 带参构造函数 —— 指定数据目录 */
    explicit DataManager(const std::string& dataDir);

    // ========================================================================
    //  读取（从文件加载全部数据到内存）
    // ========================================================================

    /** @brief 从 books.txt 加载全部图书 */
    std::vector<Book> loadBooks() const;

    /** @brief 从 readers.txt 加载全部读者 */
    std::vector<Reader> loadReaders() const;

    /** @brief 从 loans.txt 加载全部借阅记录 */
    std::list<Loan> loadLoans() const;

    /** @brief 从 reservations.txt 加载全部预约记录 */
    std::vector<Reservation> loadReservations() const;

    // ========================================================================
    //  覆盖保存（清空原文件，写入新数据）
    // ========================================================================

    /** @brief 将全部图书覆盖写入 books.txt */
    void overwriteBooks(const std::vector<Book>& books) const;

    /** @brief 将全部读者覆盖写入 readers.txt */
    void overwriteReaders(const std::vector<Reader>& readers) const;

    /** @brief 将全部借阅记录覆盖写入 loans.txt */
    void overwriteLoans(const std::list<Loan>& loans) const;

    /** @brief 将全部预约记录覆盖写入 reservations.txt */
    void overwriteReservations(const std::vector<Reservation>& reservations) const;

    // ========================================================================
    //  追加（在文件末尾追加单条记录，不清空原有数据）
    // ========================================================================

    /** @brief 在 books.txt 末尾追加一条图书记录 */
    void appendBook(const Book& book) const;

    /** @brief 在 readers.txt 末尾追加一条读者记录 */
    void appendReader(const Reader& reader) const;

    /** @brief 在 loans.txt 末尾追加一条借阅记录 */
    void appendLoan(const Loan& loan) const;

    // ========================================================================
    //  批量保存（覆盖全部三个文件）
    // ========================================================================

    /** @brief 一次性将三种数据覆盖写入对应文件 */
    void saveAll(const std::vector<Book>& books,
                 const std::vector<Reader>& readers,
                 const std::list<Loan>& loans) const;

    // ========================================================================
    //  配置
    // ========================================================================

    /** @brief 设置数据目录路径 */
    void setDataDir(const std::string& dir);

    /** @brief 获取当前数据目录路径 */
    std::string getDataDir() const;

private:
    // ========================================================================
    //  内部辅助方法
    // ========================================================================

    /** @brief 根据 dataDir 更新三个文件的完整路径 */
    void updatePaths();

    /** @brief 确保数据目录存在，不存在则创建 */
    bool ensureDirectory() const;

    // ========================================================================
    //  成员变量
    // ========================================================================

    std::string dataDir;     // 数据目录路径（如 "data/"）
    std::string booksPath;   // books.txt 完整路径
    std::string readersPath; // readers.txt 完整路径
    std::string loansPath;        // loans.txt 完整路径
    std::string reservationsPath; // reservations.txt 完整路径
};

#endif // DATA_MANAGER_H
