#pragma once

namespace ult {

using task_id_t = decltype(sizeof(int));     // size_t
using stack_size_t = decltype(sizeof(int));  // size_t

}  // namespace ult

namespace ult::internal {

using task_function_ptr = int (*)(void*, void*);

}  // namespace ult::internal
