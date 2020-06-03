#pragma once

namespace ult::internal {

struct TaskData;

class TaskDataPtr {
 public:
  TaskDataPtr() = default;

  TaskDataPtr(const TaskDataPtr&);
  TaskDataPtr& operator=(const TaskDataPtr&);

  TaskDataPtr(TaskDataPtr&&) noexcept;
  TaskDataPtr& operator=(TaskDataPtr&&) noexcept;

  ~TaskDataPtr();

 protected:
  explicit TaskDataPtr(TaskData* data);

  TaskData* ptr = nullptr;
};

}  // namespace ult::internal
