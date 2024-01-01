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

#ifndef ADV761X_REGS_H_
#define ADV761X_REGS_H_

// IO map
#define ADV761X_VIDEO_STD           0x00
#define ADV761X_PRIM_MODE           0x01
#define ADV761X_IO_REG_02           0x02
#define ADV761X_IO_REG_03           0x03
#define ADV761X_IO_REG_04           0x04
#define ADV761X_IO_REG_0C           0x0C
#define ADV761X_IO_REG_14           0x14
#define ADV761X_IO_REG_15           0x15
#define ADV761X_LLC_DLL             0x19
#define ADV761X_HPA_REG1            0x20
#define ADV761X_HPA_REG2            0x21
#define ADV761X_IO_REG_33           0x33
#define ADV761X_HDMI_LVL_RAW_STAT_3 0x6A
#define ADV761X_CEC_SLAVEADDR       0xF4
#define ADV761X_INFRM_SLAVEADDR     0xF5
#define ADV761X_DPLL_SLAVEADDR      0xF8
#define ADV761X_KSV_SLAVEADDR       0xF9
#define ADV761X_EDID_SLAVEADDR      0xFA
#define ADV761X_HDMI_SLAVEADDR      0xFB
#define ADV761X_CP_SLAVEADDR        0xFD

// HDMI map
#define ADV761X_HDMI_REG_00H        0x00
#define ADV761X_HDMI_REG_01H        0x01
#define ADV761X_HDMI_REG_03H        0x03
#define ADV761X_HDMI_REG_04H        0x04
#define ADV761X_HDMI_REG_05H        0x05
#define ADV761X_LINE_WIDTH_1        0x07
#define ADV761X_LINE_WIDTH_2        0x08
#define ADV761X_FIELD0_HEIGHT_1     0x09
#define ADV761X_FIELD0_HEIGHT_2     0x0A
#define ADV761X_FIELD1_HEIGHT_1     0x0B
#define ADV761X_FIELD1_HEIGHT_2     0x0C
#define ADV761X_HDMI_REG_0DH        0x0D
#define ADV761X_AUDIO_MUTE_MSK      0x14
#define ADV761X_TOTAL_LINE_WIDTH_1  0x1E
#define ADV761X_TOTAL_LINE_WIDTH_2  0x1F
#define ADV761X_HSYNC_PULSEWIDTH_1  0x22
#define ADV761X_HSYNC_PULSEWIDTH_2  0x23
#define ADV761X_HSYNC_BACKPORCH_1   0x24
#define ADV761X_HSYNC_BACKPORCH_2   0x25
#define ADV761X_FIELD0_TOT_HEIGHT_1 0x26
#define ADV761X_FIELD0_TOT_HEIGHT_2 0x27
#define ADV761X_FIELD1_TOT_HEIGHT_1 0x28
#define ADV761X_FIELD1_TOT_HEIGHT_2 0x29
#define ADV761X_FIELD0_VS_WIDTH_1   0x2E
#define ADV761X_FIELD0_VS_WIDTH_2   0x2F
#define ADV761X_FIELD1_VS_WIDTH_1   0x30
#define ADV761X_FIELD1_VS_WIDTH_2   0x31
#define ADV761X_FIELD0_VS_BPORCH_1  0x32
#define ADV761X_FIELD0_VS_BPORCH_2  0x33
#define ADV761X_FIELD1_VS_BPORCH_1  0x34
#define ADV761X_FIELD1_VS_BPORCH_2  0x35
#define ADV761X_IEC60958_DATA_1     0x36
#define ADV761X_IEC60958_DATA_2     0x37
#define ADV761X_IEC60958_DATA_3     0x38
#define ADV761X_IEC60958_DATA_4     0x39
#define ADV761X_IEC60958_DATA_5     0x3A
#define ADV761X_DEREP               0x41
#define ADV761X_HDMI_REG_47H        0x47
#define ADV761X_TMDSFREQ_1          0x51
#define ADV761X_TMDSFREQ_2          0x52
#define ADV761X_HPA_CFG_REG         0x6C
#define ADV761X_DST_MAP_REG         0x6E
#define ADV761X_HDMI_REGISTER_02H   0x83

// Infoframe map
#define ADV761X_AVI_INFOFRAME_DB5   0x05
#define ADV761X_AUD_INFOFRAME_DB1   0x1D
#define ADV761X_AUD_INFOFRAME_DB2   0x1E
#define ADV761X_AUD_INFOFRAME_DB3   0x1F
#define ADV761X_AUD_INFOFRAME_DB4   0x20
#define ADV761X_SPD_INFOFRAME_DB1   0x2B

#endif /* ADV761X_REGS_H_ */
