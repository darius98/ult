#include <gtest/gtest.h>

#include <ult.hpp>

using ult::Scheduler;
using ult::Task;
using ult::TaskPromise;

void fibonacci_exit(Task& current_task, int n) {
  if (n <= 1) {
    current_task.exit(n);
  }
  const auto sub_task_1 = current_task.scheduler().add_task([n](Task& sub_task) {
    fibonacci_exit(sub_task, n - 1);
  });
  const auto sub_task_2 = current_task.scheduler().add_task([n](Task& sub_task) {
    fibonacci_exit(sub_task, n - 2);
  });
  while (!sub_task_1.is_done() || !sub_task_2.is_done()) {
    current_task.yield();
  }
  current_task.exit(sub_task_1.exit_status() + sub_task_2.exit_status());
}

TEST(Integration, EnqueueTasksFromTasksWithExit) {
  Scheduler s;
  const auto promise = s.add_task([](Task& current_task) {
    fibonacci_exit(current_task, 17);
  });
  s.run();
  ASSERT_TRUE(promise.is_done());
  ASSERT_EQ(promise.exit_status(), 1597);
}

int fibonacci_return(Task& current_task, int n) {
  if (n <= 1) {
    return n;
  }
  const auto sub_task_1 = current_task.scheduler().add_task([n](Task& sub_task) {
    return fibonacci_return(sub_task, n - 1);
  });
  const auto sub_task_2 = current_task.scheduler().add_task([n](Task& sub_task) {
    return fibonacci_return(sub_task, n - 2);
  });
  while (!sub_task_1.is_done() || !sub_task_2.is_done()) {
    current_task.yield();
  }
  return sub_task_1.exit_status() + sub_task_2.exit_status();
}

TEST(Integration, EnqueueTasksFromTasksWithReturn) {
  Scheduler s;
  const auto promise = s.add_task([](Task& current_task) {
    fibonacci_exit(current_task, 17);
  });
  s.run();
  ASSERT_TRUE(promise.is_done());
  ASSERT_EQ(promise.exit_status(), 1597);
}

int fibonacci_stack(Task& current_task, int n) {
  if (n <= 1) {
    return n;
  }
  const auto sub_task_1 = current_task.scheduler().add_task([n](Task& sub_task) {
    return fibonacci_stack(sub_task, n - 1);
  });
  const auto n_2 = fibonacci_stack(current_task, n - 2);
  while (!sub_task_1.is_done()) {
    current_task.yield();
  }
  return sub_task_1.exit_status() + n_2;
}

TEST(Integration, EnqueueTasksFromTasksWithStack) {
  Scheduler s;
  const auto promise = s.add_task([](Task& current_task) {
    return fibonacci_stack(current_task, 17);
  });
  s.run();
  ASSERT_TRUE(promise.is_done());
  ASSERT_EQ(promise.exit_status(), 1597);
}
