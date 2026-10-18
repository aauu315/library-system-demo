/**
 * @file    ReaderManager.h
 * @brief   读者管理业务逻辑 —— 读者 CRUD 与封禁管理
 *
 * 职责：
 *   - 校验读者数据的合法性
 *   - 封装读者增删改查的完整业务流程
 *   - 封禁与解除封禁管理
 *
 * 依赖：
 *   - ReaderRepository 的引用（仅操作读者容器）
 *
 * 禁止：
 *   - 涉及 Loan 相关操作
 *   - 直接操作 ReaderRepository 的内部数据结构
 *   - 包含 UI 相关代码
 */

#ifndef READER_MANAGER_H
#define READER_MANAGER_H

#include <string>
#include <vector>
#include "../models/Reader.h"
#include "../repositories/ReaderRepository.h"

class ReaderManager {
public:
    // ========================================================================
    //  业务常量
    // ========================================================================

    static const int MAX_BORROW_STUDENT = 3;   // 学生最大借阅数
    static const int MAX_BORROW_TEACHER = 5;   // 教师最大借阅数

    // ========================================================================
    //  构造函数
    // ========================================================================

    /** @brief 构造函数 —— 绑定读者数据仓库 */
    explicit ReaderManager(ReaderRepository& readerRepo);

    // ========================================================================
    //  新增读者
    // ========================================================================

    /**
     * @brief 新增读者
     * @details 校验数据合法性 → 检查 cardId 不重复 → 调用 repo.add()
     * @param  reader 读者对象
     * @return 成功返回 true，失败时可通过 getLastError() 获取原因
     */
    bool addReader(const Reader& reader);

    // ========================================================================
    //  删除读者
    // ========================================================================

    /**
     * @brief 删除读者
     * @details 检查读者是否存在 → 调用 repo.remove()
     * @param  cardId 借书证号
     * @return 成功返回 true
     */
    bool removeReader(const std::string& cardId);

    // ========================================================================
    //  修改读者
    // ========================================================================

    /**
     * @brief 修改读者信息
     * @details 校验新数据 → 检查原读者存在 → 检查编号冲突 → 调用 repo.update()
     * @param  cardId  要修改的借书证号
     * @param  newInfo 新的读者信息
     * @return 成功返回 true
     */
    bool updateReader(const std::string& cardId, const Reader& newInfo);

    // ========================================================================
    //  查询读者
    // ========================================================================

    /**
     * @brief 按借书证号精确查找读者
     * @param  cardId 借书证号
     * @return 找到返回 Reader 指针，未找到返回 nullptr
     */
    Reader* findReader(const std::string& cardId);

    /**
     * @brief 获取全部读者列表
     * @return 全部读者的 vector 副本
     */
    std::vector<Reader> listAllReaders() const;

    // ========================================================================
    //  封禁管理
    // ========================================================================

    /**
     * @brief 封禁读者 —— 将指定读者的 isBanned 置为 true
     * @details 检查读者存在 → 设置 isBanned = true → 调用 repo.update()
     * @param  cardId 借书证号
     * @return 成功返回 true
     */
    bool banReader(const std::string& cardId);

    /**
     * @brief 解除封禁 —— 将指定读者的 isBanned 置为 false
     * @details 检查读者存在 → 设置 isBanned = false → 调用 repo.update()
     * @param  cardId 借书证号
     * @return 成功返回 true
     */
    bool unbanReader(const std::string& cardId);

    // ========================================================================
    //  错误信息
    // ========================================================================

    /** @brief 获取最近一次操作的错误信息 */
    std::string getLastError() const;

private:
    // ========================================================================
    //  业务校验（每个方法职责单一）
    // ========================================================================

    /** @brief 校验 Reader 数据的合法性（字段非空、身份合法等） */
    bool validateReader(const Reader& reader);

    /** @brief 设置错误信息（内部辅助） */
    void setError(const std::string& msg);

    // ========================================================================
    //  成员变量
    // ========================================================================

    ReaderRepository& readerRepo;  // 读者数据仓库引用
    std::string       lastError;   // 最近一次错误信息
};

#endif // READER_MANAGER_H
