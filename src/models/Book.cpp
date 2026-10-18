/**
 * @file    Book.cpp
 * @brief   图书数据模型实现 —— 构造函数、Get/Set、打印、文件序列化
 */

#include "../../include/models/Book.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <vector>

// ============================================================================
//  构造函数
// ============================================================================

/** @brief 默认构造函数 —— 所有字段初始化为默认值 */
Book::Book()
    : bookId("")
    , title("")
    , author("")
    , category("")
    , totalStock(0)
    , currentStock(0)
    , borrowCount(0)
{
}

/** @brief 带参构造函数 —— 初始化全部字段 */
Book::Book(const std::string& id,
           const std::string& t,
           const std::string& a,
           const std::string& cat,
           int total,
           int current,
           int count)
    : bookId(id)
    , title(t)
    , author(a)
    , category(cat)
    , totalStock(total)
    , currentStock(current)
    , borrowCount(count)
{
}

// ============================================================================
//  运算符重载
// ============================================================================

/** @brief 相等比较 —— 图书编号相同即视为同一本书 */
bool Book::operator==(const Book& other) const {
    return bookId == other.bookId;
}

/** @brief 小于比较 —— 按图书编号字典序，BST 内部排序使用 */
bool Book::operator<(const Book& other) const {
    return bookId < other.bookId;
}

// ============================================================================
//  Getter 方法
// ============================================================================

std::string Book::getBookId()       const { return bookId; }
std::string Book::getTitle()        const { return title; }
std::string Book::getAuthor()       const { return author; }
std::string Book::getCategory()     const { return category; }
int         Book::getTotalStock()   const { return totalStock; }
int         Book::getCurrentStock() const { return currentStock; }
int         Book::getBorrowCount()  const { return borrowCount; }

// ============================================================================
//  Setter 方法
// ============================================================================

void Book::setBookId(const std::string& id)  { bookId = id; }
void Book::setTitle(const std::string& t)     { title = t; }
void Book::setAuthor(const std::string& a)    { author = a; }
void Book::setCategory(const std::string& cat){ category = cat; }
void Book::setTotalStock(int total)            { totalStock = total; }
void Book::setCurrentStock(int current)        { currentStock = current; }
void Book::setBorrowCount(int count)           { borrowCount = count; }

// ============================================================================
//  打印与输入
// ============================================================================

/** @brief 将图书信息以表格形式打印到控制台 */
void Book::print() const {
    std::cout << "┌─────────────────────────────────────────────────────┐" << std::endl;
    std::cout << "│ 图书编号：" << std::left << std::setw(44) << bookId       << "│" << std::endl;
    std::cout << "│ 书名    ：" << std::left << std::setw(44) << title        << "│" << std::endl;
    std::cout << "│ 作者    ：" << std::left << std::setw(44) << author       << "│" << std::endl;
    std::cout << "│ 类别    ：" << std::left << std::setw(44) << category     << "│" << std::endl;
    std::cout << "│ 总库存  ：" << std::left << std::setw(44) << totalStock   << "│" << std::endl;
    std::cout << "│ 当前库存：" << std::left << std::setw(44) << currentStock << "│" << std::endl;
    std::cout << "│ 借阅次数：" << std::left << std::setw(44) << borrowCount  << "│" << std::endl;
    std::cout << "└─────────────────────────────────────────────────────┘" << std::endl;
}

/** @brief 从控制台逐字段输入图书信息 */
void Book::input() {
    std::cout << "请输入图书编号：";
    std::getline(std::cin, bookId);
    std::cout << "请输入书名：";
    std::getline(std::cin, title);
    std::cout << "请输入作者：";
    std::getline(std::cin, author);
    std::cout << "请输入类别（书籍/期刊/报刊）：";
    std::getline(std::cin, category);
    std::string input;
    std::cout << "请输入总库存：";
    std::getline(std::cin, input);
    if (!input.empty()) {
        try { totalStock = std::stoi(input); } catch (...) { totalStock = 0; }
    }
    std::cout << "请输入当前库存：";
    std::getline(std::cin, input);
    if (!input.empty()) {
        try { currentStock = std::stoi(input); } catch (...) { currentStock = 0; }
    }
    std::cout << "请输入借阅次数：";
    std::getline(std::cin, input);
    if (!input.empty()) {
        try { borrowCount = std::stoi(input); } catch (...) { borrowCount = 0; }
    }
}

/** @brief 修改图书信息 —— 显示当前值，直接回车保留原值 */
void Book::modify() {
    std::string input;
    std::cout << "请输入图书编号 [" << bookId << "]：";
    std::getline(std::cin, input);
    if (!input.empty()) bookId = input;

    std::cout << "请输入书名 [" << title << "]：";
    std::getline(std::cin, input);
    if (!input.empty()) title = input;

    std::cout << "请输入作者 [" << author << "]：";
    std::getline(std::cin, input);
    if (!input.empty()) author = input;

    std::cout << "请输入类别（书籍/期刊/报刊） [" << category << "]：";
    std::getline(std::cin, input);
    if (!input.empty()) category = input;

    std::cout << "请输入总库存 [" << totalStock << "]：";
    std::getline(std::cin, input);
    if (!input.empty()) totalStock = std::stoi(input);

    std::cout << "请输入当前库存 [" << currentStock << "]：";
    std::getline(std::cin, input);
    if (!input.empty()) currentStock = std::stoi(input);

    std::cout << "请输入借阅次数 [" << borrowCount << "]：";
    std::getline(std::cin, input);
    if (!input.empty()) borrowCount = std::stoi(input);
}

// ============================================================================
//  文件格式序列化 / 反序列化
// ============================================================================

/**
 * @brief 将当前对象序列化为 txt 格式字符串
 * @return 格式：bookId|title|author|category|totalStock|currentStock|borrowCount
 */
std::string Book::toFileString() const {
    std::ostringstream oss;
    oss << bookId      << "|"
        << title       << "|"
        << author      << "|"
        << category    << "|"
        << totalStock  << "|"
        << currentStock<< "|"
        << borrowCount;
    return oss.str();
}

/**
 * @brief 从 txt 格式字符串反序列化为 Book 对象
 * @param  line 一行由 '|' 分隔的 txt 数据
 * @return Book 对象
 */
Book Book::fromFileString(const std::string& line) {
    // 手动按 '|' 拆分字符串
    std::vector<std::string> fields;
    std::string field;
    std::istringstream stream(line);
    while (std::getline(stream, field, '|')) {
        fields.push_back(field);
    }
    // 字段数不足时用默认值补齐
    if (fields.size() < 7) {
        return Book();
    }
    return Book(
        fields[0],                          // bookId
        fields[1],                          // title
        fields[2],                          // author
        fields[3],                          // category
        std::stoi(fields[4]),               // totalStock
        std::stoi(fields[5]),               // currentStock
        std::stoi(fields[6])                // borrowCount
    );
}
