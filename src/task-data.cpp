#include "task-data.hpp"

#include "ult/task.hpp"

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

}  // namespace ult::internal
