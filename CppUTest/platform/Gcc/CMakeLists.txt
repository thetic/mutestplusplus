target_sources(CppUTest
    PRIVATE
        src/PlatformSpecificFunctions.cpp
)

include(CheckCXXSymbolExists)
check_cxx_symbol_exists(gettimeofday "sys/time.h" CPPUTEST_HAVE_GETTIMEOFDAY)
check_cxx_symbol_exists(fopen_s "stdio.h" CPPUTEST_HAVE_SECURE_STDLIB)

target_compile_definitions(CppUTest
    PRIVATE
        $<$<BOOL:${CPPUTEST_HAVE_GETTIMEOFDAY}>:CPPUTEST_HAVE_GETTIMEOFDAY>
        $<$<BOOL:${CPPUTEST_HAVE_SECURE_STDLIB}>:CPPUTEST_HAVE_SECURE_STDLIB>
)
