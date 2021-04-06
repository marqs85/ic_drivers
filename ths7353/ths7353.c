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

    dev->chmask = 0;

    //Initialize all channels
    ths7353_writereg(dev, THS_CH_1, (THS_LPF_DEFAULT<<THS_LPF_OFFS));
    ths7353_writereg(dev, THS_CH_2, (THS_LPF_DEFAULT<<THS_LPF_OFFS));
    ths7353_writereg(dev, THS_CH_3, (THS_LPF_DEFAULT<<THS_LPF_OFFS));

    return (ths7353_readreg(dev, THS_CH_1) != (THS_LPF_DEFAULT<<THS_LPF_OFFS));
}

void ths7353_set_lpf(ths7353_dev *dev, uint8_t val)
{
    ths_channel_t ch;
    uint8_t status;

    for (ch=THS_CH_1; ch<=THS_CH_3; ch++) {
        if (dev->chmask & (1<<(ch-THS_CH_1))) {
            status = ths7353_readreg(dev, ch) & ~THS_LPF_MASK;
            status |= (val<<THS_LPF_OFFS);
            ths7353_writereg(dev, ch, status);
            printf("THS channel %u: LPF %u\n", ch, val);
        }
    }
}

void ths7353_set_input_biasmode(ths7353_dev *dev, ths_biasmode_t mode, uint8_t lpf, uint8_t stc_lpf)
{
    ths_channel_t ch;
    uint8_t status;

    for (ch=THS_CH_1; ch<=THS_CH_3; ch++) {
        if (dev->chmask & (1<<(ch-THS_CH_1))) {
            status = ths7353_readreg(dev, ch) & ~(THS_LPF_MASK|THS_STC_LPF_MASK|THS_MODE_MASK);
            status |= (stc_lpf<<THS_STC_LPF_OFFS)|(lpf<<THS_LPF_OFFS)|(mode<<THS_MODE_OFFS);
            ths7353_writereg(dev, ch, status);
            printf("THS channel %u: bias mode %u, LPF %u, stc LPF %u\n", ch, mode, lpf, stc_lpf);
        }
    }
}

// Legacy functionality
void ths7353_source_sel(ths7353_dev *dev, ths_input_t input, uint8_t lpf)
{
    uint8_t status = ths7353_readreg(dev, THS_CH_1) & ~(THS_SRC_MASK|THS_MODE_MASK);

    if (input == THS_STANDBY)
        status |= (THS_MODE_AVMUTE<<THS_MODE_OFFS);
    else
        status |= (THS_BIAS_AC | (input<<THS_SRC_OFFS));

    dev->chmask = 0x7;

    ths7353_writereg(dev, THS_CH_1, status);
    ths7353_writereg(dev, THS_CH_2, status);
    ths7353_writereg(dev, THS_CH_3, status);
    printf("THS source set to %u\n", input);
}

void ths7353_singlech_source_sel(ths7353_dev *dev, ths_channel_t ch, ths_input_t input, ths_biasmode_t mode, uint8_t lpf, uint8_t stc_lpf)
{
    uint8_t regval = ((stc_lpf<<THS_STC_LPF_OFFS) | (input<<THS_SRC_OFFS) | (lpf<<THS_LPF_OFFS) | mode);

    ths7353_writereg(dev, THS_CH_1, (ch==THS_CH_1) ? regval : THS_MODE_DISABLE);
    ths7353_writereg(dev, THS_CH_2, (ch==THS_CH_2) ? regval : THS_MODE_DISABLE);
    ths7353_writereg(dev, THS_CH_3, (ch==THS_CH_3) ? regval : THS_MODE_DISABLE);

    if (ch != THS_CH_NONE) {
        dev->chmask = (1<<(ch-THS_CH_1));
        printf("THS channel %u: input %c, bias mode %u, LPF %u, stc LPF %u\n", ch, input ? 'B' : 'A', mode, lpf, stc_lpf);
    } else {
        dev->chmask = 0;
    }
}
