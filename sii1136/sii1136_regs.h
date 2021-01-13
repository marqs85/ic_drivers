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

#ifndef SII1136_REGS_H_
#define SII1136_REGS_H_

#define SII1136_PCLK_LSB                0x00
#define SII1136_PCLK_MSB                0x01

#define SII1136_INPUTBUSFMT             0x08
#define SII1136_INPUTFORMAT             0x09
#define SII1136_OUTPUTFORMAT            0x0A
#define SII1136_YC_MODESEL              0x0B

#define SII1136_SYSCTRL                 0x1A
#define SII1136_DEVICE_ID               0x1B
#define SII1136_PWRCTRL                 0x1E

// audio configuration (0x1F...0x28)
#define SII1136_I2S_IN_MAP              0x1F
#define SII1136_I2S_IN_CFG              0x20

#define SII1136_I2S_STAT                0x21
#define SII1136_I2S_CATCODE             0x22
#define SII1136_I2S_CHNUM               0x23
#define SII1136_I2S_ACC_SFS             0x24
#define SII1136_I2S_OFS_LEN             0x25

#define SII1136_SPDIF_HDR               0x24
#define SII1136_SPDIF_PROC              0x25

#define SII1136_AUDIOMODE               0x26

// interrupt registers
#define SII1136_IRQ_ENABLE              0x3C
#define SII1136_IRQ_STATUS              0x3D

#define SII1136_TPI_CTRL                0x40

#define SII1136_SYNCGENCTRL             0x60

// registers for indexed register accesses
#define SII1136_IDX_PAGE                0xBC
#define SII1136_IDX_OFFSET              0xBD
#define SII1136_IDX_RW                  0xBE

#define SII1136_TPI_ENABLE              0xC7

#endif /* SII1136_REGS_H_ */
