#ifndef KAFKA_BROKER_STUB_UTIL_HPP_INC_
#define KAFKA_BROKER_STUB_UTIL_HPP_INC_

#include <stdint.h>
#include <stddef.h>
#include <stdexcept>

namespace kafka_broker_stub { namespace util {

	/**
	 * Helper function template for byte swapping
	 */
	template <typename T>
   inline T byte_swap(const T* p)
	{
		if (!p)
			throw std::runtime_error("Invalid input NULL to byteswap");

		union ret_value { T value; uint8_t raw[sizeof(T)]; };
		union ret_value result;
		result.value = (*p);

		size_t lo = 0;
		size_t hi = sizeof(T)-1;
      for (; hi > lo; lo++, hi--)
      {
			uint8_t tmp = result.raw[lo];
			result.raw[lo] = result.raw[hi];
			result.raw[hi] = tmp;
      }

		return result.value;
   }

	/**
	 * Helper function templates for reading integral types from raw bytes
	 */
 	template <typename T>
	inline T read_type(const uint8_t* data)
	{
	   const T* ptr = reinterpret_cast<const T*>(data);
	   return byte_swap(ptr);
	}

	/**
	 * Helper function templates for writing integral types to raw bytes
	 */
	template <typename T>
	inline void write_type(const T& val, uint8_t* data)
	{
	   T* tmp = reinterpret_cast<T*>(data);
	   (*tmp) = byte_swap(&val);
	}

}}

#endif
