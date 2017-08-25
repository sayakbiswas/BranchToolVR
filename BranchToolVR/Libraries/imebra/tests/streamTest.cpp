#include "../library/imebra/include/imebra.h"

#include <gtest/gtest.h>

namespace puntoexe
{

namespace imebra
{

namespace tests
{

TEST(streamTest, testFailOpen)
{
    ptr<stream> failOpenStream(new stream());
    try
    {
        failOpenStream->openFile(L"doNotExist.dcm", std::ios_base::in);
        EXPECT_FALSE(true);
    }
    catch(streamExceptionOpen& e)
    {
        std::string errorMessage(e.what());
        EXPECT_TRUE(errorMessage.find("2") != errorMessage.npos);
    }

}

}

}

}
