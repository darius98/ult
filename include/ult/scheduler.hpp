#pragma once

#include "internal/meta.hpp"
#include "internal/task-proxy.hpp"
#include "task-promise.hpp"

namespace ult {

class Scheduler {
 public:
  Scheduler();

  ~Scheduler();

  template <class T>
  TaskPromise add_task(T callable) {
    return add_task_raw(&internal::task_proxy<T>, new T(internal::move(callable)), 8192);
  }

  void run();

 private:
  struct SchedulerData;

  TaskPromise add_task_raw(internal::task_function_ptr task, void* arg, stack_size_t stack_size);

  void yield_task();

  [[noreturn]] void exit_task();

  task_id_t generate_task_id();

  SchedulerData* impl;

  friend class TaskControl;
};

}  // namespace ult
