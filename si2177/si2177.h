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

#ifndef SI2177_H_
#define SI2177_H_

#include <stdio.h>
#include <stdint.h>
#include "sysconfig.h"
#include "si2177_regs.h"

typedef struct {
    uint8_t tmp;
} si2177_config;

typedef struct {
    uint32_t i2cm_base;
    uint8_t i2c_addr;
    uint8_t powered_on;
    si2177_config cfg;
} si2177_dev;

int si2177_init(si2177_dev *dev);

void si2177_enable_power(si2177_dev *dev, int enable);

#endif /* SI2177_H_ */
