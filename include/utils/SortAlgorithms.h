/**
 * @file    SortAlgorithms.h
 * @brief   排序算法工具类 —— 提供经典快速排序实现
 *
 * 模板化设计，通过传入比较器函数对象灵活定义排序规则。
 * 在 StatisticsManager 中用于对图书按 borrowCount 降序排列。
 */

#ifndef SORT_ALGORITHMS_H
#define SORT_ALGORITHMS_H

#include <vector>
#include <functional>

class SortAlgorithms {
public:
    // ========================================================================
    //  公开排序方法
    // ========================================================================

    /**
     * @brief 快速排序 —— 对 vector 进行原地排序
     *
     * @tparam T          元素类型
     * @param  arr        待排序的 vector 引用
     * @param  comparator 比较函数：
     *                    comparator(a, b) 返回 true 表示 a 应排在 b 前面
     *                    例如：降序排列 borrowCount → return a.borrowCount > b.borrowCount
     */
    template<typename T>
    static void quickSort(std::vector<T>& arr,
                          std::function<bool(const T&, const T&)> comparator);

private:
    // ========================================================================
    //  私有递归方法
    // ========================================================================

    /** @brief 快速排序递归辅助 —— 对 [low, high] 区间进行递归分治 */
    template<typename T>
    static void quickSortHelper(std::vector<T>& arr,
                                int low,
                                int high,
                                std::function<bool(const T&, const T&)> comp);

    /** @brief 分区函数 —— 选取基准元素，将数组分为两部分，返回基准最终位置 */
    template<typename T>
    static int partition(std::vector<T>& arr,
                         int low,
                         int high,
                         std::function<bool(const T&, const T&)> comp);

    // 禁止实例化
    SortAlgorithms() = delete;
};

#endif // SORT_ALGORITHMS_H

// ================================================================================
//  模板实现（模板类必须将实现放在头文件中）
// ================================================================================

/**
 * @brief 快速排序（公开接口） —— 选取 last 元素为 pivot，原地排序
 */
template<typename T>
void SortAlgorithms::quickSort(std::vector<T>& arr,
                               std::function<bool(const T&, const T&)> comparator) {
    if (arr.size() <= 1) return;
    quickSortHelper<T>(arr, 0, static_cast<int>(arr.size()) - 1, comparator);
}

/**
 * @brief 快速排序递归辅助 —— 对 [low, high] 区间分治
 */
template<typename T>
void SortAlgorithms::quickSortHelper(std::vector<T>& arr,
                                     int low, int high,
                                     std::function<bool(const T&, const T&)> comp) {
    if (low >= high) return;
    int pivotIdx = partition<T>(arr, low, high, comp);
    quickSortHelper<T>(arr, low, pivotIdx - 1, comp);   // 左半部
    quickSortHelper<T>(arr, pivotIdx + 1, high, comp);  // 右半部
}

/**
 * @brief 分区函数 —— 以 arr[high] 为 pivot
 *        遍历 [low, high)，将符合 comp 的元素交换到左侧
 *        最终 pivot 归位，返回 pivot 下标
 */
template<typename T>
int SortAlgorithms::partition(std::vector<T>& arr,
                              int low, int high,
                              std::function<bool(const T&, const T&)> comp) {
    T pivot = arr[high];               // 选最后一个元素为基准
    int i = low - 1;                    // i 指向"已处理的小元素区"末尾

    for (int j = low; j < high; ++j) {
        // comp(arr[j], pivot) = true → arr[j] 应排在 pivot 前面
        if (comp(arr[j], pivot)) {
            ++i;
            std::swap(arr[i], arr[j]);
        }
    }
    // 将 pivot 放到正确位置
    std::swap(arr[i + 1], arr[high]);
    return i + 1;
}
