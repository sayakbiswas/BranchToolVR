#include "../library/imebra/include/imebra.h"
#include <gtest/gtest.h>

namespace puntoexe
{

namespace imebra
{

namespace tests
{

TEST(decimalStringHandlerTest, decimalTest)
{
	ptr<data> tag(new data(ptr<baseObject>(0)));
	{
		ptr<handlers::dataHandler> hTag= tag->getDataHandler(0, true, "DS");
		hTag->setSize(1);

		hTag->setDouble(0, 0.000001);
		std::wstring decimalString = hTag->getUnicodeString(0);
        ASSERT_TRUE(decimalString == L"1e-006" || decimalString == L"1e-06" || decimalString == L"1e-6");
		double decimal = hTag->getDouble(0);
        EXPECT_DOUBLE_EQ(0.000001, decimal);
	}
}



} // namespace tests

} // namespace imebra

} // namespace puntoexe
