/**
 * @file    ReaderRepository.h
 * @brief   读者数据仓库 —— 管理全部读者的内存存储，提供增删改查接口
 *
 * 核心数据结构：
 *   - vector<Reader> readers : 读者主存储（顺序存储，读者数量较少）
 *
 * 职责边界：
 *   - 只负责内存数据的增删改查
 *   - 不包含任何业务规则判断
 *   - 不进行文件 IO（由 FileIO 处理）
 */

#ifndef READER_REPOSITORY_H
#define READER_REPOSITORY_H

#include <string>
#include <vector>
#include "../models/Reader.h"

class ReaderRepository {
public:
    // ========================================================================
    //  构造函数
    // ========================================================================

    ReaderRepository();

    // ========================================================================
    //  增删改查接口
    // ========================================================================

    /** @brief 新增读者 */
    bool add(const Reader& reader);

    /** @brief 删除读者（按借书证号） */
    bool remove(const std::string& cardId);

    /** @brief 按借书证号查找读者 */
    Reader* findById(const std::string& cardId);

    /** @brief 获取全部读者的引用 */
    std::vector<Reader>& getAll();

    /** @brief 更新读者信息（以 cardId 定位） */
    bool update(const Reader& reader);

    /** @brief 检查读者是否存在 */
    bool exists(const std::string& cardId);

private:
    // ========================================================================
    //  内部辅助方法
    // ========================================================================

    /** @brief 在 vector 中按 cardId 线性查找，返回下标 */
    int findIndexInVector(const std::string& cardId) const;

    // ========================================================================
    //  成员变量
    // ========================================================================

    std::vector<Reader> readers;  // 读者主存储
};

#endif // READER_REPOSITORY_H
