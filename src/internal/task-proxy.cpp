#include "ult/internal/task-proxy.hpp"

#include "../task.hpp"
#include "ult/task-control.hpp"

namespace ult::internal {

TaskControl get_task_control(Task* task) {
  return task->control();
}

}  // namespace ult::internal
