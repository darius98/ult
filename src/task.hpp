#pragma once

#include "ult/internal/task-data.hpp"
#include "ult/task-control.hpp"
#include "ult/task-promise.hpp"

namespace ult {

class Task : public internal::TaskDataPtr {
 public:
  Task() = default;

  explicit Task(internal::TaskData* ptr);

  Task(Task&&) = default;
  Task& operator=(Task&&) = default;

  Task(const Task&) = delete;
  Task& operator=(const Task&) = delete;

  ~Task() = default;

  TaskControl control();

  [[noreturn]] void run();

  [[noreturn]] void exit(int status = 0);
};

}  // namespace ult
