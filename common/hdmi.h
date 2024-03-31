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
    HDMI_AVI_INFOFRAME_TYPE = 0x02,
    HDMI_SPD_INFOFRAME_TYPE = 0x03,
    HDMI_AUDIO_INFOFRAME_TYPE = 0x04,
    HDMI_MPEG_INFOFRAME_TYPE = 0x05,
    HDMI_HDR_INFOFRAME_TYPE = 0x07,
} HDMI_infoframe_id_t;

typedef enum {
    HDMI_VENDORSPEC_INFOFRAME_VER = 0x01,
    HDMI_AVI_INFOFRAME_VER        = 0x02,
    HDMI_SPD_INFOFRAME_VER        = 0x01,
    HDMI_AUDIO_INFOFRAME_VER      = 0x01,
    HDMI_MPEG_INFOFRAME_VER       = 0x01,
    HDMI_HDR_INFOFRAME_VER        = 0x01,
} HDMI_infoframe_ver_t;

typedef enum {
    HDMI_VENDORSPEC_INFOFRAME_LEN = 8,
    HDMI_AVI_INFOFRAME_LEN        = 13,
    HDMI_SPD_INFOFRAME_LEN        = 25,
    HDMI_AUDIO_INFOFRAME_LEN      = 10,
    HDMI_MPEG_INFOFRAME_LEN       = 10,
    HDMI_HDR_INFOFRAME_LEN        = 26,
} HDMI_infoframe_len_t;

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
    IEC60958_SAMPLE_LPCM = 0,
    IEC60958_SAMPLE_NONPCM = 1
} HDMI_audio_sample_type_t;

typedef enum {
    CC_HDR = 0,
    CC_2CH = 1,
    CC_3CH = 2,
    CC_4CH = 3,
    CC_5CH = 4,
    CC_6CH = 5,
    CC_7CH = 6,
    CC_8CH = 7
} HDMI_audio_cc_t;

typedef enum {
    CA_2p0 = 0x00,
    CA_4p0 = 0x08,
    CA_5p1 = 0x0b,
    CA_7p1 = 0x13,
    CA_SP_MSK = 0xfe,
    CA_CH_IDX = 0xff
} HDMI_audio_ca_t;

typedef enum {
    SF_STRM     = 0,
    SF_32KHZ    = 1,
    SF_44P1KHZ  = 2,
    SF_48KHZ    = 3,
    SF_88P2KHZ  = 4,
    SF_96KHZ    = 5,
    SF_176P4KHZ = 6,
    SF_192KHZ   = 7
} HDMI_audio_sf_t;

typedef enum {
    IEC60958_FS_32KHZ = 0x3,
    IEC60958_FS_44P1KHZ = 0x0,
    IEC60958_FS_48KHZ = 0x2,
    IEC60958_FS_88P2KHZ = 0x8,
    IEC60958_FS_96KHZ = 0xa,
    IEC60958_FS_176P4KHZ = 0xc,
    IEC60958_FS_192KHZ = 0xe,
    IEC60958_FS_768KHZ = 0x9
} HDMI_i2s_fs_t;

typedef enum {
    I2S_2CH_STEREO = 0,
    I2S_4CH_STEREO_4p0,
    I2S_4CH_STEREO_5p1,
    I2S_4CH_STEREO_7p1,
} HDMI_i2s_stereo_cfg_t;

#endif
