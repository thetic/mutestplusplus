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
#include "CppUTest/TestFailure.hpp"
#include "CppUTest/TestResult.hpp"

#include <stdlib.h>

#ifdef CPPUTEST_HAVE_GETTIMEOFDAY
    #include <sys/time.h>
#endif

#include <setjmp.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

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

static long TimeInMillisImplementation()
{
#ifdef CPPUTEST_HAVE_GETTIMEOFDAY
    struct timeval tv;
    struct timezone tz;
    gettimeofday(&tv, &tz);
    return static_cast<long>(
        (tv.tv_sec * 1000) +
        static_cast<time_t>(static_cast<double>(tv.tv_usec) * 0.001)
    );
#else
    return 0;
#endif
}

static const char* TimeStringImplementation()
{
    time_t theTime = time(nullptr);
    static char dateTime[80];
#ifdef CPPUTEST_HAVE_SECURE_STDLIB
    static struct tm lastlocaltime;
    localtime_s(&lastlocaltime, &theTime);
    struct tm* tmp = &lastlocaltime;
#else
    struct tm* tmp = localtime(&theTime);
#endif
    strftime(dateTime, 80, "%Y-%m-%dT%H:%M:%S", tmp);
    return dateTime;
}

long (*GetPlatformSpecificTimeInMillis)() = TimeInMillisImplementation;
const char* (*GetPlatformSpecificTimeString)() = TimeStringImplementation;

int (*PlatformSpecificVSNprintf)(
    char* str, size_t size, const char* format, va_list va_args_list
) = vsnprintf;

static PlatformSpecificFile
PlatformSpecificFOpenImplementation(const char* filename, const char* flag)
{
#ifdef STDC_WANT_SECURE_LIB
    FILE* file;
    fopen_s(&file, filename, flag);
    return file;
#else
    return fopen(filename, flag);
#endif
}

static void
PlatformSpecificFPutsImplementation(const char* str, PlatformSpecificFile file)
{
    fputs(str, reinterpret_cast<FILE*>(file));
}

static void PlatformSpecificFCloseImplementation(PlatformSpecificFile file)
{
    fclose(reinterpret_cast<FILE*>(file));
}

static void PlatformSpecificFlushImplementation()
{
    fflush(stdout);
}

PlatformSpecificFile PlatformSpecificStdOut = stdout;

PlatformSpecificFile (*PlatformSpecificFOpen)(const char*, const char*) =
    PlatformSpecificFOpenImplementation;
void (*PlatformSpecificFPuts)(const char*, PlatformSpecificFile) =
    PlatformSpecificFPutsImplementation;
void (*PlatformSpecificFClose)(PlatformSpecificFile
) = PlatformSpecificFCloseImplementation;

void (*PlatformSpecificFlush)() = PlatformSpecificFlushImplementation;
