#pragma once

#include <iostream>
#include <iomanip>
#include <cmath>

//#define DEBUG

namespace half_float
{
	class half;

  unsigned int floatToBits(const float &_rhs)
	{
		union {
			float a;
			unsigned int b;
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
			//		01111111 1000000 00000000 00000000
			//	0x	 7F				80			 00			00
			//  To extract the mantissa part:
			//		00000000 01111111 11111111 11111111
			//	0x	 00				7F			 FF				FF

			// Half float 2 bytes:
			//	 0x0000
			//	 0		 00000		0000000000
			//	SIGN	EXPONENT	 MANTISSA
			//
			//	Exponent part:
			//		01111100 00000000
			//	0x	7C				00
			//	Mantissa part:
			//		00000011 11111111
			//	0x   03				ff

			unsigned int bitData = floatToBits(_rhs);

			// Extract sign, exponent and mantissa
			uint16_t sign = (bitData >> 16) & 0x8000;
			int32_t exponent = (bitData & 0x7f800000) >> 23;
			int32_t mantissa = (bitData & 0x007fffff);

#ifdef DEBUG
			std::cout << "Sign: " << std::bitset<1>(sign) << "\n";
			std::cout << "Exponent: " << std::bitset<8>(exponent) << "\n";
			std::cout << "Mantissa: " << std::bitset<24>(mantissa) << "\n";
#endif

			if(exponent)
			{
				// If the exponent is not all zeroes, add 2^23 to mantissa
				mantissa += std::pow(2, 23);
			}
			else
			{
				// Else add 1
				mantissa += 1;
			}

			// Subtract the float bias (127) and add half bias (15)
			exponent = exponent - 0x7f + 0xf;

			if(exponent < -11)
			{
				m_data |= sign << 15;
				return;
			}

			/*
			* If the exponent field is less than 1:
			*  Set a temporary value to the significand shifted left by a number of bits equal to the 1 minus the exponent field.
			*  Shift the significand right that number of bits plus 13.
			*  If the temporary value exceeds 2^23, add one to the significand field (to round up).
			*  If that addition does not increase the significand to 2^10,
			*		set the exponent to zero and return the subnormal number (or zero) formed from the sign bit, the zero exponent, and the significand.
			*  If the significand did increase to 2^10, return the number formed from the sign bit, an exponent field of one, and a zero significand.
			*/
			if(exponent < 0)
			{
				uint32_t tmp = mantissa;
				tmp <<= 1 - exponent;
				mantissa >>= 13 + (1 - exponent);
				if(tmp > std::pow(2, 23))
					mantissa += 1;
				if(mantissa < std::pow(2, 10))
				{
					exponent = 0;
					m_data = sign | (exponent << 10) | (mantissa & 0x03ff);
					return;
				}
				exponent = 1;
				mantissa = 0;
				m_data = sign | (exponent << 10) | (mantissa & 0x03ff);
			}

			// If the low 13 bits of the significand exceed 2^12 or they equal 2^12 and the next higher bit is 1, add 2^13 to the significand.
			// Shift the significand right 13 bits. If it is not less than 2^11, add one to the exponent and shift the significand right one bit.
			int a = std::pow(2, 12);
			if((mantissa & 0x00001fff) >= a)
			{
				mantissa += a;
			}

			mantissa >>= 13;
			if(mantissa > std::pow(2, 11))
			{
				exponent += 1;
				mantissa >>= 1;
			}
			if(exponent > 30)
			{
				m_data = sign + 0x7c00;
				return;
			}

			m_data = sign | (exponent << 10) | (mantissa & 0x03ff);
		}

		half operator=(const float &_rhs) { return half(_rhs); }

		inline float operator/(const float &_val)
		{
			return asFloat(*this) / _val;
		}

		uint16_t data() const { return m_data; }

		static inline float asFloat(const half &_h)
		{
			float exponent = std::powf(2, ((_h.data() & 0x7c00) >> 10) - 15);
			uint16_t mantissa = (_h.data() & 0x03ff) << 6;
			float m = 0.f;
			for(int i = 0; i < 10; ++i)
			{
				if((mantissa & 0x8000) >> 15)
					m += std::powf(2.f, -(i+1));
				mantissa <<= 1;
			}

			return std::pow(-1, ((_h.data() & 0x8000) >> 15)) * (1 + m)*exponent;
		}

		friend std::ostream& operator<<(std::ostream &_os, const half &_val)
		{
			_os << asFloat(_val);
			return _os;
		}

	private:
		///
		/// \brief m_data
		///
		uint16_t m_data;
	};
}
