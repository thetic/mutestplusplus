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

#include "CppUTest/PlatformSpecificFunctions.hpp"
#include "CppUTest/TestHarness.hpp"
#include "CppUTest/TestOutput.hpp"
#include "CppUTest/TestTestingFixture.hpp"

#include <math.h>

using namespace cpputest;

TEST_GROUP(UtestShell)
{
    TestTestingFixture fixture;
};

static void failMethod_()
{
    FAIL("This test fails");
}

static void passingTestMethod_()
{
    CHECK(true);
}

static void passingCheckEqualTestMethod_()
{
    CHECK_EQUAL(1, 1);
}

static void exitTestMethod_()
{
    TEST_EXIT;
    FAIL("Should not get here");
}

TEST(UtestShell, compareDoubles)
{
    CHECK(doubles_equal(1.0, 1.001, 0.01));
    CHECK(!doubles_equal(1.0, 1.1, 0.05));
    double a = 1.2345678;
    CHECK(doubles_equal(a, a, 0.000000001));
}

#ifdef NAN
TEST(UtestShell, compareDoublesNaN)
{
    CHECK(!doubles_equal(static_cast<double>(NAN), 1.001, 0.01));
    CHECK(!doubles_equal(1.0, static_cast<double>(NAN), 0.01));
    CHECK(!doubles_equal(1.0, 1.001, static_cast<double>(NAN)));
}
#endif

#ifdef INFINITY
TEST(UtestShell, compareDoublesInf)
{
    CHECK(!doubles_equal(static_cast<double>(INFINITY), 1.0, 0.01));
    CHECK(!doubles_equal(1.0, static_cast<double>(INFINITY), 0.01));
    CHECK(doubles_equal(1.0, -1.0, static_cast<double>(INFINITY)));
    CHECK(doubles_equal(
        static_cast<double>(INFINITY), static_cast<double>(INFINITY), 0.01
    ));
    CHECK(doubles_equal(
        static_cast<double>(INFINITY), static_cast<double>(INFINITY),
        static_cast<double>(INFINITY)
    ));
}
#endif

TEST(UtestShell, FailWillIncreaseTheAmountOfChecks)
{
    fixture.setTestFunction(failMethod_);
    fixture.runAllTests();
    LONGS_EQUAL(1, fixture.getCheckCount());
}

TEST(UtestShell, PassedCheckEqualWillIncreaseTheAmountOfChecks)
{
    fixture.setTestFunction(passingCheckEqualTestMethod_);
    fixture.runAllTests();
    LONGS_EQUAL(1, fixture.getCheckCount());
}

TEST(UtestShell, MacrosUsedInSetup)
{
    fixture.setSetup(failMethod_);
    fixture.setTestFunction(passingTestMethod_);
    fixture.runAllTests();
    LONGS_EQUAL(1, fixture.getFailureCount());
}

TEST(UtestShell, MacrosUsedInTearDown)
{
    fixture.setTeardown(failMethod_);
    fixture.setTestFunction(passingTestMethod_);
    fixture.runAllTests();
    LONGS_EQUAL(1, fixture.getFailureCount());
}

TEST(UtestShell, ExitLeavesQuietly)
{
    fixture.setTestFunction(exitTestMethod_);
    fixture.runAllTests();
    LONGS_EQUAL(0, fixture.getFailureCount());
}

static bool cpputestHasCrashed;

static void crashMethod()
{
    cpputestHasCrashed = true;
}

TEST(UtestShell, FailWillNotCrashIfNotEnabled)
{
    cpputestHasCrashed = false;
    UtestShell::setCrashMethod(crashMethod);

    fixture.setTestFunction(failMethod_);
    fixture.runAllTests();

    CHECK_FALSE(cpputestHasCrashed);
    LONGS_EQUAL(1, fixture.getFailureCount());

    UtestShell::resetCrashMethod();
}

TEST(UtestShell, FailWillCrashIfEnabled)
{
    cpputestHasCrashed = false;
    UtestShell::setCrashOnFail();
    UtestShell::setCrashMethod(crashMethod);

    fixture.setTestFunction(failMethod_);
    fixture.runAllTests();

    CHECK(cpputestHasCrashed);

    UtestShell::restoreDefaultTestTerminator();
    UtestShell::resetCrashMethod();
}

static int teardownCalled = 0;

static void teardownMethod_()
{
    teardownCalled++;
}

TEST(UtestShell, TeardownCalledAfterTestFailure)
{
    teardownCalled = 0;
    fixture.setTeardown(teardownMethod_);
    fixture.setTestFunction(failMethod_);
    fixture.runAllTests();
    LONGS_EQUAL(1, fixture.getFailureCount());
    LONGS_EQUAL(1, teardownCalled);
}

static int stopAfterFailure = 0;
static void stopAfterFailureMethod_()
{
    FAIL("fail");
    stopAfterFailure++;
}

TEST(UtestShell, TestStopsAfterTestFailure)
{
    stopAfterFailure = 0;
    fixture.setTestFunction(stopAfterFailureMethod_);
    fixture.runAllTests();
    CHECK(fixture.hasTestFailed());
    LONGS_EQUAL(1, fixture.getFailureCount());
    LONGS_EQUAL(0, stopAfterFailure);
}

TEST(UtestShell, TestStopsAfterSetupFailure)
{
    stopAfterFailure = 0;
    fixture.setSetup(stopAfterFailureMethod_);
    fixture.setTeardown(stopAfterFailureMethod_);
    fixture.setTestFunction(failMethod_);
    fixture.runAllTests();
    LONGS_EQUAL(2, fixture.getFailureCount());
    LONGS_EQUAL(0, stopAfterFailure);
}

#if CPPUTEST_HAVE_EXCEPTIONS

// Prevents -Wunreachable-code; should always be 'true'
static bool shouldThrowException = true;

static void thrownUnknownExceptionMethod_()
{
    if (shouldThrowException) {
        throw 33;
    }
    stopAfterFailure++;
}

TEST(UtestShell, TestStopsAfterUnknownExceptionIsThrown)
{
    bool initialRethrowExceptions = UtestShell::isRethrowingExceptions();
    UtestShell::setRethrowExceptions(false);
    stopAfterFailure = 0;
    shouldThrowException = true;
    fixture.setTestFunction(thrownUnknownExceptionMethod_);
    fixture.runAllTests();
    LONGS_EQUAL(1, fixture.getFailureCount());
    fixture.assertPrintContains(
        "Unexpected exception of unknown type was thrown"
    );
    LONGS_EQUAL(0, stopAfterFailure);
    UtestShell::setRethrowExceptions(initialRethrowExceptions);
}

TEST(UtestShell, NoExceptionIsRethrownIfEnabledButNotThrown)
{
    bool initialRethrowExceptions = UtestShell::isRethrowingExceptions();
    bool exceptionRethrown = false;
    stopAfterFailure = 0;
    UtestShell::setRethrowExceptions(true);
    shouldThrowException = false;
    fixture.setTestFunction(thrownUnknownExceptionMethod_);
    try {
        fixture.runAllTests();
    }
    catch (...) {
        exceptionRethrown = true;
    }
    CHECK_FALSE(exceptionRethrown);
    LONGS_EQUAL(0, fixture.getFailureCount());
    LONGS_EQUAL(1, stopAfterFailure);
    UtestShell::setRethrowExceptions(initialRethrowExceptions);
}

TEST(UtestShell, UnknownExceptionIsRethrownIfEnabled)
{
    bool initialRethrowExceptions = UtestShell::isRethrowingExceptions();
    bool exceptionRethrown = false;
    stopAfterFailure = 0;
    UtestShell::setRethrowExceptions(true);
    shouldThrowException = true;
    fixture.setTestFunction(thrownUnknownExceptionMethod_);
    try {
        fixture.runAllTests();
        stopAfterFailure++;
    }
    catch (...) {
        exceptionRethrown = true;
    }
    CHECK_TRUE(exceptionRethrown);
    LONGS_EQUAL(1, fixture.getFailureCount());
    fixture.assertPrintContains(
        "Unexpected exception of unknown type was thrown"
    );
    LONGS_EQUAL(0, stopAfterFailure);
    UtestShell::setRethrowExceptions(initialRethrowExceptions);
}

    #ifndef CPPUTEST_STD_CPP_LIB_DISABLED
static void thrownStandardExceptionMethod_()
{
    if (shouldThrowException) {
        throw std::runtime_error("exception text");
    }
    stopAfterFailure++;
}

TEST(UtestShell, TestStopsAfterStandardExceptionIsThrown)
{
    bool initialRethrowExceptions = UtestShell::isRethrowingExceptions();
    UtestShell::setRethrowExceptions(false);
    stopAfterFailure = 0;
    shouldThrowException = true;
    fixture.setTestFunction(thrownStandardExceptionMethod_);
    fixture.runAllTests();
    LONGS_EQUAL(1, fixture.getFailureCount());
        #if CPPUTEST_HAVE_RTTI
    fixture.assertPrintContains("Unexpected exception of type '");
    fixture.assertPrintContains("runtime_error");
    fixture.assertPrintContains("' was thrown: exception text");
        #else
    fixture.assertPrintContains(
        "Unexpected exception of unknown type was thrown"
    );
        #endif
    LONGS_EQUAL(0, stopAfterFailure);
    UtestShell::setRethrowExceptions(initialRethrowExceptions);
}

TEST(UtestShell, StandardExceptionIsRethrownIfEnabled)
{
    bool initialRethrowExceptions = UtestShell::isRethrowingExceptions();
    bool exceptionRethrown = false;
    stopAfterFailure = 0;
    UtestShell::setRethrowExceptions(true);
    shouldThrowException = true;
    fixture.setTestFunction(thrownStandardExceptionMethod_);
    try {
        fixture.runAllTests();
        stopAfterFailure++;
    }
    catch (const std::exception&) {
        exceptionRethrown = true;
    }
    CHECK_TRUE(exceptionRethrown);
    LONGS_EQUAL(1, fixture.getFailureCount());
    fixture.assertPrintContains("Unexpected exception of type '");
    fixture.assertPrintContains("runtime_error");
    fixture.assertPrintContains("' was thrown: exception text");
    LONGS_EQUAL(0, stopAfterFailure);
    UtestShell::setRethrowExceptions(initialRethrowExceptions);
}
    #endif
#endif

TEST(UtestShell, veryVebose)
{
    UtestShell shell("Group", "name", __FILE__, __LINE__);
    StringBufferTestOutput normalOutput;
    normalOutput.verbose(TestOutput::level_veryVerbose);
    NullTestPlugin plugin;

    TestResult result(normalOutput);
    shell.runOneTestInCurrentProcess(&plugin, result);
    STRCMP_CONTAINS(
        "\n------ before runTest", normalOutput.getOutput().asCharString()
    );
}

class defaultUtestShell : public UtestShell
{
};

TEST(
    UtestShell,
    this_test_covers_the_UtestShell_createTest_and_Utest_testBody_methods
)
{
    defaultUtestShell shell;
    fixture.addTest(&shell);
    fixture.runAllTests();
    LONGS_EQUAL(2, fixture.getTestCount());
}

#if CPPUTEST_HAVE_EXCEPTIONS

static bool destructorWasCalledOnFailedTest = false;

static void destructorCalledForLocalObjects_()
{
    SetBooleanOnDestructorCall pleaseCallTheDestructor(
        destructorWasCalledOnFailedTest
    );
    destructorWasCalledOnFailedTest = false;
    FAIL("fail");
}

TEST(UtestShell, DestructorIsCalledForLocalObjectsWhenTheTestFails)
{
    fixture.setTestFunction(destructorCalledForLocalObjects_);
    fixture.runAllTests();
    CHECK(destructorWasCalledOnFailedTest);
}

#endif

TEST_GROUP(IgnoredUtestShell)
{
    TestTestingFixture fixture;
    IgnoredUtestShell ignoredTest;
    ExecFunctionTestShell normalUtestShell;

    void setup() override
    {
        fixture.addTest(&ignoredTest);
        fixture.addTest(&normalUtestShell);
    }
};

TEST(IgnoredUtestShell, doesIgnoreCount)
{
    fixture.runAllTests();
    LONGS_EQUAL(1, fixture.getIgnoreCount());
}

TEST(IgnoredUtestShell, printsIGNORE_TESTwhenVerbose)
{
    fixture.setOutputVerbose();
    fixture.runAllTests();
    fixture.assertPrintContains("IGNORE_TEST");
}

TEST(IgnoredUtestShell, runIgnoredOptionSpecifiedThenIncreaseRunCount)
{
    ignoredTest.setRunIgnored();
    fixture.runAllTests();
    LONGS_EQUAL(3, fixture.getRunCount());
    LONGS_EQUAL(0, fixture.getIgnoreCount());
}

TEST(IgnoredUtestShell, runIgnoredOptionNotSpecifiedThenIncreaseIgnoredCount)
{
    fixture.runAllTests();
    LONGS_EQUAL(2, fixture.getRunCount());
    LONGS_EQUAL(1, fixture.getIgnoreCount());
}

TEST(
    IgnoredUtestShell, runIgnoredOptionSpecifiedWillNotInfluenceNormalTestCount
)
{
    normalUtestShell.setRunIgnored();
    fixture.runAllTests();
    LONGS_EQUAL(2, fixture.getRunCount());
    LONGS_EQUAL(1, fixture.getIgnoreCount());
}

TEST(IgnoredUtestShell, runIgnoredOptionSpecifiedThenReturnTESTInFormattedName)
{
    ignoredTest.setGroupName("TestGroup");
    ignoredTest.setTestName("TestName");
    ignoredTest.setRunIgnored();
    fixture.runAllTests();
    STRCMP_EQUAL(
        "TEST(TestGroup, TestName)",
        ignoredTest.getFormattedName().asCharString()
    );
}

TEST(
    IgnoredUtestShell,
    runIgnoredOptionNotSpecifiedThenReturnIGNORETESTInFormattedName
)
{
    ignoredTest.setGroupName("TestGroup");
    ignoredTest.setTestName("TestName");
    fixture.runAllTests();
    STRCMP_EQUAL(
        "IGNORE_TEST(TestGroup, TestName)",
        ignoredTest.getFormattedName().asCharString()
    );
}

TEST(IgnoredUtestShell, runIgnoredOptionNotSpecifiedThenWillRunReturnFalse)
{
    CHECK_FALSE(ignoredTest.willRun());
}

TEST(IgnoredUtestShell, runIgnoredOptionSpecifiedThenWillRunReturnTrue)
{
    ignoredTest.setRunIgnored();
    CHECK_TRUE(ignoredTest.willRun());
}

TEST_BASE(MyOwnTest)
{
    MyOwnTest() : inTest(false) {}
    bool inTest;

    void setup() override
    {
        CHECK(!inTest);
        inTest = true;
    }
    void teardown() override
    {
        CHECK(inTest);
        inTest = false;
    }
};

TEST_GROUP_BASE(UtestMyOwn, MyOwnTest){};

TEST(UtestMyOwn, test)
{
    CHECK(inTest);
}

class NullParameterTest : public UtestShell
{
};

TEST(UtestMyOwn, NullParameters)
{
    NullParameterTest nullTest; /* Bug fix tests for creating a test without a
                                   name, fix in SimpleString */
    TestFilter emptyFilter;
    CHECK(nullTest.shouldRun(&emptyFilter, &emptyFilter));
}

class AllocateAndDeallocateInConstructorAndDestructor
{
    char* memory_;
    char* morememory_;

public:
    AllocateAndDeallocateInConstructorAndDestructor()
    {
        memory_ = new char[100];
        morememory_ = nullptr;
    }
    void allocateMoreMemory()
    {
        morememory_ = new char[123];
    }

    ~AllocateAndDeallocateInConstructorAndDestructor()
    {
        delete[] memory_;
        delete[] morememory_;
    }
};

TEST_GROUP(
    CanHaveMemberVariablesInTestGroupThatAllocateMemoryWithoutCausingMemoryLeaks
)
{
    AllocateAndDeallocateInConstructorAndDestructor dummy;
};

TEST(
    CanHaveMemberVariablesInTestGroupThatAllocateMemoryWithoutCausingMemoryLeaks,
    testInTestGroupName
)
{
    dummy.allocateMoreMemory();
}

TEST_GROUP(UtestShellPointerArrayTest)
{
    UtestShell* test0;
    UtestShell* test1;
    UtestShell* test2;

    void setup() override
    {
        test0 = new IgnoredUtestShell();
        test1 = new IgnoredUtestShell();
        test2 = new IgnoredUtestShell();

        test0->addTest(test1);
        test1->addTest(test2);
    }

    void teardown() override
    {
        delete test0;
        delete test1;
        delete test2;
    }
};

TEST(UtestShellPointerArrayTest, testsAreInOrder)
{
    UtestShellPointerArray tests(test0);
    CHECK(tests.get(0) == test0);
    CHECK(tests.get(1) == test1);
    CHECK(tests.get(2) == test2);
}

TEST(
    UtestShellPointerArrayTest,
    relinkingTestsWillKeepThemTheSameWhenNothingWasDone
)
{
    UtestShellPointerArray tests(test0);
    tests.relinkTestsInOrder();
    CHECK(tests.get(0) == test0);
    CHECK(tests.get(1) == test1);
    CHECK(tests.get(2) == test2);
}
