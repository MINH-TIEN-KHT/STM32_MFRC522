#ifndef DS1307_H_
#define DS1307_H_

#include "stm32f10x.h"

//  DS1307 register addresses
#define DS1307_SECONDS  0x00
#define DS1307_MINUTES  0x01
#define DS1307_HOURS    0x02
#define DS1307_DAY      0x03
#define DS1307_DATE     0x04
#define DS1307_MONTH    0x05
#define DS1307_YEAR     0x06
#define DS1307_CONTROL  0x07

#define DS1307_CONTROL_OUT  (1<<7)
#define DS1307_CONTROL_SQWE (1<<4)
#define DS1307_CONTROL_RS1  (1<<1)
#define DS1307_CONTROL_RS0  (1<<0)
#define DS1307_CH	(1<<7)

#define DS1307_ADDRESS  0xD0    //  the device address
#define I2C_ACK_ATTEMPTS 10000

typedef enum {
	MeridianAM,
	MeridianPM,
	MeridianMilitary
} Meridian;

/** \struct DS1307
 *  \brief  Defines data structure for date/time
 */
typedef struct {
    uint8_t second;
    uint8_t minute;
    uint8_t hour;
    uint8_t day;
    uint8_t date;
    uint8_t month;
    uint8_t year;
	uint8_t control;
    Meridian meridian;
} DS1307Date;


uint8_t bcd_2_dec(uint8_t bcd);
uint8_t dec_2_bcd(uint8_t dec);
void ds1307_firstTimeSetting(void);
uint8_t ds1307_writeSingleData(uint8_t deviceID, uint8_t RegAddr, uint8_t data);
uint8_t ds1307_writeBlockData(uint8_t deviceID, uint8_t RegAddr, uint8_t *data, uint8_t len);
uint8_t ds1307_readSingleData(uint8_t deviceID, uint8_t RegAddr);
uint8_t ds1307_readBlockData(uint8_t deviceID, uint8_t RegAddr, uint8_t *data, uint8_t len);

#endif /* DS1307_H_ */
