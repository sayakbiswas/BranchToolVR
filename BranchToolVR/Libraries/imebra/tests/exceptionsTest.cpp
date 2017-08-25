#include <gtest/gtest.h>
#include "../library/imebra/include/imebra.h"

namespace puntoexe
{

namespace imebra
{

namespace tests
{

// A buffer initialized to a default data type should use the data type OB
TEST(exceptionsTest, testExceptions)
{
	try
	{
		PUNTOEXE_FUNCTION_START(L"exceptionTest_Phase1");
		{
			PUNTOEXE_FUNCTION_START(L"exceptionTest_Phase2");
			{
				PUNTOEXE_FUNCTION_START(L"exceptionTest_Phase3");
				exceptionsManager::getMessage();
				PUNTOEXE_THROW(std::runtime_error, "testPhase3");
				PUNTOEXE_FUNCTION_END();
			}
			PUNTOEXE_FUNCTION_END();
		}
		PUNTOEXE_FUNCTION_END();
	}
	catch(...)
	{
		std::wstring exceptionMessage = puntoexe::exceptionsManager::getMessage();
        ASSERT_FALSE(exceptionMessage.empty());

		std::wstring exceptionMessageEmpty = puntoexe::exceptionsManager::getMessage();
        ASSERT_TRUE(exceptionMessageEmpty.empty());

        ASSERT_TRUE(exceptionMessage.find(L"exceptionTest_Phase1") != exceptionMessage.npos);
        ASSERT_TRUE(exceptionMessage.find(L"exceptionTest_Phase2") != exceptionMessage.npos);
        ASSERT_TRUE(exceptionMessage.find(L"exceptionTest_Phase3") != exceptionMessage.npos);
        ASSERT_TRUE(exceptionMessage.find(L"testPhase3") != exceptionMessage.npos);
		return;
	}
    ASSERT_TRUE(false);
}



} // namespace tests

} // namespace imebra

} // namespace puntoexe
