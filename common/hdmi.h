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

#ifndef HDMI_H_
#define HDMI_H_

typedef enum {
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
} HDMI_vic_t;

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
} HDMI_tx_mode_t;

typedef enum {
    CS_RGB_FULL = 0,
    CS_RGB_LIMITED,
    CS_YCBCR_601,
    CS_YCBCR_709,
} HDMI_colorspace_t;

typedef enum {
    AUDIO_I2S = 0,
    AUDIO_SPDIF
} HDMI_audio_fmt_t;

typedef enum {
    FS_48KHZ = 0,
    FS_96KHZ,
    FS_192KHZ
} HDMI_i2s_fs_t;

#endif
