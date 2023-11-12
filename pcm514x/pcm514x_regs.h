//
// Copyright (C) 2023  Markus Hiienkari <mhiienka@niksula.hut.fi>
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

#ifndef PCM514X_REGS_H_
#define PCM514X_REGS_H_

#define PCM514X_PAGESEL         0
#define PCM514X_RESET_CTRL      1
#define PCM514X_POWERCTRL       2
#define PCM514X_MUTE            3
#define PCM514X_PLL_ENABLE      4
#define PCM514X_SPI_SEL         6
#define PCM514X_SDOUT_SEL       7
#define PCM514X_GPIO_SEL        8
#define PCM514X_I2S_MODE1       9
#define PCM514X_DSP_GPIOIN      10
#define PCM514X_I2S_DIV_RST     12
#define PCM514X_PLL_SRCSEL      13
#define PCM514X_DAC_SRCSEL      14
#define PCM514X_PLL_P           20
#define PCM514X_PLL_J           21
#define PCM514X_PLL_D_MSB       22
#define PCM514X_PLL_D_LSB       23
#define PCM514X_PLL_R           24
#define PCM514X_DSP_DIV         27
#define PCM514X_DAC_DIV         28
#define PCM514X_NCP_DIV         29
#define PCM514X_OSR_DIV         30
#define PCM514X_MM_DIV1         32
#define PCM514X_MM_DIV2         33
#define PCM514X_FS_SPEED        34
#define PCM514X_IDAC1           35
#define PCM514X_IDAC2           36
#define PCM514X_CLK_MON         37
#define PCM514X_I2S_CFG1        40
#define PCM514X_I2S_CFG2        41

#endif /* PCM514X_REGS_H_ */
