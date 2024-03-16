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

#ifndef ADV7280A_H_
#define ADV7280A_H_

#include <stdio.h>
#include <stdint.h>
#include "sysconfig.h"
#include "adv7280a_regs.h"

typedef enum {
    ADV7280A_USER_SUB_MAP = 0,
    ADV7280A_VDP_SUB_MAP = 1,
    ADV7280A_USER_SUB_MAP2 = 2,
} adv7280a_reg_map;

typedef enum {
    ADV7280A_INPUT_CVBS_AIN1 = 0,
    ADV7280A_INPUT_CVBS_AIN2,
    ADV7280A_INPUT_CVBS_AIN3,
    ADV7280A_INPUT_CVBS_AIN4,
    ADV7280A_INPUT_YC_AIN12 = 8,
    ADV7280A_INPUT_YC_AIN34,
    ADV7280A_INPUT_YPBPR_AIN123 = 12,
    ADV7280A_INPUT_CVBSDIFF_AIN12 = 14,
    ADV7280A_INPUT_CVBSDIFF_AIN34,
} adv7280a_input;

typedef struct {
    uint16_t v_total;
    uint8_t interlace_flag;
    uint32_t pcnt_frame;
} adv7280a_sync_status;

typedef struct {
    uint8_t tmp;
} adv7280a_config;

typedef struct {
    uint32_t i2cm_base;
    uint8_t i2c_addr;
    uint8_t sync_active;
    uint8_t powered_on;
    adv7280a_config cfg;
    adv7280a_sync_status ss;
} adv7280a_dev;

int adv7280a_init(adv7280a_dev *dev);

void adv7280a_enable_power(adv7280a_dev *dev, int enable);

void adv7280a_select_input(adv7280a_dev *dev, adv7280a_input input);

int adv7280a_check_activity(adv7280a_dev *dev);

int adv7280a_get_sync_stats(adv7280a_dev *dev, uint16_t vtotal, uint8_t interlace_flag, uint32_t pcnt_frame);

#endif /* ADV7280A_H_ */
