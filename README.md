# C++ Kafka Broker Stub
[![Build Status](https://travis-ci.org/Wramberg/kafkabrokerstub.svg?branch=master)](https://travis-ci.org/Wramberg/kafkabrokerstub)
[![Coverage Status](https://coveralls.io/repos/github/Wramberg/kafkabrokerstub/badge.svg?branch=master)](https://coveralls.io/github/Wramberg/kafkabrokerstub?branch=master)

A simple, header-only C++ Kafka broker stub for isolated unittests.

## Support
The broker stub currently supports
- Metadata requests [API version 0]
- Produce requests [API version 0]

## Future Development
The stub was developed due to lack of any other C++ broker stubs. The authors requirements are very limited, however, so the stub only supports a small number of requests. The basis for future development has been laid though. The stub is structured in a hierarchical fashion (think composite design pattern) where all primitive kafka types have been implemented. It should thus be straightforward to add support for more requests/versions. For more details on the Kafka wire protocol see http://kafka.apache.org/protocol.html.

## Basic Usage
To use the broker stub a TCP server must first be implemented. This is not included in the stub to avoid enforcing a particular library or architecture-specific approach on the developer. Once a TCP server is implemented the following example describe the basic usage of the stub.

1. Instantiate a broker stub with node ID 0 using localhost and port 9092.

```c++
m_stub = new kafka_broker_stub::broker_stub(0, "localhost", 9092);
```

2. Make a single topic called test with two partitions (0 and 1), make broker 0 the leader for them.

```c++
std::vector<kafka_broker_stub::partition> partitions;
partitions.push_back(kafka_broker_stub::partition(0, 0));
partitions.push_back(kafka_broker_stub::partition(1, 0));
m_stub->add_topic("test", partitions);
```

3. When data is received from the TCP server parse it to the stub and send the responses to the client

```c++
std::vector<std::string> responses;
int ret = m_stub->handle_data(data, bytes_read, responses);
if (ret < 0)
{
   /* Parsing error */
}

std::vector<std::string>::iterator it = responses.begin();
for (; it != responses.end(); ++it)
{
   /* Send response to the client using it->data() and it->size() */
}

/* Note that the return value, ret, is the number of bytes read from the input chunk. The stub assumes that the server offers some kind of buffer like e.g. a boost::asio::streambuf. */
```

4. Check data on topic

```c++
/* Note: NULL checks, etc. omitted for clarity */
const kafka_broker_stub::topic* top = m_stub->get_topic("test");
const kafka_broker_stub::partition* part = top->get_partition(0);
const key_value_pair& msg = part->data()[0];
printf("First message in partition 0 is [%s,%s]\n", msg.key().c_str(), msg.data().c_str());

```

For details see the main.hpp header file.
