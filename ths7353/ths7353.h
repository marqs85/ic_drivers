//
// Copyright (C) 2015-2020  Markus Hiienkari <mhiienka@niksula.hut.fi>
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

#ifndef THS7353_H_
#define THS7353_H_

#include <stdio.h>
#include <stdint.h>
#include "sysconfig.h"
#include "ths7353_regs.h"

typedef struct {
    uint32_t i2cm_base;
    uint8_t i2c_addr;
    uint8_t chmask;
} ths7353_dev;

typedef enum {
    THS_CH_NONE = 0,
    THS_CH_1    = 1,
    THS_CH_2    = 2,
    THS_CH_3    = 3,
} ths_channel_t;

typedef enum {
    THS_INPUT_A = 0,
    THS_INPUT_B = 1,
    THS_STANDBY = 2
} ths_input_t;

typedef enum {
    THS_BIAS_DC        = 2,
    THS_BIAS_DC_OFFSET = 3,
    THS_BIAS_AC        = 4,
    THS_BIAS_STC_LOW   = 5,
    THS_BIAS_STC_MID   = 6,
    THS_BIAS_STC_HIGH  = 7,
} ths_biasmode_t;


#define THS_STC_LPF_OFFS    6
#define THS_STC_LPF_MASK    0xc0
#define THS_STC_LPF_0P5MHZ  0
#define THS_STC_LPF_2P5MHZ  1
#define THS_STC_LPF_5MHZ    2

#define THS_SRC_OFFS        5
#define THS_SRC_MASK        0x20

#define THS_LPF_OFFS        3
#define THS_LPF_MASK        0x18
#define THS_LPF_9MHZ        0x00
#define THS_LPF_16MHZ       0x01
#define THS_LPF_35MHZ       0x02
#define THS_LPF_BYPASS      0x03
#define THS_LPF_DEFAULT     0x3

#define THS_MODE_OFFS       0
#define THS_MODE_MASK       0x7
#define THS_MODE_DISABLE    0
#define THS_MODE_AVMUTE     1

int ths7353_init(ths7353_dev *dev);

void ths7353_set_lpf(ths7353_dev *dev, uint8_t val);

void ths7353_set_input_biasmode(ths7353_dev *dev, ths_biasmode_t mode, uint8_t lpf, uint8_t stc_lpf);

void ths7353_source_sel(ths7353_dev *dev, ths_input_t input, uint8_t lpf);

void ths7353_singlech_source_sel(ths7353_dev *dev, ths_channel_t ch, ths_input_t input, ths_biasmode_t mode, uint8_t lpf, uint8_t stc_lpf);

#endif /* THS7353_H_ */
