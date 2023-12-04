add_library(CppUTest
    src/CommandLineArguments.cpp
    src/TestHarness_c.cpp
    src/TestRegistry.cpp
    src/CommandLineTestRunner.cpp
    src/SimpleString.cpp
    src/SimpleStringInternalCache.cpp
    src/TestMemoryAllocator.cpp
    src/TestResult.cpp
    src/JUnitTestOutput.cpp
    src/TeamCityTestOutput.cpp
    src/TestFailure.cpp
    src/TestOutput.cpp
    src/TestFilter.cpp
    src/TestPlugin.cpp
    src/TestTestingFixture.cpp
    src/SimpleMutex.cpp
    src/Utest.cpp
)

target_compile_features(CppUTest
    PUBLIC
        cxx_std_11
        c_std_99
)

if(CPPUTEST_PLATFORM)
    add_subdirectory(platform/${CPPUTEST_PLATFORM})
endif()

target_compile_definitions(CppUTest
    PUBLIC
        $<$<BOOL:${CPPUTEST_STD_CPP_LIB_DISABLED}>:CPPUTEST_STD_CPP_LIB_DISABLED>
)

#[[Arrange for the include directory to be added to the include paths of any CMake target depending on CppUTest.]]
target_include_directories(CppUTest
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

add_library(CppUTest::CppUTest ALIAS CppUTest)

if(CPPUTEST_BUILD_TESTING)
    add_subdirectory(test)
endif()

# Installation
if(PROJECT_IS_TOP_LEVEL)
    install(
        TARGETS CppUTest
        EXPORT CppUTestTargets
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    )
    install(
        DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include/CppUTest"
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
    )
endif()
