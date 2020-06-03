#include "ult/scheduler.hpp"

#include <atomic>
#include <csetjmp>
#include <queue>

#include "internal/task-data.hpp"
#include "task.hpp"

namespace {

constexpr int setjmp_task_yield = 1;
constexpr int setjmp_task_exit = 2;

}  // namespace

namespace ult {

struct Scheduler::SchedulerData {
  jmp_buf scheduler_entry{};
  Task running_task;
  std::queue<Task> tasks;
  std::atomic<stack_size_t> next_task_id = 1;
};

Scheduler::Scheduler() : impl(new SchedulerData()) {
}

Scheduler::Scheduler(Scheduler&& other) noexcept: impl(other.impl) {
  other.impl = nullptr;
}

Scheduler& Scheduler::operator=(Scheduler&& other) noexcept {
  if (this != &other) {
    delete impl;
    impl = other.impl;
    other.impl = nullptr;
  }
  return *this;
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

TaskPromise Scheduler::add_task_raw(internal::task_function_ptr raw_task, void* arg,
                                    stack_size_t stack_size) {
  const auto ptr = new internal::TaskData(this, generate_task_id(), raw_task, arg, stack_size);
  impl->tasks.push(Task(ptr));
  return TaskPromise(ptr);
}

void Scheduler::yield_task() {
  longjmp(impl->scheduler_entry, setjmp_task_yield);
}

void Scheduler::exit_task() {
  longjmp(impl->scheduler_entry, setjmp_task_exit);
}

task_id_t Scheduler::generate_task_id() {
  return impl->next_task_id.fetch_add(1, std::memory_order_acq_rel);
}

}  // namespace ult
