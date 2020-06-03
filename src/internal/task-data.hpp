#pragma once

#include <atomic>
#include <csetjmp>
#include <cstdlib>
#include <memory>

#include "ult/scheduler.hpp"
#include "ult/types.hpp"

namespace ult::internal {

struct TaskData {
  jmp_buf state{};
  std::byte* stack_top;
  bool started = false;
  Scheduler* scheduler;
  std::atomic<int32_t> refs{1};
  std::size_t id;
  task_function_ptr task;
  void* arg;
  std::atomic<bool> is_done{false};
  exit_status_t exit_status = 0;
  std::size_t stack_size;
  std::unique_ptr<std::byte[]> stack_bottom;
};

TaskData* make_task_data(Scheduler* scheduler, task_id_t id, internal::task_function_ptr task,
                         void* arg, stack_size_t stack_size);

}  // namespace ult::internal
