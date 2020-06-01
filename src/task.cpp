#include "ult/task.hpp"

#include <memory>

#include "set-stack-pointer.hpp"
#include "task-data.hpp"
#include "ult/scheduler.hpp"

namespace ult {

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

void Task::yield() {
  if (setjmp(ptr->state)) {
    return;
  } else {
    ptr->scheduler->yield_task();
  }
}

void Task::exit(exit_status_type status) {
  ptr->exit_status = status;
  ptr->scheduler->exit_task();
}

TaskPromise Task::make_promise() {
  return TaskPromise(ptr);
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
