#include "ult/task-promise.hpp"

#include "internal/task-data.hpp"

namespace ult {

bool TaskPromise::is_done() const {
  return ptr->is_done.load(std::memory_order_acquire);
}

int TaskPromise::exit_status() const {
  return ptr->exit_status;
}

Scheduler& TaskPromise::scheduler() const {
  return *ptr->scheduler;
}

}  // namespace ult
