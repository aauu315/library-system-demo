/**
 * @file    ReaderRepository.cpp
 * @brief   读者数据仓库实现 —— vector 存储，线性查找
 */

#include "../../include/repositories/ReaderRepository.h"

// ============================================================================
//  构造函数
// ============================================================================

ReaderRepository::ReaderRepository() {
}

// ============================================================================
//  新增
// ============================================================================

bool ReaderRepository::add(const Reader& reader) {
    if (exists(reader.cardId)) {
        return false;
    }
    readers.push_back(reader);
    return true;
}

// ============================================================================
//  删除
// ============================================================================

bool ReaderRepository::remove(const std::string& cardId) {
    int idx = findIndexInVector(cardId);
    if (idx < 0) {
        return false;
    }
    readers.erase(readers.begin() + idx);
    return true;
}

// ============================================================================
//  按借书证号查找
// ============================================================================

Reader* ReaderRepository::findById(const std::string& cardId) {
    for (auto& reader : readers) {
        if (reader.cardId == cardId) {
            return &reader;
        }
    }
    return nullptr;
}

// ============================================================================
//  获取全部
// ============================================================================

std::vector<Reader>& ReaderRepository::getAll() {
    return readers;
}

// ============================================================================
//  更新
// ============================================================================

bool ReaderRepository::update(const Reader& reader) {
    int idx = findIndexInVector(reader.cardId);
    if (idx < 0) {
        return false;
    }
    readers[idx] = reader;
    return true;
}

// ============================================================================
//  存在性检查
// ============================================================================

bool ReaderRepository::exists(const std::string& cardId) {
    return findIndexInVector(cardId) >= 0;
}

// ============================================================================
//  内部：在 vector 中按 cardId 查找下标
// ============================================================================

int ReaderRepository::findIndexInVector(const std::string& cardId) const {
    for (size_t i = 0; i < readers.size(); ++i) {
        if (readers[i].cardId == cardId) {
            return static_cast<int>(i);
        }
    }
    return -1;
}
