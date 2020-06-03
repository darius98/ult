#pragma once

#include "internal/task-data.hpp"
#include "types.hpp"

namespace ult {

class Scheduler;

class TaskPromise : public internal::TaskDataPtr {
 public:
  using internal::TaskDataPtr::TaskDataPtr;

  bool is_done() const;

  exit_status_t exit_status() const;

  Scheduler& scheduler() const;
};

}  // namespace ult
