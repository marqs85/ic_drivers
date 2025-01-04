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

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "adv7280a.h"
#include "i2c_opencores.h"

#define SDP_PCNT_TOLERANCE 50

const adv7280a_config adv7280a_cfg_default = {
    .brightness = 128-32,
    .contrast = 128,
    .hue = 128,
    .sh_filt_y = 1,
    .comb_str_pal = 1,
    .comb_ctaps_pal = 2,
    .comb_ctaps_ntsc = 1,
    .cti_ab = 3,
    .cti_c_th = 8,
};

void adv7280a_writereg(adv7280a_dev *dev, uint8_t regaddr, uint8_t data) {
    I2C_start(dev->i2cm_base, dev->i2c_addr, 0);
    I2C_write(dev->i2cm_base, regaddr, 0);
    I2C_write(dev->i2cm_base, data, 1);
}

uint8_t adv7280a_readreg(adv7280a_dev *dev, uint8_t regaddr) {
    //Phase 1
    I2C_start(dev->i2cm_base, dev->i2c_addr, 0);
    I2C_write(dev->i2cm_base, regaddr, 0);

    //Phase 2
    I2C_start(dev->i2cm_base, dev->i2c_addr, 1);
    return I2C_read(dev->i2cm_base,1);
}

void adv7280a_get_default_cfg(adv7280a_config *cfg) {
    memcpy(cfg, &adv7280a_cfg_default, sizeof(adv7280a_config));
}

int adv7280a_init(adv7280a_dev *dev) {
    memcpy(&dev->cfg, &adv7280a_cfg_default, sizeof(adv7280a_config));

    if (adv7280a_readreg(dev, 0x11) != 0x43)
        return -1;

    // Set XTAL_TTL_SEL
    adv7280a_writereg(dev, 0x13, 0x04);

    // NTSC freerun
    /*adv7280a_writereg(dev, ADV7280A_INPUTCTRL, 0x07);
    adv7280a_writereg(dev, 0x0C, 0x37);
    adv7280a_writereg(dev, 0x02, 0x54);*/

    // PAL freerun
    /*adv7280a_writereg(dev, 0x0C, 0x37);
    adv7280a_writereg(dev, 0x02, 0x84);
    adv7280a_writereg(dev, ADV7280A_INPUTCTRL, 0x07);*/

    // Default input
    adv7280a_select_input(dev, ADV7280A_INPUT_YC_AIN34);

    adv7280a_writereg(dev, 0x14, 0x11);
    adv7280a_writereg(dev, ADV7280A_VIDSEL2, (dev->cfg.ntsc_pedestal<<4)|0x4);
    adv7280a_writereg(dev, 0x03, 0x0C);
    adv7280a_writereg(dev, 0x04, 0x87);
    adv7280a_writereg(dev, 0x17, 0x41);
    adv7280a_writereg(dev, 0x1D, 0x40);
    adv7280a_writereg(dev, 0x52, 0xCD);
    adv7280a_writereg(dev, 0x53, 0xCE);
    adv7280a_writereg(dev, 0x80, 0x51);
    adv7280a_writereg(dev, 0x81, 0x51);
    adv7280a_writereg(dev, 0x82, 0x68);

    /* Must disable NEWAVMODE to get real field info for non-interlace */
    adv7280a_writereg(dev, 0x6B, 0x01);
    adv7280a_writereg(dev, 0x31, 0x02);
    adv7280a_writereg(dev, 0x32, 0x81);
    adv7280a_writereg(dev, 0x33, 0x44);
    adv7280a_writereg(dev, 0x35, 0xa);

    // set default levels and filter settings
    adv7280a_set_levels(dev, dev->cfg.brightness, dev->cfg.contrast, dev->cfg.hue);
    adv7280a_set_shfilt(dev, dev->cfg.sh_filt_y, dev->cfg.sh_filt_c);
    adv7280a_set_cti(dev, dev->cfg.cti_ab, dev->cfg.cti_c_th);
    adv7280a_set_combfilt(dev, &dev->cfg);

    adv7280a_enable_power(dev, 0);

    return 0;
}

void adv7280a_enable_power(adv7280a_dev *dev, int enable) {
    adv7280a_writereg(dev, 0x0F, (!enable)<<5);
    dev->powered_on = enable;
}

void adv7280a_select_input(adv7280a_dev *dev, adv7280a_input input) {
    adv7280a_writereg(dev, ADV7280A_INPUTCTRL, input);
    adv7280a_writereg(dev, 0x0E, 0x80);
    adv7280a_writereg(dev, 0x9C, 0x00);
    adv7280a_writereg(dev, 0x9C, 0xFF);
    adv7280a_writereg(dev, 0x0E, 0x00);

    adv7280a_writereg(dev, 0x17, (input <= ADV7280A_INPUT_CVBS_AIN4) ? 0x41 : 0x01);
}

void adv7280a_set_pedestal(adv7280a_dev *dev, uint8_t ntsc_pedestal) {
    adv7280a_writereg(dev, ADV7280A_VIDSEL2, (ntsc_pedestal<<4)|0x4);
}

void adv7280a_set_levels(adv7280a_dev *dev, uint8_t brightness, uint8_t contrast, uint8_t hue) {
    adv7280a_writereg(dev, 0x0a, brightness-128);
    adv7280a_writereg(dev, 0x08, contrast);
    adv7280a_writereg(dev, 0x0b, hue-128);
}

void adv7280a_set_shfilt(adv7280a_dev *dev, uint8_t sh_filt_y, uint8_t sh_filt_c) {
    adv7280a_writereg(dev, 0x17, (sh_filt_c<<5) | sh_filt_y);
}

void adv7280a_set_cti(adv7280a_dev *dev, uint8_t cti_ab, uint8_t cti_c_th) {
    adv7280a_writereg(dev, 0x4d, (0xe3|(cti_ab<<2)));
    adv7280a_writereg(dev, 0x4e, cti_c_th);
}

void adv7280a_set_combfilt(adv7280a_dev *dev, adv7280a_config *cfg) {
    adv7280a_writereg(dev, 0x19, 0xf0 | (cfg->comb_str_ntsc<<2) | cfg->comb_str_pal);
    adv7280a_writereg(dev, 0x38, ((cfg->comb_ctaps_ntsc+1)<<6) | (((!!cfg->comb_cmode_ntsc<<2) | cfg->comb_cmode_ntsc)<<3) | ((!!cfg->comb_ymode_ntsc<<2) | cfg->comb_ymode_ntsc));
    adv7280a_writereg(dev, 0x39, ((cfg->comb_ctaps_pal+1)<<6) | (((!!cfg->comb_cmode_pal<<2) | cfg->comb_cmode_pal)<<3) | ((!!cfg->comb_ymode_pal<<2) | cfg->comb_ymode_pal));
}

int adv7280a_check_activity(adv7280a_dev *dev) {
    int activity_change = 0;
    uint8_t sync_active = (adv7280a_readreg(dev, 0x10) & 0x1);
    //uint8_t sync_active = (adv7280a_readreg(dev, 0x13) & 0x11) == 0x01;

    if (sync_active != dev->sync_active) {
        activity_change = 1;
        printf("adv7280a activity change\n");
    }

    dev->sync_active = sync_active;

    return activity_change;
}

int adv7280a_get_sync_stats(adv7280a_dev *dev, uint16_t vtotal, uint8_t interlace_flag, uint32_t pcnt_field) {
    int mode_changed = 0;

    if (((vtotal > 0) && (pcnt_field > 0)) &&
        ((vtotal != dev->ss.v_total) ||
        (interlace_flag != dev->ss.interlace_flag) ||
        (pcnt_field < (dev->ss.pcnt_field - SDP_PCNT_TOLERANCE)) ||
        (pcnt_field > (dev->ss.pcnt_field + SDP_PCNT_TOLERANCE))))
    {
        mode_changed = 1;

        printf("totlines: %u\n", vtotal);
        printf("interlace_flag: %u\n", interlace_flag);
        printf("pcnt_field: %lu\n", pcnt_field);
        //printf("interlaced: %u\n", !!(adv7280a_readreg(dev, 0x13) & 0x40));
    }

    dev->ss.v_total = vtotal;
    dev->ss.interlace_flag = interlace_flag;
    dev->ss.pcnt_field = pcnt_field;

    return mode_changed;
}

void adv7280a_update_config(adv7280a_dev *dev, adv7280a_config *cfg) {
    if (dev->powered_on) {
        if (cfg->ntsc_pedestal != dev->cfg.ntsc_pedestal)
            adv7280a_set_pedestal(dev, cfg->ntsc_pedestal);
        if ((cfg->brightness != dev->cfg.brightness) ||
            (cfg->contrast != dev->cfg.contrast) ||
            (cfg->hue != dev->cfg.hue))
            adv7280a_set_levels(dev, cfg->brightness, cfg->contrast, cfg->hue);
        if ((cfg->sh_filt_y != dev->cfg.sh_filt_y) || (cfg->sh_filt_c != dev->cfg.sh_filt_c))
            adv7280a_set_shfilt(dev, cfg->sh_filt_y, cfg->sh_filt_c);
        if (memcmp(&cfg->comb_str_pal, &dev->cfg.comb_str_pal, 8*sizeof(uint8_t)))
            adv7280a_set_combfilt(dev, cfg);
        if ((cfg->cti_ab != dev->cfg.cti_ab) || (cfg->cti_c_th != dev->cfg.cti_c_th))
            adv7280a_set_cti(dev, cfg->cti_ab, cfg->cti_c_th);

        memcpy(&dev->cfg, cfg, sizeof(adv7280a_config));
    }
}
