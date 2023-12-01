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

if(CPPUTEST_PLATFORM)
    target_sources(CppUTest
        PRIVATE
            platform/${CPPUTEST_PLATFORM}/UtestPlatform.cpp
    )
endif()

if (MINGW)
    include(CheckStructHasMember)
    check_struct_has_member("struct timespec" tv_sec time.h HAVE_STRUCT_TIMESPEC LANGUAGE CXX)
endif()

configure_file(
    CppUTestGeneratedConfig.h.cmake
    CppUTest/CppUTestGeneratedConfig.h
)

#[[Arrange for the include directory to be added to the include paths of any CMake target depending on CppUTest.]]
target_include_directories(CppUTest
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_compile_definitions(CppUTest
    PRIVATE
        $<$<BOOL:${CPPUTEST_USE_SECURE_STDLIB}>:STDC_WANT_SECURE_LIB>
        # Apply workaround for MinGW timespec redefinition (pthread.h / time.h).
        $<$<BOOL:${HAVE_STRUCT_TIMESPEC}>:_TIMESPEC_DEFINED>
)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads)

target_link_libraries(CppUTest
    PRIVATE
        $<$<BOOL:${WIN32}>:winmm>
        $<$<BOOL:${CMAKE_USE_PTHREADS_INIT}>:${CMAKE_THREAD_LIBS_INIT}>
)

add_library(CppUTest::CppUTest ALIAS CppUTest)

if(CPPUTEST_BUILD_TESTING)
    add_subdirectory(test)
endif()

# Installation
if(PROJECT_IS_TOP_LEVEL)
    install(
        FILES "${CMAKE_CURRENT_BINARY_DIR}/CppUTest/CppUTestGeneratedConfig.h"
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/CppUTest"
    )

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
