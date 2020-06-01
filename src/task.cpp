#include "ult/task.hpp"

#include <atomic>
#include <csetjmp>
#include <cstddef>
#include <cstdlib>
#include <memory>

#include "set-stack-pointer.hpp"
#include "ult/scheduler.hpp"

namespace ult {

struct internal::TaskData {
  jmp_buf state{};
  std::byte* stack_top;
  bool started = false;
  Scheduler* scheduler;
  std::atomic<int32_t> refs{1};
  std::size_t id;
  Task::raw_task_ptr task;
  void* arg;
  Task::exit_status_type exit_status = 0;
  std::size_t stack_size;
  std::unique_ptr<std::byte[]> stack_bottom;
};

TaskPromise::TaskPromise(const TaskPromise& other) : impl(other.impl) {
  if (impl != nullptr) {
    impl->refs.fetch_add(1, std::memory_order_release);
  }
}
TaskPromise& TaskPromise::operator=(const TaskPromise& other) {
  if (this != &other) {
    if (impl != nullptr) {
      if (impl->refs.fetch_sub(1, std::memory_order_acq_rel) == 1) {
        delete impl;
      }
    }
    impl = other.impl;
    if (impl != nullptr) {
      impl->refs.fetch_add(1, std::memory_order_release);
    }
  }
  return *this;
}

TaskPromise::TaskPromise(TaskPromise&& other) noexcept : impl(other.impl) {
  other.impl = nullptr;
}

TaskPromise& TaskPromise::operator=(TaskPromise&& other) noexcept {
  if (this != &other) {
    if (impl != nullptr) {
      if (impl->refs.fetch_sub(1, std::memory_order_acq_rel) == 1) {
        delete impl;
      }
    }
    impl = other.impl;
    other.impl = nullptr;
  }
  return *this;
}

TaskPromise::~TaskPromise() {
  if (impl != nullptr) {
    if (impl->refs.fetch_sub(1, std::memory_order_acq_rel) == 1) {
      delete impl;
    }
  }
}

TaskPromise::TaskPromise(internal::TaskData* data) : impl(data) {
  if (impl != nullptr) {
    impl->refs.fetch_add(1, std::memory_order_release);
  }
}

int TaskPromise::exit_status() const {
  return impl->exit_status;
}

Task::Task(Scheduler* scheduler, raw_task_ptr task, void* arg, stack_size_type stack_size)
    : impl(new internal::TaskData()) {
  impl->scheduler = scheduler;
  impl->id = scheduler->generate_task_id();
  impl->task = task;
  impl->arg = arg;
  impl->stack_size = stack_size;
  impl->stack_bottom = std::make_unique<std::byte[]>(stack_size);
  impl->stack_top = impl->stack_bottom.get() + impl->stack_size;
}

Task::Task(Task&& other) noexcept : impl(other.impl) {
  other.impl = nullptr;
}

Task& Task::operator=(Task&& other) noexcept {
  if (this != &other) {
    if (impl != nullptr && impl->refs.fetch_sub(1, std::memory_order_acq_rel) == 1) {
      delete impl;
    }
    impl = other.impl;
    other.impl = nullptr;
  }
  return *this;
}

Task::~Task() {
  if (impl != nullptr && impl->refs.fetch_sub(1, std::memory_order_acq_rel) == 1) {
    delete impl;
  }
}

void Task::yield() {
  if (setjmp(impl->state)) {
    return;
  } else {
    impl->scheduler->yield_task();
  }
}

void Task::exit(exit_status_type status) {
  impl->exit_status = status;
  impl->scheduler->exit_task();
}

TaskPromise Task::make_promise() {
  return TaskPromise(impl);
}

void Task::run() {
  if (impl->started) {
    longjmp(impl->state, 1);
  } else {
    impl->started = true;
    void* top = impl->stack_top;
    ULT_SET_STACK_POINTER(top);
    const auto status = impl->task(*this, impl->arg);
    exit(status);
  }
}

}  // namespace ult
