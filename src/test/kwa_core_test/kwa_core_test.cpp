#include "kwa_core_test.h"

namespace
{
TEST(TestTest, Foo)
{
    int a = 1;
    EXPECT_EQ(1, a);
}
}

int main(int argc, char* argv[])
{
    std::srand(static_cast<unsigned int>(std::time(0)));
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
