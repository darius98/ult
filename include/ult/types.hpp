#pragma once

namespace ult {

class Task;

using task_id_t = decltype(sizeof(int));     // size_t
using stack_size_t = decltype(sizeof(int));  // size_t
using exit_status_t = int;

}  // namespace ult

namespace ult::internal {

using task_function_ptr = exit_status_t (*)(Task*, void*);

}  // namespace ult::internal
