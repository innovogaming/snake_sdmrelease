#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "rtc.h"

namespace Tinker_RTC
{

    /**
     * @brief WriteTinkerForm::getTime
     * @param bp
     * @param dev_addr
     * @return
     */
    bool getTime( DateTime & getDate )
    {
        bool statusEnd = true;

        BP * bp;
        unsigned char version;
        unsigned char dev_addr = _DS3231_ADDRESS;

        bp= bp_open("/dev/ttyUSB0");
        if (bp == NULL)
        {
            //QMessageBox::warning(this, "Error", "No se puede conectar a Bus Pirate.");
            return false;
        }

        bp_firmware_version_high(bp);
        bp_firmware_version_low(bp);

        /* switch to binary mode */
        if (bp_bin_init(bp, &version) != BP_SUCCESS)
        {
            //error("No puedo cambiar a modo binario");
            //QMessageBox::warning(this, "Error", "No puedo cambiar a modo binario");
            return false;
        }

        usleep(100);

        /* select I2C binary mode */
        if (bp_bin_mode_i2c(bp, &version) != BP_SUCCESS)
        {
            //error("No puedo inicializar modo I2C");
            //QMessageBox::warning(this, "Error", "No puedo inicializar modo I2C");
            return false;
        }

        usleep(100);

        //Es muy importante esta velocidad ya que al usar cable plano velocidades superiores da error
        /*if (bp_bin_i2c_set_speed(bp, BP_BIN_I2C_SPEED_5K) < 0)
        {
            return false;
        }

        usleep(100);*/

        if (bp_bin_i2c_set_periph(bp, (BP_BIN_I2C_PERIPH_POWER)) < 0)
        {
            return false;
        }

        usleep(100);

        DateTime resultTime;
        unsigned char dataTime = 0;

        usleep(100);        

        ds3231_read(bp, dev_addr, 0, &dataTime);
        resultTime.seg = bcd2bin(dataTime & 0x7F);

        //qDebug() << "Segundos, bdc: " << bcdTodec(dataTime) << ", " << dataTime;

        ds3231_read(bp, dev_addr, 0, &dataTime);
        resultTime.seg = bcd2bin(dataTime & 0x7F);

        //qDebug() << "Segundos, bdc: " << bcdTodec(dataTime) << ", " << dataTime;

        ds3231_read(bp, dev_addr, 1, &dataTime);
        resultTime.min =  bcd2bin(dataTime & 0x7F);

        ds3231_read(bp, dev_addr, 2, &dataTime);
        resultTime.hrs = bcd2bin(dataTime & 0x3F);

        //qDebug() << "Horas, bdc: " << bcdTodec(dataTime) << ", " << dataTime;

        ds3231_read(bp, dev_addr, 3, &dataTime);
        resultTime.date = bcd2bin((dataTime -  1) & 0x7F);

        ds3231_read(bp, dev_addr, 4, &dataTime);
        resultTime.day = bcd2bin(dataTime & 0x7F);

        ds3231_read(bp, dev_addr, 5, &dataTime);
        resultTime.month =  bcd2bin(dataTime & 0x1F) - 1;

        ds3231_read(bp, dev_addr, 6, &dataTime);
        resultTime.year = bcd2bin(dataTime) + 100;

        ds3231_read(bp, dev_addr, 0x0F, &dataTime);
        resultTime.status = dataTime;

        if( resultTime.status & RTC_STAT_BIT_OSF )
        {
            qDebug() << "### Warning: RTC oscillator has stopped";
            ds3231_write(bp, dev_addr, 0x0F, resultTime.status & ~RTC_STAT_BIT_OSF);
            statusEnd = false;
        }

        getDate = resultTime;

        bp_close(bp);

        return statusEnd;
    }

    /**
     * @brief WriteTinkerForm::setTime
     * @param bp
     * @param dev_addr
     * @param newDate
     * @return
     */
    bool setTime(DateTime newDate)
    {

        int resultTime = true;

        BP * bp;
        unsigned char version;
        unsigned char dev_addr = _DS3231_ADDRESS;

        bp= bp_open("/dev/ttyUSB0");
        if (!bp)
        {
            //QMessageBox::warning(this, "Error", "No se puede conectar a Bus Pirate.");
            return false;
        }

        bp_firmware_version_high(bp);
        bp_firmware_version_low(bp);


        if (bp_bin_init(bp, &version) != BP_SUCCESS)
        {
            //error("No puedo cambiar a modo binario");
            //QMessageBox::warning(this, "Error", "No puedo cambiar a modo binario");
            return false;
        }

        usleep(100);


        if (bp_bin_mode_i2c(bp, &version) != BP_SUCCESS)
        {
            //error("No puedo inicializar modo I2C");
            //QMessageBox::warning(this, "Error", "No puedo inicializar modo I2C");
            return false;
        }

        usleep(100);

        //Es muy importante esta velocidad ya que al usar cable plano velocidades superiores da error
        if (bp_bin_i2c_set_speed(bp, BP_BIN_I2C_SPEED_5K) < 0)
        {
            return false;
        }

        usleep(100);

        if (bp_bin_i2c_set_periph(bp, (BP_BIN_I2C_PERIPH_POWER)) < 0)
        {
            return false;
        }

        usleep(100);

        /*if ( newDate.seg > 59 || newDate.min > 59 || newDate.hrs > 23  )
        {
            qDebug("ERROR SET DATETIME seg:%d, min:%d, hrs:%d", newDate.seg, newDate.min, newDate.hrs);
            resultTime = false;
        }*/

        uint8_t century;
        century = (newDate.year >= 2000) ? 0x80 : 0;

        newDate.month = bin2bcd( newDate.month ) + 1;
        newDate.seg = bin2bcd(newDate.seg);
        newDate.min = bin2bcd(newDate.min);
        newDate.hrs = bin2bcd(newDate.hrs);
        newDate.day = bin2bcd(newDate.day);
        newDate.year = bin2bcd(newDate.year % 100);
        newDate.date = bin2bcd(newDate.date + 1);

        ds3231_write(bp, dev_addr, 0, newDate.seg);
        ds3231_write(bp, dev_addr, 1, newDate.min);
        ds3231_write(bp, dev_addr, 2, newDate.hrs);
        ds3231_write(bp, dev_addr, 3, newDate.date);
        ds3231_write(bp, dev_addr, 4, newDate.day);
        ds3231_write(bp, dev_addr, 5, newDate.month | century);
        ds3231_write(bp, dev_addr, 6, newDate.year);

        bp_close( bp );

        return resultTime;
    }

    /**
     * @brief WriteTinkerForm::ds3231_read
     * @param bp
     * @param dev_addr
     * @param reg_addr
     * @param reg_value
     * @return
     */
    int ds3231_read(BP * bp, unsigned char dev_addr, unsigned char reg_addr, unsigned char * reg_value)
    {
        unsigned char ack;
        assert(dev_addr <= 128);

        if (bp_bin_i2c_start(bp) < 0)
        {
            return -1;
        }
        if (bp_bin_i2c_write(bp, (dev_addr << 1), &ack) < 0)
        {
            return -1;
        }
        if (ack != BP_BIN_I2C_ACK)
        {
            return -1;
        }
        if (bp_bin_i2c_write(bp, reg_addr, &ack) < 0)
        {
            return -1;
        }
        if (ack != BP_BIN_I2C_ACK)
        {
            return -1;
        }
        if (bp_bin_i2c_start(bp) < 0)
        {
            return -1;
        }
        if (bp_bin_i2c_write(bp, (dev_addr << 1)| 0x01, &ack) < 0)
        {
            return -1;
        }
        if (ack != BP_BIN_I2C_ACK)
        {
            return -1;
        }
        if (bp_bin_i2c_read(bp, reg_value) < 0)
        {
            return -1;
        }
        if (bp_bin_i2c_nack(bp) < 0)
        {
            return -1;
        }
        if (bp_bin_i2c_stop(bp) < 0)
        {
            return -1;
        }
        return 0;
    }

    /**
     * @brief WriteTinkerForm::ds3231_write
     * @param bp
     * @param dev_addr
     * @param addr
     * @param value
     * @returnwrite
     */
    int ds3231_write(BP * bp, unsigned char dev_addr, unsigned char addr, unsigned char value)
    {
        unsigned char ack;
        assert(dev_addr <= 128);

        if (bp_bin_i2c_start(bp) < 0)
        {
            return -1;
        }
        if (bp_bin_i2c_write(bp, (dev_addr << 1), &ack) < 0)
        {
            return -1;
        }
        if (ack != BP_BIN_I2C_ACK)
        {
            return -1;
        }
        if (bp_bin_i2c_write(bp, addr, &ack) < 0)
        {
            return -1;
        }
        if (ack != BP_BIN_I2C_ACK)
        {
            return -1;
        }
        if (bp_bin_i2c_write(bp, value, &ack) < 0)
        {
            return -1;
        }
        if (ack != BP_BIN_I2C_ACK)
        {
            return -1;
        }
        if (bp_bin_i2c_stop(bp) < 0)
        {
            return -1;
        }
        return 0;
    }

    uint8_t bcd2bin(uint8_t val)
    {
        return ((val >> 4) & 0xf) * 10 + (val & 0xf);
    }

    uint8_t bin2bcd(uint8_t val)
    {
        return ((val / 10) << 4) | (val % 10);
    }

}
