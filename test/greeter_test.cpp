#include <greeter.h>
#include <gtest/gtest.h>

TEST(greeter_test, greeter_coorect_output)
{
    ASSERT_EQ("Hello World!", greet());
}