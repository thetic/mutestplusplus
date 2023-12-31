cmake_minimum_required(VERSION 3.13)

project(CppUTest
  VERSION 4.0
)

if (CMAKE_VERSION VERSION_GREATER_EQUAL "3.21")
  # PROJECT_IS_TOP_LEVEL is defined automatically
elseif (CMAKE_CURRENT_BINARY_DIR STREQUAL CMAKE_BINARY_DIR)
  set(PROJECT_IS_TOP_LEVEL TRUE)
else()
  set(PROJECT_IS_TOP_LEVEL FALSE)
endif()

###############
# Conan support
###############
if (EXISTS "${CMAKE_BINARY_DIR}/conanbuildinfo.cmake")
  include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
  message(STATUS "conan_basic_setup()")
  conan_basic_setup()
endif()

include(CTest)

include(CMakeDependentOption)
option(CPPUTEST_STD_CPP_LIB_DISABLED "Use the standard C++ library")
option(CPPUTEST_EXTENSIONS "Use the CppUTest extension library" ON)
cmake_dependent_option(CPPUTEST_BUILD_TESTING "Compile and make tests for CppUTest"
  ${PROJECT_IS_TOP_LEVEL} "BUILD_TESTING" OFF)
cmake_dependent_option(CPPUTEST_TEST_DISCOVERY "Build time test discover"
  ON "CPPUTEST_BUILD_TESTING;CMAKE_CROSSCOMPILING_EMULATOR OR NOT CMAKE_CROSSCOMPILING" OFF)
cmake_dependent_option(CPPUTEST_EXAMPLES "Compile and make examples?"
  ${PROJECT_IS_TOP_LEVEL} "CPPUTEST_EXTENSIONS;NOT CPPUTEST_STD_CPP_LIB_DISABLED" OFF)

if(NOT DEFINED CPPUTEST_PLATFORM)
  if(DEFINED CPP_PLATFORM)
    message(DEPRECATION
      "The CPP_PLATFORM variable has been deprecated. "
      "Use CPPUTEST_PLATFORM instead."
    )
    set(CPPUTEST_PLATFORM ${CPP_PLATFORM})
  else()
    if(MSVC)
      set(CPPUTEST_PLATFORM VisualCpp)
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "IAR")
      set(CPPUTEST_PLATFORM Iar)
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "ARMCC")
      set(CPPUTEST_PLATFORM armcc)
    elseif(BORLAND)
      set(CPPUTEST_PLATFORM Borland)
    else()
      set(CPPUTEST_PLATFORM Gcc)
    endif()
  endif()
  message(STATUS "Setting CPPUTEST_PLATFORM: ${CPPUTEST_PLATFORM}")
endif()
set(CPPUTEST_PLATFORM "${CPPUTEST_PLATFORM}" CACHE STRING "Platform implementation")
set_property(
  CACHE CPPUTEST_PLATFORM
  PROPERTY STRINGS
    armcc Borland C2000 Gcc Iar Keil Symbian VisualCpp OFF
)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules")
if(NOT PROJECT_IS_TOP_LEVEL)
  set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH}" PARENT_SCOPE)
endif()

if(PROJECT_IS_TOP_LEVEL)
  include(cmake/warnings.cmake)
endif()
include(GNUInstallDirs)

set(CMAKE_REQUIRED_FLAGS ${CMAKE_CXX_FLAGS})

add_subdirectory(CppUTest)

if(CPPUTEST_EXTENSIONS)
  add_subdirectory(CppUTestExt)
endif()

if (CPPUTEST_EXAMPLES)
  add_subdirectory(examples)
endif()

if(PROJECT_IS_TOP_LEVEL)
  include(cmake/install.cmake)
endif()
