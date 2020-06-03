#include "ult/task-control.hpp"

#include "internal/task-data.hpp"

namespace ult {

TaskPromise TaskControl::promise() const {
  return TaskPromise(ptr);
}

Scheduler& TaskControl::scheduler() const {
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

}  // namespace ult