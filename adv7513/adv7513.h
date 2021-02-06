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

#ifndef ADV7513_H_
#define ADV7513_H_

#include <stdio.h>
#include <stdint.h>
#include "sysconfig.h"
#include "hdmi.h"
#include "adv7513_regs.h"

typedef struct {
    HDMI_tx_mode_t tx_mode;
    HDMI_audio_fmt_t audio_fmt;
    HDMI_i2s_fs_t i2s_fs;
    HDMI_i2s_stereo_cfg_t i2s_stereo_cfg;
    HDMI_audio_cc_t audio_cc_val;
    HDMI_audio_ca_t audio_ca_val;
} adv7513_config;

typedef struct {
    uint32_t i2cm_base;
    uint8_t main_base;
    uint8_t edid_base;
    uint8_t pktmem_base;
    uint8_t cec_base;
    uint8_t powered_on;
    uint8_t pixelrep;
    uint8_t pixelrep_infoframe;
    HDMI_vic_t vic;
    adv7513_config cfg;
} adv7513_dev;

int adv7513_init(adv7513_dev *dev);

void adv7513_enable_power(adv7513_dev *dev, int enable);

void adv7513_get_default_cfg(adv7513_config *cfg);

void adv7513_set_audio(adv7513_dev *dev, HDMI_audio_fmt_t fmt, HDMI_i2s_fs_t i2s_fs, HDMI_i2s_stereo_cfg_t i2s_stereo_cfg, HDMI_audio_cc_t cc_val, HDMI_audio_ca_t ca_val);

void adv7513_set_tx_mode(adv7513_dev *dev, HDMI_tx_mode_t mode);

void adv7513_set_csc_mode(adv7513_dev *dev, uint8_t enable, HDMI_colorspace_t src, HDMI_colorspace_t dst);

void adv7513_set_pixelrep_vic(adv7513_dev *dev, uint8_t pixelrep, uint8_t pixelrep_infoframe, HDMI_vic_t vic);

int adv7513_check_hpd_power(adv7513_dev *dev);

void adv7513_update_config(adv7513_dev *dev, adv7513_config *cfg);

#endif /* ADV7513_H_ */
