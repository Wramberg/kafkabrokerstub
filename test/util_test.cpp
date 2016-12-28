#include "kafka_broker_stub/util.hpp"
#include "kafka_broker_stub/util.hpp"

#include "test_common.hpp"

namespace kbs = kafka_broker_stub;

class util_test : public kbs::test::suite
{
public:
	util_test(const std::string& name): suite(name) { }
	
private:
	void tests()
	{
		// Run some tests on the byteswapping function
		int8_t a = 0x12;
		ASSERT_EQ(kbs::util::byte_swap(&a), static_cast<int8_t>(0x12));

		int16_t b = 0x1234;
		ASSERT_EQ(kbs::util::byte_swap(&b), static_cast<int16_t>(0x3412));

		int32_t c = 0x12345678;
		ASSERT_EQ(kbs::util::byte_swap(&c), static_cast<int32_t>(0x78563412));

		int64_t d = static_cast<int64_t>(0x8979695949392919);
		ASSERT_EQ(kbs::util::byte_swap(&d), static_cast<int64_t>(0x1929394959697989));
		
		bool caught_exception(false);
		try
		{
			kbs::util::byte_swap<int8_t>(NULL);
		}
		catch(const std::runtime_error&)
		{
			caught_exception = true;
		}
		ASSERT_EQ(caught_exception, static_cast<bool>(true));
		
		// Run some tests on the read function. Note that it includes byteswapping.
		{
			uint8_t in[] = {0x00, 0x00, 0x00, 0x01};
			int32_t out = kbs::util::read_type<int32_t>(in);
			ASSERT_EQ(out, static_cast<int32_t>(1));
		}
		{
			uint8_t in[] = {0x00, 0x00, 0x01, 0x01};
			int32_t out = kbs::util::read_type<int32_t>(in);
			ASSERT_EQ(out, static_cast<int32_t>(257));
		}
		{
			uint8_t in[] = {0xFF, 0xFF, 0xFF, 0xFF};
			int32_t out = kbs::util::read_type<int32_t>(in);
			ASSERT_EQ(out, static_cast<int32_t>(-1));
		}
		{
			uint8_t in[] = {0xFF, 0xFF, 0xFF, 0xFE};
			int32_t out = kbs::util::read_type<int32_t>(in);
			ASSERT_EQ(out, static_cast<int32_t>(-2));
		}
		{
			uint8_t in[] = {0x00, 0x05};
			int16_t out = kbs::util::read_type<int16_t>(in);
			ASSERT_EQ(out, static_cast<int16_t>(5));
		}
		{
			uint8_t in[] = {0xFF};
			int8_t out = kbs::util::read_type<int8_t>(in);
			ASSERT_EQ(out, static_cast<int8_t>(-1));
		}
		{
			uint8_t in[] = {0x7F};
			int8_t out = kbs::util::read_type<int8_t>(in);
			ASSERT_EQ(out, static_cast<int8_t>(127));
		}
		{
			uint8_t in[] = {0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01};
			int64_t out = kbs::util::read_type<int64_t>(in);
			ASSERT_EQ(out, static_cast<int64_t>(72057594037927937));
		}
		
		
		// Run some tests on the write function. Note that it includes byteswapping.
		// We just verified the read_type function so for simplicity we use it here.
		uint8_t data[16];

		kbs::util::write_type<int8_t>(127, data);
		ASSERT_EQ(kbs::util::read_type<int8_t>(data), static_cast<int8_t>(127));
		
		kbs::util::write_type<int8_t>(1, data);
		ASSERT_EQ(kbs::util::read_type<int8_t>(data), static_cast<int8_t>(1));

		kbs::util::write_type<int8_t>(-1, data);
		ASSERT_EQ(kbs::util::read_type<int8_t>(data), static_cast<int8_t>(-1));

		kbs::util::write_type<int8_t>(-2, data);
		ASSERT_EQ(kbs::util::read_type<int8_t>(data), static_cast<int8_t>(-2));

		kbs::util::write_type<int16_t>(128, data);
		ASSERT_EQ(kbs::util::read_type<int16_t>(data), static_cast<int16_t>(128));
		
		kbs::util::write_type<int16_t>(-100, data);
		ASSERT_EQ(kbs::util::read_type<int16_t>(data), static_cast<int16_t>(-100));
		
		kbs::util::write_type<int32_t>(123465432, data);
		ASSERT_EQ(kbs::util::read_type<int32_t>(data), static_cast<int32_t>(123465432));
		
		kbs::util::write_type<int64_t>(-100000, data);
		ASSERT_EQ(kbs::util::read_type<int64_t>(data), static_cast<int64_t>(-100000));

		kbs::util::write_type<int64_t>(0, data);
		ASSERT_EQ(kbs::util::read_type<int64_t>(data), static_cast<int64_t>(0));

		kbs::util::write_type<int64_t>(1, data);
		ASSERT_EQ(kbs::util::read_type<int64_t>(data), static_cast<int64_t>(1));
	}
	
};

int main()
{
	util_test suite("Util unittests");
	suite.execute_tests();
	return 0;
}
