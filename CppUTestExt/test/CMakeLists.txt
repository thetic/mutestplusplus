add_executable(CppUTestExtTests
    src/AllTests.cpp
    src/CodeMemoryReporterTest.cpp
    src/ExpectedFunctionsListTest.cpp
    src/ExpectedFunctionsListTest.cpp
    src/GMockTest.cpp
    src/GTest1Test.cpp
    src/GTest2ConvertorTest.cpp
    src/IEEE754PluginTest.cpp
    src/IEEE754PluginTest_c.c
    src/MemoryReportAllocatorTest.cpp
    src/MemoryReportFormatterTest.cpp
    src/MemoryReporterPluginTest.cpp
    src/MockActualCallTest.cpp
    src/MockCallTest.cpp
    src/MockCheatSheetTest.cpp
    src/MockComparatorCopierTest.cpp
    src/MockComparatorCopierTest.cpp
    src/MockComparatorCopierTest.cpp
    src/MockExpectedCallTest.cpp
    src/MockFailureReporterForTest.cpp
    src/MockFailureReporterForTest.cpp
    src/MockFailureReporterForTest.cpp
    src/MockFailureReporterForTest.cpp
    src/MockFailureReporterForTest.cpp
    src/MockFailureReporterForTest.cpp
    src/MockFailureReporterForTest.cpp
    src/MockFailureReporterForTest.cpp
    src/MockFailureReporterForTest.cpp
    src/MockFailureTest.cpp
    src/MockHierarchyTest.cpp
    src/MockHierarchyTest.cpp
    src/MockHierarchyTest.cpp
    src/MockNamedValueTest.cpp
    src/MockParameterTest.cpp
    src/MockPluginTest.cpp
    src/MockPluginTest.cpp
    src/MockReturnValueTest.cpp
    src/MockReturnValueTest.cpp
    src/MockStrictOrderTest.cpp
    src/MockSupportTest.cpp
    src/MockSupport_cTest.cpp
    src/MockSupport_cTestCFile.c
    src/OrderedTestTest.cpp
    src/OrderedTestTest_c.c
)

target_include_directories(CppUTestExtTests
    PRIVATE
        include
)

target_link_libraries(CppUTestExtTests
    PRIVATE
        CppUTest
        CppUTestExt
)

include(CppUTest)
cpputest_discover_tests(CppUTestExtTests
    DETAILED FALSE
)

if(CPPUTEST_TEST_GTEST)
    find_package(GTest)
    if(NOT GTest_FOUND)
        if(CMAKE_VERSION VERSION_GREATER_EQUAL "3.11")
            include(FetchContent)
            FetchContent_Declare(googletest
                URL https://github.com/google/googletest/archive/release-1.8.0.zip
            )
            if(NOT googletest_POPULATED)
                FetchContent_Populate(googletest)
                add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR})
            endif()
            set(GTEST_LIBRARIES gmock gtest)
        else()
            # FetchContent is not supported.
            message(FATAL_ERROR
                "GTest not found"
            )
        endif()
    endif()
    target_link_libraries(CppUTestExtTests PRIVATE ${GTEST_LIBRARIES})
    target_compile_definitions(CppUTestExtTests PRIVATE CPPUTEST_INCLUDE_GTEST_TESTS)
endif()
