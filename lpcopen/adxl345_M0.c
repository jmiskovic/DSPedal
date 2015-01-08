/*
 * ADXL345.c
 *
 *    Created on: 3 oct. 2014
 *            Author: ldo
 *    Modified for NXP LPC4357-EVB board with LPCOpen library on: 6 jan. 2015
 *
 *    ADXL345 is a 3-axis Accelerometer with high resolution (13 bits) measurement at up +/- 16g.
 *    Digital output are accessible through either a SPI or I2C digital interface.
 */

#include <stdint.h>
#include "board.h"
#include "adxl345.h"

static uint16_t sensitivity = SENSITIVITY_16g;

static xyz low_pass;

/*
 * \fn void ADXL345_Init (void)
 * \brief ADXL345 setup function
 * \param mr full-scale range (2, 4, 8 or 16 g)
 * */
void ADXL345_Init (ADXL345_measurement_range mr)
{
    uint8_t buffer[2];

    /* set the g range (2, 4, 8 or 16 g) */
    switch (mr)
        {
        case MR_2_g:
            buffer[0] = DATA_FORMAT;
            buffer[1] = G_RANGE_2g;
            sensitivity = SENSITIVITY_2g;
            break;
        case MR_4_g:
            buffer[0] = DATA_FORMAT;
            buffer[1] = G_RANGE_4g;
            sensitivity = SENSITIVITY_4g;
            break;
        case MR_8_g:
            buffer[0] = DATA_FORMAT;
            buffer[1] = G_RANGE_8g;
            sensitivity = SENSITIVITY_8g;
            break;
        case MR_16_g:
        default:
            buffer[0] = DATA_FORMAT;
            buffer[1] = G_RANGE_16g;
            sensitivity = SENSITIVITY_16g;
            break;
        }
    buffer[1] |= FULL_RES;
    Chip_I2C_MasterSend(ADXL345_I2C_BUS, ADXL345_ADDRESS_LOW, buffer, 2);
    low_pass.x = 0.0;
    low_pass.y = 0.0;
    low_pass.z = 0.0;
}

/*
 * \fn void ADXL345_power_on (void)
 * \brief setup measurement mode and sensitivity
 * */
void ADXL345_power_on (void)
{
    uint8_t buffer[2];

    /* setting of 1 in the measure bit places the part into measurement mode */
    buffer[0] = POWER_CTL;
    buffer[1] = MEASURE;
    Chip_I2C_MasterSend(ADXL345_I2C_BUS, ADXL345_ADDRESS_LOW, buffer, 2);
}

/*
 * \fn uint8_t ADXL345_read_device_id (void)
 * \brief read device id
 * \return a fixed device id code of 0xE5
 * */
uint8_t ADXL345_read_device_id (void)
{
    uint8_t reg = DEVICE;
    uint8_t device_id;

    Chip_I2C_MasterCmdRead(ADXL345_I2C_BUS, ADXL345_ADDRESS_LOW, reg, &device_id, 1);

    return device_id;
}

/*
 * \fn xyz ADXL345_self_test (void)
 * \brief self test (g range +/- 16g, result right justified, 10-bits resolution)
 * \return x, y, z test values [g] */
xyz ADXL345_self_test (void)
{
    uint8_t buffer[2];
    volatile xyz acc;

    /* self-test function is enable (SELF_TEST bit in DATA_FORMAT is set) */
    buffer[0] = DATA_FORMAT;
    buffer[1] = SELF_TEST + G_RANGE_16g;
    Chip_I2C_MasterSend(ADXL345_I2C_BUS, ADXL345_ADDRESS_LOW, buffer, 2);

    /* read x, y and y value */
    acc = ADXL345_raw_read_acc_xyz ();

    /* self-test function is disable (SELF_TEST bit in DATA_FORMAT is reset) */
    buffer[0] = DATA_FORMAT;
    buffer[1] = G_RANGE_16g;
    Chip_I2C_MasterSend(ADXL345_I2C_BUS, ADXL345_ADDRESS_LOW, buffer, 2);

    return acc;
}

/*
 * \fn xyz ADXL345_read_offset_xyz (void)
 * \brief read x, y and z offset value
 * \return x, y and z offset value
 * */
xyz ADXL345_read_offset_xyz (void)
{
    uint8_t reg = OFSX;
    uint8_t buffer[3];
    xyz offset;

    Chip_I2C_MasterCmdRead(ADXL345_I2C_BUS, ADXL345_ADDRESS_LOW, reg, buffer, 3);

    offset.x = buffer[0];
    offset.y = buffer[1];
    offset.z = buffer[2];

    return offset;
}

/*
 * \fn void ADXL345_write_offset_xyz (xyz offset)
 * \brief write x, y and z offset value
 * \param x, y and z offset value
 * */
void ADXL345_write_offset_xyz (xyz offset)
{
    uint8_t buffer[4];

    buffer[0] = OFSX;
    buffer[1] = offset.x;
    buffer[2] = offset.y;
    buffer[3] = offset.z;

    Chip_I2C_MasterSend(ADXL345_I2C_BUS, ADXL345_ADDRESS_LOW, buffer, 4);
}

/**
 * \brief low pass filter y(t) = a.xm(t) + (1-a).y(t-1)
 * */
void low_pass_filter (xyz *measure)
{
    low_pass.x = (((int32_t) low_pass.x) * 4 + measure->x) / 5;
    low_pass.y = (((int32_t) low_pass.y) * 4 + measure->y) / 5;
    low_pass.z = (((int32_t) low_pass.z) * 4 + measure->z) / 5;

    *measure = low_pass;
}

/*
 * \fn xyz ADXL345_read_acc_xyz (void)
 * \brief read x,y and z acceleration value
 * \return x, y and z acceleration [g]
 * */
xyz ADXL345_raw_read_acc_xyz (void)
{
    volatile uint8_t reg = DATAX0;
    uint8_t buffer[6];
    xyz acc;

    Chip_I2C_MasterCmdRead(ADXL345_I2C_BUS, ADXL345_ADDRESS_LOW, reg, buffer, 6);

    // calculate acceleration: reading / sensitivity
    acc.x = (int16_t) ((uint16_t) buffer[0] + (buffer[1] << 8));
    acc.y = (int16_t) ((uint16_t) buffer[2] + (buffer[3] << 8));
    acc.z = (int16_t) ((uint16_t) buffer[4] + (buffer[5] << 8));

    return acc;
}
