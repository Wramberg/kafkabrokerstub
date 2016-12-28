#ifndef KAFKA_BROKER_STUB_PRODUCE_HPP_INC_
#define KAFKA_BROKER_STUB_PRODUCE_HPP_INC_

/**
 * Definitions used for handling produce requests and responses.
 */

#include "primitive.hpp"
#include "headers.hpp"

namespace kafka_broker_stub { namespace produce {

	class message : public kafka_elementI
	{
	public:
		message():
			m_offset(),
			m_message_size(),
			m_crc(),
			m_magicbyte(),
			m_attributes(),
			m_key(),
			m_value()
		{

		}

		const uint8_t* deserialize(const uint8_t* data)
		{
			data = m_offset.deserialize(data);
			data = m_message_size.deserialize(data);
			data = m_crc.deserialize(data);
			data = m_magicbyte.deserialize(data);
			data = m_attributes.deserialize(data);
			data = m_key.deserialize(data);
			data = m_value.deserialize(data);
			return data;
		}

		primitive::int64 offset() const
		{
			return m_offset;
		}

		primitive::int32 message_size() const
		{
			return m_message_size;
		}

		primitive::int32 crc() const
		{
			return m_crc;
		}

		primitive::int8 magicbyte() const
		{
			return m_magicbyte;
		}

		primitive::int8 attributes() const
		{
			return m_attributes;
		}

		const primitive::bytearray& key() const
		{
			return m_key;
		}

		const primitive::bytearray& value() const
		{
			return m_value;
		}

	private:
		primitive::int64 m_offset;
		primitive::int32 m_message_size;
		primitive::int32 m_crc;
		primitive::int8 m_magicbyte;
		primitive::int8 m_attributes;
		primitive::bytearray m_key;
		primitive::bytearray m_value;
	};

	class partition_record : public kafka_elementI
	{
	public:
		partition_record():
			m_partition(),
			m_record()
		{

		}

		const uint8_t* deserialize(const uint8_t* data)
		{
			data = m_partition.deserialize(data);
			data = m_record.deserialize(data);
			return data;
		}

		const primitive::int32& partition() const
		{
			return m_partition;
		}

		const primitive::bytearray& record() const
		{
			return m_record;
		}

	private:
		primitive::int32 m_partition;
		primitive::bytearray m_record;
	};

	class topic_record : public kafka_elementI
	{
	public:
		topic_record():
			m_topic_name(),
			m_partition_records()
		{

		}

		const uint8_t* deserialize(const uint8_t* data)
		{
			data = m_topic_name.deserialize(data);
			return m_partition_records.deserialize(data);
		}

		const primitive::string& topic_name() const
		{
			return m_topic_name;
		}

		const primitive::array<partition_record>& partition_records() const
		{
			return m_partition_records;
		}

	private:
		primitive::string m_topic_name;
		primitive::array<partition_record> m_partition_records;
	};

	class request_v0 : public kafka_elementI
	{
	public:
		request_v0():
			m_req_header(),
			m_acks(),
			m_timeout(),
			m_topic_records()
		{

		}

		const uint8_t* deserialize(const uint8_t* data)
		{
			data = m_req_header.deserialize(data);
			data = m_acks.deserialize(data);
			data = m_timeout.deserialize(data);
			data = m_topic_records.deserialize(data);
			return data;
		}

		const headers::request_hdr& header() const
		{
			return m_req_header;
		}

		primitive::int16 acks() const
		{
			return m_acks;
		}

		primitive::int32 timeout() const
		{
			return m_timeout;
		}

		const primitive::array<topic_record>& topic_records() const
		{
			return m_topic_records;
		}

	private:
		headers::request_hdr m_req_header;
		primitive::int16 m_acks;
		primitive::int32 m_timeout;
		primitive::array<topic_record> m_topic_records;
	};

	class partition_result : public kafka_elementI
	{
	public:
		partition_result():
			m_partition(),
			m_err_code(),
			m_offset()
		{

		}

		partition_result(primitive::int32 partition, primitive::int16 err_code, primitive::int64 offset):
			m_partition(partition),
			m_err_code(err_code),
			m_offset(offset)
		{

		}

		uint8_t* serialize(uint8_t* data) const
		{
			data = m_partition.serialize(data);
			data = m_err_code.serialize(data);
			data = m_offset.serialize(data);
			return data;
		}

		size_t serial_size() const
		{
			size_t size = 0;
			size += m_partition.serial_size();
			size += m_err_code.serial_size();
			size += m_offset.serial_size();
			return size;
		}

	private:
		primitive::int32 m_partition;
		primitive::int16 m_err_code;
		primitive::int64 m_offset;
	};

	class topic_result : public kafka_elementI
	{
	public:
		topic_result():
			m_topic_name(),
			m_part_results()
		{

		}

		topic_result(primitive::string topic, const primitive::array<partition_result>& part_results):
			m_topic_name(topic),
			m_part_results(part_results)
		{

		}

		uint8_t* serialize(uint8_t* data) const
		{
			data = m_topic_name.serialize(data);
			data = m_part_results.serialize(data);
			return data;
		}

		size_t serial_size() const
		{
			size_t size = 0;
			size += m_topic_name.serial_size();
			size += m_part_results.serial_size();
			return size;
		}

	private:
		primitive::string m_topic_name;
		primitive::array<partition_result> m_part_results;
	};

	class response_v0 : public kafka_elementI
	{
	public:
		response_v0(primitive::int32 corr_id, const primitive::array<topic_result>& topic_results):
			m_resp_header(corr_id),
			m_topic_results(topic_results)
			// m_throttle_time(0)
		{

		}

		uint8_t* serialize(uint8_t* data) const
		{
			data = m_resp_header.serialize(data);
			data = m_topic_results.serialize(data);
			// data = m_throttle_time.serialize(data);
			return data;
		}

		size_t serial_size() const
		{
			size_t size = 0;
			size += m_resp_header.serial_size();
			size += m_topic_results.serial_size();
			// size += m_throttle_time.serial_size();
			return size;
		}

	private:
		headers::response_hdr m_resp_header;
		primitive::array<topic_result> m_topic_results;
		// primitive::int32 m_throttle_time;
	};

}}

#endif
