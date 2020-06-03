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

TaskData* make_task_data(Scheduler* scheduler, task_id_t id, internal::task_function_ptr task,
                         void* arg, stack_size_t stack_size) {
  const auto ptr = new TaskData();
  ptr->scheduler = scheduler;
  ptr->id = id;
  ptr->task = task;
  ptr->arg = arg;
  ptr->stack_size = stack_size;
  ptr->stack_bottom = std::make_unique<std::byte[]>(stack_size);
  ptr->stack_top = ptr->stack_bottom.get() + ptr->stack_size;
  return ptr;
}

}  // namespace ult::internal
