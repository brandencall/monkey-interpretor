#include "object/String.h"
#include <gtest/gtest.h>

TEST(ObjectTest, StringHashKey) {

    object::String *hello1 = new object::String("Hello World");
    object::String *hello2 = new object::String("Hello World");
    object::String *diff1 = new object::String("My name is johnny");
    object::String *diff2 = new object::String("My name is johnny");

    EXPECT_EQ(hello1->hashKey().value, hello2->hashKey().value) << "string with same content have different hash keys" << '\n';
    EXPECT_EQ(diff1->hashKey().value, diff2->hashKey().value) << "string with same content have different hash keys" << '\n';
    EXPECT_NE(hello1->hashKey().value, diff1->hashKey().value) << "string with diff content have same hash keys" << '\n';
}
