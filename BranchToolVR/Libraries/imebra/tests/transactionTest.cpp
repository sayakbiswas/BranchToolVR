#include "../library/imebra/include/imebra.h"
#include <gtest/gtest.h>

namespace puntoexe
{

namespace imebra
{

namespace tests
{

TEST(transactionTest, testOneTransaction)
{
	ptr<dataSet> newDataSet(new dataSet);
	charsetsList::tCharsetsList charsets;
    charsets.push_back("ISO_IR 100");
	newDataSet->setCharsetsList(&charsets);

	// Test without transactions
	newDataSet->setUnicodeString(20, 0, 10, 0, L"test 0", "PN");
	newDataSet->setUnicodeString(20, 0, 11, 0, L"test 1", "PN");
	newDataSet->setUnicodeString(20, 0, 12, 0, L"test 2", "PN");
	newDataSet->setUnicodeString(20, 0, 13, 0, L"test 3", "PN");

    EXPECT_EQ(std::wstring(L"test 0"), newDataSet->getUnicodeString(20, 0, 10, 0));
    EXPECT_EQ(std::wstring(L"test 1"), newDataSet->getUnicodeString(20, 0, 11, 0));
    EXPECT_EQ(std::wstring(L"test 2"), newDataSet->getUnicodeString(20, 0, 12, 0));
    EXPECT_EQ(std::wstring(L"test 3"), newDataSet->getUnicodeString(20, 0, 13, 0));

	// Test one transaction
	IMEBRA_TRANSACTION_START();

		newDataSet->setUnicodeString(10, 0, 10, 0, L"test 0", "PN");
		newDataSet->setUnicodeString(10, 0, 11, 0, L"test 1", "PN");
		newDataSet->setUnicodeString(10, 0, 12, 0, L"test 2", "PN");
		newDataSet->setUnicodeString(10, 0, 13, 0, L"test 3", "PN");

        EXPECT_NE(std::wstring(L"test 0"), newDataSet->getUnicodeString(10, 0, 10, 0));
        EXPECT_NE(std::wstring(L"test 1"), newDataSet->getUnicodeString(10, 0, 11, 0));
        EXPECT_NE(std::wstring(L"test 2"), newDataSet->getUnicodeString(10, 0, 12, 0));
        EXPECT_NE(std::wstring(L"test 3"), newDataSet->getUnicodeString(10, 0, 13, 0));

	IMEBRA_TRANSACTION_END();

    EXPECT_EQ(std::wstring(L"test 0"), newDataSet->getUnicodeString(10, 0, 10, 0));
    EXPECT_EQ(std::wstring(L"test 1"), newDataSet->getUnicodeString(10, 0, 11, 0));
    EXPECT_EQ(std::wstring(L"test 2"), newDataSet->getUnicodeString(10, 0, 12, 0));
    EXPECT_EQ(std::wstring(L"test 3"), newDataSet->getUnicodeString(10, 0, 13, 0));
}

TEST(transactionTest, testNestedTransactions0)
{
	ptr<dataSet> newDataSet(new dataSet);
	charsetsList::tCharsetsList charsets;
    charsets.push_back("ISO_IR 100");
	newDataSet->setCharsetsList(&charsets);

	IMEBRA_TRANSACTION_START();

		newDataSet->setUnicodeString(10, 0, 10, 0, L"test 0", "PN");
		newDataSet->setUnicodeString(10, 0, 11, 0, L"test 1", "PN");
		newDataSet->setUnicodeString(10, 0, 12, 0, L"test 2", "PN");
		newDataSet->setUnicodeString(10, 0, 13, 0, L"test 3", "PN");

		IMEBRA_TRANSACTION_START();

			newDataSet->setUnicodeString(20, 0, 10, 0, L"test 4", "PN");
			newDataSet->setUnicodeString(20, 0, 11, 0, L"test 5", "PN");
			newDataSet->setUnicodeString(20, 0, 12, 0, L"test 6", "PN");
			newDataSet->setUnicodeString(20, 0, 13, 0, L"test 7", "PN");

            EXPECT_NE(std::wstring(L"test 4"), newDataSet->getUnicodeString(20, 0, 10, 0));
            EXPECT_NE(std::wstring(L"test 5"), newDataSet->getUnicodeString(20, 0, 11, 0));
            EXPECT_NE(std::wstring(L"test 6"), newDataSet->getUnicodeString(20, 0, 12, 0));
            EXPECT_NE(std::wstring(L"test 7"), newDataSet->getUnicodeString(20, 0, 13, 0));
		
		IMEBRA_TRANSACTION_END();

        EXPECT_NE(std::wstring(L"test 0"), newDataSet->getUnicodeString(10, 0, 10, 0));
        EXPECT_NE(std::wstring(L"test 1"), newDataSet->getUnicodeString(10, 0, 11, 0));
        EXPECT_NE(std::wstring(L"test 2"), newDataSet->getUnicodeString(10, 0, 12, 0));
        EXPECT_NE(std::wstring(L"test 3"), newDataSet->getUnicodeString(10, 0, 13, 0));

        EXPECT_NE(std::wstring(L"test 4"), newDataSet->getUnicodeString(20, 0, 10, 0));
        EXPECT_NE(std::wstring(L"test 5"), newDataSet->getUnicodeString(20, 0, 11, 0));
        EXPECT_NE(std::wstring(L"test 6"), newDataSet->getUnicodeString(20, 0, 12, 0));
        EXPECT_NE(std::wstring(L"test 7"), newDataSet->getUnicodeString(20, 0, 13, 0));
	
	IMEBRA_TRANSACTION_END();

    EXPECT_EQ(std::wstring(L"test 0"), newDataSet->getUnicodeString(10, 0, 10, 0));
    EXPECT_EQ(std::wstring(L"test 1"), newDataSet->getUnicodeString(10, 0, 11, 0));
    EXPECT_EQ(std::wstring(L"test 2"), newDataSet->getUnicodeString(10, 0, 12, 0));
    EXPECT_EQ(std::wstring(L"test 3"), newDataSet->getUnicodeString(10, 0, 13, 0));

    EXPECT_EQ(std::wstring(L"test 4"), newDataSet->getUnicodeString(20, 0, 10, 0));
    EXPECT_EQ(std::wstring(L"test 5"), newDataSet->getUnicodeString(20, 0, 11, 0));
    EXPECT_EQ(std::wstring(L"test 6"), newDataSet->getUnicodeString(20, 0, 12, 0));
    EXPECT_EQ(std::wstring(L"test 7"), newDataSet->getUnicodeString(20, 0, 13, 0));
}

TEST(transactionTest, testNestedTransactions1)
{
	ptr<dataSet> newDataSet(new dataSet);
	charsetsList::tCharsetsList charsets;
    charsets.push_back("ISO_IR 100");
	newDataSet->setCharsetsList(&charsets);

	// Test one transaction
	IMEBRA_TRANSACTION_START();

		newDataSet->setUnicodeString(10, 0, 10, 0, L"test 0", "PN");
		newDataSet->setUnicodeString(10, 0, 11, 0, L"test 1", "PN");
		newDataSet->setUnicodeString(10, 0, 12, 0, L"test 2", "PN");
		newDataSet->setUnicodeString(10, 0, 13, 0, L"test 3", "PN");

		IMEBRA_COMMIT_TRANSACTION_START();

			newDataSet->setUnicodeString(20, 0, 10, 0, L"test 4", "PN");
			newDataSet->setUnicodeString(20, 0, 11, 0, L"test 5", "PN");
			newDataSet->setUnicodeString(20, 0, 12, 0, L"test 6", "PN");
			newDataSet->setUnicodeString(20, 0, 13, 0, L"test 7", "PN");

            EXPECT_NE(std::wstring(L"test 4"), newDataSet->getUnicodeString(20, 0, 10, 0));
            EXPECT_NE(std::wstring(L"test 5"), newDataSet->getUnicodeString(20, 0, 11, 0));
            EXPECT_NE(std::wstring(L"test 6"), newDataSet->getUnicodeString(20, 0, 12, 0));
            EXPECT_NE(std::wstring(L"test 7"), newDataSet->getUnicodeString(20, 0, 13, 0));
		
		IMEBRA_TRANSACTION_END();

        EXPECT_NE(std::wstring(L"test 0"), newDataSet->getUnicodeString(10, 0, 10, 0));
        EXPECT_NE(std::wstring(L"test 1"), newDataSet->getUnicodeString(10, 0, 11, 0));
        EXPECT_NE(std::wstring(L"test 2"), newDataSet->getUnicodeString(10, 0, 12, 0));
        EXPECT_NE(std::wstring(L"test 3"), newDataSet->getUnicodeString(10, 0, 13, 0));

        EXPECT_EQ(std::wstring(L"test 4"), newDataSet->getUnicodeString(20, 0, 10, 0));
        EXPECT_EQ(std::wstring(L"test 5"), newDataSet->getUnicodeString(20, 0, 11, 0));
        EXPECT_EQ(std::wstring(L"test 6"), newDataSet->getUnicodeString(20, 0, 12, 0));
        EXPECT_EQ(std::wstring(L"test 7"), newDataSet->getUnicodeString(20, 0, 13, 0));
	
	IMEBRA_TRANSACTION_END();

    EXPECT_EQ(std::wstring(L"test 0"), newDataSet->getUnicodeString(10, 0, 10, 0));
    EXPECT_EQ(std::wstring(L"test 1"), newDataSet->getUnicodeString(10, 0, 11, 0));
    EXPECT_EQ(std::wstring(L"test 2"), newDataSet->getUnicodeString(10, 0, 12, 0));
    EXPECT_EQ(std::wstring(L"test 3"), newDataSet->getUnicodeString(10, 0, 13, 0));

    EXPECT_EQ(std::wstring(L"test 4"), newDataSet->getUnicodeString(20, 0, 10, 0));
    EXPECT_EQ(std::wstring(L"test 5"), newDataSet->getUnicodeString(20, 0, 11, 0));
    EXPECT_EQ(std::wstring(L"test 6"), newDataSet->getUnicodeString(20, 0, 12, 0));
    EXPECT_EQ(std::wstring(L"test 7"), newDataSet->getUnicodeString(20, 0, 13, 0));
}

TEST(transactionTest, testNestedTransactionsFail0)
{
	ptr<dataSet> newDataSet(new dataSet);
	charsetsList::tCharsetsList charsets;
    charsets.push_back("ISO_IR 6");
	newDataSet->setCharsetsList(&charsets);

	newDataSet->setUnicodeString(10, 0, 10, 0, L"initial test 0", "PN");
	newDataSet->setUnicodeString(10, 0, 11, 0, L"initial test 1", "PN");
	newDataSet->setUnicodeString(10, 0, 12, 0, L"initial test 2", "PN");
	newDataSet->setUnicodeString(10, 0, 13, 0, L"initial test 3", "PN");

	newDataSet->setUnicodeString(20, 0, 10, 0, L"initial test 4", "PN");
	newDataSet->setUnicodeString(20, 0, 11, 0, L"initial test 5", "PN");
	newDataSet->setUnicodeString(20, 0, 12, 0, L"initial test 6", "PN");
	newDataSet->setUnicodeString(20, 0, 13, 0, L"initial test 7", "PN");

	IMEBRA_TRANSACTION_START();

		IMEBRA_TRANSACTION_START();

			newDataSet->setUnicodeString(20, 0, 10, 0, L"test 4", "PN");
			newDataSet->setUnicodeString(20, 0, 11, 0, L"test 5", "PN");
			newDataSet->setUnicodeString(20, 0, 12, 0, L"test 6", "PN");
			newDataSet->setUnicodeString(20, 0, 13, 0, L"test 7", "PN");
		
		IMEBRA_TRANSACTION_END();

		newDataSet->setUnicodeString(10, 0, 10, 0, L"test 0", "PN");
		newDataSet->setUnicodeString(10, 0, 11, 0, L"test 1", "PN");
		newDataSet->setUnicodeString(10, 0, 12, 0, L"\0x420\x062a\x062b^\0x400\0x410\x0628\x062a", "PN");
		newDataSet->setUnicodeString(10, 0, 13, 0, L"test 3", "PN");

		IMEBRA_TRANSACTION_ABORT();

	IMEBRA_TRANSACTION_END();

    EXPECT_EQ(std::wstring(L"initial test 0"), newDataSet->getUnicodeString(10, 0, 10, 0));
    EXPECT_EQ(std::wstring(L"initial test 1"), newDataSet->getUnicodeString(10, 0, 11, 0));
    EXPECT_EQ(std::wstring(L"initial test 2"), newDataSet->getUnicodeString(10, 0, 12, 0));
    EXPECT_EQ(std::wstring(L"initial test 3"), newDataSet->getUnicodeString(10, 0, 13, 0));

    EXPECT_EQ(std::wstring(L"initial test 4"), newDataSet->getUnicodeString(20, 0, 10, 0));
    EXPECT_EQ(std::wstring(L"initial test 5"), newDataSet->getUnicodeString(20, 0, 11, 0));
    EXPECT_EQ(std::wstring(L"initial test 6"), newDataSet->getUnicodeString(20, 0, 12, 0));
    EXPECT_EQ(std::wstring(L"initial test 7"), newDataSet->getUnicodeString(20, 0, 13, 0));
}

TEST(transactionTest, testNestedTransactionsFail1)
{
	ptr<dataSet> newDataSet(new dataSet);
	charsetsList::tCharsetsList charsets;
    charsets.push_back("ISO_IR 100");
	newDataSet->setCharsetsList(&charsets);

	newDataSet->setUnicodeString(10, 0, 10, 0, L"initial test 0", "PN");
	newDataSet->setUnicodeString(10, 0, 11, 0, L"initial test 1", "PN");
	newDataSet->setUnicodeString(10, 0, 12, 0, L"initial test 2", "PN");
	newDataSet->setUnicodeString(10, 0, 13, 0, L"initial test 3", "PN");

	newDataSet->setUnicodeString(20, 0, 10, 0, L"initial test 4", "PN");
	newDataSet->setUnicodeString(20, 0, 11, 0, L"initial test 5", "PN");
	newDataSet->setUnicodeString(20, 0, 12, 0, L"initial test 6", "PN");
	newDataSet->setUnicodeString(20, 0, 13, 0, L"initial test 7", "PN");

	try
	{
		IMEBRA_TRANSACTION_START();

			IMEBRA_COMMIT_TRANSACTION_START();

				newDataSet->setUnicodeString(20, 0, 10, 0, L"test 4", "PN");
				newDataSet->setUnicodeString(20, 0, 11, 0, L"test 5", "PN");
				newDataSet->setUnicodeString(20, 0, 12, 0, L"test 6", "PN");
				newDataSet->setUnicodeString(20, 0, 13, 0, L"test 7", "PN");

			IMEBRA_TRANSACTION_END()

			newDataSet->setUnicodeString(10, 0, 10, 0, L"test 0", "PN");
			newDataSet->setUnicodeString(10, 0, 11, 0, L"test 1", "PN");
			newDataSet->setUnicodeString(10, 0, 12, 0, L"\0x420\x062a\x062b^\0x400\0x410\x0628\x062a", "PN");
			newDataSet->setUnicodeString(10, 0, 13, 0, L"test 3", "PN");

			PUNTOEXE_THROW(std::runtime_error, "test abort");

		IMEBRA_TRANSACTION_END();
	}
	catch(std::runtime_error&)
	{
		std::wstring message = puntoexe::exceptionsManager::getMessage();
        ASSERT_TRUE(message.find(L"test abort") != std::string::npos);
	}

    EXPECT_EQ(std::wstring(L"initial test 0"), newDataSet->getUnicodeString(10, 0, 10, 0));
    EXPECT_EQ(std::wstring(L"initial test 1"), newDataSet->getUnicodeString(10, 0, 11, 0));
    EXPECT_EQ(std::wstring(L"initial test 2"), newDataSet->getUnicodeString(10, 0, 12, 0));
    EXPECT_EQ(std::wstring(L"initial test 3"), newDataSet->getUnicodeString(10, 0, 13, 0));

    EXPECT_EQ(std::wstring(L"test 4"), newDataSet->getUnicodeString(20, 0, 10, 0));
    EXPECT_EQ(std::wstring(L"test 5"), newDataSet->getUnicodeString(20, 0, 11, 0));
    EXPECT_EQ(std::wstring(L"test 6"), newDataSet->getUnicodeString(20, 0, 12, 0));
    EXPECT_EQ(std::wstring(L"test 7"), newDataSet->getUnicodeString(20, 0, 13, 0));
}


} // namespace tests

} // namespace imebra

} // namespace puntoexe
