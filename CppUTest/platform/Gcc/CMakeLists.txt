target_sources(CppUTest
    PRIVATE
        src/PlatformSpecificFunctions.cpp
)

include(CheckCXXSymbolExists)
check_cxx_symbol_exists(gettimeofday "sys/time.h" CPPUTEST_HAVE_GETTIMEOFDAY)
check_cxx_symbol_exists(fork "unistd.h" CPPUTEST_HAVE_FORK)
check_cxx_symbol_exists(waitpid "sys/wait.h" CPPUTEST_HAVE_WAITPID)
check_cxx_symbol_exists(pthread_mutex_lock "pthread.h" CPPUTEST_HAVE_PTHREAD_MUTEX_LOCK)
check_cxx_symbol_exists(fopen_s "stdio.h" CPPUTEST_HAVE_SECURE_STDLIB)

if (MINGW)
    include(CheckStructHasMember)
    check_struct_has_member("struct timespec" tv_sec time.h HAVE_STRUCT_TIMESPEC LANGUAGE CXX)
endif()

target_compile_definitions(CppUTest
    PRIVATE
        $<$<BOOL:${CPPUTEST_HAVE_GETTIMEOFDAY}>:CPPUTEST_HAVE_GETTIMEOFDAY>
        $<$<BOOL:${CPPUTEST_HAVE_WAITPID}>:CPPUTEST_HAVE_WAITPID>
        $<$<BOOL:${CPPUTEST_HAVE_FORK}>:CPPUTEST_HAVE_FORK>
        $<$<BOOL:${CPPUTEST_HAVE_PTHREAD_MUTEX_LOCK}>:CPPUTEST_HAVE_PTHREAD_MUTEX_LOCK>
        $<$<BOOL:${CPPUTEST_HAVE_SECURE_STDLIB}>:CPPUTEST_HAVE_SECURE_STDLIB>
        # Apply workaround for MinGW timespec redefinition (pthread.h / time.h).
        $<$<BOOL:${HAVE_STRUCT_TIMESPEC}>:_TIMESPEC_DEFINED>
)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads)

target_link_libraries(CppUTest
    PRIVATE
        $<$<BOOL:${CMAKE_THREAD_LIBS_INIT}>:${CMAKE_THREAD_LIBS_INIT}>
)
