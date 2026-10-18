/**
 * @file    BookRepository.cpp
 * @brief   图书数据仓库实现 —— vector 主存储 + BST 索引
 *
 * 同步规则：add/remove/update 操作必须同时维护 vector 和 BST。
 * 查找：按 bookId 走 BST 索引（O(log n)）。
 */

#include "../../include/repositories/BookRepository.h"

// ============================================================================
//  构造函数
// ============================================================================

BookRepository::BookRepository() {
}

// ============================================================================
//  新增
// ============================================================================

bool BookRepository::add(const Book& book) {
    // 重复检查
    if (exists(book.bookId)) {
        return false;
    }
    // 同步写入 vector 和 BST
    books.push_back(book);
    bookIndex.insert(book);
    return true;
}

// ============================================================================
//  删除
// ============================================================================

bool BookRepository::remove(const std::string& bookId) {
    int idx = findIndexInVector(bookId);
    if (idx < 0) {
        return false;
    }
    // 同步从 BST 和 vector 删除
    bookIndex.remove(books[idx]);
    books.erase(books.begin() + idx);
    return true;
}

// ============================================================================
//  按编号查找（走 BST）
// ============================================================================

Book* BookRepository::findById(const std::string& bookId) {
    // 构造一个只有 bookId 的临时对象用于 BST 查找
    Book key(bookId, "", "", "", 0, 0, 0);
    return bookIndex.search(key);
}

// ============================================================================
//  获取全部
// ============================================================================

std::vector<Book>& BookRepository::getAll() {
    return books;
}

// ============================================================================
//  更新
// ============================================================================

bool BookRepository::update(const Book& book) {
    int idx = findIndexInVector(book.bookId);
    if (idx < 0) {
        return false;
    }
    Book oldBook = books[idx];          // 保存旧值
    books[idx] = book;                  // 更新 vector
    syncBST(oldBook, book);             // 同步 BST 索引
    return true;
}

// ============================================================================
//  存在性检查
// ============================================================================

bool BookRepository::exists(const std::string& bookId) {
    Book key(bookId, "", "", "", 0, 0, 0);
    return bookIndex.search(key) != nullptr;
}

// ============================================================================
//  内部：在 vector 中按 bookId 查找下标
// ============================================================================

int BookRepository::findIndexInVector(const std::string& bookId) const {
    for (size_t i = 0; i < books.size(); ++i) {
        if (books[i].bookId == bookId) {
            return static_cast<int>(i);
        }
    }
    return -1;  // 未找到
}

// ============================================================================
//  内部：同步 BST —— 删旧插新
// ============================================================================

void BookRepository::syncBST(const Book& oldBook, const Book& newBook) {
    bookIndex.remove(oldBook);
    bookIndex.insert(newBook);
}
