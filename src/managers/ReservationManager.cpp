/**
 * @file    ReservationManager.cpp
 * @brief   预约管理业务逻辑实现
 */

#include "../../include/managers/ReservationManager.h"
#include "../../include/utils/DateUtils.h"
#include <iostream>

// ============================================================================
//  构造函数
// ============================================================================

ReservationManager::ReservationManager(BookRepository& bookRepo,
                                       ReaderRepository& readerRepo,
                                       LoanRepository& loanRepo,
                                       ReservationRepository& reservationRepo)
    : bookRepo(bookRepo)
    , readerRepo(readerRepo)
    , loanRepo(loanRepo)
    , reservationRepo(reservationRepo)
    , lastError("")
{
}

// ============================================================================
//  预约业务操作
// ============================================================================

bool ReservationManager::reserveBook(const std::string& bookId, const std::string& cardId) {
    // 1. 检查读者存在
    Reader* reader = readerRepo.findById(cardId);
    if (reader == nullptr) {
        setError("借书证号 [" + cardId + "] 不存在");
        return false;
    }

    // 2. 检查读者未被封禁
    if (reader->isBanned) {
        setError("读者 [" + cardId + "] 已被封禁，无法预约");
        return false;
    }

    // 3. 检查图书存在
    Book* book = bookRepo.findById(bookId);
    if (book == nullptr) {
        setError("图书编号 [" + bookId + "] 不存在");
        return false;
    }

    // 4. 库存不为 0 时应直接借书
    if (book->currentStock > 0) {
        setError("图书 [" + bookId + "] 当前库存充足（" +
                 std::to_string(book->currentStock) + "册），请直接借书");
        return false;
    }

    // 5. 检查读者是否正在借阅此书（已借未还）
    Loan* activeLoan = loanRepo.findActive(bookId, cardId);
    if (activeLoan != nullptr) {
        setError("读者 [" + cardId + "] 已借阅此书且未归还，无需预约");
        return false;
    }

    // 6. 检查是否已预约过
    if (reservationRepo.hasReservation(bookId, cardId)) {
        setError("读者 [" + cardId + "] 已预约过此书，请勿重复预约。当前排队位置：" +
                 std::to_string(reservationRepo.getQueueSize(bookId)));
        return false;
    }

    // 7. 创建预约记录并加入队列
    std::string today = DateUtils::getToday();
    Reservation res(bookId, cardId, today);
    reservationRepo.add(res);

    int position = reservationRepo.getQueueSize(bookId);
    setError("");  // 清除错误
    return true;
}

bool ReservationManager::cancelReservation(const std::string& bookId, const std::string& cardId) {
    if (!reservationRepo.hasReservation(bookId, cardId)) {
        setError("未找到读者 [" + cardId + "] 对图书 [" + bookId + "] 的预约记录");
        return false;
    }
    reservationRepo.remove(bookId, cardId);
    return true;
}

void ReservationManager::processReturnedBook(const std::string& bookId) {
    Reservation next = reservationRepo.popNext(bookId);
    if (next.bookId.empty()) {
        return;  // 无预约记录
    }

    std::cout << std::endl;
    std::cout << "==================================================" << std::endl;
    std::cout << "  [预约通知] 图书 [" << bookId << "] 已归还" << std::endl;

    // 检查预约者是否仍有效（未被封禁）
    Reader* reader = readerRepo.findById(next.cardId);
    if (reader == nullptr) {
        std::cout << "  预约者 [" << next.cardId << "] 已不存在" << std::endl;
    } else if (reader->isBanned) {
        std::cout << "  预约者 [" << next.cardId << "] 已被封禁，预约自动取消" << std::endl;
    } else {
        std::cout << "  请通知读者 [" << next.cardId << "] 前来借阅" << std::endl;
        std::cout << "  预约日期：" << next.reserveDate << std::endl;
        std::cout << "  （该预约已出队，读者需自行到借书界面借阅）" << std::endl;
    }
    std::cout << "==================================================" << std::endl;
}

// ============================================================================
//  查询
// ============================================================================

std::vector<Reservation> ReservationManager::listReservations() const {
    return reservationRepo.getAll();
}

const std::deque<Reservation>* ReservationManager::getQueueForBook(const std::string& bookId) const {
    return reservationRepo.findByBookId(bookId);
}

// ============================================================================
//  错误信息
// ============================================================================

std::string ReservationManager::getLastError() const {
    return lastError;
}

void ReservationManager::setError(const std::string& error) {
    lastError = error;
}
