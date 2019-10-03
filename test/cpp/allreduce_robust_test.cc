#define RABIT_CXXTESTDEFS_H
#include <gtest/gtest.h>

#include <chrono>
#include <string>
#include <iostream>
#include "../../src/allreduce_robust.h"

TEST(allreduce_robust, sync_error_timeout)
{
  rabit::engine::AllreduceRobust m;

  std::string mock_str = "rabit_timeout=2";
  char cmd[mock_str.size()+1];
  std::copy(mock_str.begin(), mock_str.end(), cmd);
  cmd[mock_str.size()] = '\0';

  char* argv[] = {cmd};
  m.Init(1, argv);
  m.rank = 0;
  rabit::engine::AllreduceRobust::ReturnType err_type(rabit::engine::AllreduceRobust::ReturnTypeEnum::kSockError);
  EXPECT_EQ(m.CheckAndRecover(err_type), false);

  EXPECT_EXIT({
    std::this_thread::sleep_for(std::chrono::seconds(3));
    }, ::testing::ExitedWithCode(255), "");
}

TEST(allreduce_robust, sync_error_reset)
{
  rabit::engine::AllreduceRobust m;

  std::string rabit_timeout = "rabit_timeout=3";
  char cmd[rabit_timeout.size()+1];
  std::copy(rabit_timeout.begin(), rabit_timeout.end(), cmd);
  cmd[rabit_timeout.size()] = '\0';

  std::string rabit_debug = "rabit_debug=1";
  char cmd2[rabit_debug.size()+1];
  std::copy(rabit_debug.begin(), rabit_debug.end(), cmd2);
  cmd2[rabit_debug.size()] = '\0';

  char* argv[] = {cmd, cmd2};
  m.Init(2, argv);
  m.rank = 0;
  rabit::engine::AllreduceRobust::ReturnType err_type(rabit::engine::AllreduceRobust::ReturnTypeEnum::kSockError);
  rabit::engine::AllreduceRobust::ReturnType succ_type(rabit::engine::AllreduceRobust::ReturnTypeEnum::kSuccess);
  EXPECT_EQ(m.CheckAndRecover(err_type), false);
  std::this_thread::sleep_for(std::chrono::seconds(1));
  EXPECT_EQ(m.CheckAndRecover(succ_type), false);
  EXPECT_EXIT({
    std::this_thread::sleep_for(std::chrono::seconds(3));
  }, ::testing::ExitedWithCode(0), "");
}