#pragma once

#include "internal/meta.hpp"
#include "internal/task-data.hpp"

namespace ult {

using task_id_t = decltype(sizeof(int));     // size_t
using stack_size_t = decltype(sizeof(int));  // size_t
using exit_status_t = int;

class Scheduler;

class TaskPromise : public internal::TaskDataPtr {
 public:
  using internal::TaskDataPtr::TaskDataPtr;

  bool is_done() const;

  int exit_status() const;

  friend class TaskControl;
};

class TaskControl : internal::TaskDataPtr {
 public:
  using internal::TaskDataPtr::TaskDataPtr;

  TaskPromise promise();

  Scheduler& scheduler();

  void yield();

  [[noreturn]] void exit(exit_status_t status = 0);

  friend class Task;
};

class Task : public internal::TaskDataPtr {
 public:
  using raw_task_ptr = exit_status_t (*)(Task&);

  Task() = default;

  Task(Task&&) = default;
  Task& operator=(Task&&) = default;

  Task(const Task&) = delete;
  Task& operator=(const Task&) = delete;

  ~Task() = default;

  TaskControl control();

  TaskPromise promise();

  [[noreturn]] void exit(exit_status_t status = 0);

 private:
  template <class T>
  Task(Scheduler* scheduler, task_id_t id, T callable, stack_size_t stack_size)
      : Task(scheduler, id, &task_proxy<T>, new T(internal::move(callable)), stack_size) {
  }

  Task(Scheduler* scheduler, task_id_t id, raw_task_ptr task, void* arg, stack_size_t stack_size);

  void* arg();

  [[noreturn]] void run();

  template <class Callable>
  static exit_status_t task_proxy(Task& task) {
    const auto callable = static_cast<Callable*>(task.arg());
    exit_status_t exit_status = 0;
    if constexpr (internal::is_convertible<decltype((*callable)(task.control())), exit_status_t>) {
      exit_status = (*callable)(task.control());
    } else {
      static_assert(internal::is_same<void, decltype((*callable)(task.control()))>,
                    "A task's return type should be either void or convertible to int.");
      (*callable)(task.control());
    }
    delete callable;
    return exit_status;
  }

  friend class Scheduler;
  friend class TaskPromise;
};

}  // namespace ult
