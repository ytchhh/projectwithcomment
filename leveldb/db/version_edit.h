// Copyright (c) 2011 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef STORAGE_LEVELDB_DB_VERSION_EDIT_H_
#define STORAGE_LEVELDB_DB_VERSION_EDIT_H_

#include <set>
#include <utility>
#include <vector>
#include "db/dbformat.h"

namespace leveldb {

class VersionSet;

// sstable 文件的元信息封装成 FileMetaData
struct FileMetaData {
  int refs;                   // 引用计数
  int allowed_seeks;          // Seeks allowed until compaction  // compact 之前允许的 seek 次数
  uint64_t number;            // FileNumer
  uint64_t file_size;         // File size in bytes
  InternalKey smallest;       // Smallest internal key served by table  // sstable 文件的最小 key
  InternalKey largest;        // Largest internal key served by table  // sstable 文件的最大 key

  FileMetaData() : refs(0), allowed_seeks(1 << 30), file_size(0) { }
};

/**
 * compact 过程中会有一系列改变当前 Version 的操作（FileNumber 增加，删除 input 的 sstable，增加输出的 sstable……）
 * 为了缩小 Version 切换的时间点，将这些操作封装成 VersionEdit，
 * compact 完成时，将 VersionEdit 中的操作一次应用到当前 Version 即可得到最新状态的 Version。
 * 每次 compact 之后都会将对应的 VersionEdit encode 入 manifest 文件。
 */
class VersionEdit {
 public:
  VersionEdit() { Clear(); }
  ~VersionEdit() { }

  void Clear();

  void SetComparatorName(const Slice& name) {
    has_comparator_ = true;
    comparator_ = name.ToString();
  }
  void SetLogNumber(uint64_t num) {
    has_log_number_ = true;
    log_number_ = num;
  }
  void SetPrevLogNumber(uint64_t num) {
    has_prev_log_number_ = true;
    prev_log_number_ = num;
  }
  void SetNextFile(uint64_t num) {
    has_next_file_number_ = true;
    next_file_number_ = num;
  }
  void SetLastSequence(SequenceNumber seq) {
    has_last_sequence_ = true;
    last_sequence_ = seq;
  }
  void SetCompactPointer(int level, const InternalKey& key) {
    compact_pointers_.push_back(std::make_pair(level, key));
  }

  // Add the specified file at the specified number.
  // REQUIRES: This version has not been saved (see VersionSet::SaveTo)
  // REQUIRES: "smallest" and "largest" are smallest and largest keys in file
  void AddFile(int level, uint64_t file,
               uint64_t file_size,
               const InternalKey& smallest,
               const InternalKey& largest) {
    FileMetaData f;
    f.number = file;
    f.file_size = file_size;
    f.smallest = smallest;
    f.largest = largest;
    new_files_.push_back(std::make_pair(level, f));
  }

  // Delete the specified "file" from the specified "level".
  void DeleteFile(int level, uint64_t file) {
    deleted_files_.insert(std::make_pair(level, file));
  }

  void EncodeTo(std::string* dst) const;
  Status DecodeFrom(const Slice& src);

  std::string DebugString() const;

 private:
  friend class VersionSet;

  typedef std::set< std::pair<int, uint64_t> > DeletedFileSet;

  std::string comparator_; 
  uint64_t log_number_;          // log 的 FileNumber
  uint64_t prev_log_number_;     // 辅助 log 的 FileNumber
  uint64_t next_file_number_;    // 下一个可用的 FileNumber
  SequenceNumber last_sequence_; // 用过的最后一个 SequnceNumber
  bool has_comparator_;          // 以下标识是否存在，验证使用
  bool has_log_number_;
  bool has_prev_log_number_;
  bool has_next_file_number_;
  bool has_last_sequence_;

  // 要更新的 level ==》 compact_pointer。
  std::vector< std::pair<int, InternalKey> > compact_pointers_;
  // 要删除的 sstable 文件（compact 的 input）
  DeletedFileSet deleted_files_;
  // 新的文件（compact 的 output）
  std::vector< std::pair<int, FileMetaData> > new_files_;
};

}  // namespace leveldb

#endif  // STORAGE_LEVELDB_DB_VERSION_EDIT_H_
