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

#include "CppUTest/TestHarness.hpp"
#include "CppUTestExt/MockCheckedActualCall.hpp"
#include "CppUTestExt/MockCheckedExpectedCall.hpp"
#include "CppUTestExt/MockExpectedCallsList.hpp"
#include "CppUTestExt/MockFailure.hpp"
#include "MockFailureReporterForTest.hpp"

TEST_GROUP(MockCheckedActualCall)
{
    cpputest::extensions::MockExpectedCallsList* emptyList;
    cpputest::extensions::MockExpectedCallsList* list;
    cpputest::extensions::MockFailureReporter* reporter;

    void setup() override
    {
        emptyList = new cpputest::extensions::MockExpectedCallsList;
        list = new cpputest::extensions::MockExpectedCallsList;
        reporter = MockFailureReporterForTest::getReporter();
    }

    void teardown() override
    {
        CHECK_NO_MOCK_FAILURE();

        MockFailureReporterForTest::clearReporter();

        delete emptyList;
        delete list;
    }
};

TEST(MockCheckedActualCall, unExpectedCall)
{
    cpputest::extensions::MockCheckedActualCall actualCall(
        1, reporter, *emptyList
    );
    actualCall.withName("unexpected");

    cpputest::extensions::MockUnexpectedCallHappenedFailure expectedFailure(
        mockFailureTest(), "unexpected", *list
    );
    CHECK_EXPECTED_MOCK_FAILURE(expectedFailure);
}

TEST(MockCheckedActualCall, unExpectedCallWithAParameter)
{
    cpputest::extensions::MockCheckedActualCall actualCall(
        1, reporter, *emptyList
    );
    actualCall.withName("unexpected").withParameter("bar", 0);

    cpputest::extensions::MockUnexpectedCallHappenedFailure expectedFailure(
        mockFailureTest(), "unexpected", *list
    );
    CHECK_EXPECTED_MOCK_FAILURE(expectedFailure);
}

TEST(MockCheckedActualCall, unExpectedCallWithAnOutputParameter)
{
    cpputest::extensions::MockCheckedActualCall actualCall(
        1, reporter, *emptyList
    );
    actualCall.withName("unexpected").withOutputParameter("bar", nullptr);

    cpputest::extensions::MockUnexpectedCallHappenedFailure expectedFailure(
        mockFailureTest(), "unexpected", *list
    );
    CHECK_EXPECTED_MOCK_FAILURE(expectedFailure);
}

TEST(MockCheckedActualCall, unExpectedCallOnObject)
{
    int object;

    cpputest::extensions::MockCheckedActualCall actualCall(
        1, reporter, *emptyList
    );
    actualCall.withName("unexpected").onObject(&object);

    cpputest::extensions::MockUnexpectedCallHappenedFailure expectedFailure(
        mockFailureTest(), "unexpected", *list
    );
    CHECK_EXPECTED_MOCK_FAILURE(expectedFailure);

    CHECK(actualCall.hasFailed()
    ); // Checks that onObject() doesn't "reset" call state
}

TEST(
    MockCheckedActualCall,
    actualCallWithNoReturnValueAndMeaninglessCallOrderForCoverage
)
{
    cpputest::extensions::MockCheckedActualCall actualCall(
        1, reporter, *emptyList
    );
    actualCall.withName("noreturn").withCallOrder(0).returnValue();

    cpputest::extensions::MockUnexpectedCallHappenedFailure expectedFailure(
        mockFailureTest(), "noreturn", *list
    );
    CHECK_EXPECTED_MOCK_FAILURE(expectedFailure);
}

TEST(MockCheckedActualCall, unExpectedParameterName)
{
    cpputest::extensions::MockCheckedExpectedCall call1;
    call1.withName("func");
    list->addExpectedCall(&call1);

    cpputest::extensions::MockCheckedActualCall actualCall(1, reporter, *list);
    actualCall.withName("func").withParameter("integer", 1);

    cpputest::extensions::MockNamedValue parameter("integer");
    parameter.setValue(1);

    cpputest::extensions::MockUnexpectedInputParameterFailure expectedFailure(
        mockFailureTest(), "func", parameter, *list
    );
    CHECK_EXPECTED_MOCK_FAILURE(expectedFailure);
}

TEST(MockCheckedActualCall, multipleSameFunctionsExpectingAndHappenGradually)
{
    cpputest::extensions::MockCheckedExpectedCall* call1 =
        new cpputest::extensions::MockCheckedExpectedCall();
    cpputest::extensions::MockCheckedExpectedCall* call2 =
        new cpputest::extensions::MockCheckedExpectedCall();
    call1->withName("func");
    call2->withName("func");
    list->addExpectedCall(call1);
    list->addExpectedCall(call2);

    LONGS_EQUAL(2, list->amountOfUnfulfilledExpectations());

    cpputest::extensions::MockCheckedActualCall actualCall1(1, reporter, *list);
    actualCall1.withName("func");
    actualCall1.checkExpectations();

    LONGS_EQUAL(1, list->amountOfUnfulfilledExpectations());

    cpputest::extensions::MockCheckedActualCall actualCall2(2, reporter, *list);
    actualCall2.withName("func");
    actualCall2.checkExpectations();

    LONGS_EQUAL(0, list->amountOfUnfulfilledExpectations());

    list->deleteAllExpectationsAndClearList();
}

TEST(MockCheckedActualCall, MockIgnoredActualCallWorksAsItShould)
{
    cpputest::extensions::MockIgnoredActualCall actual;
    actual.withName("func");
    actual.withCallOrder(1);

    CHECK(false == actual.returnBoolValue());
    CHECK(true == actual.returnBoolValueOrDefault(true));
    CHECK(false == actual.returnBoolValueOrDefault(false));
    CHECK(0 == actual.returnUnsignedLongIntValue());
    CHECK(0 == actual.returnIntValue());
    CHECK(1ul == actual.returnUnsignedLongIntValueOrDefault(1ul));
    CHECK(1 == actual.returnIntValueOrDefault(1));
    CHECK(0 == actual.returnLongIntValue());
    CHECK(1l == actual.returnLongIntValueOrDefault(1l));
    CHECK(0 == actual.returnUnsignedIntValue());
    CHECK(1u == actual.returnUnsignedIntValueOrDefault(1u));
    CHECK(0 == actual.returnLongLongIntValue());
    CHECK(1ll == actual.returnLongLongIntValueOrDefault(1ll));
    CHECK(0 == actual.returnUnsignedLongLongIntValue());
    CHECK(1ull == actual.returnUnsignedLongLongIntValueOrDefault(1ull));
    DOUBLES_EQUAL(0.0, actual.returnDoubleValue(), 0.0);
    DOUBLES_EQUAL(1.5, actual.returnDoubleValueOrDefault(1.5), 0.0);
    STRCMP_EQUAL("bla", actual.returnStringValueOrDefault("bla"));
    STRCMP_EQUAL("", actual.returnStringValue());
    CHECK(nullptr == actual.returnPointerValue());
    CHECK(
        reinterpret_cast<void*>(0x2) ==
        actual.returnPointerValueOrDefault(reinterpret_cast<void*>(0x2))
    );
    CHECK(nullptr == actual.returnConstPointerValue());
    CHECK(
        reinterpret_cast<const void*>(0x2) ==
        actual.returnConstPointerValueOrDefault(
            reinterpret_cast<const void*>(0x2)
        )
    );
    CHECK(nullptr == actual.returnFunctionPointerValue());
    CHECK(
        reinterpret_cast<void (*)()>(1) ==
        actual.returnFunctionPointerValueOrDefault(
            reinterpret_cast<void (*)()>(0x1)
        )
    );
    CHECK_FALSE(actual.hasReturnValue());
    CHECK(actual.returnValue().equals(cpputest::extensions::MockNamedValue(""))
    );
}

TEST(MockCheckedActualCall, remainderOfMockActualCallTraceWorksAsItShould)
{
    int value;
    const int const_value = 1;
    const unsigned char mem_buffer[] = {0xFE, 0x15};
    void (*function_value)() = reinterpret_cast<void (*)()>(0xDEAD);
    cpputest::extensions::MockActualCallTrace actual;
    actual.withName("func");
    actual.withCallOrder(1);
    actual.onObject(&value);

    actual.withBoolParameter("bool", true);
    actual.withUnsignedIntParameter("unsigned_int", 1U);
    actual.withUnsignedLongIntParameter("unsigned_long", 1UL);
    actual.withLongIntParameter("long_int", 1L);
    actual.withLongLongIntParameter("long_long_int", 1LL);
    actual.withUnsignedLongLongIntParameter("unsigned_long_long_int", 1ULL);
    actual.withPointerParameter("pointer", &value);
    actual.withConstPointerParameter("const_pointer", &const_value);
    actual.withFunctionPointerParameter("function_pointer", function_value);
    actual.withMemoryBufferParameter(
        "mem_buffer", mem_buffer, sizeof(mem_buffer)
    );
    actual.withParameterOfType("int", "named_type", &const_value);

    cpputest::SimpleString expectedString("\nFunction name:func");
    expectedString += " withCallOrder:1";
    expectedString += " onObject:0x";
    expectedString += cpputest::HexStringFrom(&value);
    expectedString += " bool:true";
    expectedString += " unsigned_int:1 (0x1)";
    expectedString += " unsigned_long:1 (0x1)";
    expectedString += " long_int:1 (0x1)";
    expectedString += " long_long_int:1 (0x1)";
    expectedString += " unsigned_long_long_int:1 (0x1)";
    expectedString += " pointer:0x";
    expectedString += cpputest::HexStringFrom(&value);
    expectedString += " const_pointer:0x";
    expectedString += cpputest::HexStringFrom(&const_value);
    expectedString += " function_pointer:0x";
    expectedString += cpputest::HexStringFrom(function_value);
    expectedString += " mem_buffer:Size = 2 | HexContents = FE 15";
    expectedString += " int named_type:0x";
    expectedString += cpputest::HexStringFrom(&const_value);
    STRCMP_EQUAL(expectedString.asCharString(), actual.getTraceOutput());

    CHECK_FALSE(actual.hasReturnValue());
    CHECK(actual.returnValue().equals(cpputest::extensions::MockNamedValue(""))
    );
    CHECK(false == actual.returnBoolValue());
    CHECK(false == actual.returnBoolValueOrDefault(true));
    CHECK(0 == actual.returnLongIntValue());
    CHECK(0 == actual.returnUnsignedLongIntValue());
    CHECK(0 == actual.returnIntValue());
    CHECK(0 == actual.returnUnsignedLongIntValueOrDefault(1ul));
    CHECK(0 == actual.returnIntValueOrDefault(1));
    CHECK(0 == actual.returnLongIntValue());
    CHECK(0 == actual.returnLongIntValueOrDefault(1l));
    CHECK(0 == actual.returnLongLongIntValue());
    CHECK(0 == actual.returnLongLongIntValueOrDefault(1ll));
    CHECK(0 == actual.returnUnsignedLongLongIntValue());
    CHECK(0 == actual.returnUnsignedLongLongIntValueOrDefault(1ull));
    CHECK(0 == actual.returnUnsignedIntValue());
    CHECK(0 == actual.returnUnsignedIntValueOrDefault(1u));
    DOUBLES_EQUAL(0.0, actual.returnDoubleValue(), 0.0);
    DOUBLES_EQUAL(0.0, actual.returnDoubleValueOrDefault(1.0), 0.0);
    STRCMP_EQUAL("", actual.returnStringValueOrDefault("bla"));
    STRCMP_EQUAL("", actual.returnStringValue());
    CHECK(nullptr == actual.returnPointerValue());
    CHECK(nullptr == actual.returnPointerValueOrDefault(nullptr));
    CHECK(nullptr == actual.returnConstPointerValue());
    CHECK(nullptr == actual.returnConstPointerValueOrDefault(nullptr));
    CHECK(nullptr == actual.returnFunctionPointerValue());
    CHECK(nullptr == actual.returnFunctionPointerValueOrDefault(nullptr));
}

TEST(MockCheckedActualCall, MockActualCallTraceClear)
{
    cpputest::extensions::MockActualCallTrace actual;
    actual.withName("func");
    actual.clear();
    STRCMP_EQUAL("", actual.getTraceOutput());
}
