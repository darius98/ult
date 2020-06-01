#pragma once

#include "task.hpp"

namespace ult {

class Scheduler {
 public:
  Scheduler();

  ~Scheduler();

  template <class T>
  TaskPromise add_task(T task) {
    return add_task_raw(Task(this, static_cast<T&&>(task), 8192));
  }

  void run();

 private:
  struct SchedulerData;

  TaskPromise add_task_raw(Task task);

  void yield_task();

  [[noreturn]] void exit_task();

  Task::stack_size_type generate_task_id();

  SchedulerData* impl;

  friend class Task;
};

}  // namespace ult
