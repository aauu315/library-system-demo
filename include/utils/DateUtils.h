/**
 * @file    DateUtils.h
 * @brief   日期工具类 —— 纯静态方法，提供日期计算与校验功能
 *
 * 所有日期均以 "YYYY-MM-DD" 格式字符串表示。
 * 内部实现考虑闰年、各月天数差异、跨月跨年场景。
 */

#ifndef DATE_UTILS_H
#define DATE_UTILS_H

#include <string>

class DateUtils {
public:
    // ========================================================================
    //  日期获取
    // ========================================================================

    /** @brief 获取当前系统日期，格式为 "YYYY-MM-DD" */
    static std::string getToday();

    // ========================================================================
    //  日期运算
    // ========================================================================

    /** @brief 计算两天之间的天数差 = d2 - d1（返回值可为负数） */
    static int daysBetween(const std::string& d1, const std::string& d2);

    /** @brief 日期 + 天数 → 新日期字符串 */
    static std::string addDays(const std::string& date, int days);

    // ========================================================================
    //  日期比较
    // ========================================================================

    /** @brief 判断 d1 是否在 d2 之前（不含相等） */
    static bool isBefore(const std::string& d1, const std::string& d2);

    /** @brief 判断 d1 是否在 d2 之后（不含相等） */
    static bool isAfter(const std::string& d1, const std::string& d2);

    // ========================================================================
    //  日期校验
    // ========================================================================

    /** @brief 校验日期格式是否合法（格式 YYYY-MM-DD 且日期真实存在） */
    static bool isValidDate(const std::string& date);

private:
    // ========================================================================
    //  内部辅助方法
    // ========================================================================

    /** @brief 将 "YYYY-MM-DD" 字符串解析为 年/月/日 三个整数 */
    static bool parseDate(const std::string& date, int& year, int& month, int& day);

    /** @brief 判断是否为闰年 */
    static bool isLeapYear(int year);

    /** @brief 获取某年某月的天数 */
    static int daysInMonth(int year, int month);

    /** @brief 将日期转换为从某个纪元开始的天数（用于差值计算） */
    static int dateToDays(int year, int month, int day);

    /** @brief 将天数转换回年/月/日 */
    static void daysToDate(int totalDays, int& year, int& month, int& day);

    // 禁止实例化
    DateUtils() = delete;
};

#endif // DATE_UTILS_H
