#include "task.hpp"

#include "internal/task-data.hpp"
#include "set-stack-pointer.hpp"

namespace ult {

Task::Task(internal::TaskData* ptr): internal::TaskDataPtr(ptr) {}

TaskControl Task::control() {
  return TaskControl(ptr);
}

void Task::exit(int status) {
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
