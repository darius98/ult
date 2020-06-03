#include "task-data.hpp"

#include "ult/internal/task-data.hpp"

namespace ult::internal {

TaskDataPtr::TaskDataPtr(const TaskDataPtr& other) : ptr(other.ptr) {
  if (ptr != nullptr) {
    ptr->refs.fetch_add(1, std::memory_order_release);
  }
}

TaskDataPtr& TaskDataPtr::operator=(const TaskDataPtr& other) {
  if (this != &other) {
    if (ptr != nullptr && ptr->refs.fetch_sub(1, std::memory_order_acq_rel) == 1) {
      delete ptr;
    }
    ptr = other.ptr;
    if (ptr != nullptr) {
      ptr->refs.fetch_add(1, std::memory_order_release);
    }
  }
  return *this;
}

TaskDataPtr::TaskDataPtr(TaskDataPtr&& other) noexcept : ptr(other.ptr) {
  other.ptr = nullptr;
}

TaskDataPtr& TaskDataPtr::operator=(TaskDataPtr&& other) noexcept {
  if (this != &other) {
    if (ptr != nullptr && ptr->refs.fetch_sub(1, std::memory_order_acq_rel) == 1) {
      delete ptr;
    }
    ptr = other.ptr;
    other.ptr = nullptr;
  }
  return *this;
}

TaskDataPtr::~TaskDataPtr() {
  if (ptr != nullptr && ptr->refs.fetch_sub(1, std::memory_order_acq_rel) == 1) {
    delete ptr;
  }
}

TaskDataPtr::TaskDataPtr(internal::TaskData* data) : ptr(data) {
  if (ptr != nullptr) {
    ptr->refs.fetch_add(1, std::memory_order_release);
  }
}

TaskData::TaskData(Scheduler* scheduler, task_id_t id, internal::task_function_ptr task, void* arg,
                   stack_size_t stack_size)
    : scheduler(scheduler), id(id), task(task), arg(arg), stack_size(stack_size) {
  stack_bottom = std::make_unique<std::byte[]>(stack_size);
  stack_top = stack_bottom.get() + stack_size;
}

}  // namespace ult::internal
