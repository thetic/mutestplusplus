#ifndef D_TeamCityTestOutput_h
#define D_TeamCityTestOutput_h

#include "CppUTest/SimpleString.hpp"
#include "CppUTest/TestOutput.hpp"

namespace cpputest
{
    class TeamCityTestOutput : public ConsoleTestOutput
    {
    public:
        TeamCityTestOutput(void);
        virtual ~TeamCityTestOutput(void) override;

        virtual void printCurrentTestStarted(const UtestShell& test) override;
        virtual void printCurrentTestEnded(const TestResult& res) override;
        virtual void printCurrentGroupStarted(const UtestShell& test) override;
        virtual void printCurrentGroupEnded(const TestResult& res) override;

        virtual void printFailure(const TestFailure& failure) override;

    protected:
    private:
        void printEscaped(const char* s);
        const UtestShell* currtest_;
        SimpleString currGroup_;
    };
}

#endif
