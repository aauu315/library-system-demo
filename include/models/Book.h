/**
 * @file    Book.h
 * @brief   图书数据模型 —— 纯数据容器，不含任何业务逻辑
 *
 * 本类是系统三大核心数据模型之一。
 * 所有字段公有，便于 Repository 和 Manager 直接访问。
 * 提供 operator== 和 operator< 用于 BST 比较（按 bookId 字典序）。
 */

#ifndef BOOK_H
#define BOOK_H

#include <string>

class Book {
public:
    // ========================================================================
    //  构造函数
    // ========================================================================

    /** @brief 默认构造函数 —— 所有字段初始化为默认值 */
    Book();

    /** @brief 带参构造函数 —— 初始化全部字段 */
    Book(const std::string& id,
         const std::string& t,
         const std::string& a,
         const std::string& cat,
         int total,
         int current,
         int count);

    // ========================================================================
    //  运算符重载（用于 BST 比较，按图书编号字典序）
    // ========================================================================

    /** @brief 相等比较 —— 图书编号相同即视为同一本书 */
    bool operator==(const Book& other) const;

    /** @brief 小于比较 —— 按图书编号字典序，BST 内部排序使用 */
    bool operator<(const Book& other) const;

    // ========================================================================
    //  Getter 方法
    // ========================================================================

    std::string getBookId()       const;
    std::string getTitle()        const;
    std::string getAuthor()       const;
    std::string getCategory()     const;
    int         getTotalStock()   const;
    int         getCurrentStock() const;
    int         getBorrowCount()  const;

    // ========================================================================
    //  Setter 方法
    // ========================================================================

    void setBookId(const std::string& id);
    void setTitle(const std::string& t);
    void setAuthor(const std::string& a);
    void setCategory(const std::string& cat);
    void setTotalStock(int total);
    void setCurrentStock(int current);
    void setBorrowCount(int count);

    // ========================================================================
    //  打印与输入
    // ========================================================================

    /** @brief 打印图书信息到控制台 */
    void print() const;

    /** @brief 从控制台逐字段输入图书信息 */
    void input();

    /** @brief 修改图书信息 —— 显示当前值，直接回车保留原值 */
    void modify();

    // ========================================================================
    //  文件格式序列化 / 反序列化
    // ========================================================================

    /**
     * @brief 将当前对象序列化为 txt 格式字符串
     * @return 格式：bookId|title|author|category|totalStock|currentStock|borrowCount
     */
    std::string toFileString() const;

    /**
     * @brief 从 txt 格式字符串反序列化为 Book 对象
     * @param  line 一行 txt 数据
     * @return Book 对象
     */
    static Book fromFileString(const std::string& line);

    // ========================================================================
    //  数据字段（全部公有）
    // ========================================================================

    std::string bookId;          // 图书编号（唯一标识）
    std::string title;           // 书名
    std::string author;          // 作者
    std::string category;        // 图书类别："书籍" / "期刊" / "报刊"
    int         totalStock;      // 总库存
    int         currentStock;    // 当前库存
    int         borrowCount;     // 借阅次数（排行榜使用）
};

#endif // BOOK_H
