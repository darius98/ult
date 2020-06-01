#include "ult-scheduler.hpp"

namespace ult {

void Scheduler::run() {
  const auto setjmp_status = setjmp(scheduler_entry);
  if (setjmp_status == setjmp_task_exit) {
    tasks.erase(tasks.begin() + running_task_index);
  }
  if (setjmp_status == setjmp_task_yield) {
    running_task_index++;
  }
  if (tasks.empty()) {
    return;
  }
  running_task_index = running_task_index % tasks.size();
  tasks[running_task_index].run();
}

std::size_t Scheduler::generate_task_id() {
  return next_task_id.fetch_add(1, std::memory_order_acq_rel);
}

}  // namespace ult
