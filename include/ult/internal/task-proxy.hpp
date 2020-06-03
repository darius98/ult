#pragma once

#include "../task-control.hpp"
#include "../types.hpp"
#include "meta.hpp"

namespace ult {

class Task;

}  // namespace ult

namespace ult::internal {
TaskControl get_task_control(Task* task);

template <class Callable>
static int task_proxy(void* raw_task, void* arg) {
  const auto task = static_cast<Task*>(raw_task);
  const auto callable = static_cast<Callable*>(arg);
  int exit_status = 0;
  if constexpr (is_convertible<decltype((*callable)(get_task_control(task))), int>) {
    exit_status = (*callable)(get_task_control(task));
  } else {
    static_assert(is_same<void, decltype((*callable)(get_task_control(task)))>,
                  "A task's return type should be either void or convertible to int.");
    (*callable)(get_task_control(task));
  }
  delete callable;
  return exit_status;
}

}  // namespace ult::internal
