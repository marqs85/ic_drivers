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
    .tx_mode = TX_HDMI_RGB_FULL,
    .audio_fmt = AUDIO_I2S,
    .i2s_fs = FS_48KHZ,
    .i2s_chcfg = SII_2CH_STEREO
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

void sii1136_get_default_cfg(sii1136_config *cfg) {
    memcpy(cfg, &sii1136_cfg_default, sizeof(sii1136_config));
}

void sii1136_init(sii1136_dev *dev) {
    uint8_t regval;

    memcpy(&dev->cfg, &sii1136_cfg_default, sizeof(sii1136_config));

    // Enable TPI mode
    sii1136_writereg(dev, SII1136_TPI_ENABLE, 0x00);

    // SW reset
    sii1136_writereg(dev, SII1136_TPI_CTRL, 0x81);
    usleep(1000);
    sii1136_writereg(dev, SII1136_TPI_CTRL, 0x80);

    // Power off TX
    sii1136_enable_power(dev, 0);

    // Set input format ro 8-bit RGB
    sii1136_writereg(dev, SII1136_INPUTFORMAT, 0x00);

    // Set I2S audio input config
    sii1136_writereg(dev, SII1136_I2S_IN_CFG, 0x80);

    // Disable and clear interrupts
    sii1136_writereg(dev, SII1136_IRQ_ENABLE, 0x00);
    sii1136_writereg(dev, SII1136_IRQ_STATUS, 0xF3);

    // Enable interrupts
    //sii1136_writereg(dev, SII1136_IRQ_ENABLE, 0xFB);

    sii1136_set_audio(dev, dev->cfg.audio_fmt, dev->cfg.i2s_fs, dev->cfg.i2s_chcfg);
    sii1136_set_tx_mode(dev, dev->cfg.tx_mode);
}

void sii1136_enable_power(sii1136_dev *dev, int enable) {
    if (enable) {
        // Return if already powered on
        if (dev->powered_on)
            return;

        // Enter D0 mode (power up TX)
        sii1136_writereg(dev, SII1136_PWRCTRL, 0x00);

        // Enable source termination
        sii1136_writereg(dev, SII1136_IDX_PAGE, 0x01);
        sii1136_writereg(dev, SII1136_IDX_OFFSET, 0x80);
        sii1136_writereg(dev, SII1136_IDX_RW, 0x24);

        dev->powered_on = 1;
    } else {
        // Return if already powered off
        /*if (!dev->powered_on)
            return;*/

        // SW reset
        /*sii1136_writereg(dev, SII1136_TPI_CTRL, 0x81);
        usleep(1000);
        sii1136_writereg(dev, SII1136_TPI_CTRL, 0x80);*/

        // Enter D2 mode (automatically disables TMDS output)
        sii1136_writereg(dev, SII1136_PWRCTRL, 0x02);

        dev->powered_on = 0;
    }
}

void sii1136_enable_tmds_output(sii1136_dev *dev, int enable) {
    uint8_t regval;

    regval = sii1136_readreg(dev, SII1136_SYSCTRL);

    if (enable)
        regval &= ~(1<<4);
    else
        regval |= (1<<4);

    sii1136_writereg(dev, SII1136_SYSCTRL, regval);
}

void sii1136_update_infoframe(sii1136_dev *dev, uint8_t type) {
    uint8_t ifr_reg;
    uint8_t crc;

    // Calculate CRC (type 0 is AVI Infoframe, type 1 is Audio Infoframe)
    if (type == 0) {
        crc = 0x82 + 0x02 + 13;

        for (ifr_reg=0x0D; ifr_reg<=0x19; ifr_reg++)
            crc += sii1136_readreg(dev, ifr_reg);

        crc = 0x100 - crc;

        sii1136_writereg(dev, 0x0C, crc);

        // commit update
        sii1136_writereg(dev, 0x19, sii1136_readreg(dev, 0x19));
    } else {
        crc = 0x84 + 0x01 + 10;

        for (ifr_reg=0xC4; ifr_reg<=0xCD; ifr_reg++)
            crc += sii1136_readreg(dev, ifr_reg);

        crc = 0x100 - crc;

        sii1136_writereg(dev, 0xC3, crc);

        // commit update
        sii1136_writereg(dev, 0xCD, sii1136_readreg(dev, 0xCD));
    }
}

void sii1136_set_audio(sii1136_dev *dev, HDMI_audio_fmt_t fmt, HDMI_i2s_fs_t i2s_fs, sii1136_i2s_chcfg_t i2s_chcfg) {
    int i;
    uint8_t fs_val=0, ca_val=0, cc_val=1;
    uint8_t regval;

    // Set audio mode (also selects correct register map) and mute
    sii1136_writereg(dev, SII1136_AUDIOMODE, (((fmt==AUDIO_I2S) ? 2 : 1) << 6) | (1<<5) | (1<<4));

    if (fmt == AUDIO_I2S) {
        switch (i2s_fs) {
            case FS_48KHZ:
                fs_val = 0x2;
                break;
            case FS_96KHZ:
                fs_val = 0xa;
                break;
            case FS_192KHZ:
                fs_val = 0xe;
                break;
            default:
                break;
        }

        switch (i2s_chcfg) {
            case SII_2CH_STEREO:
                ca_val = 0x00;
                cc_val = 0x1;
                break;
            case SII_4CH_STEREO_4p0:
                ca_val = 0x08;
                cc_val = 0x3;
                break;
            case SII_4CH_STEREO_5p1:
                ca_val = 0x0b;
                cc_val = 0x5;
                break;
            case SII_4CH_STEREO_7p1:
                ca_val = 0x13;
                cc_val = 0x7;
                break;
            default:
                break;
        }

        // Set I2S FIFO mapping (quad stereo)
        sii1136_writereg(dev, SII1136_I2S_IN_MAP, 0x80);
        sii1136_writereg(dev, SII1136_I2S_IN_MAP, 0x01);
        sii1136_writereg(dev, SII1136_I2S_IN_MAP, 0x82);
        sii1136_writereg(dev, SII1136_I2S_IN_MAP, 0x03);

        // Disable copyright protection
        sii1136_writereg(dev, SII1136_I2S_STAT, (1<<2));

        // Set category code
        sii1136_writereg(dev, SII1136_I2S_CATCODE, 0x20);

        // Set channel number
        sii1136_writereg(dev, SII1136_I2S_CHNUM, 0x00);

        // Clock accuracy and samplerate
        sii1136_writereg(dev, SII1136_I2S_ACC_SFS, fs_val);

        // 24-bit I2S audio
        sii1136_writereg(dev, SII1136_I2S_OFS_LEN, 0x0B);
    }

    // Setup audio infoframe
    sii1136_writereg(dev, 0xBF, 0x82);
    sii1136_writereg(dev, 0xC0, 0x84);
    sii1136_writereg(dev, 0xC1, 0x01);
    sii1136_writereg(dev, 0xC2, 10);
    sii1136_writereg(dev, 0xC4, cc_val);
    sii1136_writereg(dev, 0xC7, ca_val);

    // Commit audio infoframe update
    sii1136_update_infoframe(dev, 1);

    // Unmute audio
    regval = sii1136_readreg(dev, SII1136_AUDIOMODE);
    regval &= ~(1<<4);
    sii1136_writereg(dev, SII1136_AUDIOMODE, regval);
}

void sii1136_set_tx_mode(sii1136_dev *dev, HDMI_tx_mode_t mode) {
    uint8_t was_powered_on = dev->powered_on;
    uint8_t regval;

    // Set CSC
    if (mode == TX_HDMI_YCBCR444)
        sii1136_writereg(dev, SII1136_OUTPUTFORMAT, (1<<4) | 1);
    else if (mode == TX_HDMI_RGB_LIM)
        sii1136_writereg(dev, SII1136_OUTPUTFORMAT, (2<<2));
    else
        sii1136_writereg(dev, SII1136_OUTPUTFORMAT, 0);

    if (was_powered_on)
        sii1136_enable_power(dev, 0);

    // Set TX mode
    regval = sii1136_readreg(dev, SII1136_SYSCTRL);
    regval &= 0xfe;
    regval |= (mode != TX_DVI);
    sii1136_writereg(dev, SII1136_SYSCTRL, regval);

    if (was_powered_on) {
        sii1136_enable_power(dev, 1);
        sii1136_enable_tmds_output(dev, 1);
    }

    // Setup AVI InfoFrame
    sii1136_writereg(dev, 0x0D, 0x02 | ((mode==TX_HDMI_YCBCR444)<<6));  // RGB/YCbCr444, no overscan
    sii1136_writereg(dev, 0x0F, (mode==TX_HDMI_RGB_LIM) ? 0x04 : 0x08); // Full/limited range RGB

    // Commit AVI infoframe update
    sii1136_update_infoframe(dev, 0);
}

void sii1136_init_mode(sii1136_dev *dev, uint8_t pixelrep, uint8_t pixelrep_infoframe, HDMI_vic_t vic, uint32_t pclk_hz) {
    uint8_t regval;

    // Disable TMDS output (skip to avoid interruption)
    //sii1136_enable_tmds_output(dev, 0);

    // Set pclk
    sii1136_writereg(dev, SII1136_PCLK_LSB, (pclk_hz/10000) & 0xff);
    sii1136_writereg(dev, SII1136_PCLK_MSB, (pclk_hz/10000) >> 8);

    // Update AVI infoframe
    sii1136_writereg(dev, 0x10, vic);
    sii1136_writereg(dev, 0x11, pixelrep_infoframe);

    // Commit AVI infoframe update
    sii1136_update_infoframe(dev, 0);

    // Enable TMDS output
    sii1136_enable_tmds_output(dev, 1);

    // Set pixelrep
    sii1136_writereg(dev, SII1136_INPUTBUSFMT, ((1+pixelrep)<<6) | (1<<5));

    dev->pixelrep = pixelrep;
    dev->pixelrep_infoframe = pixelrep_infoframe;
    dev->vic = vic;
}

void sii1136_update_config(sii1136_dev *dev, sii1136_config *cfg) {
    if (dev->powered_on) {
        if (cfg->tx_mode != dev->cfg.tx_mode)
            sii1136_set_tx_mode(dev, cfg->tx_mode);
        if ((cfg->audio_fmt != dev->cfg.audio_fmt) ||
            (cfg->i2s_fs != dev->cfg.i2s_fs) ||
            (cfg->i2s_chcfg != dev->cfg.i2s_chcfg))
            sii1136_set_audio(dev, cfg->audio_fmt, cfg->i2s_fs, cfg->i2s_chcfg);

        memcpy(&dev->cfg, cfg, sizeof(sii1136_config));
    }
}
