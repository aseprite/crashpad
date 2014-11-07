// Copyright 2014 The Crashpad Authors. All rights reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef CRASHPAD_MINIDUMP_MINIDUMP_MODULE_CRASHPAD_INFO_WRITER_H_
#define CRASHPAD_MINIDUMP_MINIDUMP_MODULE_CRASHPAD_INFO_WRITER_H_

#include <stdint.h>
#include <sys/types.h>

#include <vector>

#include "base/basictypes.h"
#include "base/memory/scoped_ptr.h"
#include "minidump/minidump_extensions.h"
#include "minidump/minidump_location_descriptor_list_writer.h"
#include "minidump/minidump_string_writer.h"
#include "minidump/minidump_writable.h"
#include "util/stdlib/pointer_container.h"

namespace crashpad {

class MinidumpSimpleStringDictionaryWriter;
class ModuleSnapshot;

//! \brief The writer for a MinidumpModuleCrashpadInfo object in a minidump
//!     file.
class MinidumpModuleCrashpadInfoWriter final
    : public internal::MinidumpWritable {
 public:
  MinidumpModuleCrashpadInfoWriter();
  ~MinidumpModuleCrashpadInfoWriter() override;

  //! \brief Initializes MinidumpModuleCrashpadInfo based on \a module_snapshot.
  //!
  //! Only data in \a module_snapshot that is considered useful will be
  //! included. For simple annotations, usefulness is determined by
  //! MinidumpSimpleStringDictionaryWriter::IsUseful().
  //!
  //! \param[in] module_snapshot The module snapshot to use as source data.
  //! \param[in] module_list_index The index of the MINIDUMP_MODULE initialized
  //!     from \a module_snapshot in the minidump file’s MINIDUMP_MODULE_LIST
  //!     stream.
  //!
  //! \note Valid in #kStateMutable. No mutator methods may be called before
  //!     this method, and it is not normally necessary to call any mutator
  //!     methods after this method.
  void InitializeFromSnapshot(const ModuleSnapshot* module_snapshot,
                              size_t module_list_index);

  //! \brief Sets MinidumpModuleCrashpadInfo::minidump_module_list_index.
  void SetMinidumpModuleListIndex(uint32_t minidump_module_list_index) {
    module_.minidump_module_list_index = minidump_module_list_index;
  }

  //! \brief Arranges for MinidumpModuleCrashpadInfo::list_annotations to point
  //!     to the internal::MinidumpUTF8StringListWriter object to be written by
  //!     \a list_annotations.
  //!
  //! This object takes ownership of \a simple_annotations and becomes its
  //! parent in the overall tree of internal::MinidumpWritable objects.
  //!
  //! \note Valid in #kStateMutable.
  void SetListAnnotations(
      scoped_ptr<MinidumpUTF8StringListWriter> list_annotations);

  //! \brief Arranges for MinidumpModuleCrashpadInfo::simple_annotations to
  //!     point to the MinidumpSimpleStringDictionaryWriter object to be written
  //!     by \a simple_annotations.
  //!
  //! This object takes ownership of \a simple_annotations and becomes its
  //! parent in the overall tree of internal::MinidumpWritable objects.
  //!
  //! \note Valid in #kStateMutable.
  void SetSimpleAnnotations(
      scoped_ptr<MinidumpSimpleStringDictionaryWriter> simple_annotations);

  //! \brief Determines whether the object is useful.
  //!
  //! A useful object is one that carries data that makes a meaningful
  //! contribution to a minidump file. An object carrying list annotations or
  //! simple annotations would be considered useful.
  //!
  //! \return `true` if the object is useful, `false` otherwise.
  bool IsUseful() const;

 protected:
  // MinidumpWritable:
  bool Freeze() override;
  size_t SizeOfObject() override;
  std::vector<MinidumpWritable*> Children() override;
  bool WriteObject(FileWriterInterface* file_writer) override;

 private:
  MinidumpModuleCrashpadInfo module_;
  scoped_ptr<MinidumpUTF8StringListWriter> list_annotations_;
  scoped_ptr<MinidumpSimpleStringDictionaryWriter> simple_annotations_;

  DISALLOW_COPY_AND_ASSIGN(MinidumpModuleCrashpadInfoWriter);
};

//! \brief The writer for a MinidumpModuleCrashpadInfoList object in a minidump
//!     file, containing a list of MinidumpModuleCrashpadInfo objects.
class MinidumpModuleCrashpadInfoListWriter final
    : public internal::MinidumpLocationDescriptorListWriter {
 public:
  MinidumpModuleCrashpadInfoListWriter();
  ~MinidumpModuleCrashpadInfoListWriter() override;

  //! \brief Adds an initialized MinidumpModuleCrashpadInfo for modules in \a
  //!     module_snapshots to the MinidumpModuleCrashpadInfoList.
  //!
  //! Only modules in \a module_snapshots that would produce a useful
  //! MinidumpModuleCrashpadInfo structure are included. Usefulness is
  //! determined by MinidumpModuleCrashpadInfoWriter::IsUseful().
  //!
  //! \param[in] module_snapshots The module snapshots to use as source data.
  //!
  //! \note Valid in #kStateMutable. AddModule() may not be called before this
  //!     method, and it is not normally necessary to call AddModule() after
  //!     this method.
  void InitializeFromSnapshot(
      const std::vector<const ModuleSnapshot*>& module_snapshots);

  //! \brief Adds a MinidumpModuleCrashpadInfo to the
  //!     MinidumpModuleCrashpadInfoList.
  //!
  //! This object takes ownership of \a module and becomes its parent in the
  //! overall tree of internal::MinidumpWritable objects.
  //!
  //! \note Valid in #kStateMutable.
  void AddModule(scoped_ptr<MinidumpModuleCrashpadInfoWriter> module);

  //! \brief Determines whether the object is useful.
  //!
  //! A useful object is one that carries data that makes a meaningful
  //! contribution to a minidump file. An object carrying children would be
  //! considered useful.
  //!
  //! \return `true` if the object is useful, `false` otherwise.
  bool IsUseful() const;

 private:
  DISALLOW_COPY_AND_ASSIGN(MinidumpModuleCrashpadInfoListWriter);
};

}  // namespace crashpad

#endif  // CRASHPAD_MINIDUMP_MINIDUMP_MODULE_CRASHPAD_INFO_WRITER_H_
