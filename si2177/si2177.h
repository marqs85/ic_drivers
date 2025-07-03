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
#include "Silabs_L0_API.h"
#include "Si2177_Commands.h"
#include "Si2177_Properties.h"
#include "Si2177_L1_API.h"
#include "Si2177_L2_API.h"
#include "Si2177_typedefs.h"

typedef struct {
    uint32_t freq;
    int16_t ft_offset;
    uint8_t tv_system;
} si2177_channel;

typedef struct {
    uint8_t audio_sys;
    uint8_t audio_demod_mode;
    uint8_t ch_idx;
    uint8_t cvbs_gain_sel;
    si2177_channel chlist[MAX_POSSIBLE_CHANNELS];
} si2177_config;

typedef struct {
    uint32_t i2cm_base;
    uint8_t i2c_addr;
    uint8_t powered_on;
    si2177_config cfg;
    L1_Si2177_Context l1_ctx;
    si2177_channel ch;
} si2177_dev;

int si2177_init(si2177_dev *dev);

void si2177_get_default_cfg(si2177_config *cfg);

void si2177_update_config(si2177_dev *dev, si2177_config *cfg);

int si2177_channelscan(si2177_dev *dev, si2177_channel *chlist, uint8_t tv_std_id_idx, uint32_t start_freq, uint32_t stop_freq);

int si2177_tune(si2177_dev *dev, si2177_channel *ch);

int si2177_fine_tune(si2177_dev *dev, si2177_channel *ch);

int si2177_set_audiomode(si2177_dev *dev, uint8_t audio_sys, uint8_t demod_mode);

int si2177_set_cvbs_params(si2177_dev *dev, uint8_t gain, uint8_t offset);

int si2177_set_video_eq(si2177_dev *dev, uint8_t eq_slope);

#endif /* SI2177_H_ */
