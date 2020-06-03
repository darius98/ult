#include "task.hpp"

#include <memory>

#include "internal/task-data.hpp"
#include "set-stack-pointer.hpp"
#include "ult/scheduler.hpp"

namespace ult {

Task::Task(Scheduler* scheduler, task_id_t id, internal::task_function_ptr task, void* arg,
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

void Task::run() {
  if (ptr->started) {
    longjmp(ptr->state, 1);
  } else {
    ptr->started = true;
    auto top = ptr->stack_top;
    ULT_SET_STACK_POINTER(top);
    const auto status = ptr->task(this, ptr->arg);
    exit(status);
  }
}

}  // namespace ult
