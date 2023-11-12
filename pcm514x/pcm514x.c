//
// Copyright (C) 2023  Markus Hiienkari <mhiienka@niksula.hut.fi>
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
#include <unistd.h>
#include <string.h>
#include "system.h"
#include "i2c_opencores.h"
#include "pcm514x.h"

const pcm514x_config pcm514x_cfg_default = {
    .gain = 0
};

static uint32_t pcm514x_readreg(pcm514x_dev *dev, uint8_t regaddr)
{
    //Phase 1
    I2C_start(dev->i2cm_base, dev->i2c_addr, 0);
    I2C_write(dev->i2cm_base, regaddr, 0);

    //Phase 2
    I2C_start(dev->i2cm_base, dev->i2c_addr, 1);
    return I2C_read(dev->i2cm_base,1);
}

static void pcm514x_writereg(pcm514x_dev *dev, uint8_t regaddr, uint8_t data)
{
    I2C_start(dev->i2cm_base, dev->i2c_addr, 0);
    I2C_write(dev->i2cm_base, regaddr, 0);
    I2C_write(dev->i2cm_base, data, 1);
}

static void pcm514x_reset(pcm514x_dev *dev) {
    pcm514x_writereg(dev, PCM514X_RESET_CTRL, 0x01);
}

void pcm514x_set_gain(pcm514x_dev *dev, int8_t db_gain) {

}

int pcm514x_init(pcm514x_dev *dev)
{
    if (pcm514x_readreg(dev, PCM514X_PLL_P) == 0xff)
        return -1;

    //pcm514x_reset(dev);

    // Select BCK as clk source for PLL and DAC
    pcm514x_writereg(dev, PCM514X_PLL_SRCSEL, 0x10);
    pcm514x_writereg(dev, PCM514X_DAC_SRCSEL, 0x10);

    // Set manual clock mode and ignore SCK detection
    pcm514x_writereg(dev, PCM514X_CLK_MON, 0x0a);

    // Set dividers
    pcm514x_writereg(dev, PCM514X_DSP_DIV, 1);
    pcm514x_writereg(dev, PCM514X_DAC_DIV, 15);
    pcm514x_writereg(dev, PCM514X_NCP_DIV, 3);
    pcm514x_writereg(dev, PCM514X_OSR_DIV, 7);

    // Configure PLL for 98.30MHz PLL output
    pcm514x_writereg(dev, PCM514X_PLL_ENABLE, 0x00);
    pcm514x_writereg(dev, PCM514X_PLL_P, 0);
    pcm514x_writereg(dev, PCM514X_PLL_R, 1);
    pcm514x_writereg(dev, PCM514X_PLL_J, 16);
    pcm514x_writereg(dev, PCM514X_PLL_D_LSB, 0);
    pcm514x_writereg(dev, PCM514X_PLL_D_MSB, 0);
    pcm514x_writereg(dev, PCM514X_PLL_ENABLE, 0x01);

    //pcm514x_enable_power(dev, 0);

    memcpy(&dev->cfg, &pcm514x_cfg_default, sizeof(pcm514x_config));

    return 0;
}

void pcm514x_enable_power(pcm514x_dev *dev, int enable) {

}

void pcm514x_get_default_cfg(pcm514x_config *cfg) {
    memcpy(cfg, &pcm514x_cfg_default, sizeof(pcm514x_config));
}

void pcm514x_update_config(pcm514x_dev *dev, pcm514x_config *cfg) {
    if (cfg->gain != dev->cfg.gain)
        pcm514x_set_gain(dev, cfg->gain);

    memcpy(&dev->cfg, cfg, sizeof(pcm514x_config));
}
