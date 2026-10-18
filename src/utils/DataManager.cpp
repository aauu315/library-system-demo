/**
 * @file    DataManager.cpp
 * @brief   数据文件管理器实现 —— 使用 fstream 操作三个 txt 文件
 *
 * 每个方法独立打开、操作、关闭文件，避免文件句柄泄漏。
 * 序列化委托给 Model 的 toFileString()，反序列化委托给 fromFileString()。
 */

#include "../../include/utils/DataManager.h"
#include <fstream>
#include <iostream>
#include <sys/stat.h>    // 用于目录存在性检查（Windows: _mkdir / POSIX: mkdir）

#ifdef _WIN32
    #include <direct.h>   // _mkdir
    #define MKDIR(path) _mkdir(path)
#else
    #include <sys/stat.h>
    #define MKDIR(path) mkdir(path, 0755)
#endif

// ============================================================================
//  构造函数
// ============================================================================

/** @brief 默认构造函数 —— 数据目录为 "data/" */
DataManager::DataManager()
    : dataDir("data/")
{
    updatePaths();
    ensureDirectory();
}

/** @brief 带参构造函数 —— 指定数据目录 */
DataManager::DataManager(const std::string& dataDir)
    : dataDir(dataDir)
{
    // 确保目录路径以 '/' 或 '\\' 结尾
    if (!this->dataDir.empty()) {
        char lastChar = this->dataDir.back();
        if (lastChar != '/' && lastChar != '\\') {
            this->dataDir += '/';
        }
    }
    updatePaths();
    ensureDirectory();
}

// ============================================================================
//  配置
// ============================================================================

/** @brief 设置数据目录路径并更新文件路径 */
void DataManager::setDataDir(const std::string& dir) {
    dataDir = dir;
    if (!dataDir.empty()) {
        char lastChar = dataDir.back();
        if (lastChar != '/' && lastChar != '\\') {
            dataDir += '/';
        }
    }
    updatePaths();
    ensureDirectory();
}

/** @brief 获取当前数据目录路径 */
std::string DataManager::getDataDir() const {
    return dataDir;
}

// ============================================================================
//  内部辅助
// ============================================================================

/** @brief 根据 dataDir 更新三个文件的完整路径 */
void DataManager::updatePaths() {
    booksPath       = dataDir + "books.txt";
    readersPath     = dataDir + "readers.txt";
    loansPath       = dataDir + "loans.txt";
    reservationsPath = dataDir + "reservations.txt";
}

/** @brief 确保数据目录存在，不存在则创建 */
bool DataManager::ensureDirectory() const {
    if (dataDir.empty()) {
        return true;  // 当前目录，无需创建
    }
    // 去掉末尾的路径分隔符
    std::string dir = dataDir;
    while (!dir.empty() && (dir.back() == '/' || dir.back() == '\\')) {
        dir.pop_back();
    }
    if (dir.empty()) {
        return true;
    }
    int result = MKDIR(dir.c_str());
    // 返回 0 表示创建成功，EEXIST (或非零) 表示目录已存在
    (void)result;      // 忽略返回值：目录已存在时创建失败是可接受的
    return true;       // 目录存在或创建成功，调用者总是可以继续
}

// ============================================================================
//  读取（加载全部数据）
// ============================================================================

/** @brief 从 books.txt 逐行读取并反序列化为 Book 对象 */
std::vector<Book> DataManager::loadBooks() const {
    std::vector<Book> books;
    std::ifstream file(booksPath);
    if (!file.is_open()) {
        // 文件不存在（首次运行），返回空 vector
        return books;
    }
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) {
            continue;  // 跳过空行
        }
        books.push_back(Book::fromFileString(line));
    }
    file.close();
    return books;
}

/** @brief 从 readers.txt 逐行读取并反序列化为 Reader 对象 */
std::vector<Reader> DataManager::loadReaders() const {
    std::vector<Reader> readers;
    std::ifstream file(readersPath);
    if (!file.is_open()) {
        return readers;
    }
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) {
            continue;
        }
        readers.push_back(Reader::fromFileString(line));
    }
    file.close();
    return readers;
}

/** @brief 从 loans.txt 逐行读取并反序列化为 Loan 对象 */
std::list<Loan> DataManager::loadLoans() const {
    std::list<Loan> loans;
    std::ifstream file(loansPath);
    if (!file.is_open()) {
        return loans;
    }
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) {
            continue;
        }
        loans.push_back(Loan::fromFileString(line));
    }
    file.close();
    return loans;
}

/** @brief 从 reservations.txt 逐行读取并反序列化为 Reservation 对象 */
std::vector<Reservation> DataManager::loadReservations() const {
    std::vector<Reservation> reservations;
    std::ifstream file(reservationsPath);
    if (!file.is_open()) {
        return reservations;
    }
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) {
            continue;
        }
        reservations.push_back(Reservation::fromFileString(line));
    }
    file.close();
    return reservations;
}

// ============================================================================
//  覆盖保存（清空原文件后写入）
// ============================================================================

/** @brief 将全部图书覆盖写入 books.txt */
void DataManager::overwriteBooks(const std::vector<Book>& books) const {
    ensureDirectory();
    std::ofstream file(booksPath, std::ios::out | std::ios::trunc);
    if (!file.is_open()) {
        std::cerr << "[警告] 无法打开文件进行写入: " << booksPath << std::endl;
        return;
    }
    for (const auto& book : books) {
        file << book.toFileString() << std::endl;
    }
    file.close();
}

/** @brief 将全部读者覆盖写入 readers.txt */
void DataManager::overwriteReaders(const std::vector<Reader>& readers) const {
    ensureDirectory();
    std::ofstream file(readersPath, std::ios::out | std::ios::trunc);
    if (!file.is_open()) {
        std::cerr << "[警告] 无法打开文件进行写入: " << readersPath << std::endl;
        return;
    }
    for (const auto& reader : readers) {
        file << reader.toFileString() << std::endl;
    }
    file.close();
}

/** @brief 将全部借阅记录覆盖写入 loans.txt */
void DataManager::overwriteLoans(const std::list<Loan>& loans) const {
    ensureDirectory();
    std::ofstream file(loansPath, std::ios::out | std::ios::trunc);
    if (!file.is_open()) {
        std::cerr << "[警告] 无法打开文件进行写入: " << loansPath << std::endl;
        return;
    }
    for (const auto& loan : loans) {
        file << loan.toFileString() << std::endl;
    }
    file.close();
}

/** @brief 将全部预约记录覆盖写入 reservations.txt */
void DataManager::overwriteReservations(const std::vector<Reservation>& reservations) const {
    ensureDirectory();
    std::ofstream file(reservationsPath, std::ios::out | std::ios::trunc);
    if (!file.is_open()) {
        std::cerr << "[警告] 无法打开文件进行写入: " << reservationsPath << std::endl;
        return;
    }
    for (const auto& res : reservations) {
        file << res.toFileString() << std::endl;
    }
    file.close();
}

// ============================================================================
//  追加（在文件末尾追加，不清空原数据）
// ============================================================================

/** @brief 在 books.txt 末尾追加一条图书记录 */
void DataManager::appendBook(const Book& book) const {
    ensureDirectory();
    // std::ios::app 模式：写入前自动定位到文件末尾，保留原有内容
    std::ofstream file(booksPath, std::ios::out | std::ios::app);
    if (!file.is_open()) {
        std::cerr << "[警告] 无法打开文件进行追加: " << booksPath << std::endl;
        return;
    }
    file << book.toFileString() << std::endl;
    file.close();
}

/** @brief 在 readers.txt 末尾追加一条读者记录 */
void DataManager::appendReader(const Reader& reader) const {
    ensureDirectory();
    std::ofstream file(readersPath, std::ios::out | std::ios::app);
    if (!file.is_open()) {
        std::cerr << "[警告] 无法打开文件进行追加: " << readersPath << std::endl;
        return;
    }
    file << reader.toFileString() << std::endl;
    file.close();
}

/** @brief 在 loans.txt 末尾追加一条借阅记录 */
void DataManager::appendLoan(const Loan& loan) const {
    ensureDirectory();
    std::ofstream file(loansPath, std::ios::out | std::ios::app);
    if (!file.is_open()) {
        std::cerr << "[警告] 无法打开文件进行追加: " << loansPath << std::endl;
        return;
    }
    file << loan.toFileString() << std::endl;
    file.close();
}

// ============================================================================
//  批量保存
// ============================================================================

/** @brief 一次性将三种数据覆盖写入对应文件 */
void DataManager::saveAll(const std::vector<Book>& books,
                           const std::vector<Reader>& readers,
                           const std::list<Loan>& loans) const {
    overwriteBooks(books);
    overwriteReaders(readers);
    overwriteLoans(loans);
}
