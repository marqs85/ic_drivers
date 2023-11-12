//
// Copyright (C) 2023 Markus Hiienkari <mhiienka@niksula.hut.fi>
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

#ifndef PCM514X_H_
#define PCM514X_H_

#include <stdint.h>
#include "pcm514x_regs.h"

typedef struct {
    uint8_t gain;
} pcm514x_config;

typedef struct {
    uint32_t i2cm_base;
    uint8_t i2c_addr;
    pcm514x_config cfg;
} pcm514x_dev;

void pcm514x_set_gain(pcm514x_dev *dev, int8_t db_gain);

int pcm514x_init(pcm514x_dev *dev);

void pcm514x_enable_power(pcm514x_dev *dev, int enable);

void pcm514x_get_default_cfg(pcm514x_config *cfg);

void pcm514x_update_config(pcm514x_dev *dev, pcm514x_config *cfg);

#endif /* PCM514X_H_ */
