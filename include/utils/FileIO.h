/**
 * @file    FileIO.h
 * @brief   文件读写工具类 —— 负责 txt 数据文件的加载与保存
 *
 * 纯静态方法类，不实例化。
 * 数据格式：每行一条记录，字段用 '|' 分隔。
 * 异常处理：文件不存在时返回空容器（首次运行场景）；
 *           格式错误的行跳过并输出警告到标准错误。
 */

#ifndef FILE_IO_H
#define FILE_IO_H

#include <string>
#include <vector>
#include <list>
#include "../models/Book.h"
#include "../models/Reader.h"
#include "../models/Loan.h"

class FileIO {
public:
    // ========================================================================
    //  图书文件操作
    // ========================================================================

    /** @brief 从 txt 文件加载全部图书 */
    static std::vector<Book> loadBooks(const std::string& filepath);

    /** @brief 将全部图书保存到 txt 文件 */
    static void saveBooks(const std::string& filepath,
                          const std::vector<Book>& books);

    // ========================================================================
    //  读者文件操作
    // ========================================================================

    /** @brief 从 txt 文件加载全部读者 */
    static std::vector<Reader> loadReaders(const std::string& filepath);

    /** @brief 将全部读者保存到 txt 文件 */
    static void saveReaders(const std::string& filepath,
                            const std::vector<Reader>& readers);

    // ========================================================================
    //  借阅记录文件操作
    // ========================================================================

    /** @brief 从 txt 文件加载全部借阅记录 */
    static std::list<Loan> loadLoans(const std::string& filepath);

    /** @brief 将全部借阅记录保存到 txt 文件 */
    static void saveLoans(const std::string& filepath,
                          const std::list<Loan>& loans);

private:
    // ========================================================================
    //  内部辅助方法
    // ========================================================================

    /** @brief 将字符串按分隔符拆分为多个子串 */
    static std::vector<std::string> split(const std::string& str, char delimiter);

    /** @brief 去除字符串首尾空白字符 */
    static std::string trim(const std::string& str);

    // 禁止实例化
    FileIO() = delete;
};

#endif // FILE_IO_H
