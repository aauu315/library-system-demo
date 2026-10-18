/**
 * @file    Loan.h
 * @brief   借阅记录数据模型 —— 纯数据容器，不含任何业务逻辑
 *
 * 本类是系统三大核心数据模型之一。
 * Loan 不需提供外部增删改查功能，只能由借书/还书/续借业务自动维护。
 */

#ifndef LOAN_H
#define LOAN_H

#include <string>

class Loan {
public:
    // ========================================================================
    //  构造函数
    // ========================================================================

    /** @brief 默认构造函数 —— 所有字段初始化为默认值 */
    Loan();

    /** @brief 带参构造函数 —— 初始化全部字段 */
    Loan(const std::string& bId,
         const std::string& cId,
         const std::string& bDate,
         const std::string& dDate,
         bool returned,
         bool renewed,
         double f);

    // ========================================================================
    //  Getter 方法
    // ========================================================================

    std::string getBookId()     const;
    std::string getCardId()     const;
    std::string getBorrowDate() const;
    std::string getDueDate()    const;
    bool        getIsReturned() const;
    bool        getIsRenewed()  const;
    double      getFine()       const;

    // ========================================================================
    //  Setter 方法
    // ========================================================================

    void setBookId(const std::string& bId);
    void setCardId(const std::string& cId);
    void setBorrowDate(const std::string& bDate);
    void setDueDate(const std::string& dDate);
    void setIsReturned(bool returned);
    void setIsRenewed(bool renewed);
    void setFine(double f);

    // ========================================================================
    //  打印与输入
    // ========================================================================

    /** @brief 打印借阅记录信息到控制台 */
    void print() const;

    /** @brief 从控制台逐字段输入借阅记录信息 */
    void input();

    // ========================================================================
    //  文件格式序列化 / 反序列化
    // ========================================================================

    /**
     * @brief 将当前对象序列化为 txt 格式字符串
     * @return 格式：bookId|cardId|borrowDate|dueDate|isReturned|isRenewed|fine
     */
    std::string toFileString() const;

    /**
     * @brief 从 txt 格式字符串反序列化为 Loan 对象
     * @param  line 一行 txt 数据
     * @return Loan 对象
     */
    static Loan fromFileString(const std::string& line);

    // ========================================================================
    //  数据字段（全部公有）
    // ========================================================================

    std::string bookId;          // 图书编号
    std::string cardId;          // 借书证号
    std::string borrowDate;      // 借阅日期（格式：YYYY-MM-DD）
    std::string dueDate;         // 到期日期（格式：YYYY-MM-DD）
    bool        isReturned;      // 是否归还
    bool        isRenewed;       // 是否续借（每本书限续借一次）
    double      fine;            // 当前罚款金额（0.00 ~ 10.00）
};

#endif // LOAN_H
