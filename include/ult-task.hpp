#pragma once

#include <csetjmp>
#include <cstddef>
#include <cstdlib>

#include <memory>

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
  Task(Task&& other) noexcept = default;
  Task& operator=(Task&& other) noexcept = default;

  ~Task() = default;

  void yield();

  [[noreturn]] void exit(int status = 0);

 private:
  using raw_task = void (*)(Task&, void*);

  Task(Scheduler* scheduler, raw_task task, void* arg, std::size_t stack_size);

  [[noreturn]] void run();

  template <class Callable>
  static void task_proxy(Task& task, void* arg) {
    const auto callable = static_cast<Callable*>(arg);
    (*callable)(task);
    delete callable;
  }

  jmp_buf state{};
  std::byte* stack_top;
  bool started = false;
  Scheduler* scheduler;
  std::size_t id;
  raw_task task;
  void* arg;
  std::size_t stack_size;
  std::unique_ptr<std::byte[]> stack_bottom;
  void* switched_from_stack;

  friend class Scheduler;
};

}  // namespace ult
