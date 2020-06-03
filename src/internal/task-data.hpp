#pragma once

#include <atomic>
#include <csetjmp>
#include <cstdlib>
#include <memory>

#include "ult/scheduler.hpp"
#include "ult/types.hpp"

namespace ult::internal {

struct TaskData {
  std::byte* stack_top;
  Scheduler* scheduler;
  std::atomic<int32_t> refs{1};
  exit_status_t exit_status = 0;
  bool started = false;
  std::atomic<bool> is_done{false};
  jmp_buf state{};
  std::size_t id;
  task_function_ptr task;
  void* arg;
  std::size_t stack_size;
  std::unique_ptr<std::byte[]> stack_bottom;

  TaskData(Scheduler* scheduler, task_id_t id, internal::task_function_ptr task,
           void* arg, stack_size_t stack_size);
};

}  // namespace ult::internal
