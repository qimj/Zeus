

#include <gtest/gtest.h>
#include "src/future/apply.hh"

int func1(int arg1, char arg2, double arg3, const std::string& arg4) {
    std::cout << "call func1(" << arg1 << ", " << arg2 << ", " << arg3 << ", " << arg4 << ")" << std::endl;
    return 0;
}

int func2(int arg1, int arg2) {
    std::cout << "call func2(" << arg1 << ", " << arg2 << ")" << std::endl;
    return arg1 + arg2;
}

TEST(tuple, test){

    auto tuple1 = std::make_tuple(1, 'A', 1.2, "Just a String");
    auto result1 = apply(func1, tuple1);
    std::cout << "result1 = " << result1 << std::endl;

    auto tuple2 = std::make_tuple(1, 2);
    auto result2 = apply(func2, tuple2);
    std::cout << "result2 = " << result2 << std::endl;
}