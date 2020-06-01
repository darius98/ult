#pragma once

#include <atomic>
#include <vector>

#include "ult-task.hpp"

namespace ult {

class Scheduler {
  static constexpr int setjmp_task_yield = 1;
  static constexpr int setjmp_task_exit = 2;

 public:
  template <class T>
  void enqueue_task(T task) {
    tasks.emplace_back(this, std::move(task), 8192);
  }

  void run();

 private:
  std::size_t generate_task_id();

  jmp_buf scheduler_entry{};
  std::size_t running_task_index = 0;
  std::vector<Task> tasks;

  std::atomic<std::size_t> next_task_id = 1;

  friend class Task;
};

}  // namespace ult
