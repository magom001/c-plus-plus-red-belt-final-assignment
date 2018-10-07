#include "search_server.h"
#include "parse.h"
#include "test_runner.h"
#include "heavyTests.h"
#include "profile.h"
#include "add_duration.h"

#include <algorithm>
#include <iterator>
#include <map>
#include <vector>
#include <string>
#include <sstream>
#include <fstream>
#include <random>
#include <thread>


using namespace std;

int main() {
    LOG_DURATION("all program");
    TestRunner tr;
    RUN_TEST(tr, TestSplitIntoWords);
    RUN_TEST(tr, TestSerpFormat);
    RUN_TEST(tr, TestTop5);
    RUN_TEST(tr, TestHitcount);
    RUN_TEST(tr, TestRanking);
    RUN_TEST(tr, TestBasicSearch);
    RUN_TEST(tr, TestMidLoad);
    RUN_TEST(tr, TestHeavyLoad);
}
