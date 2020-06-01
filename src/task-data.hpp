#pragma once

#include <atomic>
#include <csetjmp>
#include <cstdlib>
#include <memory>

#include "ult/task.hpp"

namespace ult::internal {

struct TaskData {
  jmp_buf state{};
  std::byte* stack_top;
  bool started = false;
  Scheduler* scheduler;
  std::atomic<int32_t> refs{1};
  std::size_t id;
  Task::raw_task_ptr task;
  void* arg;
  Task::exit_status_type exit_status = 0;
  std::size_t stack_size;
  std::unique_ptr<std::byte[]> stack_bottom;
};

}  // namespace ult::internal
