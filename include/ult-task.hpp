#pragma once

#include <cstdlib>
#include <memory>
#include <utility>

namespace ult {

class Scheduler;

class Task {
 public:
  template <class Callable>
  Task(Scheduler* scheduler, Callable callable, std::size_t stack_size)
      : Task(scheduler, &task_proxy<Callable>, new Callable(std::move(callable)), stack_size) {
  }

  Task(const Task&) = delete;
  Task& operator=(const Task&) = delete;
  Task(Task&& other) noexcept;
  Task& operator=(Task&& other) noexcept;
  ~Task();

  void yield();

  [[noreturn]] void exit(int status = 0);

 private:
  using raw_task = void (*)(Task&, void*);

  Task() = default;

  Task(Scheduler* scheduler, raw_task task, void* arg, std::size_t stack_size);

  [[noreturn]] void run();

  template <class Callable>
  static void task_proxy(Task& task, void* arg) {
    const auto callable = static_cast<Callable*>(arg);
    (*callable)(task);
    delete callable;
  }

  struct TaskData;

  TaskData* impl = nullptr;

  friend class Scheduler;
};

}  // namespace ult
