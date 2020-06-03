#pragma once

#include "task-promise.hpp"

namespace ult {

class Scheduler;

class TaskControl : internal::TaskDataPtr {
 public:
  using internal::TaskDataPtr::TaskDataPtr;

  TaskPromise promise();

  Scheduler& scheduler();

  void yield();

  [[noreturn]] void exit(exit_status_t status = 0);

  friend class Task;
};

}  // namespace ult
