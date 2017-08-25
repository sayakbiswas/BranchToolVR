// imebra_tests.cpp : Defines the entry point for the console application.
//
#include <gtest/gtest.h>

#include "testsSettings.h"

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);

    tests::settings& settings = tests::settings::getSettings();

    for(int scanArguments(0); scanArguments != argc; ++scanArguments)
    {
        std::string argument = argv[scanArguments];
        settings.set(argument);
    }

    return RUN_ALL_TESTS();
}


