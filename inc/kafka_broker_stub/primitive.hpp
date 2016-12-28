#ifndef KAFKA_BROKER_STUB_PRIMITIVE_HPP_INC_
#define KAFKA_BROKER_STUB_PRIMITIVE_HPP_INC_

#include "util.hpp"
#include <string>
#include <vector>
#include <stdint.h>
#include <stdexcept>
#include <string.h>

/*
 * Class definitions for the Kafka primitives.
 *
 * Note that these primitives are used in all other files to build composites.
 * These composites adhere to same interface.
 */

namespace kafka_broker_stub {

	/**
	 * Interface for all kafka elements (primitives and composites)
	 */
	class kafka_elementI
	{
	public:
		virtual ~kafka_elementI() { };

		/**
		 * Must return pointer to adress following the last byte read
		 */
		virtual const uint8_t* deserialize(const uint8_t*)
		{
			throw std::logic_error("Deserialize() function not implemented");
		}

		/**
		 * Must return pointer to address following the last byte written
		 */
		virtual uint8_t* serialize(uint8_t*) const
		{
			throw std::logic_error("Serialize() function not implemented");
		}

		/**
		 * Must return the number of bytes written if element is serialized
		 */
		virtual size_t serial_size() const
		{
			throw std::logic_error("Serial_size() function not implemented");
		}

	};

	namespace primitive {

		/**
		 *	Kafka int8_t primitive. Stored as a basic int8_t.
		 */
		class int8 : public kafka_elementI
		{
		public:
			int8():
				m_value(0)
			{
			}

			int8(int8_t val):
				m_value(val)
			{
			}

			const uint8_t* deserialize(const uint8_t* data)
			{
				m_value = util::read_type<int8_t>(data);
				return data + sizeof(m_value);
			}

			uint8_t* serialize(uint8_t* data) const
			{
				util::write_type<int8_t>(m_value, data);
				return data + sizeof(m_value);
			}

			size_t serial_size() const
			{
				return sizeof(m_value);
			}

			operator int() const
			{
				return m_value;
			}

		private:
			int8_t m_value;
		};

		/**
		 *	Kafka int16_t primitive. Stored as a basic int16_t.
		 */
		class int16 : public kafka_elementI
		{
		public:
			int16():
				m_value(0)
			{
			}

			int16(int16_t val):
				m_value(val)
			{
			}

			const uint8_t* deserialize(const uint8_t* data)
			{
				m_value = util::read_type<int16_t>(data);
				return data + sizeof(m_value);
			}

			uint8_t* serialize(uint8_t* data) const
			{
				util::write_type<int16_t>(m_value, data);
				return data + sizeof(m_value);
			}

			size_t serial_size() const
			{
				return sizeof(m_value);
			}

			operator int() const
			{
				return m_value;
			}

		private:
			int16_t m_value;
		};

		/**
		 *	Kafka int32_t primitive. Stored as a basic int32_t.
		 */
		class int32 : public kafka_elementI
		{
		public:
			int32():
				m_value(0)
			{
			}

			int32(int32_t val):
				m_value(val)
			{
			}

			const uint8_t* deserialize(const uint8_t* data)
			{
				m_value = util::read_type<int32_t>(data);
				return data + sizeof(m_value);
			}

			uint8_t* serialize(uint8_t* data) const
			{
				util::write_type<int32_t>(m_value, data);
				return data + sizeof(m_value);
			}

			size_t serial_size() const
			{
				return sizeof(m_value);
			}

			operator int() const
			{
				return m_value;
			}

		private:
			int32_t m_value;
		};

		/**
		 *	Kafka int64_t primitive. Stored as a basic int64_t.
		 */
		class int64 : public kafka_elementI
		{
		public:
			int64():
				m_value(0)
			{
			}

			int64(int64_t val):
				m_value(val)
			{
			}

			const uint8_t* deserialize(const uint8_t* data)
			{
				m_value = util::read_type<int64_t>(data);
				return data + sizeof(m_value);
			}

			uint8_t* serialize(uint8_t* data) const
			{
				util::write_type<int64_t>(m_value, data);
				return data + sizeof(m_value);
			}

			size_t serial_size() const
			{
				return sizeof(m_value);
			}

			operator int() const
			{
				return m_value;
			}

			operator int64_t() const
			{
				return m_value;
			}
			
		private:
			int64_t m_value;
		};

		/**
		 *	Kafka string primitive. Stored as two bytes describing the length
		 * of the string followed by the raw bytes of the string (no zero termination).
		 */
		class string : public kafka_elementI
		{
		public:
			string(): m_value() { }

			string(const char* value):
				m_value(value)
			{
			}

			const uint8_t* deserialize(const uint8_t* data)
			{
				int16_t length = util::read_type<int16_t>(data);
				if (length > 0)
				{
					m_value.assign(reinterpret_cast<const char*>(data+2), static_cast<size_t>(length));
					return data + 2 + length;
				}
				else
				{
					m_value.clear();
				}
				
				return data + 2;
			}

			uint8_t* serialize(uint8_t* data) const
			{
				// Write string length
				int16 length(m_value.size());
				data = length.serialize(data);

				// Write content
				memcpy(data, m_value.data(), m_value.size());
				return data + m_value.size();
			}

			size_t serial_size() const
			{
				return size()+2;
			}

			/*
			 * A few functions for interacting with the class in a string-like way
			 */
			size_t size() const
			{
				return m_value.size();
			}

			const char* c_str() const
			{
				return m_value.c_str();
			}

			const std::string& std_str() const
			{
				return m_value;
			}

		private:
			std::string m_value;
		};

		/**
		 *	Kafka byte array primitive. Stored as four bytes describing the length
		 * of the array followed by the raw bytes.
		 */
		class bytearray : public kafka_elementI
		{
		public:
			bytearray(): m_value() { }

			const uint8_t* deserialize(const uint8_t* start)
			{
				int32_t length = util::read_type<int32_t>(start);
				if (length > 0)
				{
					m_value.assign(reinterpret_cast<const char*>(start+4), static_cast<size_t>(length));
					return start + 4 + length;
				}
				else
				{
					m_value.clear();
				}
				
				return start + 4;
			}

			uint8_t* serialize(uint8_t* dest) const
			{
				// Write byte length
				int32 length(m_value.size());
				dest = length.serialize(dest);

				// Write content
				memcpy(dest, m_value.data(), m_value.size());
				return dest + m_value.size();
			}

			size_t serial_size() const
			{
				return size() + 4;
			}

			/*
			 * A few functions to interact with the class in a array-like way
			 */
			size_t size() const
			{
				return m_value.size();
			}

			uint8_t operator[] (size_t x) const
			{
			   return static_cast<uint8_t>(m_value[x]);
			}

			const uint8_t* data() const
			{
				return reinterpret_cast<const uint8_t*>(m_value.data());
			}

			const std::string& std_str() const
			{
				return m_value;
			}

		private:
			std::string m_value;
		};

		/**
		 *	Kafka array primitive. Stored as four bytes describing the number of
		 * elements in the array followed by the elements.
		 */
		template <typename T>
		class array : public kafka_elementI
		{
		public:
			array(): m_value() { }

			const uint8_t* deserialize(const uint8_t* data)
			{
				m_value.clear();
				
				// Read the length of the array
				int32_t length = util::read_type<int32_t>(data);

				// Move to array start
				data += 4;

				// Loop over array
				for (int32_t i=0; i<length; i++)
				{
					T arrElement;
					data = arrElement.deserialize(data);
					m_value.push_back(arrElement);
				}

				return data;
			}

			uint8_t* serialize(uint8_t* data) const
			{
				// Write array length
				int32 length(m_value.size());
				data = length.serialize(data);

				// Write array content
				size_t arr_size = m_value.size();
				for (size_t i=0; i<arr_size; ++i)
				{
					data = m_value[i].serialize(data);
				}
				return data;
			}

			size_t serial_size() const
			{
				size_t arr_size = 4; // Empty array
				if (m_value.size() > 0)
				{
					arr_size += (m_value[0].serial_size() * m_value.size());
				}
				return arr_size;
			}

			/*
			 * A few function to interact with the class in an array-like way
			 */
			size_t size() const
			{
				return m_value.size();
			}

			const T& operator[] (size_t x) const
			{
				 return m_value[x];
			}

			void push_back(const T& val)
			{
				m_value.push_back(val);
			}

		private:
			std::vector<T> m_value;
		};

	}
}

#endif
