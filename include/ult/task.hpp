#pragma once

#include <ult/internal/meta.hpp>

namespace ult {

class Scheduler;

namespace internal {
struct TaskData;
}  // namespace internal

class TaskPromise {
 public:
  TaskPromise() = default;

  TaskPromise(const TaskPromise&);
  TaskPromise& operator=(const TaskPromise&);

  TaskPromise(TaskPromise&&) noexcept;
  TaskPromise& operator=(TaskPromise&&) noexcept;

  ~TaskPromise();

  int exit_status() const;

 private:
  explicit TaskPromise(internal::TaskData* data);

  internal::TaskData* impl = nullptr;

  friend class Task;
};

class Task {
 public:
  using id_type = decltype(sizeof(int));          // size_t
  using stack_size_type = decltype(sizeof(int));  // size_t
  using exit_status_type = int;
  using raw_task_ptr = exit_status_type (*)(Task&, void*);

  template <class Callable>
  Task(Scheduler* scheduler, Callable callable, stack_size_type stack_size)
      : Task(scheduler, &task_proxy<Callable>, new Callable(static_cast<Callable&&>(callable)),
             stack_size) {
  }

  Task(const Task&) = delete;
  Task& operator=(const Task&) = delete;

  Task(Task&& other) noexcept;
  Task& operator=(Task&& other) noexcept;

  ~Task();

  void yield();

  [[noreturn]] void exit(exit_status_type status = 0);

  TaskPromise make_promise();

 private:
  Task() = default;

  Task(Scheduler* scheduler, raw_task_ptr task, void* arg, stack_size_type stack_size);

  [[noreturn]] void run();

  template <class Callable>
  static exit_status_type task_proxy(Task& task, void* arg) {
    const auto callable = static_cast<Callable*>(arg);
    exit_status_type exit_status = 0;
    if constexpr (internal::IsConvertibleTo<decltype((*callable)(task)), exit_status_type>::value) {
      exit_status = (*callable)(task);
    } else {
      static_assert(internal::IsSame<void, decltype((*callable)(task))>::value,
                    "Task callable should only return int or void.");
      (*callable)(task);
    }
    delete callable;
    return exit_status;
  }

  internal::TaskData* impl = nullptr;

  friend class Scheduler;
  friend class TaskPromise;
};

}  // namespace ult
