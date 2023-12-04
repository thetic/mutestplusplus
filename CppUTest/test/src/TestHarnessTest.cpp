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

#include "CppUTest/TestHarness.h"

#include "CppUTest/PlatformSpecificFunctions.hpp"
#include "CppUTest/TestHarness.hpp"
#include "CppUTest/TestOutput.hpp"
#include "CppUTest/TestRegistry.hpp"
#include "CppUTest/TestTestingFixture.hpp"

extern "C" int setup_teardown_was_called_in_test_group_in_C;
extern "C" int test_was_called_in_test_group_in_C;
int setup_teardown_was_called_in_test_group_in_C = 0;
int test_was_called_in_test_group_in_C = 0;

TEST_GROUP_C_WRAPPER(TestGroupInC){TEST_GROUP_C_SETUP_WRAPPER(TestGroupInC
) TEST_GROUP_C_TEARDOWN_WRAPPER(TestGroupInC)};

TEST_C_WRAPPER(TestGroupInC, checkThatTheTestHasRun)
IGNORE_TEST_C_WRAPPER(TestGroupInC, ignoreMacroForCFile)

/*
 * This test is a bit strange. They use the fact that you can do -r2 for
 * repeating the same run. When you do so, the same statics will be shared and
 * therefore we can test whether the setup/teardown is run correctly.
 */

TEST(TestGroupInC, setupHasBeenCalled)
{
    test_was_called_in_test_group_in_C++;
    /* Increased in setup, decreased in teardown. So at this point it must be 1
     * also on a multiple run */
    LONGS_EQUAL(1, setup_teardown_was_called_in_test_group_in_C);
}

static bool hasDestructorOfTheDestructorCheckedBeenCalled;

class HasTheDestructorBeenCalledChecker
{
public:
    HasTheDestructorBeenCalledChecker() {}
    ~HasTheDestructorBeenCalledChecker()
    {
        hasDestructorOfTheDestructorCheckedBeenCalled = true;
    }
};

TEST_GROUP(TestHarness_c)
{
    TestTestingFixture* fixture;
    TEST_SETUP()
    {
        hasDestructorOfTheDestructorCheckedBeenCalled = false;
        fixture = new TestTestingFixture();
    }
    TEST_TEARDOWN()
    {
        delete fixture;
    }
};

static void failBoolMethod_()
{
    HasTheDestructorBeenCalledChecker checker;
    CHECK_EQUAL_C_BOOL(1, 0);
} // LCOV_EXCL_LINE

TEST(TestHarness_c, checkBool)
{
    CHECK_EQUAL_C_BOOL(1, 1);
    CHECK_EQUAL_C_BOOL(1, 2);
    fixture->setTestFunction(failBoolMethod_);
    fixture->runAllTests();
    fixture->assertPrintContains("expected <true>\n\tbut was  <false>");
    fixture->assertPrintContains("arness_c");
    CHECK(!hasDestructorOfTheDestructorCheckedBeenCalled);
}

static void failBoolTextMethod_()
{
    HasTheDestructorBeenCalledChecker checker;
    CHECK_EQUAL_C_BOOL_TEXT(1, 0, "BoolTestText");
} // LCOV_EXCL_LINE

TEST(TestHarness_c, checkBoolText)
{
    CHECK_EQUAL_C_BOOL_TEXT(1, 1, "Text");
    CHECK_EQUAL_C_BOOL_TEXT(1, 2, "Text");
    fixture->setTestFunction(failBoolTextMethod_);
    fixture->runAllTests();
    fixture->assertPrintContains("expected <true>\n\tbut was  <false>");
    fixture->assertPrintContains("arness_c");
    fixture->assertPrintContains("Message: BoolTestText");
    CHECK(!hasDestructorOfTheDestructorCheckedBeenCalled);
}

static void failIntMethod_()
{
    HasTheDestructorBeenCalledChecker checker;
    CHECK_EQUAL_C_INT(1, 2);
} // LCOV_EXCL_LINE

TEST(TestHarness_c, checkInt)
{
    CHECK_EQUAL_C_INT(2, 2);
    fixture->setTestFunction(failIntMethod_);
    fixture->runAllTests();
    fixture->assertPrintContains("expected <1 (0x1)>\n\tbut was  <2 (0x2)>");
    fixture->assertPrintContains("arness_c");
    CHECK(!hasDestructorOfTheDestructorCheckedBeenCalled);
}

static void failIntTextMethod_()
{
    HasTheDestructorBeenCalledChecker checker;
    CHECK_EQUAL_C_INT_TEXT(1, 2, "IntTestText");
} // LCOV_EXCL_LINE

TEST(TestHarness_c, checkIntText)
{
    CHECK_EQUAL_C_INT_TEXT(2, 2, "Text");
    fixture->setTestFunction(failIntTextMethod_);
    fixture->runAllTests();
    fixture->assertPrintContains("expected <1 (0x1)>\n\tbut was  <2 (0x2)>");
    fixture->assertPrintContains("arness_c");
    fixture->assertPrintContains("Message: IntTestText");
    CHECK(!hasDestructorOfTheDestructorCheckedBeenCalled);
}

static void failUnsignedIntMethod_()
{
    HasTheDestructorBeenCalledChecker checker;
    CHECK_EQUAL_C_UINT(1, 2);
} // LCOV_EXCL_LINE

TEST(TestHarness_c, checkUnsignedInt)
{
    CHECK_EQUAL_C_UINT(2, 2);
    fixture->setTestFunction(failUnsignedIntMethod_);
    fixture->runAllTests();
    fixture->assertPrintContains("expected <1 (0x1)>\n\tbut was  <2 (0x2)>");
    fixture->assertPrintContains("arness_c");
    CHECK(!hasDestructorOfTheDestructorCheckedBeenCalled);
}

static void failUnsignedIntTextMethod_()
{
    HasTheDestructorBeenCalledChecker checker;
    CHECK_EQUAL_C_UINT_TEXT(1, 2, "UnsignedIntTestText");
} // LCOV_EXCL_LINE

TEST(TestHarness_c, checkUnsignedIntText)
{
    CHECK_EQUAL_C_UINT_TEXT(2, 2, "Text");
    fixture->setTestFunction(failUnsignedIntTextMethod_);
    fixture->runAllTests();
    fixture->assertPrintContains("expected <1 (0x1)>\n\tbut was  <2 (0x2)>");
    fixture->assertPrintContains("arness_c");
    fixture->assertPrintContains("Message: UnsignedIntTestText");
    CHECK(!hasDestructorOfTheDestructorCheckedBeenCalled);
}

static void failLongIntMethod_()
{
    HasTheDestructorBeenCalledChecker checker;
    CHECK_EQUAL_C_LONG(1, 2);
} // LCOV_EXCL_LINE

TEST(TestHarness_c, checkLongInt)
{
    CHECK_EQUAL_C_LONG(2, 2);
    fixture->setTestFunction(failLongIntMethod_);
    fixture->runAllTests();
    fixture->assertPrintContains("expected <1 (0x1)>\n\tbut was  <2 (0x2)>");
    fixture->assertPrintContains("arness_c");
    CHECK(!hasDestructorOfTheDestructorCheckedBeenCalled);
}

static void failLongIntTextMethod_()
{
    HasTheDestructorBeenCalledChecker checker;
    CHECK_EQUAL_C_LONG_TEXT(1, 2, "LongIntTestText");
} // LCOV_EXCL_LINE

TEST(TestHarness_c, checkLongIntText)
{
    CHECK_EQUAL_C_LONG_TEXT(2, 2, "Text");
    fixture->setTestFunction(failLongIntTextMethod_);
    fixture->runAllTests();
    fixture->assertPrintContains("expected <1 (0x1)>\n\tbut was  <2 (0x2)>");
    fixture->assertPrintContains("arness_c");
    fixture->assertPrintContains("Message: LongIntTestText");
    CHECK(!hasDestructorOfTheDestructorCheckedBeenCalled);
}

static void failUnsignedLongIntMethod_()
{
    HasTheDestructorBeenCalledChecker checker;
    CHECK_EQUAL_C_ULONG(1, 2);
} // LCOV_EXCL_LINE

TEST(TestHarness_c, checkUnsignedLongInt)
{
    CHECK_EQUAL_C_ULONG(2, 2);
    fixture->setTestFunction(failUnsignedLongIntMethod_);
    fixture->runAllTests();
    fixture->assertPrintContains("expected <1 (0x1)>\n\tbut was  <2 (0x2)>");
    fixture->assertPrintContains("arness_c");
    CHECK(!hasDestructorOfTheDestructorCheckedBeenCalled);
}

static void failUnsignedLongIntTextMethod_()
{
    HasTheDestructorBeenCalledChecker checker;
    CHECK_EQUAL_C_ULONG_TEXT(1, 2, "UnsignedLongIntTestText");
} // LCOV_EXCL_LINE

TEST(TestHarness_c, checkUnsignedLongIntText)
{
    CHECK_EQUAL_C_ULONG_TEXT(2, 2, "Text");
    fixture->setTestFunction(failUnsignedLongIntTextMethod_);
    fixture->runAllTests();
    fixture->assertPrintContains("expected <1 (0x1)>\n\tbut was  <2 (0x2)>");
    fixture->assertPrintContains("arness_c");
    fixture->assertPrintContains("Message: UnsignedLongIntTestText");
    CHECK(!hasDestructorOfTheDestructorCheckedBeenCalled);
}

static void failLongLongIntMethod_()
{
    HasTheDestructorBeenCalledChecker checker;
    CHECK_EQUAL_C_LONGLONG(1, 2);
} // LCOV_EXCL_LINE

TEST(TestHarness_c, checkLongLongInt)
{
    CHECK_EQUAL_C_LONGLONG(2, 2);
    fixture->setTestFunction(failLongLongIntMethod_);
    fixture->runAllTests();
    fixture->assertPrintContains("expected <1 (0x1)>\n\tbut was  <2 (0x2)>");
    fixture->assertPrintContains("arness_c");
    CHECK(!hasDestructorOfTheDestructorCheckedBeenCalled);
}

static void failLongLongIntTextMethod_()
{
    HasTheDestructorBeenCalledChecker checker;
    CHECK_EQUAL_C_LONGLONG_TEXT(1, 2, "LongLongTestText");
} // LCOV_EXCL_LINE

TEST(TestHarness_c, checkLongLongIntText)
{
    CHECK_EQUAL_C_LONGLONG_TEXT(2, 2, "Text");
    fixture->setTestFunction(failLongLongIntTextMethod_);
    fixture->runAllTests();
    fixture->assertPrintContains("expected <1 (0x1)>\n\tbut was  <2 (0x2)>");
    fixture->assertPrintContains("arness_c");
    fixture->assertPrintContains("Message: LongLongTestText");
    CHECK(!hasDestructorOfTheDestructorCheckedBeenCalled);
}

static void failUnsignedLongLongIntMethod_()
{
    HasTheDestructorBeenCalledChecker checker;
    CHECK_EQUAL_C_ULONGLONG(1, 2);
} // LCOV_EXCL_LINE

TEST(TestHarness_c, checkUnsignedLongLongInt)
{
    CHECK_EQUAL_C_ULONGLONG(2, 2);
    fixture->setTestFunction(failUnsignedLongLongIntMethod_);
    fixture->runAllTests();
    fixture->assertPrintContains("expected <1 (0x1)>\n\tbut was  <2 (0x2)>");
    fixture->assertPrintContains("arness_c");
    CHECK(!hasDestructorOfTheDestructorCheckedBeenCalled);
}

static void failUnsignedLongLongIntTextMethod_()
{
    HasTheDestructorBeenCalledChecker checker;
    CHECK_EQUAL_C_ULONGLONG_TEXT(1, 2, "UnsignedLongLongTestText");
} // LCOV_EXCL_LINE

TEST(TestHarness_c, checkUnsignedLongLongIntText)
{
    CHECK_EQUAL_C_ULONGLONG_TEXT(2, 2, "Text");
    fixture->setTestFunction(failUnsignedLongLongIntTextMethod_);
    fixture->runAllTests();
    fixture->assertPrintContains("expected <1 (0x1)>\n\tbut was  <2 (0x2)>");
    fixture->assertPrintContains("arness_c");
    fixture->assertPrintContains("Message: UnsignedLongLongTestText");
    CHECK(!hasDestructorOfTheDestructorCheckedBeenCalled);
}

static void failRealMethod_()
{
    HasTheDestructorBeenCalledChecker checker;
    CHECK_EQUAL_C_REAL(1.0, 2.0, 0.5);
} // LCOV_EXCL_LINE

TEST(TestHarness_c, checkReal)
{
    CHECK_EQUAL_C_REAL(1.0, 1.1, 0.5);
    fixture->setTestFunction(failRealMethod_);
    fixture->runAllTests();
    fixture->assertPrintContains("expected <1>\n\tbut was  <2>");
    fixture->assertPrintContains("arness_c");
    CHECK(!hasDestructorOfTheDestructorCheckedBeenCalled);
}

static void failRealTextMethod_()
{
    HasTheDestructorBeenCalledChecker checker;
    CHECK_EQUAL_C_REAL_TEXT(1.0, 2.0, 0.5, "RealTestText");
} // LCOV_EXCL_LINE

TEST(TestHarness_c, checkRealText)
{
    CHECK_EQUAL_C_REAL_TEXT(1.0, 1.1, 0.5, "Text");
    fixture->setTestFunction(failRealTextMethod_);
    fixture->runAllTests();
    fixture->assertPrintContains("expected <1>\n\tbut was  <2>");
    fixture->assertPrintContains("arness_c");
    fixture->assertPrintContains("Message: RealTestText");
    CHECK(!hasDestructorOfTheDestructorCheckedBeenCalled);
}

static void failCharMethod_()
{
    HasTheDestructorBeenCalledChecker checker;
    CHECK_EQUAL_C_CHAR('a', 'c');
}

TEST(TestHarness_c, checkChar)
{
    CHECK_EQUAL_C_CHAR('a', 'a');
    fixture->setTestFunction(failCharMethod_);
    fixture->runAllTests();
    fixture->assertPrintContains("expected <a>\n\tbut was  <c>");
    fixture->assertPrintContains("arness_c");
    CHECK(!hasDestructorOfTheDestructorCheckedBeenCalled);
}

static void failCharTextMethod_()
{
    HasTheDestructorBeenCalledChecker checker;
    CHECK_EQUAL_C_CHAR_TEXT('a', 'c', "CharTestText");
}

TEST(TestHarness_c, checkCharText)
{
    CHECK_EQUAL_C_CHAR_TEXT('a', 'a', "Text");
    fixture->setTestFunction(failCharTextMethod_);
    fixture->runAllTests();
    fixture->assertPrintContains("expected <a>\n\tbut was  <c>");
    fixture->assertPrintContains("arness_c");
    fixture->assertPrintContains("Message: CharTestText");
    CHECK(!hasDestructorOfTheDestructorCheckedBeenCalled);
}

static void failUnsignedByteMethod_()
{
    HasTheDestructorBeenCalledChecker checker;
    CHECK_EQUAL_C_UBYTE(254, 253);
}

TEST(TestHarness_c, checkUnsignedByte)
{
    CHECK_EQUAL_C_UBYTE(254, 254);
    fixture->setTestFunction(failUnsignedByteMethod_);
    fixture->runAllTests();
    fixture->assertPrintContains("expected <254>\n\tbut was  <253>");
    fixture->assertPrintContains("arness_c");
    CHECK(!hasDestructorOfTheDestructorCheckedBeenCalled);
}

static void failUnsignedByteTextMethod_()
{
    HasTheDestructorBeenCalledChecker checker;
    CHECK_EQUAL_C_UBYTE_TEXT(254, 253, "UnsignedByteTestText");
}

TEST(TestHarness_c, checkUnsignedByteText)
{
    CHECK_EQUAL_C_UBYTE_TEXT(254, 254, "Text");
    fixture->setTestFunction(failUnsignedByteTextMethod_);
    fixture->runAllTests();
    fixture->assertPrintContains("expected <254>\n\tbut was  <253>");
    fixture->assertPrintContains("arness_c");
    fixture->assertPrintContains("Message: UnsignedByteTestText");
    CHECK(!hasDestructorOfTheDestructorCheckedBeenCalled);
}

static void failSignedByteMethod_()
{
    HasTheDestructorBeenCalledChecker checker;
    CHECK_EQUAL_C_SBYTE(-3, -5);
}

TEST(TestHarness_c, checkSignedByte)
{
    CHECK_EQUAL_C_SBYTE(-3, -3);
    fixture->setTestFunction(failSignedByteMethod_);
    fixture->runAllTests();
    fixture->assertPrintContains("expected <-3>\n\tbut was  <-5>");
    fixture->assertPrintContains("arness_c");
    CHECK(!hasDestructorOfTheDestructorCheckedBeenCalled);
}

static void failSignedByteTextMethod_()
{
    HasTheDestructorBeenCalledChecker checker;
    CHECK_EQUAL_C_SBYTE_TEXT(-3, -5, "SignedByteTestText");
}

TEST(TestHarness_c, checkSignedByteText)
{
    CHECK_EQUAL_C_SBYTE_TEXT(-3, -3, "Text");
    fixture->setTestFunction(failSignedByteTextMethod_);
    fixture->runAllTests();
    fixture->assertPrintContains("expected <-3>\n\tbut was  <-5>");
    fixture->assertPrintContains("arness_c");
    fixture->assertPrintContains("Message: SignedByteTestText");
    CHECK(!hasDestructorOfTheDestructorCheckedBeenCalled);
}

static void failStringMethod_()
{
    HasTheDestructorBeenCalledChecker checker;
    CHECK_EQUAL_C_STRING("Hello", "Hello World");
} // LCOV_EXCL_LINE

TEST(TestHarness_c, checkString)
{
    CHECK_EQUAL_C_STRING("Hello", "Hello");
    fixture->setTestFunction(failStringMethod_);
    fixture->runAllTests();

    StringEqualFailure failure(
        UtestShell::getCurrent(), "file", 1, "Hello", "Hello World", ""
    );
    fixture->assertPrintContains(failure.getMessage());
    fixture->assertPrintContains("arness_c");
    CHECK(!hasDestructorOfTheDestructorCheckedBeenCalled);
}

static void failStringTextMethod_()
{
    HasTheDestructorBeenCalledChecker checker;
    CHECK_EQUAL_C_STRING_TEXT("Hello", "Hello World", "StringTestText");
} // LCOV_EXCL_LINE

TEST(TestHarness_c, checkStringText)
{
    CHECK_EQUAL_C_STRING_TEXT("Hello", "Hello", "Text");
    fixture->setTestFunction(failStringTextMethod_);
    fixture->runAllTests();

    StringEqualFailure failure(
        UtestShell::getCurrent(), "file", 1, "Hello", "Hello World", ""
    );
    fixture->assertPrintContains(failure.getMessage());
    fixture->assertPrintContains("arness_c");
    fixture->assertPrintContains("Message: StringTestText");
    CHECK(!hasDestructorOfTheDestructorCheckedBeenCalled);
}

static void failPointerMethod_()
{
    HasTheDestructorBeenCalledChecker checker;
    CHECK_EQUAL_C_POINTER(nullptr, reinterpret_cast<void*>(0x1));
}

TEST(TestHarness_c, checkPointer)
{
    CHECK_EQUAL_C_POINTER(nullptr, nullptr);
    fixture->setTestFunction(failPointerMethod_);
    fixture->runAllTests();
    fixture->assertPrintContains("expected <0x0>\n\tbut was  <0x1>");
    fixture->assertPrintContains("arness_c");
    CHECK(!hasDestructorOfTheDestructorCheckedBeenCalled);
}

static void failPointerTextMethod_()
{
    HasTheDestructorBeenCalledChecker checker;
    CHECK_EQUAL_C_POINTER_TEXT(
        nullptr, reinterpret_cast<void*>(0x1), "PointerTestText"
    );
}

TEST(TestHarness_c, checkPointerText)
{
    CHECK_EQUAL_C_POINTER_TEXT(nullptr, nullptr, "Text");
    fixture->setTestFunction(failPointerTextMethod_);
    fixture->runAllTests();
    fixture->assertPrintContains("expected <0x0>\n\tbut was  <0x1>");
    fixture->assertPrintContains("arness_c");
    fixture->assertPrintContains("Message: PointerTestText");
    CHECK(!hasDestructorOfTheDestructorCheckedBeenCalled);
}

static void failBitsMethod_()
{
    HasTheDestructorBeenCalledChecker checker;
    CHECK_EQUAL_C_BITS(0x0001, static_cast<unsigned short>(0x0003), 0xFFFF);
}

TEST(TestHarness_c, checkBits)
{
    CHECK_EQUAL_C_BITS(0xABCD, static_cast<unsigned short>(0xABCD), 0xFFFF);
    fixture->setTestFunction(failBitsMethod_);
    fixture->runAllTests();
    fixture->assertPrintContains(
        "expected <00000000 00000001>\n\tbut was  <00000000 00000011>"
    );
    fixture->assertPrintContains("arness_c");
    CHECK(!hasDestructorOfTheDestructorCheckedBeenCalled);
}

static void failBitsTextMethod_()
{
    HasTheDestructorBeenCalledChecker checker;
    CHECK_EQUAL_C_BITS_TEXT(
        0x0001, static_cast<unsigned short>(0x0003), 0xFFFF, "BitsTestText"
    );
}

TEST(TestHarness_c, checkBitsText)
{
    CHECK_EQUAL_C_BITS_TEXT(
        0xABCD, static_cast<unsigned short>(0xABCD), 0xFFFF, "Text"
    );
    fixture->setTestFunction(failBitsTextMethod_);
    fixture->runAllTests();
    fixture->assertPrintContains(
        "expected <00000000 00000001>\n\tbut was  <00000000 00000011>"
    );
    fixture->assertPrintContains("arness_c");
    fixture->assertPrintContains("Message: BitsTestText");
    CHECK(!hasDestructorOfTheDestructorCheckedBeenCalled);
}

static void failTextMethod_()
{
    HasTheDestructorBeenCalledChecker checker;
    FAIL_TEXT_C("Booo");
} // LCOV_EXCL_LINE

TEST(TestHarness_c, checkFailText)
{
    fixture->setTestFunction(failTextMethod_);
    fixture->runAllTests();
    fixture->assertPrintContains("Booo");
    fixture->assertPrintContains("arness_c");
    CHECK(!hasDestructorOfTheDestructorCheckedBeenCalled);
}

static void failMethod_()
{
    HasTheDestructorBeenCalledChecker checker;
    FAIL_C();
} // LCOV_EXCL_LINE

TEST(TestHarness_c, checkFail)
{
    fixture->setTestFunction(failMethod_);
    fixture->runAllTests();
    LONGS_EQUAL(1, fixture->getFailureCount());
    fixture->assertPrintContains("arness_c");
    CHECK(!hasDestructorOfTheDestructorCheckedBeenCalled);
}

static bool cpputestHasCrashed;

static void crashMethod()
{
    cpputestHasCrashed = true;
}

TEST(TestHarness_c, doesNotCrashIfNotSetToCrash)
{
    cpputestHasCrashed = false;
    UtestShell::setCrashMethod(crashMethod);
    fixture->setTestFunction(failMethod_);

    fixture->runAllTests();

    CHECK_FALSE(cpputestHasCrashed);
    LONGS_EQUAL(1, fixture->getFailureCount());
    CHECK(!hasDestructorOfTheDestructorCheckedBeenCalled);

    UtestShell::resetCrashMethod();
}

TEST(TestHarness_c, doesCrashIfSetToCrash)
{
    cpputestHasCrashed = false;
    UtestShell::setCrashOnFail();
    UtestShell::setCrashMethod(crashMethod);
    fixture->setTestFunction(failMethod_);

    fixture->runAllTests();

    CHECK(cpputestHasCrashed);
    CHECK(!hasDestructorOfTheDestructorCheckedBeenCalled);

    UtestShell::restoreDefaultTestTerminator();
    UtestShell::resetCrashMethod();
}

static void CheckMethod_()
{
    HasTheDestructorBeenCalledChecker checker;
    CHECK_C(false);
} // LCOV_EXCL_LINE

TEST(TestHarness_c, checkCheck)
{
    CHECK_C(true);
    fixture->setTestFunction(CheckMethod_);
    fixture->runAllTests();
    LONGS_EQUAL(1, fixture->getFailureCount());
    CHECK(!hasDestructorOfTheDestructorCheckedBeenCalled);
}

static void CheckTextMethod_()
{
    HasTheDestructorBeenCalledChecker checker;
    CHECK_C_TEXT(false, "CheckTestText");
} // LCOV_EXCL_LINE

TEST(TestHarness_c, checkCheckText)
{
    CHECK_C_TEXT(true, "Text");
    fixture->setTestFunction(CheckTextMethod_);
    fixture->runAllTests();
    LONGS_EQUAL(1, fixture->getFailureCount());
    fixture->assertPrintContains("Message: CheckTestText");
    CHECK(!hasDestructorOfTheDestructorCheckedBeenCalled);
}
