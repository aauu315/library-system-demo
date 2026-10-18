/**
 * @file    ReservationRepository.h
 * @brief   预约记录数据仓库 —— 管理全部预约记录的内存存储
 *
 * 核心数据结构：
 *   - map<string, deque<Reservation>> reservations : 按图书编号索引的预约队列
 *
 * 使用 std::deque 而非 std::queue，原因是：
 *   1. 需要遍历（文件持久化）
 *   2. 需要从队列中删除指定元素（取消预约）
 *
 * 职责边界：
 *   - 只负责内存数据的增删改查
 *   - 不包含任何业务规则判断
 *   - 不进行文件 IO（由 DataManager 处理）
 */

#ifndef RESERVATION_REPOSITORY_H
#define RESERVATION_REPOSITORY_H

#include <string>
#include <vector>
#include <deque>
#include <map>
#include "../models/Reservation.h"

class ReservationRepository {
public:
    // ========================================================================
    //  构造函数
    // ========================================================================

    ReservationRepository();

    // ========================================================================
    //  增删改查接口
    // ========================================================================

    /** @brief 新增预约记录（加入对应图书的队列尾部） */
    bool add(const Reservation& reservation);

    /**
     * @brief 从队列中删除指定的预约记录
     * @param  bookId 图书编号
     * @param  cardId 借书证号
     * @return 找到并删除返回 true，未找到返回 false
     */
    bool remove(const std::string& bookId, const std::string& cardId);

    /**
     * @brief 取出并移除队列头部的预约记录（FIFO 出队）
     * @param  bookId 图书编号
     * @return 队列头部的 Reservation；队列空时返回默认 Reservation（bookId 为空）
     */
    Reservation popNext(const std::string& bookId);

    /**
     * @brief 获取某本书的预约队列指针（只读查看，不修改）
     * @param  bookId 图书编号
     * @return 预约队列指针；不存在时返回 nullptr
     */
    const std::deque<Reservation>* findByBookId(const std::string& bookId) const;

    /**
     * @brief 获取某读者的全部预约记录
     * @param  cardId 借书证号
     * @return 预约记录 vector
     */
    std::vector<Reservation> findByCardId(const std::string& cardId) const;

    /**
     * @brief 获取全部预约记录（扁平化，用于文件持久化）
     * @return 全部预约记录 vector
     */
    std::vector<Reservation> getAll() const;

    /**
     * @brief 检查某读者是否已预约某本书
     * @param  bookId 图书编号
     * @param  cardId 借书证号
     * @return 已预约返回 true
     */
    bool hasReservation(const std::string& bookId, const std::string& cardId) const;

    /**
     * @brief 获取某本书的当前预约队列长度
     * @param  bookId 图书编号
     * @return 队列长度
     */
    int getQueueSize(const std::string& bookId) const;

    /** @brief 所有预约队列是否为空 */
    bool isEmpty() const;

private:
    // ========================================================================
    //  成员变量
    // ========================================================================

    std::map<std::string, std::deque<Reservation>> reservations;  // bookId → 预约队列
};

#endif // RESERVATION_REPOSITORY_H
