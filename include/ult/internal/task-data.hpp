#pragma once

namespace ult::internal {

struct TaskData;

class TaskDataPtr {
 public:
  TaskDataPtr() = default;

  explicit TaskDataPtr(TaskData* data);

  TaskDataPtr(const TaskDataPtr&);
  TaskDataPtr& operator=(const TaskDataPtr&);

  TaskDataPtr(TaskDataPtr&&) noexcept;
  TaskDataPtr& operator=(TaskDataPtr&&) noexcept;

  ~TaskDataPtr();

 protected:
  TaskData* ptr = nullptr;
};

}  // namespace ult::internal
