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
		partitions.push_back(kbs::partition(0, 1));
		partitions.push_back(kbs::partition(1, 1));

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

	}

	void produce_v0_test()
	{

	}

	void misc_test()
	{

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
