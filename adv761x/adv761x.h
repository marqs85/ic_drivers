//
// Copyright (C) 2019-2020  Markus Hiienkari <mhiienka@niksula.hut.fi>
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

#ifndef ADV761X_H_
#define ADV761X_H_

#include <stdio.h>
#include <stdint.h>
#include "sysconfig.h"
#include "hdmi.h"
#include "adv761x_regs.h"

typedef enum {
    ADV761X_IO_MAP = 0,
    ADV761X_DPLL_MAP,
    ADV761X_HDMI_MAP,
    ADV761X_KSV_MAP,
    ADV761X_INFOFRAME_MAP,
    ADV761X_CP_MAP,
    ADV761X_CEC_MAP,
    ADV761X_EDID_MAP,
} adv761x_reg_map;

typedef enum {
    ADV761X_RGB_LIMITED = 0,
    ADV761X_RGB_FULL,
} adv761x_rgb_range;

typedef struct {
    uint16_t h_active;
    uint16_t v_active;
    uint16_t h_total;
    uint16_t v_total;
    uint16_t h_backporch;
    uint16_t v_backporch;
    uint16_t h_synclen;
    uint16_t v_synclen;
    uint8_t interlace_flag;
    uint8_t h_polarity;
    uint8_t v_polarity;
} adv761x_sync_status;

typedef struct {
    adv761x_rgb_range default_rgb_range;
    uint8_t pixelderep_mode;
} adv761x_config;

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
    adv761x_sync_status ss;
    uint32_t pclk_hz;
    uint32_t pixelderep;
    uint32_t pixelderep_ifr;
    uint8_t hdmi_mode;
    uint8_t powered_on;
    HDMI_audio_sample_type_t audio_sample_type;
    adv761x_config cfg;
} adv761x_dev;

void adv761x_init(adv761x_dev *dev);

void adv761x_enable_power(adv761x_dev *dev, int enable);

void adv761x_set_default_rgb_range(adv761x_dev *dev, adv761x_rgb_range rng);

void adv761x_set_pixelderep(adv761x_dev *dev, uint8_t pixelderep_mode);

void adv761x_set_spdif_mux(adv761x_dev *dev, int enable);

void adv761x_set_input_cs(adv761x_dev *dev);

int adv761x_check_activity(adv761x_dev *dev);

int adv761x_get_sync_stats(adv761x_dev *dev);

HDMI_audio_sample_type_t adv761x_get_audio_sample_type(adv761x_dev *dev);

HDMI_i2s_fs_t adv761x_get_i2s_fs(adv761x_dev *dev);

HDMI_audio_cc_t adv761x_get_audio_cc(adv761x_dev *dev);

HDMI_audio_ca_t adv761x_get_audio_ca(adv761x_dev *dev);

void adv761x_update_config(adv761x_dev *dev, adv761x_config *cfg);

#endif /* ADV761X_H_ */
