/**
 * @file    ReservationRepository.cpp
 * @brief   预约记录数据仓库实现
 */

#include "../../include/repositories/ReservationRepository.h"

// ============================================================================
//  构造函数
// ============================================================================

ReservationRepository::ReservationRepository() {
}

// ============================================================================
//  增删改查
// ============================================================================

/** @brief 新增预约记录（加入对应图书的队列尾部） */
bool ReservationRepository::add(const Reservation& reservation) {
    reservations[reservation.bookId].push_back(reservation);
    return true;
}

/** @brief 从队列中删除指定的预约记录 */
bool ReservationRepository::remove(const std::string& bookId, const std::string& cardId) {
    auto it = reservations.find(bookId);
    if (it == reservations.end()) {
        return false;
    }
    std::deque<Reservation>& queue = it->second;
    for (auto qi = queue.begin(); qi != queue.end(); ++qi) {
        if (qi->cardId == cardId) {
            queue.erase(qi);
            // 队列为空时删除该 key，保持 map 整洁
            if (queue.empty()) {
                reservations.erase(it);
            }
            return true;
        }
    }
    return false;
}

/** @brief 取出并移除队列头部的预约记录（FIFO 出队） */
Reservation ReservationRepository::popNext(const std::string& bookId) {
    auto it = reservations.find(bookId);
    if (it == reservations.end() || it->second.empty()) {
        return Reservation();
    }
    Reservation front = it->second.front();
    it->second.pop_front();
    if (it->second.empty()) {
        reservations.erase(it);
    }
    return front;
}

/** @brief 获取某本书的预约队列指针（只读查看） */
const std::deque<Reservation>* ReservationRepository::findByBookId(const std::string& bookId) const {
    auto it = reservations.find(bookId);
    if (it == reservations.end()) {
        return nullptr;
    }
    return &(it->second);
}

/** @brief 获取某读者的全部预约记录 */
std::vector<Reservation> ReservationRepository::findByCardId(const std::string& cardId) const {
    std::vector<Reservation> result;
    for (const auto& pair : reservations) {
        for (const auto& res : pair.second) {
            if (res.cardId == cardId) {
                result.push_back(res);
            }
        }
    }
    return result;
}

/** @brief 获取全部预约记录（扁平化，用于文件持久化） */
std::vector<Reservation> ReservationRepository::getAll() const {
    std::vector<Reservation> result;
    for (const auto& pair : reservations) {
        for (const auto& res : pair.second) {
            result.push_back(res);
        }
    }
    return result;
}

/** @brief 检查某读者是否已预约某本书 */
bool ReservationRepository::hasReservation(const std::string& bookId, const std::string& cardId) const {
    auto it = reservations.find(bookId);
    if (it == reservations.end()) {
        return false;
    }
    for (const auto& res : it->second) {
        if (res.cardId == cardId) {
            return true;
        }
    }
    return false;
}

/** @brief 获取某本书的当前预约队列长度 */
int ReservationRepository::getQueueSize(const std::string& bookId) const {
    auto it = reservations.find(bookId);
    if (it == reservations.end()) {
        return 0;
    }
    return static_cast<int>(it->second.size());
}

/** @brief 所有预约队列是否为空 */
bool ReservationRepository::isEmpty() const {
    return reservations.empty();
}
