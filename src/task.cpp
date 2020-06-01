#include <atomic>

#include "set-stack-pointer.hpp"
#include "ult-scheduler.hpp"
#include "ult-task.hpp"

namespace ult {

Task::Task(Scheduler* scheduler, raw_task task, void* arg, std::size_t stack_size)
    : scheduler(scheduler),
      id(scheduler->generate_task_id()),
      task(task),
      arg(arg),
      stack_size(stack_size) {
  stack_bottom = std::make_unique<std::byte[]>(stack_size);
  stack_top = stack_bottom.get() + stack_size;
}

void Task::yield() {
  if (setjmp(state)) {
    return;
  } else {
    longjmp(scheduler->scheduler_entry, Scheduler::setjmp_task_yield);
  }
}

void Task::exit(int status) {
  longjmp(scheduler->scheduler_entry, Scheduler::setjmp_task_exit);
}

void Task::run() {
  if (started) {
    longjmp(state, 1);
  } else {
    started = true;
    void* top = stack_top;
    ULT_SET_STACK_POINTER(top);
    task(*this, arg);
    exit();
  }
}

}  // namespace ult
