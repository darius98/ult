#pragma once

#include "task-promise.hpp"

namespace ult {

class Scheduler;

class TaskControl : internal::TaskDataPtr {
 public:
  using internal::TaskDataPtr::TaskDataPtr;

  TaskPromise promise() const;

  Scheduler& scheduler() const;

  void yield();

  [[noreturn]] void exit(int status = 0);
};

}  // namespace ult
