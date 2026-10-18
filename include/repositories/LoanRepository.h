/**
 * @file    LoanRepository.h
 * @brief   借阅记录数据仓库 —— 管理全部借阅记录的内存存储
 *
 * 核心数据结构：
 *   - list<Loan> loans : 借阅记录主存储（std::list，适合频繁增删）
 *
 * 职责边界：
 *   - 只负责内存数据的增删改查
 *   - 不包含任何业务规则判断
 *   - 不进行文件 IO（由 FileIO 处理）
 *
 * 设计理由：
 *   - 借阅记录频繁增删，list 的迭代器在插入/删除后不失效
 *   - 遍历操作主要用于统计（性能可接受）
 */

#ifndef LOAN_REPOSITORY_H
#define LOAN_REPOSITORY_H

#include <string>
#include <vector>
#include <list>
#include "../models/Loan.h"

class LoanRepository {
public:
    // ========================================================================
    //  构造函数
    // ========================================================================

    LoanRepository();

    // ========================================================================
    //  增删改查接口
    // ========================================================================

    /** @brief 新增借阅记录 */
    bool add(const Loan& loan);

    /**
     * @brief 删除指定借阅记录
     * @param  loan 要删除的 Loan 对象引用（通过对象内容匹配）
     * @return 成功返回 true
     */
    bool remove(const Loan& loan);

    /**
     * @brief 查找活跃借阅记录（未归还）
     * @param  bookId 图书编号
     * @param  cardId 借书证号
     * @return 找到返回 Loan 指针，未找到返回 nullptr
     */
    Loan* findActive(const std::string& bookId, const std::string& cardId);

    /** @brief 获取全部借阅记录的引用 */
    std::list<Loan>& getAll();

    /**
     * @brief 获取某读者的全部借阅记录（含已归还）
     * @param  cardId 借书证号
     * @return 指针 vector（指向 list 中元素的地址）
     */
    std::vector<Loan*> findByReaderId(const std::string& cardId);

    /**
     * @brief 获取某读者的活跃借阅记录（未归还）
     * @param  cardId 借书证号
     * @return 指针 vector
     */
    std::vector<Loan*> findActiveByReaderId(const std::string& cardId);

    /**
     * @brief 获取某图书相关的全部借阅记录
     * @param  bookId 图书编号
     * @return 指针 vector
     */
    std::vector<Loan*> findByBookId(const std::string& bookId);

    /**
     * @brief 获取全部超期借阅记录（未归还且到期日期早于当前日期）
     * @return 指针 vector
     */
    std::vector<Loan*> findOverdue();

    /**
     * @brief 统计某读者的当前借阅数量（活跃记录数）
     * @param  cardId 借书证号
     * @return 活跃借阅记录数量
     */
    int countActiveByReaderId(const std::string& cardId);

    /**
     * @brief 计算某读者的累计罚款总额
     * @param  cardId 借书证号
     * @return 该读者所有借阅记录的 fine 之和
     */
    double getTotalFineByReaderId(const std::string& cardId);

private:
    // ========================================================================
    //  成员变量
    // ========================================================================

    std::list<Loan> loans;  // 借阅记录主存储（频繁增删场景使用 list）
};

#endif // LOAN_REPOSITORY_H
