#ifndef TINKERINTERFACE_H
#define TINKERINTERFACE_H

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <time.h>
#include <stdint.h>
#include <map>
#include <vector>
#include <list>
#include <string.h>
#include <errno.h>
#include <rtc.h>

/*typedef struct DateTime
{
    uint8_t seg;
    uint8_t min;
    uint8_t hrs;
    uint8_t day;
    uint8_t month;
    uint8_t year;
} DateTime;*/

struct SystemState
{
    uint64_t 	in;
    uint64_t	out;
    uint32_t 	machineId;
    uint32_t 	gameId;
    uint8_t 	file10[64];    
    uint32_t 	period;
    uint32_t    validations;
    uint32_t    config;
    uint64_t    keyPass;
    DateTime 	timeStamp;
    DateTime 	tinkerDate;    

}__attribute__((packed));

/*
struct SystemStateOld
{
    uint64_t 	in;
    uint64_t	out;
    uint32_t	lockBits;
    uint64_t 	timeStamp;
    uint64_t 	timeValue;
    uint32_t 	MachineId;
    uint64_t 	Hash;
    uint32_t	gameId;

}__attribute__((packed));
*/
#endif // TINKERINTERFACE_H
