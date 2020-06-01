#include <gtest/gtest.h>

#include <ult.hpp>

using ult::Scheduler;
using ult::Task;
using ult::TaskPromise;

TEST(TaskReturn, NoReturnNoExit) {
  Scheduler s;
  const auto task = s.enqueue_task([](Task&) {});
  s.run();
  ASSERT_EQ(task.exit_status(), 0);
}

TEST(TaskReturn, ZeroExit) {
  Scheduler s;
  const auto task = s.enqueue_task([](Task& task) {
    task.exit(0);
  });
  s.run();
  ASSERT_EQ(task.exit_status(), 0);
}

TEST(TaskReturn, NonZeroExit) {
  Scheduler s;
  const auto task = s.enqueue_task([](Task& task) {
    task.exit(17);
  });
  s.run();
  ASSERT_EQ(task.exit_status(), 17);
}

TEST(TaskReturn, ZeroReturn) {
  Scheduler s;
  const auto task = s.enqueue_task([](Task&) {
    return 0;
  });
  s.run();
  ASSERT_EQ(task.exit_status(), 0);
}

TEST(TaskReturn, NonZeroReturn) {
  Scheduler s;
  const auto task = s.enqueue_task([](Task&) {
    return 17;
  });
  s.run();
  ASSERT_EQ(task.exit_status(), 17);
}

TEST(TaskReturn, TwoTasks) {
  Scheduler s;
  const auto task1 = s.enqueue_task([](Task&) {
    return 17;
  });
  const auto task2 = s.enqueue_task([](Task&) {
    return 23;
  });
  s.run();
  ASSERT_EQ(task1.exit_status(), 17);
  ASSERT_EQ(task2.exit_status(), 23);
}

TEST(TaskReturn, ArrayOfTasks) {
  Scheduler s;
  std::vector<TaskPromise> tasks;
  for (int i = 0; i < 1000; i++) {
    auto task = s.enqueue_task([i](Task& t) {
      if (i % 2 == 0) {
        t.exit(i);
      }
      return i;
    });
    tasks.push_back(std::move(task));
  }
  s.run();
  for (int i = 0; i < 1000; i++) {
    ASSERT_EQ(tasks[i].exit_status(), i);
  }
}

