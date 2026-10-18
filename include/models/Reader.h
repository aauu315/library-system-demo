/**
 * @file    Reader.h
 * @brief   读者数据模型 —— 纯数据容器，不含任何业务逻辑
 *
 * 本类是系统三大核心数据模型之一。
 * 所有字段公有，便于 Repository 和 Manager 直接访问。
 */

#ifndef READER_H
#define READER_H

#include <string>

class Reader {
public:
    // ========================================================================
    //  构造函数
    // ========================================================================

    /** @brief 默认构造函数 —— 所有字段初始化为默认值 */
    Reader();

    /** @brief 带参构造函数 —— 初始化全部字段 */
    Reader(const std::string& id,
           const std::string& n,
           const std::string& ident,
           int borrow,
           bool banned);

    // ========================================================================
    //  Getter 方法
    // ========================================================================

    std::string getCardId()       const;
    std::string getName()         const;
    std::string getIdentity()     const;
    int         getCurrentBorrow() const;
    bool        getIsBanned()     const;

    // ========================================================================
    //  Setter 方法
    // ========================================================================

    void setCardId(const std::string& id);
    void setName(const std::string& n);
    void setIdentity(const std::string& ident);
    void setCurrentBorrow(int borrow);
    void setIsBanned(bool banned);

    // ========================================================================
    //  打印与输入
    // ========================================================================

    /** @brief 打印读者信息到控制台 */
    void print() const;

    /** @brief 从控制台逐字段输入读者信息 */
    void input();

    /** @brief 修改读者信息 —— 显示当前值，直接回车保留原值 */
    void modify();

    // ========================================================================
    //  文件格式序列化 / 反序列化
    // ========================================================================

    /**
     * @brief 将当前对象序列化为 txt 格式字符串
     * @return 格式：cardId|name|identity|currentBorrow|isBanned
     */
    std::string toFileString() const;

    /**
     * @brief 从 txt 格式字符串反序列化为 Reader 对象
     * @param  line 一行 txt 数据
     * @return Reader 对象
     */
    static Reader fromFileString(const std::string& line);

    // ========================================================================
    //  数据字段（全部公有）
    // ========================================================================

    std::string cardId;          // 借书证号（唯一标识）
    std::string name;            // 姓名
    std::string identity;        // 身份："学生" / "教师"
    int         currentBorrow;   // 当前借阅数量
    bool        isBanned;        // 是否封禁（true = 已封禁）
};

#endif // READER_H
