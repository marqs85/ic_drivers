//
// Copyright (C) 2020  Markus Hiienkari <mhiienka@niksula.hut.fi>
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

#ifndef SII1136_H_
#define SII1136_H_

#include <stdio.h>
#include <stdint.h>
#include "sysconfig.h"
#include "hdmi.h"
#include "sii1136_regs.h"

typedef struct {
    HDMI_tx_mode_t tx_mode;
    HDMI_audio_fmt_t audio_fmt;
    HDMI_i2s_fs_t i2s_fs;
    HDMI_i2s_stereo_cfg_t i2s_stereo_cfg;
    HDMI_audio_cc_t audio_cc_val;
    HDMI_audio_ca_t audio_ca_val;
    uint8_t hdr;
} sii1136_config;

typedef struct {
    uint32_t i2cm_base;
    uint8_t i2c_addr;
    uint8_t powered_on;
    uint8_t pixelrep;
    uint8_t pixelrep_infoframe;
    HDMI_vic_t vic;
    sii1136_config cfg;
} sii1136_dev;

int sii1136_init(sii1136_dev *dev);

void sii1136_get_default_cfg(sii1136_config *cfg);

void sii1136_enable_power(sii1136_dev *dev, int enable);

void sii1136_set_audio(sii1136_dev *dev, HDMI_audio_fmt_t fmt, HDMI_i2s_fs_t i2s_fs, HDMI_i2s_stereo_cfg_t i2s_stereo_cfg, HDMI_audio_cc_t cc_val, HDMI_audio_ca_t ca_val);

void sii1136_set_hdr(sii1136_dev *dev, uint8_t hdr_enable);

void sii1136_set_tx_mode(sii1136_dev *dev, HDMI_tx_mode_t mode);

void sii1136_init_mode(sii1136_dev *dev, uint8_t pixelrep, uint8_t pixelrep_infoframe, HDMI_vic_t vic, uint32_t pclk_hz);

void sii1136_update_config(sii1136_dev *dev, sii1136_config *cfg);

#endif /* SII1136_H_ */
