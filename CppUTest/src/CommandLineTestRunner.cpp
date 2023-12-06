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

#include "CppUTest/CommandLineTestRunner.hpp"
#include "CppUTest/JUnitTestOutput.hpp"
#include "CppUTest/PlatformSpecificFunctions.hpp"
#include "CppUTest/TeamCityTestOutput.hpp"
#include "CppUTest/TestOutput.hpp"
#include "CppUTest/TestPlugin.hpp"
#include "CppUTest/TestRegistry.hpp"
#include "CppUTest/TestResult.hpp"
#include "CppUTest/Utest.hpp"

namespace cpputest
{
    int CommandLineTestRunner::RunAllTests(int ac, char** av)
    {
        return RunAllTests(ac, reinterpret_cast<const char* const*>(av));
    }

    int CommandLineTestRunner::RunAllTests(int ac, const char* const* av)
    {
        int result = 0;
        ConsoleTestOutput backupOutput;

        {
            CommandLineTestRunner runner(
                ac, av, TestRegistry::getCurrentRegistry()
            );
            result = runner.runAllTestsMain();
        }

        TestRegistry::getCurrentRegistry()->removePluginByName(
            DEF_PLUGIN_MEM_LEAK
        );
        return result;
    }

    CommandLineTestRunner::CommandLineTestRunner(
        int ac, const char* const* av, TestRegistry* registry
    ) :
        output_(nullptr),
        arguments_(nullptr),
        registry_(registry)
    {
        arguments_ = new CommandLineArguments(ac, av);
    }

    CommandLineTestRunner::~CommandLineTestRunner()
    {
        delete arguments_;
        delete output_;
    }

    int CommandLineTestRunner::runAllTestsMain()
    {
        int testResult = 1;

        SetPointerPlugin pPlugin(DEF_PLUGIN_SET_POINTER);
        registry_->installPlugin(&pPlugin);

        if (parseArguments(registry_->getFirstPlugin()))
            testResult = runAllTests();

        registry_->removePluginByName(DEF_PLUGIN_SET_POINTER);
        return testResult;
    }

    void CommandLineTestRunner::initializeTestRun()
    {
        registry_->setGroupFilters(arguments_->getGroupFilters());
        registry_->setNameFilters(arguments_->getNameFilters());

        if (arguments_->isVerbose())
            output_->verbose(TestOutput::level_verbose);
        if (arguments_->isVeryVerbose())
            output_->verbose(TestOutput::level_veryVerbose);
        if (arguments_->isColor())
            output_->color();
        if (arguments_->runTestsInSeperateProcess())
            registry_->setRunTestsInSeperateProcess();
        if (arguments_->isRunIgnored())
            registry_->setRunIgnored();
        if (arguments_->isCrashingOnFail())
            UtestShell::setCrashOnFail();

        UtestShell::setRethrowExceptions(arguments_->isRethrowingExceptions());
    }

    int CommandLineTestRunner::runAllTests()
    {
        initializeTestRun();
        size_t loopCount = 0;
        size_t failedTestCount = 0;
        size_t failedExecutionCount = 0;
        size_t repeatCount = arguments_->getRepeatCount();

        if (arguments_->isListingTestGroupNames()) {
            TestResult tr(*output_);
            registry_->listTestGroupNames(tr);
            return 0;
        }

        if (arguments_->isListingTestGroupAndCaseNames()) {
            TestResult tr(*output_);
            registry_->listTestGroupAndCaseNames(tr);
            return 0;
        }

        if (arguments_->isListingTestLocations()) {
            TestResult tr(*output_);
            registry_->listTestLocations(tr);
            return 0;
        }

        if (arguments_->isReversing())
            registry_->reverseTests();

        while (loopCount++ < repeatCount) {

            output_->printTestRun(loopCount, repeatCount);
            TestResult tr(*output_);
            registry_->runAllTests(tr);
            failedTestCount += tr.getFailureCount();
            if (tr.isFailure()) {
                failedExecutionCount++;
            }
        }
        return static_cast<int>(
            failedTestCount != 0 ? failedTestCount : failedExecutionCount
        );
    }

    TestOutput* CommandLineTestRunner::createTeamCityOutput()
    {
        return new TeamCityTestOutput;
    }

    TestOutput*
    CommandLineTestRunner::createJUnitOutput(const SimpleString& packageName)
    {
        JUnitTestOutput* junitOutput = new JUnitTestOutput;
        if (junitOutput != nullptr) {
            junitOutput->setPackageName(packageName);
        }
        return junitOutput;
    }

    TestOutput* CommandLineTestRunner::createConsoleOutput()
    {
        return new ConsoleTestOutput;
    }

    TestOutput* CommandLineTestRunner::createCompositeOutput(
        TestOutput* outputOne, TestOutput* outputTwo
    )
    {
        CompositeTestOutput* composite = new CompositeTestOutput;
        composite->setOutputOne(outputOne);
        composite->setOutputTwo(outputTwo);
        return composite;
    }

    bool CommandLineTestRunner::parseArguments(TestPlugin* plugin)
    {
        if (!arguments_->parse(plugin)) {
            output_ = createConsoleOutput();
            output_->print(
                (arguments_->needHelp()) ? arguments_->help()
                                         : arguments_->usage()
            );
            return false;
        }

        if (arguments_->isJUnitOutput()) {
            output_ = createJUnitOutput(arguments_->getPackageName());
            if (arguments_->isVerbose() || arguments_->isVeryVerbose())
                output_ = createCompositeOutput(output_, createConsoleOutput());
        } else if (arguments_->isTeamCityOutput()) {
            output_ = createTeamCityOutput();
        } else
            output_ = createConsoleOutput();
        return true;
    }
}
