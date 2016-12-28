#ifndef KAFKA_BROKER_STUB_METADATA_HPP_INC_
#define KAFKA_BROKER_STUB_METADATA_HPP_INC_

/*
 * Definitions used for handling metadata requests and responses.
 */

#include "primitive.hpp"
#include "headers.hpp"

namespace kafka_broker_stub { namespace metadata {

	/**
	 * Metadata request message
	 * Layout consists of a request header and an array of strings (topic names)
	 */
	class request_v0 : public kafka_elementI
	{
	public:
		request_v0():
			m_req_header(),
			m_topics()
		{

		}

		const uint8_t* deserialize(const uint8_t* data)
		{
			data = m_req_header.deserialize(data);
			data = m_topics.deserialize(data);
			return data;
		}

		const primitive::array<primitive::string>& topics() const
		{
			return m_topics;
		}

		const headers::request_hdr& header() const
		{
			return m_req_header;
		}

	private:
		headers::request_hdr m_req_header;
		primitive::array<primitive::string> m_topics;
	};

	typedef class request_v0 request_v1;
	typedef class request_v0 request_v2;

	/**
	 * Metadata information about a broker. Used for metadata response.
	 */
	class broker : public kafka_elementI
	{
	public:
		broker():
			m_node_id(),
			m_host(),
			m_port()
		{

		}

		broker(primitive::int32 node, const primitive::string& host, primitive::int32 port):
			m_node_id(node),
			m_host(host),
			m_port(port)
		{
		}

		uint8_t* serialize(uint8_t* data) const
		{
			data = m_node_id.serialize(data);
			data = m_host.serialize(data);
			data = m_port.serialize(data);
			return data;
		}

		size_t serial_size() const
		{
			size_t size = 0;
			size += m_node_id.serial_size();
			size += m_host.serial_size();
			size += m_port.serial_size();
			return size;
		}

	private:
		primitive::int32 m_node_id;
		primitive::string m_host;
		primitive::int32 m_port;
	};

	/**
	 * Metadata information about a partition. Used for metadata response.
	 */
	class partition : public kafka_elementI
	{
	public:
		partition():
			m_err_code(),
			m_id(),
			m_leader(),
			m_replicas(),
			m_isr()
		{

		}

		partition(primitive::int16 err_code, primitive::int32 id, primitive::int32 leader,
					 const primitive::array<primitive::int32>& replicas, const primitive::array<primitive::int32>& isr):
			m_err_code(err_code),
			m_id(id),
			m_leader(leader),
			m_replicas(replicas),
			m_isr(isr)
		{
		}

		uint8_t* serialize(uint8_t* data) const
		{
			data = m_err_code.serialize(data);
			data = m_id.serialize(data);
			data = m_leader.serialize(data);
			data = m_replicas.serialize(data);
			data = m_isr.serialize(data);
			return data;
		}

		size_t serial_size() const
		{
			size_t size = 0;
			size += m_err_code.serial_size();
			size += m_id.serial_size();
			size += m_leader.serial_size();
			size += m_replicas.serial_size();
			size += m_isr.serial_size();
			return size;
		}

	private:
		primitive::int16 m_err_code;
		primitive::int32 m_id;
		primitive::int32 m_leader;
		primitive::array<primitive::int32> m_replicas;
		primitive::array<primitive::int32> m_isr;
	};

	/**
	 * Metadata information about a topic. Used for metadata response.
	 */
	class topic : public kafka_elementI
	{
	public:
		topic():
			m_err_code(),
			m_name(),
			m_partitions()
		{

		}

		topic(primitive::int16 err_code, const primitive::string& tname, const primitive::array<partition>& partitions):
			m_err_code(err_code),
			m_name(tname),
			m_partitions(partitions)
		{
		}

		uint8_t* serialize(uint8_t* data) const
		{
			data = m_err_code.serialize(data);
			data = m_name.serialize(data);
			return m_partitions.serialize(data);
		}

		size_t serial_size() const
		{
			size_t size = 0;
			size += m_err_code.serial_size();
			size += m_name.serial_size();
			size += m_partitions.serial_size();
			return size;
		}

		const primitive::string& name() const
		{
			return m_name;
		}

	private:
		primitive::int16 m_err_code;
		primitive::string m_name;
		primitive::array<partition> m_partitions;
	};

	/**
	 * Metadata response message
	 * Layout consists of a response header, an array of brokers and an array of topics
	 */
	class response_v0 : public kafka_elementI
	{
	public:
		response_v0():
			m_resp_header(),
			m_brokers(),
			m_topics()
		{

		}

		response_v0(primitive::int32 corr_id, const primitive::array<broker>& brokers, const primitive::array<topic>& topics):
			m_resp_header(corr_id),
			m_brokers(brokers),
			m_topics(topics)
		{

		}

		uint8_t* serialize(uint8_t* data) const
		{
			data = m_resp_header.serialize(data);
			data = m_brokers.serialize(data);
			data = m_topics.serialize(data);
			return data;
		}

		size_t serial_size() const
		{
			size_t size = 0;
			size += m_resp_header.serial_size();
			size += m_brokers.serial_size();
			size += m_topics.serial_size();
			return size;
		}

	private:
		headers::response_hdr m_resp_header;
		primitive::array<broker> m_brokers;
		primitive::array<topic> m_topics;
	};

}}

#endif
