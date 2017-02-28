#pragma once

#include <iostream>
#include <iomanip>
#include <cmath>

namespace half_float {
	class half;

	int floatToBits(const float &_rhs)
	{
		union {
			float a;
			int b;
		} convert;
		convert.a = _rhs;

		return convert.b;
	}

	class half
	{
	public:
		///
		/// \brief half Default ctor, initialise to 0.0
		///
		half() : m_data(0x0000) {}

		half(const float &_rhs) :
			m_data(0x0000)
		{
			// Floating point 4 bytes:
			//		0x00000000
			//		0			00000000	00000000000000000000000
			//  SIGN		EXPONENT				MANTISSA
			//  To extract the exponent part:
			//		01111111 10000000 00000000 00000000
			//	0x	 7F				80			 00				00

			// Half float 2 bytes:
			//	 0x0000
			//	 0		 00000		0000000000
			//	SIGN	EXPONENT	 MANTISSA
			//
			//	Exponent part:
			//		01111100 00000000
			//	0x	7B				00

			int bitData = floatToBits(_rhs);
			// Exponent part
			m_data |= ((bitData & 0x7f800000) >> 16) & 0x7b00;

			// Mantissa part
			m_data += ((bitData & 0x007fffff) >> 16) & 0x03ff;
			std::cout << "0x" << std::setfill('0') << std::hex << std::setw(4) << m_data << "\n";
		}

		half operator=(const float &_rhs) { return half(_rhs); }

		uint16_t data() const { return m_data; }

		friend std::ostream& operator<<(std::ostream &_os, const half &_val)
		{
			float mantissa = 0.f;
			float exponent = std::pow(2, (_val.data() >> 8) - 15);
			std::cout << std::dec << (_val.data() >> 8) << "\n";
			_os << (1 + mantissa)*exponent << "\n";
		}

	private:
		///
		/// \brief m_data
		///
		uint16_t m_data;
	};
}
