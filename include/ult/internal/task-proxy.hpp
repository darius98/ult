#pragma once

#include "../task-control.hpp"
#include "../types.hpp"
#include "meta.hpp"

namespace ult::internal {

TaskControl get_task_control(Task* task);

template <class Callable>
static exit_status_t task_proxy(Task* task, void* arg) {
  const auto callable = static_cast<Callable*>(arg);
  exit_status_t exit_status = 0;
  if constexpr (is_convertible<decltype((*callable)(get_task_control(task))), exit_status_t>) {
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
