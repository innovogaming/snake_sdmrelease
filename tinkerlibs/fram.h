#ifndef FRAM_H
#define FRAM_H

#define _FRAM_WRITE_ENABLE	0X06
#define _FRAM_WRITE_DISABLE	0X04
#define _FRAM_READ_STATUS	0X05
#define _FRAM_WRITE_STATUS	0X01
#define _FRAM_READ_MEMORY	0X03
#define _FRAM_WRITE_MEMORY	0X02

#define _FRAM_BLOCK_NONE		0X00
#define _FRAM_BLOCK_UP_QUARTER	0X01
#define _FRAM_BLOCK_UP_HALF		0X02
#define _FRAM_BLOCK_ALL			0X03

#include "buspirate.h"
#include <spi.h>

namespace Tinker_Fram {
	
	enum
	{
		Size = 32768 // The total size of FRAM in bytes
	};

	enum
	{
		CS0 = 6, // The total size of FRAM in bytes
		AUX1 = 27,
		AUX2 = 28,
		AUX3 = 29,
		//CS1 = 7 // The total size of FRAM in bytes
	};

	enum BlockProtection // Describes which part of FRAM is to be write-protected
	{
		None = 0,
		FourthQuarter = 1,
		SecondHalf = 2,
		All = 3
	};
	void error(const char *);
	int fram_open(char * device);
	int fram_close(void);
	int fram_read_status(uint8_t *);
	int fram_write_status(uint8_t );
	//int fram_read(unsigned, uint8_t *, unsigned);
	int fram_read(unsigned, void *, unsigned);
	//int fram_write(unsigned, uint8_t *, unsigned);
	int fram_write(unsigned, void const *, unsigned);
	int fram_format(void);
	int fram_is_present(void);
	int fram_write_enable(void);
	int fram_write_disable(void);
	int fram_setBlockProtection( BlockProtection b );
} 
#endif // FRAM_H
