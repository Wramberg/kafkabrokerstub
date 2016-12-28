#ifndef KAFKA_BROKER_STUB_HEADERS_HPP_INC_
#define KAFKA_BROKER_STUB_HEADERS_HPP_INC_

#include "primitive.hpp"

namespace kafka_broker_stub { namespace headers {

	class request_hdr : public kafka_elementI
	{
	public:
	   request_hdr():
			m_api_key(0),
			m_api_version(0),
			m_correlation_id(0),
			m_client_id()
		{

		}

		const uint8_t* deserialize(const uint8_t* data)
		{
			data = m_api_key.deserialize(data);
			data = m_api_version.deserialize(data);
			data = m_correlation_id.deserialize(data);
			data = m_client_id.deserialize(data);
			return data;
		}

		primitive::int16 api_key() const
		{
			return m_api_key;
		}

		primitive::int16 api_version() const
		{
			return m_api_version;
		}

		primitive::int32 correlation_id() const
		{
			return m_correlation_id;
		}

		const primitive::string& client_id() const
		{
			return m_client_id;
		}

	private:
		primitive::int16 m_api_key;
		primitive::int16 m_api_version;
		primitive::int32 m_correlation_id;
		primitive::string m_client_id;
	};

	class response_hdr : public kafka_elementI
	{
	public:
		response_hdr():
			m_correlation_id(0)
		{

		}

	   response_hdr(primitive::int32 correlation_id):
			m_correlation_id(correlation_id)
		{

		}

		uint8_t* serialize(uint8_t* data) const
		{
			return m_correlation_id.serialize(data);
		}

		size_t serial_size() const
		{
			return m_correlation_id.serial_size();
		}

	private:
		primitive::int32 m_correlation_id;
	};

}}

#endif
