#include <gtest/gtest.h>
#include "../library/imebra/include/imebra.h"

namespace puntoexe
{

namespace imebra
{

namespace tests
{

TEST(ageStringHandlerTest, ageTest)
{
	ptr<data> tag(new data(ptr<baseObject>(0)));
	{
		ptr<handlers::dataHandler> hTag= tag->getDataHandler(0, true, "AS");
		hTag->setSize(1);

		hTag->setDouble(0, 0.01);
        EXPECT_EQ(std::wstring(L"003D"), hTag->getUnicodeString(0));

		hTag->setDouble(0, 0.2);
        EXPECT_EQ(std::wstring(L"010W"), hTag->getUnicodeString(0));

		hTag->setDouble(0, 0.9);
        EXPECT_EQ(std::wstring(L"010M"), hTag->getUnicodeString(0));

		hTag->setDouble(0, 0.5);
        EXPECT_EQ(std::wstring(L"006M"), hTag->getUnicodeString(0));

        hTag->setDouble(0, 4.3);
        EXPECT_EQ(std::wstring(L"004Y"), hTag->getUnicodeString(0));
        EXPECT_EQ(4, hTag->getUnsignedLong(0));
        EXPECT_DOUBLE_EQ(4, hTag->getDouble(0));

        hTag->setUnicodeString(0, L"3");
        EXPECT_EQ(3, hTag->getUnsignedLong(0));

        hTag->setUnicodeString(0, L"05Y");
        EXPECT_EQ(5, hTag->getUnsignedLong(0));

        hTag->setUnicodeString(0, L"6M");
        EXPECT_EQ(0, hTag->getUnsignedLong(0));
        EXPECT_DOUBLE_EQ(0.5, hTag->getDouble(0));

        hTag->setUnicodeString(0, L"4W");
        EXPECT_EQ(0, hTag->getUnsignedLong(0));
        EXPECT_DOUBLE_EQ(0.076716532412734947, hTag->getDouble(0));

        hTag->setUnicodeString(0, L"100D");
        EXPECT_EQ(0, hTag->getUnsignedLong(0));
        EXPECT_DOUBLE_EQ(0.27397260273972601, hTag->getDouble(0));

    }
}



} // namespace tests

} // namespace imebra

} // namespace puntoexe
