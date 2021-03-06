#ifndef BINARY_HEAP_H
#define BINARY_HEAP_H
#include "jret.h"

#ifdef __cplusplus
extern "C" {
#endif

/* 堆空值 */
#define JBINARY_HEAP_NULL JRET_PTR_NULL

/* 堆类型——最大堆/最小堆 */
typedef enum {
    JBINARY_HEAP_TYPE_MIN,
    JBINARY_HEAP_TYPE_MAX
} JBinaryHeapType;

/* 堆结构 */
typedef struct _JBinaryHeap JBinaryHeap;

/* 堆中存储的值 */
typedef void* JBinaryHeapValue;

/**
 * 堆中用来比较大小的函数指针类型
 *
 * @return: value1等于value2返回    RET_EQUAL
 *          value1小于value2返回    RET_SMALLER
 *          value1大于value2返回    RET_BIGGER
 */
typedef int (* binary_heap_compare_cb) (JBinaryHeapValue value1, JBinaryHeapValue value2);

/**
 * 创建新的堆
 * @param type:                     堆类型
 * @param compareFunction:          值比较函数
 *
 * @return 成功：  返回新的堆
 *         失败： 返回 RET_PTR_NULL
 */
JBinaryHeap* binary_heap_new(JBinaryHeapType type, binary_heap_compare_cb compareFunction);


/**
 * 释放堆
 * @param heap:                     要是放的堆指针
 * 注意: 只释放了本堆自己申请的内存空间，至于用户自己申请的堆 value 需要用户自己去释放
 */
void binary_heap_free(JBinaryHeap* heap);


/**
 * 插入值
 * @param heap:                     堆
 * @param value:                    要插入的值
 *
 * @return                          成功： RET_OK
 *                                  失败： RET_ERROR
 */
int binary_heap_insert(JBinaryHeap* heap, JBinaryHeapValue value);


/**
 * 弹出堆顶元素
 * @param heap:                     堆
 *
 * @return                          成功: 返回堆顶元素
 *                                  失败: 返回 RET_PTR_NULL
 */
JBinaryHeapValue binary_heap_pop(JBinaryHeap* heap);


/**
 * 堆中值得数量
 * @param heap:                     堆
 *
 * @return                          堆中值得数量
 */
unsigned int binary_heap_num(JBinaryHeap* heap);

#ifdef __cplusplus
}
#endif
#endif // BINARY_HEAP_H
