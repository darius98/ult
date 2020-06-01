#include <csetjmp>
#include <cstddef>
#include <cstdlib>
#include <memory>

#include "set-stack-pointer.hpp"
#include "ult-scheduler.hpp"
#include "ult-task.hpp"

namespace ult {

struct Task::TaskData {
  jmp_buf state{};
  std::byte* stack_top;
  bool started = false;
  Scheduler* scheduler;
  std::size_t id;
  raw_task task;
  void* arg;
  int exit_status = 0;
  std::size_t stack_size;
  std::unique_ptr<std::byte[]> stack_bottom;
};

Task::Task(Scheduler* scheduler, raw_task task, void* arg, stack_size_type stack_size)
    : impl(new TaskData()) {
  impl->scheduler = scheduler;
  impl->id = scheduler->generate_task_id();
  impl->task = task;
  impl->arg = arg;
  impl->stack_size = stack_size;
  impl->stack_bottom = std::make_unique<std::byte[]>(stack_size);
  impl->stack_top = impl->stack_bottom.get() + impl->stack_size;
}

Task::Task(Task&& other) noexcept : impl(other.impl) {
  other.impl = nullptr;
}

Task& Task::operator=(Task&& other) noexcept {
  if (this != &other) {
    delete impl;
    impl = other.impl;
    other.impl = nullptr;
  }
  return *this;
}

Task::~Task() {
  delete impl;
}

void Task::yield() {
  if (setjmp(impl->state)) {
    return;
  } else {
    impl->scheduler->yield_task();
  }
}

void Task::exit(int status) {
  impl->exit_status = status;
  impl->scheduler->exit_task();
}

void Task::run() {
  if (impl->started) {
    longjmp(impl->state, 1);
  } else {
    impl->started = true;
    void* top = impl->stack_top;
    ULT_SET_STACK_POINTER(top);
    impl->task(*this, impl->arg);
    exit();
  }
}

}  // namespace ult
