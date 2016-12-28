#include "kafka_broker_stub/produce.hpp"
#include "kafka_broker_stub/produce.hpp"

#include "test_common.hpp"

namespace kbs = kafka_broker_stub;

class produce_test : public kbs::test::suite
{
public:
	produce_test(const std::string& name): suite(name) { }

private:
	void request_test()
	{

	}

	void response_test()
	{

	}

	void tests()
	{
		request_test();
		response_test();
	}
};

int main()
{
	produce_test suite("Produce unittests");
	suite.execute_tests();
	return 0;
}
