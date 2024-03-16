//
// Copyright (C) 2024  Markus Hiienkari <mhiienka@niksula.hut.fi>
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
#include "si2177.h"
#include "i2c_opencores.h"

const si2177_config si2177_cfg_default = {
    .tmp = 1
};

void si2177_writereg(si2177_dev *dev, uint8_t regaddr, uint8_t data) {
    I2C_start(dev->i2cm_base, dev->i2c_addr, 0);
    I2C_write(dev->i2cm_base, regaddr, 0);
    I2C_write(dev->i2cm_base, data, 1);
}

uint8_t si2177_readreg(si2177_dev *dev, uint8_t regaddr) {
    //Phase 1
    I2C_start(dev->i2cm_base, dev->i2c_addr, 0);
    I2C_write(dev->i2cm_base, regaddr, 0);

    //Phase 2
    I2C_start(dev->i2cm_base, dev->i2c_addr, 1);
    return I2C_read(dev->i2cm_base,1);
}

int si2177_init(si2177_dev *dev) {
    memcpy(&dev->cfg, &si2177_cfg_default, sizeof(si2177_config));

    /*if (si2177_readreg(dev, 0x11) != 0x43)
        return -1;*/

    /*for (int i=0; i<=0x13; i++)
        printf("SI2177 0x%.2x 0x%.2x\n", i, si2177_readreg(dev, i));*/

    //si2177_enable_power(dev, 0);

    return 0;
}

void si2177_enable_power(si2177_dev *dev, int enable) {

}
