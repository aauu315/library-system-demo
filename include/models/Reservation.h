/**
 * @file    Reservation.h
 * @brief   预约记录数据模型 —— 纯数据容器，不含任何业务逻辑
 *
 * 本类为扩展模块数据模型。
 * 所有字段公有，便于 Repository 和 Manager 直接访问。
 * 每本书维护一个 FIFO 预约队列（std::deque<Reservation>）。
 */

#ifndef RESERVATION_H
#define RESERVATION_H

#include <string>

class Reservation {
public:
    // ========================================================================
    //  构造函数
    // ========================================================================

    /** @brief 默认构造函数 —— 所有字段初始化为默认值 */
    Reservation();

    /** @brief 带参构造函数 —— 初始化全部字段 */
    Reservation(const std::string& bId,
                const std::string& cId,
                const std::string& rDate);

    // ========================================================================
    //  Getter 方法
    // ========================================================================

    std::string getBookId()      const;
    std::string getCardId()      const;
    std::string getReserveDate() const;

    // ========================================================================
    //  Setter 方法
    // ========================================================================

    void setBookId(const std::string& bId);
    void setCardId(const std::string& cId);
    void setReserveDate(const std::string& rDate);

    // ========================================================================
    //  打印
    // ========================================================================

    /** @brief 打印预约记录信息到控制台 */
    void print() const;

    // ========================================================================
    //  文件格式序列化 / 反序列化
    // ========================================================================

    /**
     * @brief 将当前对象序列化为 txt 格式字符串
     * @return 格式：bookId|cardId|reserveDate
     */
    std::string toFileString() const;

    /**
     * @brief 从 txt 格式字符串反序列化为 Reservation 对象
     * @param  line 一行 txt 数据
     * @return Reservation 对象
     */
    static Reservation fromFileString(const std::string& line);

    // ========================================================================
    //  数据字段（全部公有）
    // ========================================================================

    std::string bookId;       // 图书编号
    std::string cardId;       // 借书证号
    std::string reserveDate;  // 预约日期（YYYY-MM-DD）
};

#endif // RESERVATION_H
