#ifndef TEST_COMMON_HPP_INC_
#define TEST_COMMON_HPP_INC_

#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <iostream>

#define ASSERT_EQ(a, b)\
   do \
   { \
      this->assert_equal(a, b, __FILE__, __LINE__);\
   } while(0)

#define ASSERT_NEQ(a, b)\
   do \
   { \
      this->assert_not_equal(a, b, __FILE__, __LINE__);\
   } while(0)

namespace kafka_broker_stub { namespace test {

	class suite
	{
	public:
		suite(const std::string& name):
			m_name(name),
			m_num_pass(0),
			m_num_fail(0)
		{

		}

		virtual ~suite() { }

		void execute_tests()
		{
			printf("\n------ [%s] ------\n", m_name.c_str());
			tests();
			printf("Passed: [%lu] Failed: [%lu]\n\n", m_num_pass, m_num_fail);
			if (m_num_fail > 0)
			{
				exit(EXIT_FAILURE);
			}
		}

	protected:
		template <typename T>
		void assert_equal(const T& a, const T& b, const char* file, int line)
		{
			if (a != b)
			{
				std::cout << "Assert equal failed [" << file << ":" << line << "] ";
				std::cout << "[" << a << " == " << b << "]\n";
				m_num_fail++;
				return;
			}

			m_num_pass++;
		}

		void assert_equal(uint8_t* a, uint8_t* b, const char* file, int line)
		{
			if (a != b)
			{
				std::cout << "Assert equal failed [" << file << ":" << line << "] ";
				std::cout << "[" << reinterpret_cast<const size_t>(a) << " == ";
				std::cout << reinterpret_cast<const size_t>(b) << "]\n";
				m_num_fail++;
				return;
			}

			m_num_pass++;
		}

		template <typename T>
		void assert_not_equal(const T& a, const T& b, const char* file, int line)
		{
			if (a == b)
			{
				std::cout << "Assert equal failed [" << file << ":" << line << "] ";
				std::cout << "[" << a << " != " << b << "]\n";
				m_num_fail++;
				return;
			}

			m_num_pass++;
		}

	private:
		virtual void tests() = 0;

		std::string m_name;
		size_t m_num_pass;
		size_t m_num_fail;
	};

}}

#endif
