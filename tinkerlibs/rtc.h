#ifndef RTC_H
#define RTC_H

#include <assert.h>
#include <stdint.h>
#include <map>
#include <vector>
#include <list>
#include <string.h>
#include <errno.h>
#include "buspirate.h"
#include "i2c.h"
#include <QDebug>

#define RTC_STAT_BIT_OSF 0x80
#define _DS3231_ADDR_PREFIX 0xD0
#define _DS3231_ADDRESS 0x68

typedef struct DateTime
{
    uint8_t seg;
    uint8_t min;
    uint8_t hrs;
    uint8_t day;
    uint8_t date;
    uint8_t month;
    uint16_t year;
    uint8_t control;
    uint8_t status;
} DateTime;

using namespace std;

namespace Tinker_RTC
{
    bool            getTime( DateTime & getDate );
    bool             setTime( DateTime newDate );
    int             ds3231_read(BP *, unsigned char, unsigned char,unsigned char *);
    int             ds3231_write(BP *, unsigned char, unsigned char, unsigned char);
    unsigned int    bcdTodec (unsigned int);
    unsigned int    decTobcd (unsigned int);

    uint8_t         bcd2bin(uint8_t val);
    uint8_t         bin2bcd(uint8_t val);
} 

#endif // RTC_H
