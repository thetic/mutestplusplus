/*
 * Copyright (c) 2007, Michael Feathers, James Grenning and Bas Vodde
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the <organization> nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE EARLIER MENTIONED AUTHORS ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL <copyright holder> BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef D_UTestMacros_h
#define D_UTestMacros_h

#include "CppUTest/CppUTestConfig.hpp"
#include "CppUTest/Utest.hpp"

/*! \brief Define a group of tests
 *
 * All tests in a TEST_GROUP share the same setup()
 * and teardown().  setup() is run before the opening
 * curly brace of each TEST and teardown() is
 * called after the closing curly brace of TEST.
 *
 */

#define TEST_GROUP_BASE(testGroup, baseclass)                                  \
    extern int externTestGroup##testGroup;                                     \
    int externTestGroup##testGroup = 0;                                        \
    struct TEST_GROUP_##CppUTestGroup##testGroup : public baseclass

#define TEST_BASE(testBaseClass) struct testBaseClass : public cpputest::Utest

#define TEST_GROUP(testGroup) TEST_GROUP_BASE(testGroup, cpputest::Utest)

#define TEST_SETUP() virtual void setup() override

#define TEST_TEARDOWN() virtual void teardown() override

#define TEST(testGroup, testName)                                              \
    /* External declarations for strict compilers */                           \
    class TEST_##testGroup##_##testName##_TestShell;                           \
    extern TEST_##testGroup##_##testName##_TestShell                           \
        TEST_##testGroup##_##testName##_TestShell_instance;                    \
                                                                               \
    class TEST_##testGroup##_##testName##_Test                                 \
        : public TEST_GROUP_##CppUTestGroup##testGroup                         \
    {                                                                          \
    public:                                                                    \
        TEST_##testGroup##_##testName##_Test() :                               \
            TEST_GROUP_##CppUTestGroup##testGroup()                            \
        {                                                                      \
        }                                                                      \
        void testBody() override;                                              \
    };                                                                         \
    class TEST_##testGroup##_##testName##_TestShell                            \
        : public cpputest::UtestShell                                          \
    {                                                                          \
        virtual cpputest::Utest* createTest() override                         \
        {                                                                      \
            return new TEST_##testGroup##_##testName##_Test;                   \
        }                                                                      \
    } TEST_##testGroup##_##testName##_TestShell_instance;                      \
    static cpputest::TestInstaller TEST_##testGroup##_##testName##_Installer(  \
        TEST_##testGroup##_##testName##_TestShell_instance, #testGroup,        \
        #testName, __FILE__, __LINE__                                          \
    );                                                                         \
    void TEST_##testGroup##_##testName##_Test::testBody()

#define IGNORE_TEST(testGroup, testName)                                       \
    /* External declarations for strict compilers */                           \
    class IGNORE##testGroup##_##testName##_TestShell;                          \
    extern IGNORE##testGroup##_##testName##_TestShell                          \
        IGNORE##testGroup##_##testName##_TestShell_instance;                   \
                                                                               \
    class IGNORE##testGroup##_##testName##_Test                                \
        : public TEST_GROUP_##CppUTestGroup##testGroup                         \
    {                                                                          \
    public:                                                                    \
        IGNORE##testGroup##_##testName##_Test() :                              \
            TEST_GROUP_##CppUTestGroup##testGroup()                            \
        {                                                                      \
        }                                                                      \
                                                                               \
    public:                                                                    \
        void testBody() override;                                              \
    };                                                                         \
    class IGNORE##testGroup##_##testName##_TestShell                           \
        : public cpputest::IgnoredUtestShell                                   \
    {                                                                          \
        virtual cpputest::Utest* createTest() override                         \
        {                                                                      \
            return new IGNORE##testGroup##_##testName##_Test;                  \
        }                                                                      \
    } IGNORE##testGroup##_##testName##_TestShell_instance;                     \
    static cpputest::TestInstaller TEST_##testGroup##testName##_Installer(     \
        IGNORE##testGroup##_##testName##_TestShell_instance, #testGroup,       \
        #testName, __FILE__, __LINE__                                          \
    );                                                                         \
    void IGNORE##testGroup##_##testName##_Test::testBody()

#define IMPORT_TEST_GROUP(testGroup)                                           \
    extern int externTestGroup##testGroup;                                     \
    extern int* p##testGroup;                                                  \
    int* p##testGroup = &externTestGroup##testGroup

#define CPPUTEST_DEFAULT_MAIN                                                  \
    /*#include <CppUTest/CommandLineTestRunner.h>*/                            \
    int main(int argc, char** argv)                                            \
    {                                                                          \
        return cpputest::CommandLineTestRunner::RunAllTests(argc, argv);       \
    }

// Different checking macros

#define CHECK(condition)                                                       \
    CHECK_TRUE_LOCATION(                                                       \
        condition, "CHECK", #condition, nullptr, __FILE__, __LINE__            \
    )

#define CHECK_TEXT(condition, text)                                            \
    CHECK_TRUE_LOCATION(                                                       \
        static_cast<bool>(condition), "CHECK", #condition, text, __FILE__,     \
        __LINE__                                                               \
    )

#define CHECK_TRUE(condition)                                                  \
    CHECK_TRUE_LOCATION(                                                       \
        static_cast<bool>(condition), "CHECK_TRUE", #condition, nullptr,       \
        __FILE__, __LINE__                                                     \
    )

#define CHECK_TRUE_TEXT(condition, text)                                       \
    CHECK_TRUE_LOCATION(                                                       \
        condition, "CHECK_TRUE", #condition, text, __FILE__, __LINE__          \
    )

#define CHECK_FALSE(condition)                                                 \
    CHECK_FALSE_LOCATION(                                                      \
        condition, "CHECK_FALSE", #condition, nullptr, __FILE__, __LINE__      \
    )

#define CHECK_FALSE_TEXT(condition, text)                                      \
    CHECK_FALSE_LOCATION(                                                      \
        condition, "CHECK_FALSE", #condition, text, __FILE__, __LINE__         \
    )

#define CHECK_TRUE_LOCATION(                                                   \
    condition, checkString, conditionString, text, file, line                  \
)                                                                              \
    do {                                                                       \
        cpputest::UtestShell::getCurrent()->assertTrue(                        \
            (condition), checkString, conditionString, text, file, line        \
        );                                                                     \
    } while (0)

#define CHECK_FALSE_LOCATION(                                                  \
    condition, checkString, conditionString, text, file, line                  \
)                                                                              \
    do {                                                                       \
        cpputest::UtestShell::getCurrent()->assertTrue(                        \
            !(condition), checkString, conditionString, text, file, line       \
        );                                                                     \
    } while (0)

// This check needs the operator!=(), and a StringFrom(YourType) function
#define CHECK_EQUAL(expected, actual)                                          \
    CHECK_EQUAL_LOCATION(expected, actual, nullptr, __FILE__, __LINE__)

#define CHECK_EQUAL_TEXT(expected, actual, text)                               \
    CHECK_EQUAL_LOCATION(expected, actual, text, __FILE__, __LINE__)

#define CHECK_EQUAL_LOCATION(expected, actual, text, file, line)               \
    do {                                                                       \
        if ((expected) != (actual)) {                                          \
            if ((actual) != (actual))                                          \
                cpputest::UtestShell::getCurrent()->print(                     \
                    "WARNING:\n\tThe \"Actual Parameter\" parameter is "       \
                    "evaluated multiple times resulting in different "         \
                    "values.\n\tThus the value in the error message is "       \
                    "probably incorrect.",                                     \
                    file, line                                                 \
                );                                                             \
            if ((expected) != (expected))                                      \
                cpputest::UtestShell::getCurrent()->print(                     \
                    "WARNING:\n\tThe \"Expected Parameter\" parameter is "     \
                    "evaluated multiple times resulting in different "         \
                    "values.\n\tThus the value in the error message is "       \
                    "probably incorrect.",                                     \
                    file, line                                                 \
                );                                                             \
            cpputest::UtestShell::getCurrent()->assertEquals(                  \
                true, cpputest::StringFrom(expected).asCharString(),           \
                cpputest::StringFrom(actual).asCharString(), text, file, line  \
            );                                                                 \
        } else {                                                               \
            cpputest::UtestShell::getCurrent()->assertLongsEqual(              \
                0L, 0L, nullptr, file, line                                    \
            );                                                                 \
        }                                                                      \
    } while (0)

#define CHECK_EQUAL_ZERO(actual) CHECK_EQUAL(0, (actual))

#define CHECK_EQUAL_ZERO_TEXT(actual, text)                                    \
    CHECK_EQUAL_TEXT(0, (actual), (text))

#define CHECK_COMPARE(first, relop, second)                                    \
    CHECK_COMPARE_TEXT(first, relop, second, nullptr)

#define CHECK_COMPARE_TEXT(first, relop, second, text)                         \
    CHECK_COMPARE_LOCATION(first, relop, second, text, __FILE__, __LINE__)

#define CHECK_COMPARE_LOCATION(first, relop, second, text, file, line)         \
    do {                                                                       \
        bool success = (first)relop(second);                                   \
        if (!success) {                                                        \
            cpputest::SimpleString conditionString;                            \
            conditionString += cpputest::StringFrom(first);                    \
            conditionString += " ";                                            \
            conditionString += #relop;                                         \
            conditionString += " ";                                            \
            conditionString += cpputest::StringFrom(second);                   \
            cpputest::UtestShell::getCurrent()->assertCompare(                 \
                false, "CHECK_COMPARE", conditionString.asCharString(), text,  \
                __FILE__, __LINE__                                             \
            );                                                                 \
        }                                                                      \
    } while (0)

// This check checks for char* string equality using strcmp.
// This makes up for the fact that CHECK_EQUAL only compares the pointers to
// char*'s
#define STRCMP_EQUAL(expected, actual)                                         \
    STRCMP_EQUAL_LOCATION(expected, actual, nullptr, __FILE__, __LINE__)

#define STRCMP_EQUAL_TEXT(expected, actual, text)                              \
    STRCMP_EQUAL_LOCATION(expected, actual, text, __FILE__, __LINE__)

#define STRCMP_EQUAL_LOCATION(expected, actual, text, file, line)              \
    do {                                                                       \
        cpputest::UtestShell::getCurrent()->assertCstrEqual(                   \
            expected, actual, text, file, line                                 \
        );                                                                     \
    } while (0)

#define STRNCMP_EQUAL(expected, actual, length)                                \
    STRNCMP_EQUAL_LOCATION(                                                    \
        expected, actual, length, nullptr, __FILE__, __LINE__                  \
    )

#define STRNCMP_EQUAL_TEXT(expected, actual, length, text)                     \
    STRNCMP_EQUAL_LOCATION(expected, actual, length, text, __FILE__, __LINE__)

#define STRNCMP_EQUAL_LOCATION(expected, actual, length, text, file, line)     \
    do {                                                                       \
        cpputest::UtestShell::getCurrent()->assertCstrNEqual(                  \
            expected, actual, length, text, file, line                         \
        );                                                                     \
    } while (0)

#define STRCMP_NOCASE_EQUAL(expected, actual)                                  \
    STRCMP_NOCASE_EQUAL_LOCATION(expected, actual, nullptr, __FILE__, __LINE__)

#define STRCMP_NOCASE_EQUAL_TEXT(expected, actual, text)                       \
    STRCMP_NOCASE_EQUAL_LOCATION(expected, actual, text, __FILE__, __LINE__)

#define STRCMP_NOCASE_EQUAL_LOCATION(expected, actual, text, file, line)       \
    do {                                                                       \
        cpputest::UtestShell::getCurrent()->assertCstrNoCaseEqual(             \
            expected, actual, text, file, line                                 \
        );                                                                     \
    } while (0)

#define STRCMP_CONTAINS(expected, actual)                                      \
    STRCMP_CONTAINS_LOCATION(expected, actual, nullptr, __FILE__, __LINE__)

#define STRCMP_CONTAINS_TEXT(expected, actual, text)                           \
    STRCMP_CONTAINS_LOCATION(expected, actual, text, __FILE__, __LINE__)

#define STRCMP_CONTAINS_LOCATION(expected, actual, text, file, line)           \
    do {                                                                       \
        cpputest::UtestShell::getCurrent()->assertCstrContains(                \
            expected, actual, text, file, line                                 \
        );                                                                     \
    } while (0)

#define STRCMP_NOCASE_CONTAINS(expected, actual)                               \
    STRCMP_NOCASE_CONTAINS_LOCATION(                                           \
        expected, actual, nullptr, __FILE__, __LINE__                          \
    )

#define STRCMP_NOCASE_CONTAINS_TEXT(expected, actual, text)                    \
    STRCMP_NOCASE_CONTAINS_LOCATION(expected, actual, text, __FILE__, __LINE__)

#define STRCMP_NOCASE_CONTAINS_LOCATION(expected, actual, text, file, line)    \
    do {                                                                       \
        cpputest::UtestShell::getCurrent()->assertCstrNoCaseContains(          \
            expected, actual, text, file, line                                 \
        );                                                                     \
    } while (0)

// Check two long integers for equality
#define LONGS_EQUAL(expected, actual)                                          \
    LONGS_EQUAL_LOCATION(                                                      \
        (expected), (actual),                                                  \
        "LONGS_EQUAL(" #expected ", " #actual ") failed", __FILE__, __LINE__   \
    )

#define LONGS_EQUAL_TEXT(expected, actual, text)                               \
    LONGS_EQUAL_LOCATION((expected), (actual), text, __FILE__, __LINE__)

#define UNSIGNED_LONGS_EQUAL(expected, actual)                                 \
    UNSIGNED_LONGS_EQUAL_LOCATION(                                             \
        (expected), (actual), nullptr, __FILE__, __LINE__                      \
    )

#define UNSIGNED_LONGS_EQUAL_TEXT(expected, actual, text)                      \
    UNSIGNED_LONGS_EQUAL_LOCATION(                                             \
        (expected), (actual), text, __FILE__, __LINE__                         \
    )

#define LONGS_EQUAL_LOCATION(expected, actual, text, file, line)               \
    do {                                                                       \
        cpputest::UtestShell::getCurrent()->assertLongsEqual(                  \
            static_cast<long>(expected), static_cast<long>(actual), text,      \
            file, line                                                         \
        );                                                                     \
    } while (0)

#define UNSIGNED_LONGS_EQUAL_LOCATION(expected, actual, text, file, line)      \
    do {                                                                       \
        cpputest::UtestShell::getCurrent()->assertUnsignedLongsEqual(          \
            static_cast<unsigned long>(expected),                              \
            static_cast<unsigned long>(actual), text, file, line               \
        );                                                                     \
    } while (0)

#define LONGLONGS_EQUAL(expected, actual)                                      \
    LONGLONGS_EQUAL_LOCATION(expected, actual, nullptr, __FILE__, __LINE__)

#define LONGLONGS_EQUAL_TEXT(expected, actual, text)                           \
    LONGLONGS_EQUAL_LOCATION(expected, actual, text, __FILE__, __LINE__)

#define UNSIGNED_LONGLONGS_EQUAL(expected, actual)                             \
    UNSIGNED_LONGLONGS_EQUAL_LOCATION(                                         \
        expected, actual, nullptr, __FILE__, __LINE__                          \
    )

#define UNSIGNED_LONGLONGS_EQUAL_TEXT(expected, actual, text)                  \
    UNSIGNED_LONGLONGS_EQUAL_LOCATION(                                         \
        expected, actual, text, __FILE__, __LINE__                             \
    )

#define LONGLONGS_EQUAL_LOCATION(expected, actual, text, file, line)           \
    do {                                                                       \
        cpputest::UtestShell::getCurrent()->assertLongLongsEqual(              \
            expected, actual, text, file, line                                 \
        );                                                                     \
    } while (0)

#define UNSIGNED_LONGLONGS_EQUAL_LOCATION(expected, actual, text, file, line)  \
    do {                                                                       \
        cpputest::UtestShell::getCurrent()->assertUnsignedLongLongsEqual(      \
            expected, actual, text, file, line                                 \
        );                                                                     \
    } while (0)

#define BYTES_EQUAL(expected, actual)                                          \
    LONGS_EQUAL((expected) & 0xff, (actual) & 0xff)

#define BYTES_EQUAL_TEXT(expected, actual, text)                               \
    LONGS_EQUAL_TEXT((expected) & 0xff, (actual) & 0xff, text)

#define SIGNED_BYTES_EQUAL(expected, actual)                                   \
    SIGNED_BYTES_EQUAL_LOCATION(expected, actual, __FILE__, __LINE__)

#define SIGNED_BYTES_EQUAL_LOCATION(expected, actual, file, line)              \
    do {                                                                       \
        cpputest::UtestShell::getCurrent()->assertSignedBytesEqual(            \
            expected, actual, nullptr, file, line                              \
        );                                                                     \
    } while (0)

#define SIGNED_BYTES_EQUAL_TEXT(expected, actual, text)                        \
    SIGNED_BYTES_EQUAL_TEXT_LOCATION(expected, actual, text, __FILE__, __LINE__)

#define SIGNED_BYTES_EQUAL_TEXT_LOCATION(expected, actual, text, file, line)   \
    do {                                                                       \
        cpputest::UtestShell::getCurrent()->assertSignedBytesEqual(            \
            expected, actual, text, file, line                                 \
        );                                                                     \
    } while (0)

#define POINTERS_EQUAL(expected, actual)                                       \
    POINTERS_EQUAL_LOCATION((expected), (actual), nullptr, __FILE__, __LINE__)

#define POINTERS_EQUAL_TEXT(expected, actual, text)                            \
    POINTERS_EQUAL_LOCATION((expected), (actual), text, __FILE__, __LINE__)

#define POINTERS_EQUAL_LOCATION(expected, actual, text, file, line)            \
    do {                                                                       \
        cpputest::UtestShell::getCurrent()->assertPointersEqual(               \
            (expected), (actual), text, file, line                             \
        );                                                                     \
    } while (0)

#define FUNCTIONPOINTERS_EQUAL(expected, actual)                               \
    FUNCTIONPOINTERS_EQUAL_LOCATION(                                           \
        expected, actual, nullptr, __FILE__, __LINE__                          \
    )

#define FUNCTIONPOINTERS_EQUAL_TEXT(expected, actual, text)                    \
    FUNCTIONPOINTERS_EQUAL_LOCATION(expected, actual, text, __FILE__, __LINE__)

#define FUNCTIONPOINTERS_EQUAL_LOCATION(expected, actual, text, file, line)    \
    do {                                                                       \
        cpputest::UtestShell::getCurrent()->assertFunctionPointersEqual(       \
            expected, actual, text, file, line                                 \
        );                                                                     \
    } while (0)

// Check two doubles for equality within a tolerance threshold
#define DOUBLES_EQUAL(expected, actual, threshold)                             \
    DOUBLES_EQUAL_LOCATION(                                                    \
        expected, actual, threshold, nullptr, __FILE__, __LINE__               \
    )

#define DOUBLES_EQUAL_TEXT(expected, actual, threshold, text)                  \
    DOUBLES_EQUAL_LOCATION(                                                    \
        expected, actual, threshold, text, __FILE__, __LINE__                  \
    )

#define DOUBLES_EQUAL_LOCATION(expected, actual, threshold, text, file, line)  \
    do {                                                                       \
        cpputest::UtestShell::getCurrent()->assertDoublesEqual(                \
            expected, actual, threshold, text, file, line                      \
        );                                                                     \
    } while (0)

#define MEMCMP_EQUAL(expected, actual, size)                                   \
    MEMCMP_EQUAL_LOCATION(expected, actual, size, nullptr, __FILE__, __LINE__)

#define MEMCMP_EQUAL_TEXT(expected, actual, size, text)                        \
    MEMCMP_EQUAL_LOCATION(expected, actual, size, text, __FILE__, __LINE__)

#define MEMCMP_EQUAL_LOCATION(expected, actual, size, text, file, line)        \
    do {                                                                       \
        cpputest::UtestShell::getCurrent()->assertBinaryEqual(                 \
            expected, actual, size, text, file, line                           \
        );                                                                     \
    } while (0)

#define BITS_EQUAL(expected, actual, mask)                                     \
    BITS_LOCATION(expected, actual, mask, nullptr, __FILE__, __LINE__)

#define BITS_EQUAL_TEXT(expected, actual, mask, text)                          \
    BITS_LOCATION(expected, actual, mask, text, __FILE__, __LINE__)

// NOLINTBEGIN(bugprone-sizeof-expression)
#define BITS_LOCATION(expected, actual, mask, text, file, line)                \
    do {                                                                       \
        cpputest::UtestShell::getCurrent()->assertBitsEqual(                   \
            expected, actual, mask, sizeof(actual), text, file, line           \
        );                                                                     \
    } while (0)
// NOLINTEND(bugprone-sizeof-expression)

#define ENUMS_EQUAL_INT(expected, actual)                                      \
    ENUMS_EQUAL_TYPE(int, expected, actual)

#define ENUMS_EQUAL_INT_TEXT(expected, actual, text)                           \
    ENUMS_EQUAL_TYPE_TEXT(int, expected, actual, text)

#define ENUMS_EQUAL_TYPE(underlying_type, expected, actual)                    \
    ENUMS_EQUAL_TYPE_LOCATION(                                                 \
        underlying_type, expected, actual, nullptr, __FILE__, __LINE__         \
    )

#define ENUMS_EQUAL_TYPE_TEXT(underlying_type, expected, actual, text)         \
    ENUMS_EQUAL_TYPE_LOCATION(                                                 \
        underlying_type, expected, actual, text, __FILE__, __LINE__            \
    )

#define ENUMS_EQUAL_TYPE_LOCATION(                                             \
    underlying_type, expected, actual, text, file, line                        \
)                                                                              \
    do {                                                                       \
        underlying_type expected_underlying_value =                            \
            static_cast<underlying_type>(expected);                            \
        underlying_type actual_underlying_value =                              \
            static_cast<underlying_type>(actual);                              \
        if (expected_underlying_value != actual_underlying_value) {            \
            cpputest::UtestShell::getCurrent()->assertEquals(                  \
                true,                                                          \
                cpputest::StringFrom(expected_underlying_value)                \
                    .asCharString(),                                           \
                cpputest::StringFrom(actual_underlying_value).asCharString(),  \
                text, file, line                                               \
            );                                                                 \
        } else {                                                               \
            cpputest::UtestShell::getCurrent()->assertLongsEqual(              \
                0L, 0L, nullptr, file, line                                    \
            );                                                                 \
        }                                                                      \
    } while (0)

// Fail if you get to this macro
// The macro FAIL may already be taken, so allow FAIL_TEST too
#ifndef FAIL
    #define FAIL(text) FAIL_LOCATION(text, __FILE__, __LINE__)

    #define FAIL_LOCATION(text, file, line)                                    \
        do {                                                                   \
            cpputest::UtestShell::getCurrent()->fail(text, file, line);        \
        } while (0)
#endif

#define FAIL_TEST(text) FAIL_TEST_LOCATION(text, __FILE__, __LINE__)

#define FAIL_TEST_LOCATION(text, file, line)                                   \
    do {                                                                       \
        cpputest::UtestShell::getCurrent()->fail(text, file, line);            \
    } while (0)

#define TEST_EXIT                                                              \
    do {                                                                       \
        cpputest::UtestShell::getCurrent()->exitTest();                        \
    } while (0)

#define UT_PRINT_LOCATION(text, file, line)                                    \
    do {                                                                       \
        cpputest::UtestShell::getCurrent()->print(text, file, line);           \
    } while (0)

#define UT_PRINT(text) UT_PRINT_LOCATION(text, __FILE__, __LINE__)

#if CPPUTEST_HAVE_EXCEPTIONS
    #define CHECK_THROWS(expected, expression)                                 \
        do {                                                                   \
            cpputest::SimpleString failure_msg("expected to throw " #expected  \
                                               "\nbut threw nothing");         \
            bool caught_expected = false;                                      \
            try {                                                              \
                (expression);                                                  \
            }                                                                  \
            catch (const expected&) {                                          \
                caught_expected = true;                                        \
            }                                                                  \
            catch (...) {                                                      \
                failure_msg = "expected to throw " #expected                   \
                              "\nbut threw a different type";                  \
            }                                                                  \
            if (!caught_expected) {                                            \
                cpputest::UtestShell::getCurrent()->fail(                      \
                    failure_msg.asCharString(), __FILE__, __LINE__             \
                );                                                             \
            } else {                                                           \
                cpputest::UtestShell::getCurrent()->countCheck();              \
            }                                                                  \
        } while (0)
#endif /* CPPUTEST_HAVE_EXCEPTIONS */

#define UT_CRASH()                                                             \
    do {                                                                       \
        cpputest::UtestShell::crash();                                         \
    } while (0)
#define RUN_ALL_TESTS(ac, av)                                                  \
    cpputest::CommandLineTestRunner::RunAllTests(ac, av)

#endif /*D_UTestMacros_h*/
