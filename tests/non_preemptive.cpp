#include <gtest/gtest.h>

#include <ult.hpp>
#include <vector>

using ult::Scheduler;
using ult::Task;

TEST(NonPreemptive, NoTasks) {
  Scheduler s;
  s.run();
}

TEST(NonPreemptive, SingleTask) {
  Scheduler s;
  std::vector<int> checkpoints;
  s.enqueue_task([&checkpoints](Task& task) {
    checkpoints.push_back(0);
  });
  ASSERT_EQ(checkpoints, std::vector<int>{});
  s.run();
  ASSERT_EQ(checkpoints, std::vector<int>{0});
}

TEST(NonPreemptive, SingleTaskYield) {
  Scheduler s;
  std::vector<int> checkpoints;
  s.enqueue_task([&checkpoints](Task& task) {
    for (int i = 0; i < 3; i++) {
      checkpoints.push_back(i);
      task.yield();
    }
  });
  ASSERT_EQ(checkpoints, std::vector<int>{});
  s.run();
  ASSERT_EQ(checkpoints, (std::vector<int>{0, 1, 2}));
}

TEST(NonPreemptive, SingleTaskExit) {
  Scheduler s;
  std::vector<int> checkpoints;
  s.enqueue_task([&checkpoints](Task& task) {
    for (int i = 0; i < 10; i++) {
      if (i == 3) {
        task.exit();
      }
      checkpoints.push_back(i);
    }
  });
  ASSERT_EQ(checkpoints, std::vector<int>{});
  s.run();
  ASSERT_EQ(checkpoints, (std::vector<int>{0, 1, 2}));
}

TEST(NonPreemptive, SingleTaskYieldAndExit) {
  Scheduler s;
  std::vector<int> checkpoints;
  s.enqueue_task([&checkpoints](Task& task) {
    for (int i = 0; i < 10; i++) {
      if (i == 3) {
        task.exit();
      }
      checkpoints.push_back(i);
      task.yield();
    }
  });
  ASSERT_EQ(checkpoints, std::vector<int>{});
  s.run();
  ASSERT_EQ(checkpoints, (std::vector<int>{0, 1, 2}));
}

TEST(NonPreemptive, TwoTasks) {
  Scheduler s;
  std::vector<int> checkpoints;
  s.enqueue_task([&checkpoints](Task& task) {
    checkpoints.push_back(0);
  });
  ASSERT_EQ(checkpoints, std::vector<int>{});
  s.enqueue_task([&checkpoints](Task& task) {
    checkpoints.push_back(1);
  });
  ASSERT_EQ(checkpoints, std::vector<int>{});
  s.run();
  ASSERT_EQ(checkpoints, (std::vector<int>{0, 1}));
}

TEST(NonPreemptive, TwoTasksYield) {
  Scheduler s;
  std::vector<int> checkpoints;
  s.enqueue_task([&checkpoints](Task& task) {
    for (int i = 0; i < 3; i++) {
      checkpoints.push_back(i);
      task.yield();
    }
  });
  ASSERT_EQ(checkpoints, std::vector<int>{});
  s.enqueue_task([&checkpoints](Task& task) {
    for (int i = 3; i < 6; i++) {
      checkpoints.push_back(i);
      task.yield();
    }
  });
  ASSERT_EQ(checkpoints, std::vector<int>{});
  s.run();
  ASSERT_EQ(checkpoints, (std::vector<int>{0, 3, 1, 4, 2, 5}));
}

TEST(NonPreemptive, TwoTasksExit) {
  Scheduler s;
  std::vector<int> checkpoints;
  s.enqueue_task([&checkpoints](Task& task) {
    for (int i = 0; i < 10; i++) {
      if (i == 3) {
        task.exit();
      }
      checkpoints.push_back(i);
    }
  });
  ASSERT_EQ(checkpoints, std::vector<int>{});
  s.enqueue_task([&checkpoints](Task& task) {
    for (int i = 10; i < 20; i++) {
      if (i == 13) {
        task.exit();
      }
      checkpoints.push_back(i);
    }
  });
  ASSERT_EQ(checkpoints, std::vector<int>{});
  s.run();
  ASSERT_EQ(checkpoints, (std::vector<int>{0, 1, 2, 10, 11, 12}));
}

TEST(NonPreemptive, TwoTasksYieldAndExit) {
  Scheduler s;
  std::vector<int> checkpoints;
  s.enqueue_task([&checkpoints](Task& task) {
    for (int i = 0; i < 10; i++) {
      if (i == 3) {
        task.exit();
      }
      checkpoints.push_back(i);
      task.yield();
    }
  });
  ASSERT_EQ(checkpoints, std::vector<int>{});
  s.enqueue_task([&checkpoints](Task& task) {
    for (int i = 10; i < 20; i++) {
      if (i == 13) {
        task.exit();
      }
      checkpoints.push_back(i);
      task.yield();
    }
  });
  ASSERT_EQ(checkpoints, std::vector<int>{});
  s.run();
  ASSERT_EQ(checkpoints, (std::vector<int>{0, 10, 1, 11, 2, 12}));
}
