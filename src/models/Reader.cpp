/**
 * @file    Reader.cpp
 * @brief   读者数据模型实现 —— 构造函数、Get/Set、打印、文件序列化
 */

#include "../../include/models/Reader.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <vector>

// ============================================================================
//  构造函数
// ============================================================================

/** @brief 默认构造函数 —— 所有字段初始化为默认值 */
Reader::Reader()
    : cardId("")
    , name("")
    , identity("")
    , currentBorrow(0)
    , isBanned(false)
{
}

/** @brief 带参构造函数 —— 初始化全部字段 */
Reader::Reader(const std::string& id,
               const std::string& n,
               const std::string& ident,
               int borrow,
               bool banned)
    : cardId(id)
    , name(n)
    , identity(ident)
    , currentBorrow(borrow)
    , isBanned(banned)
{
}

// ============================================================================
//  Getter 方法
// ============================================================================

std::string Reader::getCardId()        const { return cardId; }
std::string Reader::getName()          const { return name; }
std::string Reader::getIdentity()      const { return identity; }
int         Reader::getCurrentBorrow() const { return currentBorrow; }
bool        Reader::getIsBanned()      const { return isBanned; }

// ============================================================================
//  Setter 方法
// ============================================================================

void Reader::setCardId(const std::string& id)   { cardId = id; }
void Reader::setName(const std::string& n)       { name = n; }
void Reader::setIdentity(const std::string& ident){ identity = ident; }
void Reader::setCurrentBorrow(int borrow)         { currentBorrow = borrow; }
void Reader::setIsBanned(bool banned)             { isBanned = banned; }

// ============================================================================
//  打印与输入
// ============================================================================

/** @brief 将读者信息以表格形式打印到控制台 */
void Reader::print() const {
    std::cout << "┌─────────────────────────────────────────────────────┐" << std::endl;
    std::cout << "│ 借书证号：" << std::left << std::setw(44) << cardId         << "│" << std::endl;
    std::cout << "│ 姓名    ：" << std::left << std::setw(44) << name           << "│" << std::endl;
    std::cout << "│ 身份    ：" << std::left << std::setw(44) << identity       << "│" << std::endl;
    std::cout << "│ 借阅数量：" << std::left << std::setw(44) << currentBorrow  << "│" << std::endl;
    std::cout << "│ 是否封禁：" << std::left << std::setw(44);
    std::cout << (isBanned ? "是" : "否") << "│" << std::endl;
    std::cout << "└─────────────────────────────────────────────────────┘" << std::endl;
}

/** @brief 从控制台逐字段输入读者信息 */
void Reader::input() {
    std::cout << "请输入借书证号：";
    std::getline(std::cin, cardId);
    std::cout << "请输入姓名：";
    std::getline(std::cin, name);
    std::cout << "请输入身份（学生/教师）：";
    std::getline(std::cin, identity);
    std::string input;
    std::cout << "请输入当前借阅数量：";
    std::getline(std::cin, input);
    if (!input.empty()) {
        try { currentBorrow = std::stoi(input); } catch (...) { currentBorrow = 0; }
    }
    std::cout << "是否封禁（1=是 / 0=否）：";
    std::getline(std::cin, input);
    if (!input.empty()) {
        try { isBanned = (std::stoi(input) != 0); } catch (...) { isBanned = false; }
    }
}

/** @brief 修改读者信息 —— 显示当前值，直接回车保留原值 */
void Reader::modify() {
    std::string input;
    std::cout << "请输入借书证号 [" << cardId << "]：";
    std::getline(std::cin, input);
    if (!input.empty()) cardId = input;

    std::cout << "请输入姓名 [" << name << "]：";
    std::getline(std::cin, input);
    if (!input.empty()) name = input;

    std::cout << "请输入身份（学生/教师） [" << identity << "]：";
    std::getline(std::cin, input);
    if (!input.empty()) identity = input;

    std::cout << "请输入当前借阅数量 [" << currentBorrow << "]：";
    std::getline(std::cin, input);
    if (!input.empty()) currentBorrow = std::stoi(input);

    std::cout << "是否封禁（1=是 / 0=否） [" << (isBanned ? "1" : "0") << "]：";
    std::getline(std::cin, input);
    if (!input.empty()) isBanned = (input == "1");
}

// ============================================================================
//  文件格式序列化 / 反序列化
// ============================================================================

/**
 * @brief 将当前对象序列化为 txt 格式字符串
 * @return 格式：cardId|name|identity|currentBorrow|isBanned
 */
std::string Reader::toFileString() const {
    std::ostringstream oss;
    oss << cardId        << "|"
        << name          << "|"
        << identity      << "|"
        << currentBorrow << "|"
        << (isBanned ? "1" : "0");
    return oss.str();
}

/**
 * @brief 从 txt 格式字符串反序列化为 Reader 对象
 * @param  line 一行由 '|' 分隔的 txt 数据
 * @return Reader 对象
 */
Reader Reader::fromFileString(const std::string& line) {
    // 手动按 '|' 拆分字符串
    std::vector<std::string> fields;
    std::string field;
    std::istringstream stream(line);
    while (std::getline(stream, field, '|')) {
        fields.push_back(field);
    }
    // 字段数不足时用默认值补齐
    if (fields.size() < 5) {
        return Reader();
    }
    return Reader(
        fields[0],                          // cardId
        fields[1],                          // name
        fields[2],                          // identity
        std::stoi(fields[3]),               // currentBorrow
        (fields[4] == "1")                  // isBanned
    );
}
