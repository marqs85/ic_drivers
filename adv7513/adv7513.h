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
#include "adv7513_regs.h"

typedef enum tagHDMI_Video_Type {
    HDMI_Unknown = 0 ,
    HDMI_640x480p60 = 1 ,
    HDMI_480p60,
    HDMI_480p60_16x9,
    HDMI_720p60,
    HDMI_1080i60,
    HDMI_480i60_PR2x,
    HDMI_480i60_PR2x_16x9,
    HDMI_240p60_PR2x,
    HDMI_240p60_PR4x = 12,
    HDMI_1080p60 = 16,
    HDMI_576p50,
    HDMI_576p50_16x9,
    HDMI_720p50 = 19,
    HDMI_1080i50,
    HDMI_576i50,
    HDMI_576i50_16x9,
    HDMI_288p50,
    HDMI_1080p50 = 31,
    /*HDMI_1080p24,
    HDMI_1080p25,
    HDMI_1080p30,
    HDMI_1080i120 = 46,*/
} HDMI_Video_Type ;

typedef enum {
    TX_1X   = 0,
    TX_2X   = 1,
    TX_4X   = 2
} HDMI_pixelrep_t;

typedef enum {
    TX_HDMI_RGB_FULL = 0,
    TX_HDMI_RGB_LIM,
    TX_HDMI_YCBCR444,
    TX_DVI
} tx_mode_t;

typedef enum {
    CSC_RGB_FULL = 0,
    CSC_RGB_LIMITED,
    CSC_YCBCR_601,
    CSC_YCBCR_709,
} csc_colorspace_t;

typedef enum {
    ADV_AUDIO_I2S = 0,
    ADV_AUDIO_SPDIF
} adv7513_audio_fmt_t;

typedef enum {
    ADV_48KHZ = 0,
    ADV_96KHZ,
    ADV_192KHZ
} adv7513_i2s_fs_t;

typedef enum {
    ADV_2CH_STEREO = 0,
    ADV_4CH_STEREO_4p0,
    ADV_4CH_STEREO_5p1,
    ADV_4CH_STEREO_7p1,
} adv7513_i2s_chcfg_t;

typedef struct {
    tx_mode_t tx_mode;
    adv7513_audio_fmt_t audio_fmt;
    adv7513_i2s_fs_t i2s_fs;
    adv7513_i2s_chcfg_t i2s_chcfg;
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
    HDMI_Video_Type vic;
    adv7513_config cfg;
} adv7513_dev;

void adv7513_init(adv7513_dev *dev, uint8_t load_default_cfg);

void adv7513_get_default_cfg(adv7513_config *cfg);

void adv7513_set_audio(adv7513_dev *dev, adv7513_audio_fmt_t fmt, adv7513_i2s_fs_t i2s_fs, adv7513_i2s_chcfg_t i2s_chcfg);

void adv7513_set_tx_mode(adv7513_dev *dev, tx_mode_t mode);

void adv7513_set_csc_mode(adv7513_dev *dev, uint8_t enable, csc_colorspace_t src, csc_colorspace_t dst);

void adv7513_set_pixelrep_vic(adv7513_dev *dev, uint8_t pixelrep, uint8_t pixelrep_infoframe, HDMI_Video_Type vic);

int adv7513_check_hpd_power(adv7513_dev *dev);

void adv7513_update_config(adv7513_dev *dev, adv7513_config *cfg);

#endif /* ADV7513_H_ */
