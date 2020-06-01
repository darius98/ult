#include "ult/scheduler.hpp"

#include <atomic>
#include <csetjmp>
#include <queue>

namespace ult {

constexpr int setjmp_task_yield = 1;
constexpr int setjmp_task_exit = 2;

struct Scheduler::SchedulerData {
  jmp_buf scheduler_entry{};
  Task running_task;
  std::queue<Task> tasks;
  std::atomic<Task::stack_size_type> next_task_id = 1;
};

Scheduler::Scheduler() : impl(new SchedulerData()) {
}

Scheduler::~Scheduler() {
  delete impl;
}

void Scheduler::run() {
  if (setjmp(impl->scheduler_entry) == setjmp_task_yield) {
    impl->tasks.push(std::move(impl->running_task));
  }
  if (!impl->tasks.empty()) {
    impl->running_task = std::move(impl->tasks.front());
    impl->tasks.pop();
    impl->running_task.run();
  }
}

void Scheduler::enqueue_raw_task(Task task) {
  impl->tasks.push(std::move(task));
}

void Scheduler::yield_task() {
  longjmp(impl->scheduler_entry, setjmp_task_yield);
}

void Scheduler::exit_task() {
  longjmp(impl->scheduler_entry, setjmp_task_exit);
}

Task::stack_size_type Scheduler::generate_task_id() {
  return impl->next_task_id.fetch_add(1, std::memory_order_acq_rel);
}

}  // namespace ult
