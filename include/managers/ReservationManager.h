/**
 * @file    ReservationManager.h
 * @brief   预约管理业务逻辑 —— 封装预约的全部业务规则
 *
 * 依赖：
 *   - BookRepository     : 查询图书信息（存在性、库存）
 *   - ReaderRepository   : 查询读者信息（存在性、封禁状态）
 *   - LoanRepository     : 查询借阅状态（是否正在借阅）
 *   - ReservationRepository : 预约数据的增删改查
 *
 * 所有业务校验在此层完成，main.cpp 仅负责调用和展示结果。
 */

#ifndef RESERVATION_MANAGER_H
#define RESERVATION_MANAGER_H

#include <string>
#include <vector>
#include <deque>
#include "../models/Reservation.h"
#include "../repositories/BookRepository.h"
#include "../repositories/ReaderRepository.h"
#include "../repositories/LoanRepository.h"
#include "../repositories/ReservationRepository.h"

class ReservationManager {
public:
    // ========================================================================
    //  构造函数
    // ========================================================================

    /**
     * @brief 构造函数 —— 传入三个已有仓库和一个预约仓库的引用
     * @param  bookRepo        图书仓库（只读）
     * @param  readerRepo      读者仓库（只读）
     * @param  loanRepo        借阅仓库（只读）
     * @param  reservationRepo 预约仓库（读写）
     */
    ReservationManager(BookRepository& bookRepo,
                       ReaderRepository& readerRepo,
                       LoanRepository& loanRepo,
                       ReservationRepository& reservationRepo);

    // ========================================================================
    //  预约业务操作
    // ========================================================================

    /**
     * @brief 预约图书
     * @param  bookId  图书编号
     * @param  cardId  借书证号
     * @return 成功返回 true
     *
     * 校验规则：
     *   1. 读者存在且未被封禁
     *   2. 图书存在
     *   3. 库存必须为 0（有库存应直接借书）
     *   4. 读者未正在借阅此书（借了未还不能再预约同一本）
     *   5. 读者未已预约此书（防重复预约）
     */
    bool reserveBook(const std::string& bookId, const std::string& cardId);

    /**
     * @brief 取消预约
     * @param  bookId  图书编号
     * @param  cardId  借书证号
     * @return 成功返回 true
     */
    bool cancelReservation(const std::string& bookId, const std::string& cardId);

    /**
     * @brief 处理还书后的预约通知
     * @param  bookId  刚归还的图书编号
     *
     * 还书成功后调用：
     *   1. 检查该书是否有预约队列
     *   2. 取出队列头部的预约者
     *   3. 如果预约者未被封禁，输出通知信息
     *   （不自动借出，仅通知）
     */
    void processReturnedBook(const std::string& bookId);

    // ========================================================================
    //  查询
    // ========================================================================

    /** @brief 获取全部预约记录（扁平化列表） */
    std::vector<Reservation> listReservations() const;

    /** @brief 获取某本书的预约队列（用于查看排队情况） */
    const std::deque<Reservation>* getQueueForBook(const std::string& bookId) const;

    // ========================================================================
    //  错误信息
    // ========================================================================

    /** @brief 获取最后一次操作的错误信息 */
    std::string getLastError() const;

private:
    // ========================================================================
    //  内部辅助
    // ========================================================================

    /** @brief 设置错误信息 */
    void setError(const std::string& error);

    // ========================================================================
    //  成员变量
    // ========================================================================

    BookRepository&        bookRepo;
    ReaderRepository&      readerRepo;
    LoanRepository&        loanRepo;
    ReservationRepository& reservationRepo;

    std::string lastError;
};

#endif // RESERVATION_MANAGER_H
