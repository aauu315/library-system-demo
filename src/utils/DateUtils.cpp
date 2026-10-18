/**
 * @file    DateUtils.cpp
 * @brief   日期工具实现 —— 以天为单位运算，考虑闰年
 *
 * 核心思路：将日期转为"绝对天数"（从 0000-01-01 起算），
 *           加减和比较都基于天数差值。
 */

#include "../../include/utils/DateUtils.h"
#include <ctime>
#include <sstream>
#include <iomanip>

// ============================================================================
//  内部常量
// ============================================================================

// 平年每月天数（1-indexed）
static const int DAYS_IN_MONTH[13] = {
    0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
};

// ============================================================================
//  getToday —— 获取当前系统日期
// ============================================================================

std::string DateUtils::getToday() {
    std::time_t now = std::time(nullptr);
    std::tm* local = std::localtime(&now);
    std::ostringstream oss;
    oss << (local->tm_year + 1900) << "-"
        << std::setw(2) << std::setfill('0') << (local->tm_mon + 1) << "-"
        << std::setw(2) << std::setfill('0') << local->tm_mday;
    return oss.str();
}

// ============================================================================
//  parseDate —— "YYYY-MM-DD" → y, m, d
// ============================================================================

bool DateUtils::parseDate(const std::string& date, int& year, int& month, int& day) {
    if (date.size() != 10)                         return false;
    if (date[4] != '-' || date[7] != '-')          return false;
    for (int i = 0; i < 10; ++i) {
        if (i == 4 || i == 7) continue;
        if (date[i] < '0' || date[i] > '9')        return false;
    }
    year  = std::stoi(date.substr(0, 4));
    month = std::stoi(date.substr(5, 2));
    day   = std::stoi(date.substr(8, 2));
    if (month < 1 || month > 12)                   return false;
    if (day < 1 || day > daysInMonth(year, month)) return false;
    return true;
}

// ============================================================================
//  isLeapYear —— 闰年判断
// ============================================================================

bool DateUtils::isLeapYear(int year) {
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

// ============================================================================
//  daysInMonth —— 某年某月天数
// ============================================================================

int DateUtils::daysInMonth(int year, int month) {
    if (month == 2 && isLeapYear(year)) {
        return 29;
    }
    return DAYS_IN_MONTH[month];
}

// ============================================================================
//  dateToDays —— 将日期转为绝对天数（从 0000-01-01 起算）
// ============================================================================

int DateUtils::dateToDays(int year, int month, int day) {
    int total = 0;
    // 累加整年天数
    for (int y = 0; y < year; ++y) {
        total += isLeapYear(y) ? 366 : 365;
    }
    // 累加整月天数
    for (int m = 1; m < month; ++m) {
        total += daysInMonth(year, m);
    }
    // 累加当月天数
    total += day;
    return total;
}

// ============================================================================
//  daysToDate —— 将绝对天数转回 y/m/d
// ============================================================================

void DateUtils::daysToDate(int totalDays, int& year, int& month, int& day) {
    year = 0;
    while (true) {
        int daysInYear = isLeapYear(year) ? 366 : 365;
        if (totalDays <= daysInYear) break;
        totalDays -= daysInYear;
        year++;
    }
    month = 1;
    while (true) {
        int dim = daysInMonth(year, month);
        if (totalDays <= dim) break;
        totalDays -= dim;
        month++;
    }
    day = totalDays;
}

// ============================================================================
//  daysBetween —— d2 - d1
// ============================================================================

int DateUtils::daysBetween(const std::string& d1, const std::string& d2) {
    int y1, m1, d1d, y2, m2, d2d;
    if (!parseDate(d1, y1, m1, d1d)) return 0;
    if (!parseDate(d2, y2, m2, d2d)) return 0;
    return dateToDays(y2, m2, d2d) - dateToDays(y1, m1, d1d);
}

// ============================================================================
//  addDays —— date + n 天
// ============================================================================

std::string DateUtils::addDays(const std::string& date, int days) {
    int y, m, d;
    if (!parseDate(date, y, m, d)) return date;
    int total = dateToDays(y, m, d) + days;
    daysToDate(total, y, m, d);
    std::ostringstream oss;
    oss << y << "-"
        << std::setw(2) << std::setfill('0') << m << "-"
        << std::setw(2) << std::setfill('0') << d;
    return oss.str();
}

// ============================================================================
//  isBefore —— d1 < d2 ?
// ============================================================================

bool DateUtils::isBefore(const std::string& d1, const std::string& d2) {
    return daysBetween(d1, d2) > 0;
}

// ============================================================================
//  isAfter —— d1 > d2 ?
// ============================================================================

bool DateUtils::isAfter(const std::string& d1, const std::string& d2) {
    return daysBetween(d2, d1) > 0;  // d1 > d2 ⇔ d2 到 d1 为正
}

// ============================================================================
//  isValidDate —— 格式 + 真实性双重校验
// ============================================================================

bool DateUtils::isValidDate(const std::string& date) {
    int y, m, d;
    return parseDate(date, y, m, d);
}
