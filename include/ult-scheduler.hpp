#pragma once

#include <cstdlib>

#include "ult-task.hpp"

namespace ult {

class Scheduler {
 public:
  Scheduler();

  ~Scheduler();

  template <class T>
  void enqueue_task(T task) {
    enqueue_raw_task(Task(this, std::move(task), 8192));
  }

  void run();

 private:
  void enqueue_raw_task(Task task);

  void yield_task();

  [[noreturn]] void exit_task();

  std::size_t generate_task_id();

  struct SchedulerData;

  SchedulerData* impl;

  friend class Task;
};

}  // namespace ult
