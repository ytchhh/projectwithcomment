// Copyright (c) 2011 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef STORAGE_LEVELDB_UTIL_ARENA_H_
#define STORAGE_LEVELDB_UTIL_ARENA_H_

#include <vector>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include "port/port.h"

namespace leveldb {

// memtable有阈值的限制（write_buffer_size）,为了便于统计内存的使用，也为了内存使用效率，对memtable的内存使用实现了比较简单的arena管理（Arena）。
class Arena {
 public:
  Arena();
  ~Arena();

  // Return a pointer to a newly allocated memory block of "bytes" bytes.
  char* Allocate(size_t bytes);

  // Allocate memory with the normal alignment guarantees provided by malloc
  char* AllocateAligned(size_t bytes);

  // Returns an estimate of the total memory usage of data allocated
  // by the arena.
  size_t MemoryUsage() const {
    return reinterpret_cast<uintptr_t>(memory_usage_.NoBarrier_Load());
  }

 private:
  char* AllocateFallback(size_t bytes);
  // 向系统申请 block_bytes 大小的内存
  char* AllocateNewBlock(size_t block_bytes);

  // Allocation state
  // 当前空闲内存 block 内的可用地址
  char* alloc_ptr_;
  // 当前空闲内存 block 内的可用大小
  size_t alloc_bytes_remaining_;

  // Array of new[] allocated memory blocks
  // 已经申请的内存 block
  std::vector<char*> blocks_;

  // Total memory usage of the arena.
  // 累计分配的内存大小,一个 memtable 对应一个 Arena
  // memtable 内的数据量就用这个值表示
  port::AtomicPointer memory_usage_;

  // No copying allowed
  Arena(const Arena&);
  void operator=(const Arena&);
};

inline char* Arena::Allocate(size_t bytes) {
  // The semantics of what to return are a bit messy if we allow
  // 0-byte allocations, so we disallow them here (we don't need
  // them for our internal use).
  assert(bytes > 0);
  // 如果申请的大小小于当前空闲 block 内可用大小，直接分配
  if (bytes <= alloc_bytes_remaining_) {
    char* result = alloc_ptr_;
    alloc_ptr_ += bytes;
    alloc_bytes_remaining_ -= bytes;
    return result;
  }
  return AllocateFallback(bytes);
}

}  // namespace leveldb

#endif  // STORAGE_LEVELDB_UTIL_ARENA_H_
