#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "crc16.h"

namespace Tinker_Crc16 {

	uint16_t crcValue;

	void init()
	{
		crcValue =0;  
	}

	void init( uint16_t initialValue )
	{
		crcValue = initialValue;
	}

	void add( uint16_t const * data, int count )
	{
		while(count--)
		{
			crcValue += *data++;
		}
	}

	void addOne( uint16_t v )
	{
		crcValue += v;
	}

	void finalize()
	{
		crcValue = (0xFFFF - crcValue + 1);
	}

	uint16_t get()
	{
		return crcValue;
	}
}
