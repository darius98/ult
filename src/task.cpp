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

TaskPromise TaskControl::promise() {
  return TaskPromise(ptr);
}

Scheduler& TaskControl::scheduler() {
  return *ptr->scheduler;
}

void TaskControl::yield() {
  if (setjmp(ptr->state)) {
    return;
  } else {
    ptr->scheduler->yield_task();
  }
}

void TaskControl::exit(exit_status_t status) {
  ptr->exit_status = status;
  ptr->is_done.store(true, std::memory_order_release);
  ptr->scheduler->exit_task();
}

Task::Task(Scheduler* scheduler, task_id_t id, raw_task_ptr task, void* arg,
           stack_size_t stack_size)
    : internal::TaskDataPtr(new internal::TaskData()) {
  ptr->scheduler = scheduler;
  ptr->id = id;
  ptr->task = task;
  ptr->arg = arg;
  ptr->stack_size = stack_size;
  ptr->stack_bottom = std::make_unique<std::byte[]>(stack_size);
  ptr->stack_top = ptr->stack_bottom.get() + ptr->stack_size;
}

TaskControl Task::control() {
  return TaskControl(ptr);
}

TaskPromise Task::promise() {
  return control().promise();
}

void Task::exit(exit_status_t status) {
  control().exit(status);
}

void* Task::arg() {
  return ptr->arg;
}

void Task::run() {
  if (ptr->started) {
    longjmp(ptr->state, 1);
  } else {
    ptr->started = true;
    auto top = ptr->stack_top;
    ULT_SET_STACK_POINTER(top);
    const auto status = ptr->task(*this);
    exit(status);
  }
}

}  // namespace ult
