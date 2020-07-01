//
// Copyright (C) 2019  Markus Hiienkari <mhiienka@niksula.hut.fi>
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

#ifndef ADV7611_H_
#define ADV7611_H_

#include <stdio.h>
#include <stdint.h>
#include "sysconfig.h"
#include "adv7611_regs.h"

typedef enum {
    ADV7611_IO_MAP = 0,
    ADV7611_DPLL_MAP,
    ADV7611_HDMI_MAP,
    ADV7611_KSV_MAP,
    ADV7611_INFOFRAME_MAP,
    ADV7611_CP_MAP,
    ADV7611_CEC_MAP,
    ADV7611_EDID_MAP,
} adv7611_reg_map;

typedef enum {
    ADV7611_RGB_LIMITED = 0,
    ADV7611_RGB_FULL,
} adv7611_rgb_range;

typedef struct {
    uint16_t h_active;
    uint16_t v_active;
    uint16_t h_total;
    uint16_t v_total;
    uint8_t h_backporch;
    uint8_t v_backporch;
    uint8_t h_synclen;
    uint8_t v_synclen;
    uint8_t interlace_flag;
    uint8_t h_polarity;
    uint8_t v_polarity;
} adv7611_sync_status;

typedef struct {
    adv7611_rgb_range default_rgb_range;
} adv7611_config;

typedef struct {
    uint32_t i2cm_base;
    uint8_t io_base;
    uint8_t cec_base;
    uint8_t infoframe_base;
    uint8_t dpll_base;
    uint8_t ksv_base;
    uint8_t edid_base;
    uint8_t hdmi_base;
    uint8_t cp_base;
    uint32_t xtal_freq;
    uint8_t *edid;
    uint16_t edid_len;
    uint8_t sync_active;
    adv7611_sync_status ss;
    uint32_t pclk_hz;
    uint32_t pixelrep;
    uint8_t hdmi_mode;
    adv7611_config cfg;
} adv7611_dev;

void adv7611_init(adv7611_dev *dev);

void adv7611_enable_power(adv7611_dev *dev, int enable);

void adv7611_set_default_rgb_range(adv7611_dev *dev, adv7611_rgb_range rng);

void adv7611_set_input_cs(adv7611_dev *dev);

int adv7611_check_activity(adv7611_dev *dev);

int adv7611_get_sync_stats(adv7611_dev *dev);

void adv7611_update_config(adv7611_dev *dev, adv7611_config *cfg);

#endif /* ADV7611_H_ */
