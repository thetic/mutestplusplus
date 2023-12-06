#include "CppUTest/PlatformSpecificFunctions.hpp"

#include "CppUTest/TestHarness.hpp"

#include <float.h>
#include <math.h>
#include <setjmp.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <Windows.h>
#include <mmsystem.h>

#include <setjmp.h>

#ifdef CPPUTEST_HAVE_SECURE_STDLIB
    #define FOPEN(fp, filename, flag) fopen_s((fp), (filename), (flag))
    #define VSNPRINTF(str, size, trunc, format, args)                          \
        _vsnprintf_s((str), (size), (trunc), (format), (args))
    #define LOCALTIME(_tm, timer) localtime_s((_tm), (timer))
#else
    #define FOPEN(fp, filename, flag) *(fp) = fopen((filename), (flag))
    #define VSNPRINTF(str, size, trunc, format, args)                          \
        _vsnprintf((str), (size), (format), (args))
    #define LOCALTIME(_tm, timer) memcpy(_tm, localtime(timer), sizeof(tm))
#endif

using namespace cpputest;

static jmp_buf test_exit_jmp_buf[10];
static int jmp_buf_index = 0;

int PlatformSpecificSetJmp(void (*function)(void* data), void* data)
{
    if (0 == setjmp(test_exit_jmp_buf[jmp_buf_index])) {
        jmp_buf_index++;
        function(data);
        jmp_buf_index--;
        return 1;
    }
    return 0;
}

void PlatformSpecificLongJmp()
{
    jmp_buf_index--;
    longjmp(test_exit_jmp_buf[jmp_buf_index], 1);
}

void PlatformSpecificRestoreJumpBuffer()
{
    jmp_buf_index--;
}

///////////// Time in millis

static long VisualCppTimeInMillis()
{
    static LARGE_INTEGER s_frequency;
    static const BOOL s_use_qpc = QueryPerformanceFrequency(&s_frequency);
    if (s_use_qpc) {
        LARGE_INTEGER now;
        QueryPerformanceCounter(&now);
        return static_cast<long>((now.QuadPart * 1000) / s_frequency.QuadPart);
    } else {
#ifdef TIMERR_NOERROR
        return static_cast<long>(timeGetTime());
#else
    #if !defined(_WIN32_WINNT) || !defined(_WIN32_WINNT_VISTA) ||              \
        (_WIN32_WINNT < _WIN32_WINNT_VISTA)
        return static_cast<long>(GetTickCount());
    #else
        return static_cast<long>(GetTickCount64());
    #endif
#endif
    }
}

long (*GetPlatformSpecificTimeInMillis)() = VisualCppTimeInMillis;

///////////// Time in String

static const char* VisualCppTimeString()
{
    time_t the_time = time(nullptr);
    struct tm the_local_time;
    static char dateTime[80];
    LOCALTIME(&the_local_time, &the_time);
    strftime(dateTime, 80, "%Y-%m-%dT%H:%M:%S", &the_local_time);
    return dateTime;
}

const char* (*GetPlatformSpecificTimeString)() = VisualCppTimeString;

////// taken from gcc

static int
VisualCppVSNprintf(char* str, size_t size, const char* format, va_list args)
{
    char* buf = nullptr;
    size_t sizeGuess = size;

    int result = VSNPRINTF(str, size, _TRUNCATE, format, args);
    str[size - 1] = 0;
    while (result == -1) {
        if (buf)
            free(buf);
        sizeGuess += 10;
        buf = reinterpret_cast<char*>(malloc(sizeGuess));
        result = VSNPRINTF(buf, sizeGuess, _TRUNCATE, format, args);
    }

    if (buf)
        free(buf);
    return result;
}

int (*PlatformSpecificVSNprintf)(
    char* str, size_t size, const char* format, va_list va_args_list
) = VisualCppVSNprintf;

static PlatformSpecificFile
VisualCppFOpen(const char* filename, const char* flag)
{
    FILE* file;
    FOPEN(&file, filename, flag);
    return file;
}

static void VisualCppFPuts(const char* str, PlatformSpecificFile file)
{
    fputs(str, reinterpret_cast<FILE*>(file));
}

static void VisualCppFClose(PlatformSpecificFile file)
{
    fclose(reinterpret_cast<FILE*>(file));
}

PlatformSpecificFile PlatformSpecificStdOut = stdout;
PlatformSpecificFile (*PlatformSpecificFOpen)(
    const char* filename, const char* flag
) = VisualCppFOpen;
void (*PlatformSpecificFPuts)(const char* str, PlatformSpecificFile file) =
    VisualCppFPuts;
void (*PlatformSpecificFClose)(PlatformSpecificFile file) = VisualCppFClose;

static void VisualCppFlush()
{
    fflush(stdout);
}

void (*PlatformSpecificFlush)(void) = VisualCppFlush;
