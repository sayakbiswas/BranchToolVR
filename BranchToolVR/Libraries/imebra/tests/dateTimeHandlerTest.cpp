#include "../library/imebra/include/imebra.h"
#include <gtest/gtest.h>

namespace puntoexe
{

namespace imebra
{

namespace tests
{

TEST(dateTimeHandlerTest, dateTest)
{
	ptr<data> tag(new data(ptr<baseObject>(0)));
	{
		ptr<handlers::dataHandler> hTag= tag->getDataHandler(0, true, "DA");
		hTag->setSize(1);

		hTag->setDate(0, 2004, 11, 5, 9, 20, 30, 5000, 1, 2);

		std::int32_t year, month, day, hour, minutes, seconds, nanoseconds, offsetHours, offsetMinutes;
		hTag->getDate(0, &year, &month, &day, &hour, &minutes, &seconds, &nanoseconds, &offsetHours, &offsetMinutes);

        EXPECT_EQ(2004, year);
        EXPECT_EQ(11, month);
        EXPECT_EQ(5, day);
        EXPECT_EQ(0, hour);
        EXPECT_EQ(0, minutes);
        EXPECT_EQ(0, seconds);
        EXPECT_EQ(0, nanoseconds);
        EXPECT_EQ(0, offsetHours);
        EXPECT_EQ(0, offsetMinutes);

        EXPECT_EQ(std::wstring(L"2004-11-05"), hTag->getUnicodeString(0));
	}

	{
		ptr<handlers::dataHandlerRaw> hTag= tag->getDataHandlerRaw(0, true, "DA");
		std::basic_string<std::uint8_t> checkString(hTag->getMemory()->data(), hTag->getMemory()->size());
        EXPECT_EQ(std::basic_string<std::uint8_t>((std::uint8_t*)"20041105"), checkString);
		hTag->getMemory()->assign((std::uint8_t*)"2004-11-5", 9);
	}

	{
		ptr<handlers::dataHandlerRaw> hTag= tag->getDataHandlerRaw(0, false, "DA");
		std::basic_string<std::uint8_t> checkString(hTag->getMemory()->data(), hTag->getMemory()->size());
		stringUint8 compString((std::uint8_t*)"2004-11-5", 9);
		compString += (std::uint8_t)0; // buffer's size is always even!
        EXPECT_EQ(compString, checkString);
	}

	{
		ptr<handlers::dataHandler> hTag= tag->getDataHandler(0, false, "DA");

		std::int32_t year, month, day, hour, minutes, seconds, nanoseconds, offsetHours, offsetMinutes;
		hTag->getDate(0, &year, &month, &day, &hour, &minutes, &seconds, &nanoseconds, &offsetHours, &offsetMinutes);

        EXPECT_EQ(2004, year);
        EXPECT_EQ(11, month);
        EXPECT_EQ(5, day);
        EXPECT_EQ(0, hour);
        EXPECT_EQ(0, minutes);
        EXPECT_EQ(0, seconds);
        EXPECT_EQ(0, nanoseconds);
        EXPECT_EQ(0, offsetHours);
        EXPECT_EQ(0, offsetMinutes);

        EXPECT_EQ(std::wstring(L"2004-11-05"), hTag->getUnicodeString(0));
	}
}

TEST(dateTimeHandlerTest, timeTest)
{
	ptr<data> tag(new data(ptr<baseObject>(0)));
	{
		ptr<handlers::dataHandler> hTag= tag->getDataHandler(0, true, "TM");
		hTag->setSize(1);

		hTag->setDate(0, 2004, 11, 5, 9, 20, 40, 5000, 1, 2);

		std::int32_t year, month, day, hour, minutes, seconds, nanoseconds, offsetHours, offsetMinutes;
		hTag->getDate(0, &year, &month, &day, &hour, &minutes, &seconds, &nanoseconds, &offsetHours, &offsetMinutes);

        EXPECT_EQ(0, year);
        EXPECT_EQ(0, month);
        EXPECT_EQ(0, day);
        EXPECT_EQ(9, hour);
        EXPECT_EQ(20, minutes);
        EXPECT_EQ(40, seconds);
        EXPECT_EQ(5000, nanoseconds);
        EXPECT_EQ(0, offsetHours);
        EXPECT_EQ(0, offsetMinutes);

        EXPECT_EQ(std::wstring(L"09:20:40.005000"), hTag->getUnicodeString(0));
	}

	{
		ptr<handlers::dataHandlerRaw> hTag= tag->getDataHandlerRaw(0, true, "TM");
		stringUint8 compString((std::uint8_t*)"092040.005000");
		compString += (std::uint8_t)0;
		std::basic_string<std::uint8_t> checkString(hTag->getMemory()->data(), hTag->getMemory()->size());
        EXPECT_EQ(compString, checkString);
		hTag->getMemory()->assign((std::uint8_t*)"9:20:40", 7);
	}

	{
		ptr<handlers::dataHandler> hTag= tag->getDataHandler(0, false, "TM");

		std::int32_t year, month, day, hour, minutes, seconds, nanoseconds, offsetHours, offsetMinutes;
		hTag->getDate(0, &year, &month, &day, &hour, &minutes, &seconds, &nanoseconds, &offsetHours, &offsetMinutes);

        EXPECT_EQ(0, year);
        EXPECT_EQ(0, month);
        EXPECT_EQ(0, day);
        EXPECT_EQ(9, hour);
        EXPECT_EQ(20, minutes);
        EXPECT_EQ(40, seconds);
        EXPECT_EQ(0, nanoseconds);
        EXPECT_EQ(0, offsetHours);
        EXPECT_EQ(0, offsetMinutes);

        EXPECT_EQ(std::wstring(L"09:20:40.000000"), hTag->getUnicodeString(0));
	}
}

TEST(dateTimeHandlerTest, dateTimeTest)
{
	ptr<data> tag(new data(ptr<baseObject>(0)));
	{
		ptr<handlers::dataHandler> hTag= tag->getDataHandler(0, true, "DT");
		hTag->setSize(1);

		hTag->setDate(0, 2004, 11, 5, 9, 20, 40, 5000, 1, 2);

		std::int32_t year, month, day, hour, minutes, seconds, nanoseconds, offsetHours, offsetMinutes;
		hTag->getDate(0, &year, &month, &day, &hour, &minutes, &seconds, &nanoseconds, &offsetHours, &offsetMinutes);

        EXPECT_EQ(2004, year);
        EXPECT_EQ(11, month);
        EXPECT_EQ(5, day);
        EXPECT_EQ(9, hour);
        EXPECT_EQ(20, minutes);
        EXPECT_EQ(40, seconds);
        EXPECT_EQ(5000, nanoseconds);
        EXPECT_EQ(1, offsetHours);
        EXPECT_EQ(2, offsetMinutes);

        EXPECT_EQ(std::wstring(L"2004-11-05 09:20:40.005000+01:02"), hTag->getUnicodeString(0));
	}

	{
		ptr<handlers::dataHandlerRaw> hTag= tag->getDataHandlerRaw(0, false, "DT");
		std::basic_string<std::uint8_t> checkString(hTag->getMemory()->data(), hTag->getMemory()->size());
        EXPECT_EQ(std::basic_string<std::uint8_t>((std::uint8_t*)"20041105092040.005000+0102"), checkString);
	}

	{
		ptr<handlers::dataHandler> hTag= tag->getDataHandler(0, true, "DT");

		std::int32_t year, month, day, hour, minutes, seconds, nanoseconds, offsetHours, offsetMinutes;
		hTag->getDate(0, &year, &month, &day, &hour, &minutes, &seconds, &nanoseconds, &offsetHours, &offsetMinutes);

        EXPECT_EQ(2004, year);
        EXPECT_EQ(11, month);
        EXPECT_EQ(5, day);
        EXPECT_EQ(9, hour);
        EXPECT_EQ(20, minutes);
        EXPECT_EQ(40, seconds);
        EXPECT_EQ(5000, nanoseconds);
        EXPECT_EQ(1, offsetHours);
        EXPECT_EQ(2, offsetMinutes);

        EXPECT_EQ(std::wstring(L"2004-11-05 09:20:40.005000+01:02"), hTag->getUnicodeString(0));

		hTag->setString(0, "2005-12-06 10:21:41.005001-4:5");
		hTag->getDate(0, &year, &month, &day, &hour, &minutes, &seconds, &nanoseconds, &offsetHours, &offsetMinutes);
        EXPECT_EQ(2005, year);
        EXPECT_EQ(12, month);
        EXPECT_EQ(6, day);
        EXPECT_EQ(10, hour);
        EXPECT_EQ(21, minutes);
        EXPECT_EQ(41, seconds);
        EXPECT_EQ(5001, nanoseconds);
        EXPECT_EQ(-4, offsetHours);
        EXPECT_EQ(-5, offsetMinutes);
	}

	{
		ptr<handlers::dataHandlerRaw> hTag= tag->getDataHandlerRaw(0, true, "DT");
		std::basic_string<std::uint8_t> checkString(hTag->getMemory()->data(), hTag->getMemory()->size());
        EXPECT_EQ(std::basic_string<std::uint8_t>((std::uint8_t*)"20051206102141.005001-0405"), checkString);
		hTag->getMemory()->assign((std::uint8_t*)"19990305", 8);
	}

	{
		ptr<handlers::dataHandler> hTag= tag->getDataHandler(0, false, "DT");

		std::int32_t year, month, day, hour, minutes, seconds, nanoseconds, offsetHours, offsetMinutes;
		hTag->getDate(0, &year, &month, &day, &hour, &minutes, &seconds, &nanoseconds, &offsetHours, &offsetMinutes);

        EXPECT_EQ(1999, year);
        EXPECT_EQ(3, month);
        EXPECT_EQ(5, day);
        EXPECT_EQ(0, hour);
        EXPECT_EQ(0, minutes);
        EXPECT_EQ(0, seconds);
        EXPECT_EQ(0, nanoseconds);
        EXPECT_EQ(0, offsetHours);
        EXPECT_EQ(0, offsetMinutes);
	}

	{
		ptr<handlers::dataHandlerRaw> hTag= tag->getDataHandlerRaw(0, true, "DT");
		hTag->getMemory()->assign((std::uint8_t*)"1999030508", 10);
	}

	{
		ptr<handlers::dataHandler> hTag= tag->getDataHandler(0, false, "DT");

		std::int32_t year, month, day, hour, minutes, seconds, nanoseconds, offsetHours, offsetMinutes;
		hTag->getDate(0, &year, &month, &day, &hour, &minutes, &seconds, &nanoseconds, &offsetHours, &offsetMinutes);

        EXPECT_EQ(1999, year);
        EXPECT_EQ(3, month);
        EXPECT_EQ(5, day);
        EXPECT_EQ(8, hour);
        EXPECT_EQ(0, minutes);
        EXPECT_EQ(0, seconds);
        EXPECT_EQ(0, nanoseconds);
        EXPECT_EQ(0, offsetHours);
        EXPECT_EQ(0, offsetMinutes);
	}
}

TEST(dateTimeHandlerTest, incompleteDateTimeTest)
{
    ptr<data> tag(new data(ptr<baseObject>(0)));
    {
        ptr<handlers::dataHandlerRaw> hTag= tag->getDataHandlerRaw(0, true, "DT");
        std::string bufferData;
        hTag->setSize(0);
        hTag->copyFrom((std::uint8_t*)bufferData.data(), bufferData.size());
    }

    std::int32_t year, month, day, hour, minutes, seconds, nanoseconds, offsetHours, offsetMinutes;
    ptr<handlers::dataHandler> hTag = tag->getDataHandler(0, false, "");
    hTag->getDate(0, &year, &month, &day, &hour, &minutes, &seconds, &nanoseconds, &offsetHours, &offsetMinutes);

    EXPECT_EQ(0, year);
    EXPECT_EQ(0, month);
    EXPECT_EQ(0, day);
    EXPECT_EQ(0, hour);
    EXPECT_EQ(0, minutes);
    EXPECT_EQ(0, seconds);
    EXPECT_EQ(0, nanoseconds);
    EXPECT_EQ(0, offsetHours);
    EXPECT_EQ(0, offsetMinutes);

    {
        ptr<handlers::dataHandlerRaw> hTag = tag->getDataHandlerRaw(0, true, "DT");
        hTag->setSize(1);
        std::string bufferData("199901");
        hTag->setSize(0);
        hTag->copyFrom((std::uint8_t*)bufferData.data(), bufferData.size());
    }

    hTag = tag->getDataHandler(0, false, "");
    hTag->getDate(0, &year, &month, &day, &hour, &minutes, &seconds, &nanoseconds, &offsetHours, &offsetMinutes);

    EXPECT_EQ(1999, year);
    EXPECT_EQ(1, month);
    EXPECT_EQ(0, day);
    EXPECT_EQ(0, hour);
    EXPECT_EQ(0, minutes);
    EXPECT_EQ(0, seconds);
    EXPECT_EQ(0, nanoseconds);
    EXPECT_EQ(0, offsetHours);
    EXPECT_EQ(0, offsetMinutes);

    {
        ptr<handlers::dataHandlerRaw> hTag = tag->getDataHandlerRaw(0, true, "DT");
        std::string bufferData("19990120");
        hTag->setSize(0);
        hTag->copyFrom((std::uint8_t*)bufferData.data(), bufferData.size());
    }

    hTag = tag->getDataHandler(0, false, "");
    hTag->getDate(0, &year, &month, &day, &hour, &minutes, &seconds, &nanoseconds, &offsetHours, &offsetMinutes);

    EXPECT_EQ(1999, year);
    EXPECT_EQ(1, month);
    EXPECT_EQ(20, day);
    EXPECT_EQ(0, hour);
    EXPECT_EQ(0, minutes);
    EXPECT_EQ(0, seconds);
    EXPECT_EQ(0, nanoseconds);
    EXPECT_EQ(0, offsetHours);
    EXPECT_EQ(0, offsetMinutes);

    {
        ptr<handlers::dataHandlerRaw> hTag = tag->getDataHandlerRaw(0, true, "DT");
        std::string bufferData("1999012012");
        hTag->setSize(0);
        hTag->copyFrom((std::uint8_t*)bufferData.data(), bufferData.size());
    }

    hTag = tag->getDataHandler(0, false, "");
    hTag->getDate(0, &year, &month, &day, &hour, &minutes, &seconds, &nanoseconds, &offsetHours, &offsetMinutes);

    EXPECT_EQ(1999, year);
    EXPECT_EQ(1, month);
    EXPECT_EQ(20, day);
    EXPECT_EQ(12, hour);
    EXPECT_EQ(0, minutes);
    EXPECT_EQ(0, seconds);
    EXPECT_EQ(0, nanoseconds);
    EXPECT_EQ(0, offsetHours);
    EXPECT_EQ(0, offsetMinutes);
}



} // namespace tests

} // namespace imebra

} // namespace puntoexe
