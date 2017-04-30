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

#ifndef MEMORY_BLOCK_TCC
#define MEMORY_BLOCK_TCC

// 计算对齐所需补的空间
template <typename T, size_t BlockSize>
inline typename MemoryPool<T, BlockSize>::size_type
MemoryPool<T, BlockSize>::padPointer(data_pointer_ p, size_type align)
const throw()
{
  size_t result = reinterpret_cast<size_t>(p);
  return ((align - result) % align);
}

/* 构造函数，所有成员初始化 */
template <typename T, size_t BlockSize>
MemoryPool<T, BlockSize>::MemoryPool()
throw()
{
  currentBlock_ = 0;
  currentSlot_ = 0;
  lastSlot_ = 0;
  freeSlots_ = 0;
}

/* 复制构造函数,调用 MemoryPool 初始化*/
template <typename T, size_t BlockSize>
MemoryPool<T, BlockSize>::MemoryPool(const MemoryPool& memoryPool)
throw()
{
  MemoryPool();
}

/* 复制构造函数,调用 MemoryPool 初始化*/
template <typename T, size_t BlockSize>
template<class U>
MemoryPool<T, BlockSize>::MemoryPool(const MemoryPool<U>& memoryPool)
throw()
{
  MemoryPool();
}

/* 析构函数，把内存池中所有 block delete 掉 */
template <typename T, size_t BlockSize>
MemoryPool<T, BlockSize>::~MemoryPool()
throw()
{
  slot_pointer_ curr = currentBlock_;
  while (curr != 0) {
    slot_pointer_ prev = curr->next;
    // 转化为 void 指针，是因为 void 类型不需要调用析构函数,只释放空间
    operator delete(reinterpret_cast<void*>(curr));
    curr = prev;
  }
}

/* 返回地址 */
template <typename T, size_t BlockSize>
inline typename MemoryPool<T, BlockSize>::pointer
MemoryPool<T, BlockSize>::address(reference x)
const throw()
{
  return &x;
}

/* 返回地址的 const 重载*/
template <typename T, size_t BlockSize>
inline typename MemoryPool<T, BlockSize>::const_pointer
MemoryPool<T, BlockSize>::address(const_reference x)
const throw()
{
  return &x;
}

// 申请一块空闲的 block 放进内存池
template <typename T, size_t BlockSize>
void
MemoryPool<T, BlockSize>::allocateBlock()
{
  // Allocate space for the new block and store a pointer to the previous one
  // operator new 申请对应大小内存，返回 void* 指针
  data_pointer_ newBlock = reinterpret_cast<data_pointer_>
                           (operator new(BlockSize));
  // 原来的 block 链头接到 newblock
  reinterpret_cast<slot_pointer_>(newBlock)->next = currentBlock_;
  // 新的 currentblock_
  currentBlock_ = reinterpret_cast<slot_pointer_>(newBlock);
  // Pad block body to staisfy the alignment requirements for elements
  data_pointer_ body = newBlock + sizeof(slot_pointer_);
  // 计算为了对齐应该空出多少位置
  size_type bodyPadding = padPointer(body, sizeof(slot_type_));
  // currentslot_ 为该 block 开始的地方加上 bodypadding 个 char* 空间
  currentSlot_ = reinterpret_cast<slot_pointer_>(body + bodyPadding);
  // 计算最后一个能放置 slot_type_ 的位置
  lastSlot_ = reinterpret_cast<slot_pointer_>
              (newBlock + BlockSize - sizeof(slot_type_) + 1);
}

// 返回指向分配新元素所需内存的指针
template <typename T, size_t BlockSize>
inline typename MemoryPool<T, BlockSize>::pointer
MemoryPool<T, BlockSize>::allocate(size_type, const_pointer)
{
  // 如果 freeSlots_ 非空，就在 freeSlots_ 中取内存
  if (freeSlots_ != 0) {
    pointer result = reinterpret_cast<pointer>(freeSlots_);
    // 更新 freeSlots_
    freeSlots_ = freeSlots_->next;
    return result;
  }
  else {
    if (currentSlot_ >= lastSlot_)
      // 之前申请的内存用完了，分配新的 block
      allocateBlock();
    // 从分配的 block 中划分出去
    return reinterpret_cast<pointer>(currentSlot_++);
  }
}

// 将元素内存归还给 free 内存链表
template <typename T, size_t BlockSize>
inline void
MemoryPool<T, BlockSize>::deallocate(pointer p, size_type)
{
  if (p != 0) {
    // 转换成 slot_pointer_ 指针，next 指向 freeSlots_ 链表
    reinterpret_cast<slot_pointer_>(p)->next = freeSlots_;
    // 新的 freeSlots_ 头为 p
    freeSlots_ = reinterpret_cast<slot_pointer_>(p);
  }
}

// 计算可以容纳的最多元素
template <typename T, size_t BlockSize>
inline typename MemoryPool<T, BlockSize>::size_type
MemoryPool<T, BlockSize>::max_size()
const throw()
{
  size_type maxBlocks = -1 / BlockSize;
  return (BlockSize - sizeof(data_pointer_)) / sizeof(slot_type_) * maxBlocks;
}

// 在已分配内存上构造对象
template <typename T, size_t BlockSize>
inline void
MemoryPool<T, BlockSize>::construct(pointer p, const_reference val)
{
  // placement new 用法，在已有内存上构造对象，调用 T 的复制构造函数，
  new (p) value_type (val);
}

// 销毁对象
template <typename T, size_t BlockSize>
inline void
MemoryPool<T, BlockSize>::destroy(pointer p)
{
  // placement new 中需要手动调用元素 T 的析构函数
  p->~value_type();
}

// 创建新元素
template <typename T, size_t BlockSize>
inline typename MemoryPool<T, BlockSize>::pointer
MemoryPool<T, BlockSize>::newElement(const_reference val)
{
  // 申请内存
  pointer result = allocate();
  // 在内存上构造对象
  construct(result, val);
  return result;
}

// 删除元素
template <typename T, size_t BlockSize>
inline void
MemoryPool<T, BlockSize>::deleteElement(pointer p)
{
  if (p != 0) {
    // placement new 中需要手动调用元素 T 的析构函数
    p->~value_type();
    // 归还内存
    deallocate(p);
  }
}

#endif // MEMORY_BLOCK_TCC
