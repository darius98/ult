#pragma once

#include "internal/task-data.hpp"
#include "types.hpp"

namespace ult {

class TaskPromise : public internal::TaskDataPtr {
 public:
  using internal::TaskDataPtr::TaskDataPtr;

  bool is_done() const;

  exit_status_t exit_status() const;

  friend class TaskControl;
};

}  // namespace ult
