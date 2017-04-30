/*-
 * Copyright (c) 2013 Cosku Acay, http://www.coskuacay.com
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#ifndef MEMORY_POOL_H
#define MEMORY_POOL_H

#include <limits.h>
#include <stddef.h>

template <typename T, size_t BlockSize = 4096>
class MemoryPool
{
  public:
    /* Member types */
    typedef T               value_type;       // T 的 value 类型
    typedef T*              pointer;          // T 的 指针类型
    typedef T&              reference;        // T 的引用类型
    typedef const T*        const_pointer;    // T 的 const 指针类型
    typedef const T&        const_reference;  // T 的 const 引用类型
    typedef size_t          size_type;        // size_t 类型
    typedef ptrdiff_t       difference_type;  // 指针减法结果类型

    // 这个来源于 allocate 的标准
    template <typename U> struct rebind {
      typedef MemoryPool<U> other;
    };

    /* Member functions */
    /* 构造函数 */
    MemoryPool() throw();
    MemoryPool(const MemoryPool& memoryPool) throw();
    template <class U> MemoryPool(const MemoryPool<U>& memoryPool) throw();

    /* 析构函数 */
    ~MemoryPool() throw();

    /* 元素取址 */
    pointer address(reference x) const throw();
    const_pointer address(const_reference x) const throw();

    // Can only allocate one object at a time. n and hint are ignored
    // 分配和收回一个元素的内存空间
    pointer allocate(size_type n = 1, const_pointer hint = 0);
    void deallocate(pointer p, size_type n = 1);

    // 最大大小
    size_type max_size() const throw();

    // 基于内存池的元素构造和析构
    void construct(pointer p, const_reference val);
    void destroy(pointer p);

    // 自带申请内存和释放内存的构造和析构
    pointer newElement(const_reference val);
    void deleteElement(pointer p);

  private:
    // union 结构体,用于存放元素或 next 指针
    union Slot_ {
      value_type element;
      Slot_* next;
    };

    typedef char* data_pointer_;  // char* 指针，主要用于指向内存首地址
    typedef Slot_ slot_type_;     // Slot_ 值类型
    typedef Slot_* slot_pointer_; // Slot_* 指针类型

    slot_pointer_ currentBlock_;  // 内存块链表的头指针
    slot_pointer_ currentSlot_;   // 元素链表的头指针
    slot_pointer_ lastSlot_;      // 可存放元素的最后指针
    slot_pointer_ freeSlots_;     // 元素构造后释放掉的内存链表头指针

    size_type padPointer(data_pointer_ p, size_type align) const throw();  // 计算对齐所需空间
    void allocateBlock();  // 申请内存块放进内存池
   /*
    static_assert(BlockSize >= 2 * sizeof(slot_type_), "BlockSize too small.");
    */
};

#include "MemoryPool.tcc"

#endif // MEMORY_POOL_H
