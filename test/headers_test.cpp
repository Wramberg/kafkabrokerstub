#include "kafka_broker_stub/headers.hpp"
#include "kafka_broker_stub/headers.hpp"

#include "test_common.hpp"

namespace kbs = kafka_broker_stub;

class headers_test : public kbs::test::suite
{
public:
	headers_test(const std::string& name): suite(name) { }
	
private:
	void tests()
	{
		// A request generated by librdkafka, recorded using tcpdump, and edited a bit for test purposes
		uint8_t req[] = {0x00, 0x00, 0x00, 0x1b, 0x00, 0x03, 0x00, 0x01, 0x00, 0x00, 0x00, 0x02, 0x00, 0x07, 0x72, 0x64, 0x6b, 0x61, 0x66, 0x6b, 0x61, 0x00, 0x00, 0x00, 0x01, 0x00, 0x04, 0x74, 0x65, 0x73, 0x74};
		
		// Decode the request header in the message - note that the first four bytes describe the length of the message
		kbs::headers::request_hdr req_hdr;
		ASSERT_EQ(req_hdr.deserialize(req+4), const_cast<const uint8_t*>(req+4+17));
		ASSERT_EQ(req_hdr.api_key(), kbs::primitive::int16(3));
		ASSERT_EQ(req_hdr.api_version(), kbs::primitive::int16(1));
		ASSERT_EQ(req_hdr.correlation_id(), kbs::primitive::int32(2));
		ASSERT_EQ(req_hdr.client_id().std_str(), std::string("rdkafka"));
		
		// Generate a response header
		uint8_t out[128];
		kbs::headers::response_hdr resp_hdr(305419896);
		ASSERT_EQ(resp_hdr.serialize(out), (out+4));
		ASSERT_EQ(resp_hdr.serial_size(), static_cast<size_t>(4));
		ASSERT_EQ(out[0], static_cast<uint8_t>(0x12));
		ASSERT_EQ(out[1], static_cast<uint8_t>(0x34));
		ASSERT_EQ(out[2], static_cast<uint8_t>(0x56));
		ASSERT_EQ(out[3], static_cast<uint8_t>(0x78));
	}
	
};

int main()
{
	headers_test suite("Headers unittests");
	suite.execute_tests();
	return 0;
}