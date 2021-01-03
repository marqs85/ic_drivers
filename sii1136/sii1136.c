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

#include <string.h>
#include <unistd.h>
#include "sii1136.h"
#include "i2c_opencores.h"

const sii1136_config sii1136_cfg_default = {
    .tx_mode = TX_HDMI_RGB_FULL
};

void sii1136_writereg(sii1136_dev *dev, uint8_t regaddr, uint8_t data)
{
    I2C_start(dev->i2cm_base, dev->i2c_addr, 0);
    I2C_write(dev->i2cm_base, regaddr, 0);
    I2C_write(dev->i2cm_base, data, 1);
}

uint8_t sii1136_readreg(sii1136_dev *dev, uint8_t regaddr)
{
    //Phase 1
    I2C_start(dev->i2cm_base, dev->i2c_addr, 0);
    I2C_write(dev->i2cm_base, regaddr, 0);

    //Phase 2
    I2C_start(dev->i2cm_base, dev->i2c_addr, 1);
    return I2C_read(dev->i2cm_base,1);
}

void sii1136_init(sii1136_dev *dev) {
    uint8_t regval;
    
    memcpy(&dev->cfg, &sii1136_cfg_default, sizeof(sii1136_config));

    // Setup fixed registers
    sii1136_writereg(dev, SII1136_TPI_ENABLE, 0x00);

    // set pclk to 148.5MHz
    //sii1136_writereg(dev, SII1136_PCLK_LSB, 0x02);
    //sii1136_writereg(dev, SII1136_PCLK_MSB, 0x3a);

    // Disable TMDS output
    sii1136_writereg(dev, SII1136_SYSCTRL, 0x11);

    // Set input format ro 8-bit RGB
    sii1136_writereg(dev, SII1136_INPUTFORMAT, 0x00);

    // Power up TX
    sii1136_writereg(dev, SII1136_PWRCTRL, 0x00);

    // Enable source termination
    sii1136_writereg(dev, SII1136_IDX_PAGE, 0x01);
    sii1136_writereg(dev, SII1136_IDX_OFFSET, 0x80);
    sii1136_writereg(dev, SII1136_IDX_RW, 0x24);

    // ???
    //sii1136_writereg(dev, 0x19, 0x01);

    // Disable and clear interrupts
    sii1136_writereg(dev, SII1136_IRQ_ENABLE, 0x00);
    sii1136_writereg(dev, SII1136_IRQ_STATUS, 0xF3);

    // Disable audio output?
    sii1136_writereg(dev, SII1136_AUDIOMODE, 0x91);

    // Set audio input config
    sii1136_writereg(dev, SII1136_I2S_IN_CFG, 0xF0);

    // Set audio to I2S stereo 48kHz
    sii1136_writereg(dev, SII1136_I2S_IN_MAP, 0x80);

    // 24-bit audio
    sii1136_writereg(dev, SII1136_I2S_OFS_LEN, 0x0B);

    // Disable audio
    sii1136_writereg(dev, SII1136_AUDIOMODE, 0x01);

    // Set audio params
    sii1136_writereg(dev, 0x27, 0x00);

    // Enable audio (I2S)
    sii1136_writereg(dev, SII1136_AUDIOMODE, 0x81);

    // vality bit?
    /*sii1136_writereg(dev, SII1136_IDX_PAGE, 0x02);
    sii1136_writereg(dev, SII1136_IDX_OFFSET, 0x1D);
    regval = sii1136_readreg(dev, SII1136_IDX_RW);
    regval ^= 0x10;
    sii1136_writereg(dev, SII1136_IDX_RW, regval);*/

    // Setup InfoFrame
    sii1136_writereg(dev, 0x0D, 0x02 | (0<<6));  // RGB/YCbCr444, no overscan
    sii1136_writereg(dev, 0x0F, 0 ? 0x04 : 0x08); // Full/limited range RGB

    // Enable TMDS output
    //sii1136_writereg(dev, SII1136_SYSCTRL, 0x01);

    // Enable interrupts
    //sii1136_writereg(dev, SII1136_IRQ_ENABLE, 0xFB);  
}

void sii1136_update_infoframe_csum(sii1136_dev *dev) {
    uint8_t ifr_reg;
    uint8_t crc = 0x82 + 0x02 + 13;

    for (ifr_reg=0x0D; ifr_reg<=0x19; ifr_reg++)
        crc += sii1136_readreg(dev, ifr_reg);

    crc = 0x100 - crc;

    sii1136_writereg(dev, 0x0C, crc);
}

void sii1136_init_mode(sii1136_dev *dev, uint32_t pclk_hz) {

    // Disable TMDS output
    sii1136_writereg(dev, SII1136_SYSCTRL, 0x11);

    // set pclk
    sii1136_writereg(dev, SII1136_PCLK_LSB, (pclk_hz/10000) & 0xff);
    sii1136_writereg(dev, SII1136_PCLK_MSB, (pclk_hz/10000) >> 8);

    // commit infoframe update
    sii1136_update_infoframe_csum(dev);
    sii1136_writereg(dev, 0x19, sii1136_readreg(dev, 0x19));

    // Enable TMDS output
    sii1136_writereg(dev, SII1136_SYSCTRL, 0x01);

    // Set pixel rep
    sii1136_writereg(dev, SII1136_INPUTBUSFMT, 0x60);
}

void sii1136_get_default_cfg(sii1136_config *cfg) {
    memcpy(cfg, &sii1136_cfg_default, sizeof(sii1136_config));
}

void sii1136_update_config(sii1136_dev *dev, sii1136_config *cfg) {
    /*if (dev->powered_on) {
        if (cfg->tx_mode != dev->cfg.tx_mode)
            adv7513_set_tx_mode(dev, cfg->tx_mode);
        if ((cfg->audio_fmt != dev->cfg.audio_fmt) ||
            (cfg->i2s_fs != dev->cfg.i2s_fs) ||
            (cfg->i2s_chcfg != dev->cfg.i2s_chcfg))
            adv7513_set_audio(dev, cfg->audio_fmt, cfg->i2s_fs, cfg->i2s_chcfg);

        memcpy(&dev->cfg, cfg, sizeof(adv7513_config));
    }*/
}
