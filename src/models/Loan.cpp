/**
 * @file    Loan.cpp
 * @brief   借阅记录数据模型实现 —— 构造函数、Get/Set、打印、文件序列化
 */

#include "../../include/models/Loan.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <vector>

// ============================================================================
//  构造函数
// ============================================================================

/** @brief 默认构造函数 —— 所有字段初始化为默认值 */
Loan::Loan()
    : bookId("")
    , cardId("")
    , borrowDate("")
    , dueDate("")
    , isReturned(false)
    , isRenewed(false)
    , fine(0.0)
{
}

/** @brief 带参构造函数 —— 初始化全部字段 */
Loan::Loan(const std::string& bId,
           const std::string& cId,
           const std::string& bDate,
           const std::string& dDate,
           bool returned,
           bool renewed,
           double f)
    : bookId(bId)
    , cardId(cId)
    , borrowDate(bDate)
    , dueDate(dDate)
    , isReturned(returned)
    , isRenewed(renewed)
    , fine(f)
{
}

// ============================================================================
//  Getter 方法
// ============================================================================

std::string Loan::getBookId()     const { return bookId; }
std::string Loan::getCardId()     const { return cardId; }
std::string Loan::getBorrowDate() const { return borrowDate; }
std::string Loan::getDueDate()    const { return dueDate; }
bool        Loan::getIsReturned() const { return isReturned; }
bool        Loan::getIsRenewed()  const { return isRenewed; }
double      Loan::getFine()       const { return fine; }

// ============================================================================
//  Setter 方法
// ============================================================================

void Loan::setBookId(const std::string& bId)    { bookId = bId; }
void Loan::setCardId(const std::string& cId)    { cardId = cId; }
void Loan::setBorrowDate(const std::string& bDate) { borrowDate = bDate; }
void Loan::setDueDate(const std::string& dDate) { dueDate = dDate; }
void Loan::setIsReturned(bool returned)          { isReturned = returned; }
void Loan::setIsRenewed(bool renewed)            { isRenewed = renewed; }
void Loan::setFine(double f)                     { fine = f; }

// ============================================================================
//  打印与输入
// ============================================================================

/** @brief 将借阅记录信息以表格形式打印到控制台 */
void Loan::print() const {
    std::cout << "┌─────────────────────────────────────────────────────┐" << std::endl;
    std::cout << "│ 图书编号：" << std::left << std::setw(44) << bookId       << "│" << std::endl;
    std::cout << "│ 借书证号：" << std::left << std::setw(44) << cardId       << "│" << std::endl;
    std::cout << "│ 借阅日期：" << std::left << std::setw(44) << borrowDate   << "│" << std::endl;
    std::cout << "│ 到期日期：" << std::left << std::setw(44) << dueDate      << "│" << std::endl;
    std::cout << "│ 是否归还：" << std::left << std::setw(44);
    std::cout << (isReturned ? "是" : "否") << "│" << std::endl;
    std::cout << "│ 是否续借：" << std::left << std::setw(44);
    std::cout << (isRenewed ? "是" : "否") << "│" << std::endl;
    std::cout << "│ 罚款金额：" << std::left << std::setw(44) << fine         << "│" << std::endl;
    std::cout << "└─────────────────────────────────────────────────────┘" << std::endl;
}

/** @brief 从控制台逐字段输入借阅记录信息 */
void Loan::input() {
    std::cout << "请输入图书编号：";
    std::getline(std::cin, bookId);
    std::cout << "请输入借书证号：";
    std::getline(std::cin, cardId);
    std::cout << "请输入借阅日期（YYYY-MM-DD）：";
    std::getline(std::cin, borrowDate);
    std::cout << "请输入到期日期（YYYY-MM-DD）：";
    std::getline(std::cin, dueDate);
    std::string input;
    std::cout << "是否归还（1=已还 / 0=未还）：";
    std::getline(std::cin, input);
    if (!input.empty()) {
        try { isReturned = (std::stoi(input) != 0); } catch (...) { isReturned = false; }
    }
    std::cout << "是否续借（1=已续 / 0=未续）：";
    std::getline(std::cin, input);
    if (!input.empty()) {
        try { isRenewed = (std::stoi(input) != 0); } catch (...) { isRenewed = false; }
    }
    std::cout << "请输入罚款金额：";
    std::getline(std::cin, input);
    if (!input.empty()) {
        try { fine = std::stod(input); } catch (...) { fine = 0.0; }
    }
}

// ============================================================================
//  文件格式序列化 / 反序列化
// ============================================================================

/**
 * @brief 将当前对象序列化为 txt 格式字符串
 * @return 格式：bookId|cardId|borrowDate|dueDate|isReturned|isRenewed|fine
 */
std::string Loan::toFileString() const {
    std::ostringstream oss;
    oss << bookId      << "|"
        << cardId      << "|"
        << borrowDate  << "|"
        << dueDate     << "|"
        << (isReturned ? "1" : "0") << "|"
        << (isRenewed  ? "1" : "0") << "|"
        << fine;
    return oss.str();
}

/**
 * @brief 从 txt 格式字符串反序列化为 Loan 对象
 * @param  line 一行由 '|' 分隔的 txt 数据
 * @return Loan 对象
 */
Loan Loan::fromFileString(const std::string& line) {
    // 手动按 '|' 拆分字符串
    std::vector<std::string> fields;
    std::string field;
    std::istringstream stream(line);
    while (std::getline(stream, field, '|')) {
        fields.push_back(field);
    }
    // 字段数不足时用默认值补齐
    if (fields.size() < 7) {
        return Loan();
    }
    return Loan(
        fields[0],                          // bookId
        fields[1],                          // cardId
        fields[2],                          // borrowDate
        fields[3],                          // dueDate
        (fields[4] == "1"),                 // isReturned
        (fields[5] == "1"),                 // isRenewed
        std::stod(fields[6])                // fine
    );
}
