/**
 * @file    Reservation.cpp
 * @brief   预约记录数据模型实现 —— 构造函数、Get/Set、打印、文件序列化
 */

#include "../../include/models/Reservation.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <vector>

// ============================================================================
//  构造函数
// ============================================================================

/** @brief 默认构造函数 —— 所有字段初始化为默认值 */
Reservation::Reservation()
    : bookId("")
    , cardId("")
    , reserveDate("")
{
}

/** @brief 带参构造函数 —— 初始化全部字段 */
Reservation::Reservation(const std::string& bId,
                         const std::string& cId,
                         const std::string& rDate)
    : bookId(bId)
    , cardId(cId)
    , reserveDate(rDate)
{
}

// ============================================================================
//  Getter 方法
// ============================================================================

std::string Reservation::getBookId()      const { return bookId; }
std::string Reservation::getCardId()      const { return cardId; }
std::string Reservation::getReserveDate() const { return reserveDate; }

// ============================================================================
//  Setter 方法
// ============================================================================

void Reservation::setBookId(const std::string& bId)      { bookId = bId; }
void Reservation::setCardId(const std::string& cId)      { cardId = cId; }
void Reservation::setReserveDate(const std::string& rDate) { reserveDate = rDate; }

// ============================================================================
//  打印
// ============================================================================

/** @brief 将预约记录信息打印到控制台 */
void Reservation::print() const {
    std::cout << "  图书编号：" << bookId
              << "  /  借书证号：" << cardId
              << "  /  预约日期：" << reserveDate
              << std::endl;
}

// ============================================================================
//  文件格式序列化 / 反序列化
// ============================================================================

/**
 * @brief 将当前对象序列化为 txt 格式字符串
 * @return 格式：bookId|cardId|reserveDate
 */
std::string Reservation::toFileString() const {
    std::ostringstream oss;
    oss << bookId      << "|"
        << cardId      << "|"
        << reserveDate;
    return oss.str();
}

/**
 * @brief 从 txt 格式字符串反序列化为 Reservation 对象
 * @param  line 一行由 '|' 分隔的 txt 数据
 * @return Reservation 对象
 */
Reservation Reservation::fromFileString(const std::string& line) {
    std::vector<std::string> fields;
    std::string field;
    std::istringstream stream(line);
    while (std::getline(stream, field, '|')) {
        fields.push_back(field);
    }
    if (fields.size() < 3) {
        return Reservation();
    }
    return Reservation(
        fields[0],  // bookId
        fields[1],  // cardId
        fields[2]   // reserveDate
    );
}
