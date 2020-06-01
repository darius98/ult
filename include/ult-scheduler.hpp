#pragma once

#include "ult-task.hpp"

namespace ult {

class Scheduler {
 public:
  Scheduler();

  ~Scheduler();

  template <class T>
  void enqueue_task(T task) {
    enqueue_raw_task(Task(this, static_cast<T&&>(task), 8192));
  }

  void run();

 private:
  struct SchedulerData;

  void enqueue_raw_task(Task task);

  void yield_task();

  [[noreturn]] void exit_task();

  Task::stack_size_type generate_task_id();

  SchedulerData* impl;

  friend class Task;
};

}  // namespace ult
