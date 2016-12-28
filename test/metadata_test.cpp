#include "kafka_broker_stub/metadata.hpp"
#include "kafka_broker_stub/metadata.hpp"

#include "test_common.hpp"

namespace kbs = kafka_broker_stub;

class metadata_test : public kbs::test::suite
{
public:
	metadata_test(const std::string& name): suite(name) { }
	
private:
	void tests()
	{
		// A metadata request generated by librdkafka and recorded using tcpdump
		uint8_t req[] = {0x00, 0x00, 0x00, 0x1b, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x00, 0x07, 0x72, 0x64, 0x6b, 0x61, 0x66, 0x6b, 0x61, 0x00, 0x00, 0x00, 0x01, 0x00, 0x04, 0x74, 0x65, 0x73, 0x74};
		
		// Decode the request header in the message - note that the first four bytes describe the length of the message
		kbs::metadata::request_v0 meta_req;
		ASSERT_EQ(meta_req.deserialize(req+4), const_cast<const uint8_t*>(req+4+27));
	}

};

int main()
{
	metadata_test suite("Metadata unittests");
	suite.execute_tests();
	return 0;
}
