/*
AMiRo-LLD is a compilation of low-level hardware drivers for the Autonomous Mini Robot (AMiRo) platform.
Copyright (C) 2016..2022  Thomas Schöpping et al.

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/**
 * @file    alld_vl6180x_i2c.c
 * @brief   VL6180X driver I2C interface.
 *
 * @addtogroup lld_VL6180X_v1
 * @{
 */

#include <vl6180x_platform.h>

/******************************************************************************/
/* LOCAL DEFINITIONS                                                          */
/******************************************************************************/

/******************************************************************************/
/* EXPORTED VARIABLES                                                         */
/******************************************************************************/

/******************************************************************************/
/* LOCAL TYPES                                                                */
/******************************************************************************/

/******************************************************************************/
/* LOCAL VARIABLES                                                            */
/******************************************************************************/

/******************************************************************************/
/* LOCAL FUNCTIONS                                                            */
/******************************************************************************/

/******************************************************************************/
/* EXPORTED FUNCTIONS                                                         */
/******************************************************************************/

/* VL6180X API I2C access functions */

int VL6180x_WrByte(VL6180xDev_t dev, uint16_t index, uint8_t data)
{
  const uint8_t txbuf[3] = {
    index >> 8,
    index & 0xFF,
    data
  };
  return apalI2CMasterTransmit(dev->i2cd, dev->addr, txbuf, 3, NULL, 0, -1);
}

int VL6180x_WrWord(VL6180xDev_t dev, uint16_t index, uint16_t data)
{
  const uint8_t txbuf[4] = {
    index >> 8,
    index & 0xFF,
    data >> 8,
    data & 0xFF
  };
  return apalI2CMasterTransmit(dev->i2cd, dev->addr, txbuf, 4, NULL, 0, -1);
}

int VL6180x_WrDWord(VL6180xDev_t dev, uint16_t index, uint32_t data)
{
  const uint8_t txbuf[6] = {
    index >> 8,
    index & 0xFF,
    data >> 24,
    (data >> 16) & 0xFF,
    (data >> 8) & 0xFF,
    data & 0xFF
  };
  return apalI2CMasterTransmit(dev->i2cd, dev->addr, txbuf, 6, NULL, 0, -1);
}

int VL6180x_UpdateByte(VL6180xDev_t dev, uint16_t index, uint8_t AndData, uint8_t OrData)
{
  uint8_t buf[3] = {
    index >> 8,
    index & 0xFF,
    0
  };
  apalExitStatus_t status = apalI2CMasterTransmit(dev->i2cd, dev->addr, &buf[0], 2, &buf[2], 1, -1);
  if (status == APAL_STATUS_OK) {
    buf[2] = (buf[2] & AndData) | OrData;
    status = apalI2CMasterTransmit(dev->i2cd, dev->addr, buf, 3, NULL, 0, -1);
  }
  return status;
}

int VL6180x_RdByte(VL6180xDev_t dev, uint16_t index, uint8_t *data)
{
  const uint8_t txbuf[2] = {index >> 8, index & 0xFF};
  return apalI2CMasterTransmit(dev->i2cd, dev->addr, txbuf, 2, data, 1, -1);
}

int VL6180x_RdWord(VL6180xDev_t dev, uint16_t index, uint16_t *data)
{
  const uint8_t txbuf[2] = {index >> 8, index & 0xFF};
  uint8_t rxbuf[2];
  const apalExitStatus_t status = apalI2CMasterTransmit(dev->i2cd, dev->addr, txbuf, 2, rxbuf, 2, -1);
  *data = ((uint16_t)rxbuf[0] << 8) |
          ((uint16_t)rxbuf[1]);
  return status;
}

int VL6180x_RdDWord(VL6180xDev_t dev, uint16_t index, uint32_t *data)
{
  const uint8_t txbuf[2] = {index >> 8, index & 0xFF};
  uint8_t rxbuf[4];
  const apalExitStatus_t status = apalI2CMasterTransmit(dev->i2cd, dev->addr, txbuf, 2, rxbuf, 2, -1);
  *data = ((uint32_t)rxbuf[0] << 24) |
          ((uint32_t)rxbuf[1] << 16) |
          ((uint32_t)rxbuf[2] <<  8) |
          ((uint32_t)rxbuf[3]);
  return status;
}

int VL6180x_RdMulti(VL6180xDev_t dev, uint16_t index, uint8_t *data, int nData)
{
  const uint8_t txbuf[2] = {index >> 8, index & 0xFF};
  return apalI2CMasterTransmit(dev->i2cd, dev->addr, txbuf, 2, data, nData, -1);
}

/** @} */

