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

#ifndef CPPUTESTCONFIG_H_
#define CPPUTESTCONFIG_H_

#ifndef CPPUTEST_USE_OWN_CONFIGURATION
    #include "CppUTest/CppUTestGeneratedConfig.h"
#endif

/* Do we use Standard C++ or not? */
#ifndef CPPUTEST_USE_STD_CPP_LIB
    #ifdef CPPUTEST_STD_CPP_LIB_DISABLED
        #define CPPUTEST_USE_STD_CPP_LIB 0
    #else
        #define CPPUTEST_USE_STD_CPP_LIB 1
    #endif
#endif

#ifdef __cplusplus
    /*
     * Detection of run-time type information (RTTI) presence. Since it's a
     * standard language feature, assume it is enabled unless we see otherwise.
     */
    #ifndef CPPUTEST_HAVE_RTTI
        #if ((__cplusplus >= 202002L) && !__cpp_rtti) ||                       \
            (defined(_MSC_VER) && !_CPPRTTI) ||                                \
            (defined(__GNUC__) && !__GXX_RTTI) ||                              \
            (defined(__ghs__) && !__RTTI) ||                                   \
            (defined(__WATCOMC__) && !_CPPRTTI)
            #define CPPUTEST_HAVE_RTTI 0
        #else
            #define CPPUTEST_HAVE_RTTI 1
        #endif
    #endif

    /*
     * Detection of exception support. Since it's a standard language feature,
     * assume it is enabled unless we see otherwise.
     */
    #ifndef CPPUTEST_HAVE_EXCEPTIONS
        #if ((__cplusplus >= 202002L) && !__cpp_exceptions) ||                 \
            (defined(_MSC_VER) && !_CPPUNWIND) ||                              \
            (defined(__GNUC__) && !__EXCEPTIONS) ||                            \
            (defined(__ghs__) && !__EXCEPTION_HANDLING) ||                     \
            (defined(__WATCOMC__) && !_CPPUNWIND)
            #define CPPUTEST_HAVE_EXCEPTIONS 0
        #else
            #define CPPUTEST_HAVE_EXCEPTIONS 1
        #endif
    #endif
#endif

#endif
