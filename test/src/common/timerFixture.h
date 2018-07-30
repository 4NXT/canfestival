#ifndef _CANFESTIVAL_TEST_SRC_COMMON_TIMERFIXTURE_H
#define _CANFESTIVAL_TEST_SRC_COMMON_TIMERFIXTURE_H

#include "gtest/gtest.h"
extern "C" {
#include "applicfg.h"
#include "timer.h"
}

class TimerFixture : public ::testing::Test {
protected:
	virtual void SetUp();
};
#endif
