#pragma once

namespace ult {

class Scheduler;

class Task {
 public:
  using id_type = decltype(sizeof(int));          // size_t
  using stack_size_type = decltype(sizeof(int));  // size_t

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

  [[noreturn]] void exit(int status = 0);

 private:
  using raw_task = void (*)(Task&, void*);
  struct TaskData;

  Task() = default;

  Task(Scheduler* scheduler, raw_task task, void* arg, stack_size_type stack_size);

  [[noreturn]] void run();

  template <class Callable>
  static void task_proxy(Task& task, void* arg) {
    const auto callable = static_cast<Callable*>(arg);
    (*callable)(task);
    delete callable;
  }

  TaskData* impl = nullptr;

  friend class Scheduler;
};

}  // namespace ult
