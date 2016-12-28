#include "kafka_broker_stub/primitive.hpp"
#include "kafka_broker_stub/primitive.hpp"

#include "test_common.hpp"

namespace kbs = kafka_broker_stub;

class primitive_test : public kbs::test::suite
{
public:
	primitive_test(const std::string& name): suite(name) { }
	
private:
	void int_tests()
	{
		// Run some tests on the int primitves
		kbs::primitive::int8 a;
		kbs::primitive::int16 b;
		kbs::primitive::int32 c;
		kbs::primitive::int64 d;
		
		ASSERT_EQ(a.serial_size(), static_cast<size_t>(1));
		ASSERT_EQ(b.serial_size(), static_cast<size_t>(2));
		ASSERT_EQ(c.serial_size(), static_cast<size_t>(4));
		ASSERT_EQ(d.serial_size(), static_cast<size_t>(8));
		
		{
			uint8_t in[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01};
			ASSERT_EQ(a.deserialize(in+7), const_cast<const uint8_t*>(in+8));
			ASSERT_EQ(static_cast<int>(a), static_cast<int>(1));
			ASSERT_EQ(b.deserialize(in+6), const_cast<const uint8_t*>(in+8));
			ASSERT_EQ(static_cast<int>(b), static_cast<int>(1));
			ASSERT_EQ(c.deserialize(in+4), const_cast<const uint8_t*>(in+8));
			ASSERT_EQ(static_cast<int>(c), static_cast<int>(1));
			ASSERT_EQ(d.deserialize(in), const_cast<const uint8_t*>(in+8));
			ASSERT_EQ(static_cast<int>(d), static_cast<int>(1));
			
			uint8_t out[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
			
			ASSERT_EQ(a.serialize(out), (out+1));
			ASSERT_EQ(memcmp(out, in+7, 1), 0);
			ASSERT_EQ(b.serialize(out), (out+2));
			ASSERT_EQ(memcmp(out, in+6, 2), 0);
			ASSERT_EQ(c.serialize(out), (out+4));
			ASSERT_EQ(memcmp(out, in+4, 4), 0);
			ASSERT_EQ(d.serialize(out), (out+8));
			ASSERT_EQ(memcmp(out, in, 8), 0);	
		}
		{
			// Note we do not read first and last byte
			uint8_t in[] = {0x88, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x77};
			ASSERT_EQ(a.deserialize(in+8), const_cast<const uint8_t*>(in+9));
			ASSERT_EQ(static_cast<int>(a), static_cast<int>(2));
			ASSERT_EQ(b.deserialize(in+7), const_cast<const uint8_t*>(in+9));
			ASSERT_EQ(static_cast<int>(b), static_cast<int>(2));
			ASSERT_EQ(c.deserialize(in+5), const_cast<const uint8_t*>(in+9));
			ASSERT_EQ(static_cast<int>(c), static_cast<int>(2));
			ASSERT_EQ(d.deserialize(in+1), const_cast<const uint8_t*>(in+9));
			ASSERT_EQ(static_cast<int>(d), static_cast<int>(2));
		}		
		{
			uint8_t in[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
			ASSERT_EQ(a.deserialize(in+7), const_cast<const uint8_t*>(in+8));
			ASSERT_EQ(static_cast<int>(a), static_cast<int>(-1));
			ASSERT_EQ(b.deserialize(in+6), const_cast<const uint8_t*>(in+8));
			ASSERT_EQ(static_cast<int>(b), static_cast<int>(-1));
			ASSERT_EQ(c.deserialize(in+4), const_cast<const uint8_t*>(in+8));
			ASSERT_EQ(static_cast<int>(c), static_cast<int>(-1));
			ASSERT_EQ(d.deserialize(in), const_cast<const uint8_t*>(in+8));
			ASSERT_EQ(static_cast<int>(d), static_cast<int>(-1));
			
			uint8_t out[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
			
			ASSERT_EQ(a.serialize(out), (out+1));
			ASSERT_EQ(memcmp(out, in, 1), 0);
			ASSERT_EQ(b.serialize(out), (out+2));
			ASSERT_EQ(memcmp(out, in, 2), 0);
			ASSERT_EQ(c.serialize(out), (out+4));
			ASSERT_EQ(memcmp(out, in, 4), 0);
			ASSERT_EQ(d.serialize(out), (out+8));
			ASSERT_EQ(memcmp(out, in, 8), 0);
		}	
		{
			uint8_t in[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE};
			a.deserialize(in+7);
			ASSERT_EQ(static_cast<int>(a), static_cast<int>(-2));
			b.deserialize(in+6);
			ASSERT_EQ(static_cast<int>(b), static_cast<int>(-2));
			c.deserialize(in+4);
			ASSERT_EQ(static_cast<int>(c), static_cast<int>(-2));
			d.deserialize(in);
			ASSERT_EQ(static_cast<int>(d), static_cast<int>(-2));
		}	
		{
			uint8_t in[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
			a.deserialize(in+7);
			ASSERT_EQ(static_cast<int>(a), static_cast<int>(0));
			b.deserialize(in+6);
			ASSERT_EQ(static_cast<int>(b), static_cast<int>(0));
			c.deserialize(in+4);
			ASSERT_EQ(static_cast<int>(c), static_cast<int>(0));
			d.deserialize(in);
			ASSERT_EQ(static_cast<int>(d), static_cast<int>(0));
		}	
		{
			uint8_t in[] = {0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77, 0x77};
			ASSERT_EQ(a.deserialize(in+7), const_cast<const uint8_t*>(in+8));
			ASSERT_EQ(static_cast<int>(a), static_cast<int>(119));
			ASSERT_EQ(b.deserialize(in+6), const_cast<const uint8_t*>(in+8));
			ASSERT_EQ(static_cast<int>(b), static_cast<int>(30583));
			ASSERT_EQ(c.deserialize(in+4), const_cast<const uint8_t*>(in+8));
			ASSERT_EQ(static_cast<int>(c), static_cast<int>(2004318071));
			ASSERT_EQ(d.deserialize(in), const_cast<const uint8_t*>(in+8));
			ASSERT_EQ(static_cast<int64_t>(d), static_cast<int64_t>(8608480567731124087));
			
			uint8_t out[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
			
			ASSERT_EQ(a.serialize(out), (out+1));
			ASSERT_EQ(memcmp(out, in, 1), 0);
			ASSERT_EQ(b.serialize(out), (out+2));
			ASSERT_EQ(memcmp(out, in, 2), 0);
			ASSERT_EQ(c.serialize(out), (out+4));
			ASSERT_EQ(memcmp(out, in, 4), 0);
			ASSERT_EQ(d.serialize(out), (out+8));
			ASSERT_EQ(memcmp(out, in, 8), 0);
		}	
	}

	void string_tests()
	{
		kbs::primitive::string str;
		{
			uint8_t in[] = {0x00, 0x03, 'h', 'e', 'j'};
			ASSERT_EQ(str.deserialize(in), const_cast<const uint8_t*>(in+5));
			ASSERT_EQ(str.size(), static_cast<size_t>(3));
			ASSERT_EQ(str.serial_size(), static_cast<size_t>(5));
			ASSERT_EQ(str.std_str(), std::string("hej"));
			
			uint8_t out[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
			ASSERT_EQ(str.serialize(out), (out+5));
			ASSERT_EQ(memcmp(out, in, 5), 0);
			ASSERT_EQ(out[5], static_cast<uint8_t>(0xFF));
		}
		{
			uint8_t in[] = {0x00, 0x02, 't', 's', 't'};
			ASSERT_EQ(str.deserialize(in), const_cast<const uint8_t*>(in+4));
			ASSERT_EQ(str.size(), static_cast<size_t>(2));
			ASSERT_EQ(str.serial_size(), static_cast<size_t>(4));
			ASSERT_EQ(str.std_str(), std::string("ts"));
			
			uint8_t out[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
			ASSERT_EQ(str.serialize(out), (out+4));
			ASSERT_EQ(memcmp(out, in, 4), 0);
			ASSERT_EQ(out[4], static_cast<uint8_t>(0xFF));
			ASSERT_EQ(out[5], static_cast<uint8_t>(0xFF));
		}
		{
			uint8_t in[] = {0x00, 0x00, 't', 's', 't'};
			ASSERT_EQ(str.deserialize(in), const_cast<const uint8_t*>(in+2));
			ASSERT_EQ(str.size(), static_cast<size_t>(0));
			ASSERT_EQ(str.serial_size(), static_cast<size_t>(2));
			ASSERT_EQ(str.std_str(), std::string(""));
			
			uint8_t out[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
			ASSERT_EQ(str.serialize(out), (out+2));
			ASSERT_EQ(memcmp(out, in, 2), 0);
		}
		{
			uint8_t in[] = {0xFF, 0xFF, 't', 's', 't'};
			ASSERT_EQ(str.deserialize(in), const_cast<const uint8_t*>(in+2));
			ASSERT_EQ(str.size(), static_cast<size_t>(0));
			ASSERT_EQ(str.serial_size(), static_cast<size_t>(2));
			ASSERT_EQ(str.std_str(), std::string(""));
		}
	}

	void bytearray_tests()
	{
		kbs::primitive::bytearray arr;
		{
			uint8_t in[] = {0x00, 0x00, 0x00, 0x03, 'h', 'e', 'j'};
			ASSERT_EQ(arr.deserialize(in), const_cast<const uint8_t*>(in+7));
			ASSERT_EQ(arr.size(), static_cast<size_t>(3));
			ASSERT_EQ(arr.serial_size(), static_cast<size_t>(7));
			ASSERT_EQ(arr.std_str(), std::string("hej"));
			ASSERT_EQ(memcmp(arr.data(), in+4, 3), 0);
			ASSERT_EQ(arr[0], static_cast<uint8_t>('h'));
			ASSERT_EQ(arr[1], static_cast<uint8_t>('e'));
			ASSERT_EQ(arr[2], static_cast<uint8_t>('j'));
			
			uint8_t out[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
			ASSERT_EQ(arr.serialize(out), (out+7));
			ASSERT_EQ(memcmp(out, in, 7), 0);
			ASSERT_EQ(out[7], static_cast<uint8_t>(0xFF));
		}
		{
			uint8_t in[] = {0x00, 0x00, 0x00, 0x02, 'h', 'e', 'j'};
			ASSERT_EQ(arr.deserialize(in), const_cast<const uint8_t*>(in+6));
			ASSERT_EQ(arr.size(), static_cast<size_t>(2));
			ASSERT_EQ(arr.serial_size(), static_cast<size_t>(6));
			ASSERT_EQ(arr.std_str(), std::string("he"));
			ASSERT_EQ(memcmp(arr.data(), in+4, 2), 0);
			
			uint8_t out[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
			ASSERT_EQ(arr.serialize(out), (out+6));
			ASSERT_EQ(memcmp(out, in, 6), 0);
			ASSERT_EQ(out[6], static_cast<uint8_t>(0xFF));
		}
		{
			uint8_t in[] = {0x00, 0x00, 0x00, 0x00, 'h', 'e', 'j'};
			ASSERT_EQ(arr.deserialize(in), const_cast<const uint8_t*>(in+4));
			ASSERT_EQ(arr.size(), static_cast<size_t>(0));
			ASSERT_EQ(arr.serial_size(), static_cast<size_t>(4));
			ASSERT_EQ(arr.std_str(), std::string(""));
			
			uint8_t out[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
			ASSERT_EQ(arr.serialize(out), (out+4));
			ASSERT_EQ(memcmp(out, in, 4), 0);
		}
	}
	
	void array_tests()
	{
		// Simple array of int16
		kbs::primitive::array<kbs::primitive::int16> int_arr;
		{
			uint8_t in[] = {0x00, 0x00, 0x00, 0x02, 0x00, 0x01, 0xFF, 0xFF};
			ASSERT_EQ(int_arr.deserialize(in), const_cast<const uint8_t*>(in+8));
			ASSERT_EQ(int_arr.size(), static_cast<size_t>(2));
			ASSERT_EQ(int_arr.serial_size(), static_cast<size_t>(8));
			ASSERT_EQ(int_arr[0], kbs::primitive::int16(1));
			ASSERT_EQ(int_arr[1], kbs::primitive::int16(-1));
			
			uint8_t out[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE};
			ASSERT_EQ(int_arr.serialize(out), (out+8));
			ASSERT_EQ(memcmp(out, in, 8), 0);
			ASSERT_EQ(out[8], static_cast<uint8_t>(0xFE));
		}
		{
			uint8_t in[] = {0x00, 0x00, 0x00, 0x00, 0xFF};
			ASSERT_EQ(int_arr.deserialize(in), const_cast<const uint8_t*>(in+4));
			ASSERT_EQ(int_arr.size(), static_cast<size_t>(0));
		}
	}
	
	void tests()
	{	
		int_tests();
		string_tests();
		bytearray_tests();
		array_tests();
	}
	
};

int main()
{
	primitive_test suite("Primitive unittests");
	suite.execute_tests();
	return 0;
}
