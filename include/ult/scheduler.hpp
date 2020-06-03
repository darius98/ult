#pragma once

#include "internal/meta.hpp"
#include "task.hpp"

namespace ult {

class Scheduler {
 public:
  Scheduler();

  ~Scheduler();

  template <class T>
  TaskPromise add_task(T task) {
    return add_task_raw(Task(this, generate_task_id(), internal::move(task), 8192));
  }

  void run();

 private:
  struct SchedulerData;

  TaskPromise add_task_raw(Task task);

  void yield_task();

  [[noreturn]] void exit_task();

  task_id_t generate_task_id();

  SchedulerData* impl;

  friend class TaskControl;
};

}  // namespace ult
