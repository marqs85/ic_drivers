//
// Copyright (C) 2015-2016  Markus Hiienkari <mhiienka@niksula.hut.fi>
//
// This file is part of Open Source Scan Converter project.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include "ths7353.h"
#include "i2c_opencores.h"

void ths7353_writereg(ths7353_dev *dev, uint8_t regaddr, uint8_t data) {
    I2C_start(dev->i2cm_base, dev->i2c_addr, 0);
    I2C_write(dev->i2cm_base, regaddr, 0);
    I2C_write(dev->i2cm_base, data, 1);
}

uint8_t ths7353_readreg(ths7353_dev *dev, uint8_t regaddr) {
    //Phase 1
    I2C_start(dev->i2cm_base, dev->i2c_addr, 0);
    I2C_write(dev->i2cm_base, regaddr, 0);

    //Phase 2
    I2C_start(dev->i2cm_base, dev->i2c_addr, 1);
    return I2C_read(dev->i2cm_base,1);
}

int ths7353_init(ths7353_dev *dev)
{
    //Avoid random FIFO state (see datasheet p.37)
    I2C_write(dev->i2cm_base, 0x00, 0);
    usleep(10);

    //Initialize all channels
    ths7353_writereg(dev, THS_CH1, (THS_LPF_DEFAULT<<THS_LPF_OFFS));
    ths7353_writereg(dev, THS_CH2, (THS_LPF_DEFAULT<<THS_LPF_OFFS));
    ths7353_writereg(dev, THS_CH3, (THS_LPF_DEFAULT<<THS_LPF_OFFS));

    return (ths7353_readreg(dev, THS_CH1) != (THS_LPF_DEFAULT<<THS_LPF_OFFS));
}

void ths7353_set_lpf(ths7353_dev *dev, uint8_t val)
{
    uint8_t status = ths7353_readreg(dev, THS_CH1) & ~THS_LPF_MASK;
    status |= (val<<THS_LPF_OFFS);

    ths7353_writereg(dev, THS_CH1, status);
    ths7353_writereg(dev, THS_CH2, status);
    ths7353_writereg(dev, THS_CH3, status);
    printf("THS LPF value set to 0x%x\n", val);
}

void ths7353_source_sel(ths7353_dev *dev, ths_input_t input, uint8_t lpf)
{
    uint8_t status = ths7353_readreg(dev, THS_CH1) & ~(THS_SRC_MASK|THS_MODE_MASK);

    if (input == THS_STANDBY)
        status |= (THS_MODE_AVMUTE<<THS_MODE_OFFS);
    else
        status |= (THS_MODE_AC_BIAS | (input<<THS_SRC_OFFS));

    //status |= (lpf<<THS_LPF_OFFS);

    ths7353_writereg(dev, THS_CH1, status);
    ths7353_writereg(dev, THS_CH2, status);
    ths7353_writereg(dev, THS_CH3, status);
    printf("THS source set to %u\n", input);
}

void ths7353_singlech_source_sel(ths7353_dev *dev, ths_channel_t ch, ths_input_t input, uint8_t lpf)
{
    uint8_t regval = (THS_MODE_AC_BIAS | (input<<THS_SRC_OFFS));

    ths7353_writereg(dev, THS_CH1, (ch==THS_CH_1) ? regval : THS_MODE_DISABLE);
    ths7353_writereg(dev, THS_CH2, (ch==THS_CH_1) ? regval : THS_MODE_DISABLE);
    ths7353_writereg(dev, THS_CH3, (ch==THS_CH_1) ? regval : THS_MODE_DISABLE);

    if (ch != THS_CH_NONE)
        printf("THS channel %u source set to %u\n", ch, input);
}
