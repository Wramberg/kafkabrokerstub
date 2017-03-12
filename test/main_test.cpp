#include "kafka_broker_stub/main.hpp"
#include "kafka_broker_stub/main.hpp"

#include "test_common.hpp"

namespace kbs = kafka_broker_stub;

class produce_test : public kbs::test::suite
{
public:
	produce_test(const std::string& name):
		suite(name),
		m_stub(NULL)
	{

	}

	~produce_test()
	{
		if (m_stub)
			delete m_stub;
	}

private:
	kbs::broker_stub* m_stub;

	void setup()
	{
		// Make two partitions
		std::vector<kbs::partition> partitions;
		partitions.push_back(kbs::partition(0, 0));
		partitions.push_back(kbs::partition(1, 0));

		// Make broker stub and add topic with the two partitions
		m_stub = new kbs::broker_stub(0, "localhost", 9092);
		m_stub->add_topic("test", partitions);

		// Add a reference to another broker
		m_stub->add_broker_reference(1, "localhost", 9093);

		// Few simple topic tests
		ASSERT_EQ(m_stub->get_topic("testt"), static_cast<const kbs::topic*>(NULL));
		ASSERT_EQ(m_stub->get_topic("tes"), static_cast<const kbs::topic*>(NULL));
		ASSERT_EQ(m_stub->get_topic("ttes"), static_cast<const kbs::topic*>(NULL));
		const kbs::topic* top = m_stub->get_topic("test");
		ASSERT_NEQ(top, static_cast<const kbs::topic*>(NULL));

		// Check stuff on the topic
		ASSERT_EQ(top->name(), std::string("test"));
		ASSERT_EQ(top->partitions().size(), static_cast<size_t>(2));
		ASSERT_NEQ(top->get_partition(0), static_cast<const kbs::partition*>(NULL));
		ASSERT_NEQ(top->get_partition(1), static_cast<const kbs::partition*>(NULL));
		ASSERT_EQ(top->get_partition(2), static_cast<const kbs::partition*>(NULL));
	}

	void metadata_v0_test()
	{
		// A metadata request generated by librdkafka and recorded using tcpdump
		// Note that we append a new 4 byte msg length at the end to test
		// boundaries
		uint8_t req[] = {
			0x00, 0x00, 0x00, 0x1b, 0x00, 0x03, 0x00, 0x00, 0x00,
        	0x00, 0x00, 0x02, 0x00, 0x07, 0x72, 0x64, 0x6b, 0x61,
	     	0x66, 0x6b, 0x61, 0x00, 0x00, 0x00, 0x01, 0x00, 0x04,
	     	0x74, 0x65, 0x73, 0x74, 0x00, 0x00, 0x00, 0x1
	   };

      std::vector<std::string> responses;
      int ret = m_stub->handle_data(req, sizeof(req), responses);
      ASSERT_EQ(ret, static_cast<int>(sizeof(req)-4));

      // Check response
      uint8_t expected_resp[] = {
			0x00, 0x00, 0x00, 0x72, // Message size
			0x00, 0x00, 0x00, 0x02, // Corr. id

			0x00, 0x00, 0x00, 0x02, // Array of brokers
			   0x00, 0x00, 0x00, 0x00, // Broker id
				0x00, 0x09, 0x6C, 0x6F, 0x63, 0x61, 0x6C, 0x68, 0x6F, 0x73, 0x74, // Host
			   0x00, 0x00, 0x23, 0x84, // Port
			   0x00, 0x00, 0x00, 0x01, // Broker id
			   0x00, 0x09, 0x6C, 0x6F, 0x63, 0x61, 0x6C, 0x68, 0x6F, 0x73, 0x74, // Host
			   0x00, 0x00, 0x23, 0x85, // Port

			0x00, 0x00, 0x00, 0x01, // Array of topic metadata
			   0x00, 0x00, // Err. code success
			   0x00, 0x04, 0x74, 0x65, 0x73, 0x74, // Name as string "test"
			   0x00, 0x00, 0x00, 0x02, // Array of partition metadata
			      0x00, 0x00, // Err. code
			      0x00, 0x00, 0x00, 0x00, // Partition ID
			      0x00, 0x00, 0x00, 0x00, // Leader ID
			      0x00, 0x00, 0x00, 0x01, // Array of replicas IDs
			         0x00, 0x00, 0x00, 0x00,
			      0x00, 0x00, 0x00, 0x01, // Array of ISR ids
			         0x00, 0x00, 0x00, 0x00,
			      0x00, 0x00, // Err. code
			      0x00, 0x00, 0x00, 0x01, // Partition ID
			      0x00, 0x00, 0x00, 0x00, // Leader ID
			      0x00, 0x00, 0x00, 0x01, // Array of replicas IDs
			         0x00, 0x00, 0x00, 0x00,
			      0x00, 0x00, 0x00, 0x01, // Array of ISR ids
			         0x00, 0x00, 0x00, 0x00
		};
      ASSERT_EQ(responses.size(), static_cast<size_t>(1));
      std::string& resp = responses[0];
      ASSERT_EQ(resp.size(), sizeof(expected_resp));

		int cmp = memcmp(resp.data(), expected_resp, sizeof(expected_resp));
      ASSERT_EQ(cmp, static_cast<int>(0));
	}

	void produce_v0_test()
	{
		// A modified produce request generated by librdkafka and recorded using tcpdump
		uint8_t req[] = {
			0x00, 0x00, 0x00, 0x52, // Length of message 82 bytes
			0x00, 0x00, // Api key 0
			0x00, 0x00, // Api version 0
			0x00, 0x00, 0x00, 0x03, // Correlation id 3
			0x00, 0x07, 0x72, 0x64, 0x6b, 0x61, 0x66, 0x6b, 0x61, // client id string
			0x00, 0x01, // acks
			0x00, 0x00, 0x13, 0x88, // timeout
			0x00, 0x00, 0x00, 0x01, // topic data array start
				0x00, 0x04, 0x74, 0x65, 0x73, 0x74, // topic name string
				0x00, 0x00, 0x00, 0x01, // data array start
					0x00, 0x00, 0x00, 0x01, // partition id
					0x00, 0x00, 0x00, 0x25, // length of binary message (rest of payload)
						0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x09, // offset
						0x00, 0x00, 0x00, 0x19, // message size
						0xa6, 0xb1, 0x36, 0x2b, // crc
						0xFF, // magic byte
						0xEE, // attributes
						0xff, 0xff, 0xff, 0xff, // key byte array
				   	0x00, 0x00, 0x00, 0x0b, // value bytearray length (rest of payload)
				   		0x74, 0x65, 0x73, 0x74, 0x6d, 0x65,
				   		0x73, 0x73, 0x61, 0x67, 0x65};

      std::vector<std::string> responses;
      int ret = m_stub->handle_data(req, sizeof(req), responses);
      ASSERT_EQ(ret, static_cast<int>(sizeof(req)));

		// Check the data against a modified tcpdump capture from an actual kafka broker
		uint8_t expected_resp[] = {
			0x00, 0x00, 0x00, 0x20,
			0x00, 0x00, 0x00, 0x03, // Correlation ID
			0x00, 0x00, 0x00, 0x01, // Array of responses start
				0x00, 0x04, 0x74, 0x65, 0x73, 0x74, // Topic name string
				0x00, 0x00, 0x00, 0x01, // Array of partition responses start
					0x00, 0x00, 0x00, 0x01, // Partition ID
					0x00, 0x00, // Partition error code
					0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 // Offset
		};

      ASSERT_EQ(responses.size(), static_cast<size_t>(1));
      std::string& resp = responses[0];
      ASSERT_EQ(resp.size(), sizeof(expected_resp));

		int cmp = memcmp(resp.data(), expected_resp, sizeof(expected_resp));
      ASSERT_EQ(cmp, static_cast<int>(0));
	}

	void misc_test()
	{
      std::vector<std::string> responses;
      int ret = m_stub->handle_data(NULL, 1, responses);
      ASSERT_EQ(ret, static_cast<int>(0));

      uint8_t dummy[3] = {0};
      ret = m_stub->handle_data(dummy, 3, responses);
      ASSERT_EQ(ret, static_cast<int>(0));
	}

	void tests()
	{
		setup();
		metadata_v0_test();
		produce_v0_test();
		misc_test();
	}

	produce_test(const produce_test&);
	const produce_test* operator=(const produce_test&);
};

int main()
{
	produce_test suite("Main functional tests");
	suite.execute_tests();
	return 0;
}
