#pragma once

#include "ult/internal/task-data.hpp"
#include "ult/task-control.hpp"
#include "ult/task-promise.hpp"

namespace ult {

class Task : public internal::TaskDataPtr {
 public:
  Task(Scheduler* scheduler, task_id_t id, internal::task_function_ptr task, void* arg,
       stack_size_t stack_size);

  Task() = default;

  Task(Task&&) = default;
  Task& operator=(Task&&) = default;

  Task(const Task&) = delete;
  Task& operator=(const Task&) = delete;

  ~Task() = default;

  TaskControl control();

  TaskPromise promise();

  [[noreturn]] void run();

  [[noreturn]] void exit(exit_status_t status = 0);
};

}  // namespace ult
