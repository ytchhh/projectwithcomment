// Copyright (c) 2011 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.
//
// File names used by DB code

#ifndef STORAGE_LEVELDB_DB_FILENAME_H_
#define STORAGE_LEVELDB_DB_FILENAME_H_

#include <stdint.h>
#include <string>
#include "leveldb/slice.h"
#include "leveldb/status.h"
#include "port/port.h"

namespace leveldb {

class Env;

enum FileType {
  kLogFile,        // binlog 文件，文件名为 [0-9]+.log，前缀数字为 FileNumber
  kDBLockFile,     // lock 文件，一个 db 同时只能有一个 db 实例操作，通过对 LOCK 文件加文件锁（flock）实现主动保护。
  kTableFile,      // sstable文件，文件名为 [0-9]+.sst
  kDescriptorFile, // db 元信息文件：MANIFEST-[0-9]+ ，每当 db 中的状态改变（VersionSet），会将这次改变（VersionEdit）追加到 descriptor 文件中。 后缀数字为 FileNumber。
  kCurrentFile,    // CURRENT 文件，保存当前使用的descriptor文件的文件名
  kTempFile,       // 临时文件：[0-9]+.dbtmp ,对 db 做修复（Repairer）时，会产生临时文件。前缀为 FileNumber。
  kInfoLogFile  // Either the current one, or an old one  // db 运行时打印日志的文件：LOG, 每次重新运行，如果已经存在 LOG 文件，会先将 LOG 文件重名成 LOG.old
};

// Return the name of the log file with the specified number
// in the db named by "dbname".  The result will be prefixed with
// "dbname".
extern std::string LogFileName(const std::string& dbname, uint64_t number);

// Return the name of the sstable with the specified number
// in the db named by "dbname".  The result will be prefixed with
// "dbname".
extern std::string TableFileName(const std::string& dbname, uint64_t number);

// Return the legacy file name for an sstable with the specified number
// in the db named by "dbname". The result will be prefixed with
// "dbname".
extern std::string SSTTableFileName(const std::string& dbname, uint64_t number);

// Return the name of the descriptor file for the db named by
// "dbname" and the specified incarnation number.  The result will be
// prefixed with "dbname".
extern std::string DescriptorFileName(const std::string& dbname,
                                      uint64_t number);

// Return the name of the current file.  This file contains the name
// of the current manifest file.  The result will be prefixed with
// "dbname".
extern std::string CurrentFileName(const std::string& dbname);

// Return the name of the lock file for the db named by
// "dbname".  The result will be prefixed with "dbname".
extern std::string LockFileName(const std::string& dbname);

// Return the name of a temporary file owned by the db named "dbname".
// The result will be prefixed with "dbname".
extern std::string TempFileName(const std::string& dbname, uint64_t number);

// Return the name of the info log file for "dbname".
extern std::string InfoLogFileName(const std::string& dbname);

// Return the name of the old info log file for "dbname".
extern std::string OldInfoLogFileName(const std::string& dbname);

// If filename is a leveldb file, store the type of the file in *type.
// The number encoded in the filename is stored in *number.  If the
// filename was successfully parsed, returns true.  Else return false.
extern bool ParseFileName(const std::string& filename,
                          uint64_t* number,
                          FileType* type);

// Make the CURRENT file point to the descriptor file with the
// specified number.
extern Status SetCurrentFile(Env* env, const std::string& dbname,
                             uint64_t descriptor_number);


}  // namespace leveldb

#endif  // STORAGE_LEVELDB_DB_FILENAME_H_
