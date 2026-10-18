/**
 * @file    ReaderManager.cpp
 * @brief   读者管理业务逻辑实现 —— 增删改查 + 封禁管理
 *
 * 每个方法职责单一：
 *   - 调用方传入参数
 *   - 执行业务校验
 *   - 通过 ReaderRepository 操作数据
 *   - 返回成功/失败
 *
 * 不涉及 Loan，只操作 ReaderRepository。
 */

#include "../../include/managers/ReaderManager.h"

// ============================================================================
//  构造函数
// ============================================================================

/** @brief 构造函数 —— 绑定读者数据仓库引用 */
ReaderManager::ReaderManager(ReaderRepository& readerRepo)
    : readerRepo(readerRepo)
{
}

// ============================================================================
//  错误信息
// ============================================================================

/** @brief 获取最近一次操作的错误信息 */
std::string ReaderManager::getLastError() const {
    return lastError;
}

/** @brief 设置错误信息（内部辅助） */
void ReaderManager::setError(const std::string& msg) {
    lastError = msg;
}

// ============================================================================
//  业务校验
// ============================================================================

/**
 * @brief 校验 Reader 数据的合法性
 * @details 校验规则：
 *          - cardId、name 不能为空
 *          - identity 只允许 "学生" / "教师"
 *          - currentBorrow >= 0
 * @param  reader 待校验的读者对象
 * @return 合法返回 true
 */
bool ReaderManager::validateReader(const Reader& reader) {
    // 借书证号不能为空
    if (reader.cardId.empty()) {
        setError("借书证号不能为空");
        return false;
    }

    // 姓名不能为空
    if (reader.name.empty()) {
        setError("姓名不能为空");
        return false;
    }

    // 身份只允许两种
    if (reader.identity != "学生" && reader.identity != "教师") {
        setError("身份必须为：学生 / 教师");
        return false;
    }

    // 当前借阅数量 >= 0
    if (reader.currentBorrow < 0) {
        setError("当前借阅数量不能为负数");
        return false;
    }

    return true;
}

// ============================================================================
//  新增读者
// ============================================================================

/**
 * @brief 新增读者
 * @details 1. 校验数据合法性
 *          2. 检查 cardId 是否已存在
 *          3. 调用 ReaderRepository.add()
 */
bool ReaderManager::addReader(const Reader& reader) {
    // 步骤1：校验数据
    if (!validateReader(reader)) {
        // validateReader 内部已设置 lastError
        return false;
    }

    // 步骤2：检查借书证号是否重复
    if (readerRepo.exists(reader.cardId)) {
        setError("借书证号 [" + reader.cardId + "] 已存在，不能重复添加");
        return false;
    }

    // 步骤3：添加到数据仓库
    if (!readerRepo.add(reader)) {
        setError("添加读者失败");
        return false;
    }

    return true;
}

// ============================================================================
//  删除读者
// ============================================================================

/**
 * @brief 删除读者
 * @details 1. 检查读者是否存在
 *          2. 调用 ReaderRepository.remove()
 */
bool ReaderManager::removeReader(const std::string& cardId) {
    // 步骤1：检查读者是否存在
    if (!readerRepo.exists(cardId)) {
        setError("借书证号 [" + cardId + "] 不存在");
        return false;
    }

    // 步骤2：从数据仓库删除
    if (!readerRepo.remove(cardId)) {
        setError("删除读者失败");
        return false;
    }

    return true;
}

// ============================================================================
//  修改读者
// ============================================================================

/**
 * @brief 修改读者信息
 * @details 1. 校验新数据合法性
 *          2. 检查原读者是否存在
 *          3. 如果修改了 cardId，检查新编号是否冲突
 *          4. 调用 ReaderRepository.update()
 */
bool ReaderManager::updateReader(const std::string& cardId, const Reader& newInfo) {
    // 步骤1：校验新数据
    if (!validateReader(newInfo)) {
        return false;
    }

    // 步骤2：检查原读者是否存在
    if (!readerRepo.exists(cardId)) {
        setError("借书证号 [" + cardId + "] 不存在");
        return false;
    }

    // 步骤3：如果修改了 cardId，检查新编号是否与已有读者冲突
    if (cardId != newInfo.cardId && readerRepo.exists(newInfo.cardId)) {
        setError("新借书证号 [" + newInfo.cardId + "] 已存在");
        return false;
    }

    // 步骤4：更新数据仓库
    if (!readerRepo.update(newInfo)) {
        setError("修改读者信息失败");
        return false;
    }

    return true;
}

// ============================================================================
//  查询读者
// ============================================================================

/**
 * @brief 按借书证号精确查找读者
 * @details 通过 ReaderRepository.findById() 查找
 */
Reader* ReaderManager::findReader(const std::string& cardId) {
    Reader* reader = readerRepo.findById(cardId);
    if (reader == nullptr) {
        setError("借书证号 [" + cardId + "] 不存在");
    }
    return reader;
}

/**
 * @brief 获取全部读者列表
 * @details 返回 ReaderRepository 中全部读者的副本
 */
std::vector<Reader> ReaderManager::listAllReaders() const {
    // 返回副本，避免外部直接修改仓库数据
    return readerRepo.getAll();
}

// ============================================================================
//  封禁读者
// ============================================================================

/**
 * @brief 封禁读者
 * @details 1. 检查读者是否存在
 *          2. 检查是否已被封禁
 *          3. 设置 isBanned = true
 *          4. 调用 ReaderRepository.update() 持久化
 */
bool ReaderManager::banReader(const std::string& cardId) {
    // 步骤1：检查读者是否存在
    Reader* reader = readerRepo.findById(cardId);
    if (reader == nullptr) {
        setError("借书证号 [" + cardId + "] 不存在");
        return false;
    }

    // 步骤2：检查是否已被封禁
    if (reader->isBanned) {
        setError("读者 [" + reader->name + "] 已被封禁，无需重复操作");
        return false;
    }

    // 步骤3：设置封禁状态
    reader->isBanned = true;

    // 步骤4：持久化
    if (!readerRepo.update(*reader)) {
        reader->isBanned = false;  // 回滚
        setError("封禁操作失败");
        return false;
    }

    return true;
}

// ============================================================================
//  解除封禁
// ============================================================================

/**
 * @brief 解除封禁
 * @details 1. 检查读者是否存在
 *          2. 检查是否未被封禁
 *          3. 设置 isBanned = false
 *          4. 调用 ReaderRepository.update() 持久化
 */
bool ReaderManager::unbanReader(const std::string& cardId) {
    // 步骤1：检查读者是否存在
    Reader* reader = readerRepo.findById(cardId);
    if (reader == nullptr) {
        setError("借书证号 [" + cardId + "] 不存在");
        return false;
    }

    // 步骤2：检查是否未被封禁
    if (!reader->isBanned) {
        setError("读者 [" + reader->name + "] 未被封禁，无需解除");
        return false;
    }

    // 步骤3：取消封禁状态
    reader->isBanned = false;

    // 步骤4：持久化
    if (!readerRepo.update(*reader)) {
        reader->isBanned = true;  // 回滚
        setError("解除封禁操作失败");
        return false;
    }

    return true;
}
