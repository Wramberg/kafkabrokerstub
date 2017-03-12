#ifndef KAFKA_BROKER_STUB_MAIN_HPP_INC_
#define KAFKA_BROKER_STUB_MAIN_HPP_INC_

#include "primitive.hpp"
#include "metadata.hpp"
#include "produce.hpp"
#include "headers.hpp"
#include "util.hpp"
#include <list>
#include <string>
#include <stdio.h>

namespace kafka_broker_stub {

	// Maximum size of a response message
	const size_t RESP_MAX_SIZE = 4096;

	/**
	 * Simple key value pair
	 *
	 * This is returned by the broker stub when looking up data in partitions
	 */
	class key_value_pair
	{
	public:
		key_value_pair(const std::string& k, const std::string& v):
			m_key(k),
			m_value(v)
		{

		}

		const std::string& key() const
		{
			return m_key;
		}

		const std::string& value() const
		{
			return m_value;
		}

	private:
		std::string m_key;
		std::string m_value;
	};

	/**
	 * Partition that holds an array of key-value pairs
	 */
	class partition
	{
	public:
		partition(int32_t part_id, int32_t leader_id):
			m_data(),
			m_part_id(part_id),
			m_leader_id(leader_id)
		{

		}

		void add_data(const std::string& key, const std::string& value)
		{
			key_value_pair keyval(key, value);
			m_data.push_back(keyval);
		}

		const std::vector<key_value_pair>& data() const
		{
			return m_data;
		}

		int32_t leader() const
		{
			return m_leader_id;
		}

		int32_t id() const
		{
			return m_part_id;
		}

	private:
		std::vector<key_value_pair> m_data;
		int32_t m_part_id;
		int32_t m_leader_id;
	};

	/**
	 * Kafka topic with a name and a number of partitions
	 */
	class topic
	{
	public:
		topic(const std::string& n, const std::vector<partition>& parts):
			m_name(n),
			m_partitions()
		{
			for (size_t i=0; i<parts.size(); ++i)
			{
				m_partitions.push_back(parts[i]);
			}
		}

		const std::string& name() const
		{
			return m_name;
		}

		const std::vector<partition>& partitions() const
		{
			return m_partitions;
		}

		const partition* get_partition(size_t num) const
		{
			if (num >= m_partitions.size())
			{
				return NULL;
			}

			return &m_partitions[num];
		}

		partition* get_partition_writeable(size_t num)
		{
			if (num >= m_partitions.size())
			{
				return NULL;
			}

			return &m_partitions[num];
		}

	private:
		std::string m_name;
		std::vector<partition> m_partitions;
	};

	/**
	 * Broker Stub
	 *
	 * This stub implements the necessary function to parse the kafka wire
	 * protocol.
	 */
	class broker_stub
	{
	public:
		broker_stub(int32_t nodeId, const char* host, int32_t port):
			m_node_id(nodeId),
			m_topics(),
			m_brokers(),
			m_broker_ids()
		{
			m_broker_ids.push_back(nodeId);
			m_brokers.push_back(metadata::broker(nodeId, host, port));
		}

		/**
		 * Add topic to the broker stub
		 */
		bool add_topic(const std::string& name, const std::vector<partition>& partitions)
		{
			topic top(name, partitions);
			m_topics.push_back(top);
			return true;
		}

		/**
		 * Add reference to another broker
		 *
		 * This could e.g. be the address of another broker stub instance
		 */
	   bool add_broker_reference(int32_t nodeId, const char* host, int32_t port)
		{
			m_brokers.push_back(metadata::broker(nodeId, host, port));
			return true;
		}

		/**
		 * Get topic with specified name
		 */
		const topic* get_topic(const std::string& name) const
		{
			for (size_t i=0; i<m_topics.size(); i++)
			{
				if (m_topics[i].name() == name)
				{
					return &m_topics[i];
				}
			}
			return NULL;
		}

		/**
		 * Parse data and return number of bytes read
		 */
		int handle_data(const uint8_t* data, size_t total_size, std::vector<std::string>& responses)
		{
			// If message size is under 4 bytes we cannot parse anything
			if ((data == NULL) || (total_size < 4))
				return 0;

			// Keep track of end address of message
			const uint8_t* msg_end = data + total_size;
			const uint8_t* cur_data = data;

			// Loop over messages until we reach end
			int bytes_read = 0;
			while ((cur_data + 4) < msg_end)
			{
				// Read message size
				int32_t msg_size = util::read_type<int32_t>(cur_data);
				if (msg_size < 4)
				{
					printf("[KafkaBrokerStub][%i] Error message size < 4\n", m_node_id);
					return -1;
				}

				// Check if we have received enough bytes to parse message
				if ((cur_data + static_cast<size_t>(msg_size+4)) > msg_end)
				{
					// We need more of the input strem to parse this packet so return
					// and wait for next call
					return bytes_read;
				}

				// Dump msg
				// for (int32_t i=0; i<msg_size+4; i++)
				// {
				// 	printf("%02X ", cur_data[i]);
				// }
				// printf("\n");

				// Skip over message size
				cur_data += 4;

				// Prepare response buffer
				int response_size = 0;
				uint8_t response_buf[RESP_MAX_SIZE];
				memset(response_buf, 0, sizeof(response_buf));

				// Read api key and handle message accordingly
				int16_t api_key = util::read_type<int16_t>(cur_data);
				int16_t api_version = util::read_type<int16_t>(cur_data+2);
				switch (api_key)
				{
					case 0:
						response_size = handle_produce_request(cur_data, api_version, response_buf+4, RESP_MAX_SIZE-4);
						break;
					case 3:
						response_size = handle_metadata_request(cur_data, api_version, response_buf+4, RESP_MAX_SIZE-4);
						break;
					default:
						printf("[KafkaBrokerStub][%i] Got unknown API key [%i]\n", m_node_id, api_key);
						break;
				}

				if (response_size < 0)
				{
					printf("[KafkaBrokerStub][%i] Error during parsing [%i]\n", m_node_id, response_size);
					return -1;
				}

				// Write size in head of response buffer
				if (response_size > 0)
				{
					util::write_type<int32_t>(response_size, response_buf);
					responses.push_back(std::string(reinterpret_cast<const char*>(response_buf),
						                             static_cast<size_t>(response_size)+4));
				}

				// Update how many bytes we parsed
				bytes_read += msg_size + 4;

				// Jump to next message
				cur_data += msg_size;
			}

			return bytes_read;
		}

	private:

		int handle_metadata_request(const uint8_t* data, int16_t api_version, uint8_t* resp_buf,
			                         size_t resp_size)
		{
			// We only support metadata response version 0
			if (api_version != 0)
			{
				printf("[KafkaBrokerStub][%i] Received metadata request with unsupported API version [%i]\n",
					    m_node_id, api_version);
				return 0;
			}

			// Deserialize request
			metadata::request_v0 req;
			req.deserialize(data);
			printf("[KafkaBrokerStub][%i] Got metadata request from [%s] with corr. ID [%i]\n",
				    m_node_id, req.header().client_id().c_str(),
				    static_cast<int>(req.header().correlation_id()));

			// Prepare list of topics for response
			primitive::array<metadata::topic> topics;

			// Insert metadata in topic array - if array is empty all topics were requested
			if (req.topics().size() == 0)
			{
				printf("[KafkaBrokerStub][%i] - Request for all topics\n", m_node_id);
				for (size_t i=0; i<m_topics.size(); i++)
				{
					// Loop over partitions and generate metadata array
					primitive::array<metadata::partition> partitions;
					for (size_t k=0; k < m_topics[i].partitions().size(); ++k)
					{
						int32_t id = m_topics[i].partitions()[k].id();
						int32_t leader_id = m_topics[i].partitions()[k].leader();
						primitive::array<primitive::int32> replicas;
						replicas.push_back(leader_id);

						//Err code, Id, leader id, array of replicas, array of isr (in-sync replica set)
						partitions.push_back(metadata::partition(0, id, leader_id, replicas, replicas));
					}
					topics.push_back(metadata::topic(0, m_topics[i].name().c_str(), partitions));
				}
			}
			else
			{
				for (size_t i=0; i<req.topics().size(); i++)
				{
					printf("[KafkaBrokerStub][%i] - Request for topic [%s]\n",
						    m_node_id, req.topics()[i].c_str());
					topics.push_back(get_topic_metadata(req.topics()[i]));
				}
			}

			// Make response
			metadata::response_v0 resp(req.header().correlation_id(), m_brokers, topics);

			// Check response size
			size_t msg_size = resp.serial_size();
			if (msg_size > resp_size)
			{
				printf("[KafkaBrokerStub][%i] Response buffer too small\n", m_node_id);
				return 0;
			}

			// Serialize response into response buffer
			resp.serialize(resp_buf);
			return msg_size;
		}

		int handle_produce_request(const uint8_t* data, int16_t api_version, uint8_t* resp_buf, size_t resp_size)
		{
			// We only support produce in version 0
			if (api_version != 0)
			{
				printf("[KafkaBrokerStub][%i] Received produce request with unsupported API version [%i]\n",
					    m_node_id, api_version);
				return 0;
			}

			// Deserialize request
			produce::request_v0 req;
			req.deserialize(data);

			// Prepare topic result array for response
			primitive::array<produce::topic_result> topic_results;

			// Handle topic records
			for (size_t i=0; i<req.topic_records().size(); i++)
			{
				// Prepare partition result array for response
				primitive::array<produce::partition_result> partition_results;

				// Get topic record from array in request
				produce::topic_record topic_record = req.topic_records()[i];

				// Check if the requested topic exists
				topic* top = get_topic_writeable(topic_record.topic_name().c_str());
				if (top == NULL)
				{
					// ToDo: Pushback error to results
					continue;
				}

				// Pull partition records out of the topic record from the request
				// (we are looping over topic records)
				primitive::array<produce::partition_record> partition_records = topic_record.partition_records();
				for (size_t k=0; k<partition_records.size(); k++)
				{
					// For each partition record pull out the raw message with key value pair
					produce::partition_record record = partition_records[i];

					// Check if the requested partitions exists
					partition* part = top->get_partition_writeable(static_cast<size_t>(record.partition()));
					if (part == NULL)
					{
						// ToDo: Pushback error to results
						continue;
					}

					// Extract the produce message from the bytearray in the record
					primitive::bytearray raw_record = record.record();

					// The produce messages are concatenated in a special array
					const uint8_t* cur_msg_start = raw_record.data();
					const uint8_t* last_msg_end = cur_msg_start + raw_record.size();
					while (cur_msg_start < last_msg_end)
					{
						produce::message msg;
						cur_msg_start = msg.deserialize(cur_msg_start);

						// Write the message to our "database"
						part->add_data(msg.key().std_str(), msg.value().std_str());
					}

					// Append "success" result to partition result array
					partition_results.push_back(produce::partition_result(record.partition(), 0, 0));
				}

				// Append results to topic result array
				topic_results.push_back(produce::topic_result(topic_record.topic_name(), partition_results));
			}

			// Make response
			produce::response_v0 resp(req.header().correlation_id(), topic_results);

			// Check response size
			size_t msg_size = resp.serial_size();
			if (msg_size > resp_size)
			{
				printf("[KafkaBrokerStub][%i] Response buffer too small\n", m_node_id);
				return 0;
			}

			// Serialize response into response buffer
			resp.serialize(resp_buf);
			return msg_size;
		}

		metadata::topic get_topic_metadata(const primitive::string& name)
		{
			for (size_t i=0; i<m_topics.size(); i++)
			{
				if (m_topics[i].name() == name.std_str())
				{
					// Loop over partitions and generate metadata array
					primitive::array<metadata::partition> partitions;
					for (size_t k=0; k < m_topics[i].partitions().size(); ++k)
					{
						int32_t id = m_topics[i].partitions()[k].id();
						int32_t leader_id = m_topics[i].partitions()[k].leader();
						primitive::array<primitive::int32> replicas;
						replicas.push_back(leader_id);

						//Err code, Id, leader id, array of replicas, array of isr (in-sync replica set)
						partitions.push_back(metadata::partition(0, id, leader_id, replicas, replicas));
					}
					return metadata::topic(0, name.c_str(), partitions);
				}

			}

			// 3 = unknown topic or partition
			return metadata::topic(3, name.c_str(), primitive::array<metadata::partition>());
		}

		topic* get_topic_writeable(const std::string& name)
		{
			for (size_t i=0; i<m_topics.size(); i++)
			{
				if (m_topics[i].name() == name)
				{
					return &m_topics[i];
				}
			}
			return NULL;
		}

		int32_t m_node_id;
		std::vector<topic> m_topics;
		primitive::array<metadata::broker> m_brokers;
		primitive::array<primitive::int32> m_broker_ids;
	};

}

#endif
