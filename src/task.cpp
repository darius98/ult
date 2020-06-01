#include "ult/task.hpp"

#include <memory>

#include "set-stack-pointer.hpp"
#include "task-data.hpp"
#include "ult/scheduler.hpp"

namespace ult {

bool TaskPromise::is_done() const {
  return ptr->is_done.load(std::memory_order_acquire);
}

int TaskPromise::exit_status() const {
  return ptr->exit_status;
}

Task::Task(Scheduler* scheduler, raw_task_ptr task, void* arg, stack_size_type stack_size)
    : internal::TaskDataPtr(new internal::TaskData()) {
  ptr->scheduler = scheduler;
  ptr->id = scheduler->generate_task_id();
  ptr->task = task;
  ptr->arg = arg;
  ptr->stack_size = stack_size;
  ptr->stack_bottom = std::make_unique<std::byte[]>(stack_size);
  ptr->stack_top = ptr->stack_bottom.get() + ptr->stack_size;
}

TaskPromise Task::promise() {
  return TaskPromise(ptr);
}

Scheduler& Task::scheduler() {
  return *ptr->scheduler;
}

void Task::yield() {
  if (setjmp(ptr->state)) {
    return;
  } else {
    ptr->scheduler->yield_task();
  }
}

void Task::exit(exit_status_type status) {
  ptr->exit_status = status;
  ptr->is_done.store(true, std::memory_order_release);
  ptr->scheduler->exit_task();
}

void Task::run() {
  if (ptr->started) {
    longjmp(ptr->state, 1);
  } else {
    ptr->started = true;
    void* top = ptr->stack_top;
    ULT_SET_STACK_POINTER(top);
    const auto status = ptr->task(*this, ptr->arg);
    exit(status);
  }
}

}  // namespace ult
