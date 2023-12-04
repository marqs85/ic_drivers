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

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "adv761x.h"
#include "i2c_opencores.h"

#define PCLK_HZ_TOLERANCE 1000000UL

const adv761x_config adv761x_cfg_default = {
    .default_rgb_range = ADV761X_RGB_LIMITED,
    .pixelderep_mode = 0
};

uint8_t adv761x_get_baseaddr(adv761x_dev *dev, adv761x_reg_map map) {
    switch (map) {
        case ADV761X_DPLL_MAP:
            return dev->dpll_base;
        case ADV761X_HDMI_MAP:
            return dev->hdmi_base;
        case ADV761X_KSV_MAP:
            return dev->ksv_base;
        case ADV761X_INFOFRAME_MAP:
            return dev->infoframe_base;
        case ADV761X_CP_MAP:
            return dev->cp_base;
        case ADV761X_CEC_MAP:
            return dev->cec_base;
        case ADV761X_EDID_MAP:
            return dev->edid_base;
        default: // default to IO_BASE
            return dev->io_base;
    }
}

void adv761x_writereg(adv761x_dev *dev, adv761x_reg_map map, uint8_t regaddr, uint8_t data)
{
    uint8_t baseaddr = adv761x_get_baseaddr(dev, map);

    I2C_start(dev->i2cm_base, (baseaddr>>1), 0);
    I2C_write(dev->i2cm_base, regaddr, 0);
    I2C_write(dev->i2cm_base, data, 1);
}

uint8_t adv761x_readreg(adv761x_dev *dev, adv761x_reg_map map, uint8_t regaddr)
{
    uint8_t baseaddr = adv761x_get_baseaddr(dev, map);

    //Phase 1
    I2C_start(dev->i2cm_base, (baseaddr>>1), 0);
    I2C_write(dev->i2cm_base, regaddr, 0);

    //Phase 2
    I2C_start(dev->i2cm_base, (baseaddr>>1), 1);
    return I2C_read(dev->i2cm_base,1);
}

void adv761x_init(adv761x_dev *dev) {
    int i;

    memcpy(&dev->cfg, &adv761x_cfg_default, sizeof(adv761x_config));

    // Set I2C mapping
    adv761x_writereg(dev, ADV761X_IO_MAP, ADV761X_CEC_SLAVEADDR, dev->cec_base);
    adv761x_writereg(dev, ADV761X_IO_MAP, ADV761X_INFRM_SLAVEADDR, dev->infoframe_base);
    adv761x_writereg(dev, ADV761X_IO_MAP, ADV761X_DPLL_SLAVEADDR, dev->dpll_base);
    adv761x_writereg(dev, ADV761X_IO_MAP, ADV761X_KSV_SLAVEADDR, dev->ksv_base);
    adv761x_writereg(dev, ADV761X_IO_MAP, ADV761X_EDID_SLAVEADDR, dev->edid_base);
    adv761x_writereg(dev, ADV761X_IO_MAP, ADV761X_HDMI_SLAVEADDR, dev->hdmi_base);
    adv761x_writereg(dev, ADV761X_IO_MAP, ADV761X_CP_SLAVEADDR, dev->cp_base);

    // ADI recommendations for ADV761X
    adv761x_writereg(dev, ADV761X_CP_MAP, 0x6c, 0x00);
    adv761x_writereg(dev, ADV761X_HDMI_MAP, 0x9b, 0x03);
    adv761x_writereg(dev, ADV761X_HDMI_MAP, 0x6f, 0x08);
    adv761x_writereg(dev, ADV761X_HDMI_MAP, 0x85, 0x1f);
    adv761x_writereg(dev, ADV761X_HDMI_MAP, 0x87, 0x70);
    adv761x_writereg(dev, ADV761X_HDMI_MAP, 0x57, 0xda);
    adv761x_writereg(dev, ADV761X_HDMI_MAP, 0x58, 0x01);
    adv761x_writereg(dev, ADV761X_HDMI_MAP, 0x03, 0x98);
    adv761x_writereg(dev, ADV761X_HDMI_MAP, 0x4c, 0x44);

    // set equalizer
    adv761x_writereg(dev, ADV761X_HDMI_MAP, 0x8d, 0x04);
    adv761x_writereg(dev, ADV761X_HDMI_MAP, 0x8e, 0x1e);
    adv761x_writereg(dev, ADV761X_HDMI_MAP, 0x96, 0x01);

    // power down
    adv761x_enable_power(dev, 0);

    // activate pads
    adv761x_writereg(dev, ADV761X_IO_MAP, ADV761X_IO_REG_15, 0x00);

    // Component order and XTAL freq select
    adv761x_writereg(dev, ADV761X_IO_MAP, ADV761X_IO_REG_04, 0x60);

    // pixel output port config
    adv761x_writereg(dev, ADV761X_IO_MAP, ADV761X_IO_REG_03, 0x40);

    // output color mode
    adv761x_writereg(dev, ADV761X_IO_MAP, ADV761X_IO_REG_02, 0xf2);

    // set free run mode 1080p
    adv761x_writereg(dev, ADV761X_IO_MAP, ADV761X_PRIM_MODE, 0x06);
    adv761x_writereg(dev, ADV761X_IO_MAP, ADV761X_VIDEO_STD, 0x19);

    // set HPA mode
    /*adv761x_writereg(dev, ADV761X_IO_MAP, ADV761X_HPA_REG1, 0x70);
    adv761x_writereg(dev, ADV761X_HDMI_MAP, ADV761X_HPA_CFG_REG, 0x23);
    usleep(1000000);*/
    adv761x_writereg(dev, ADV761X_HDMI_MAP, ADV761X_HPA_CFG_REG, 0x22);

    //program and enabled EDID
    for (i=0; i<dev->edid_len; i++)
        adv761x_writereg(dev, ADV761X_EDID_MAP, i, dev->edid[i]);
    adv761x_writereg(dev, ADV761X_KSV_MAP, 0x74, 0x01);

    // enable TMDS termination
    adv761x_writereg(dev, ADV761X_HDMI_MAP, ADV761X_HDMI_REGISTER_02H, 0x00);

    // set default RGB range
    adv761x_set_default_rgb_range(dev, dev->cfg.default_rgb_range);

    // set RX pixelderep mode
    adv761x_set_pixelderep(dev, dev->cfg.pixelderep_mode);

    // allow compressed audio
    adv761x_writereg(dev, ADV761X_HDMI_MAP, ADV761X_AUDIO_MUTE_MSK, 0x1f);

    adv761x_set_spdif_mux(dev, (dev->audio_sample_type == IEC60958_SAMPLE_NONPCM));
}

void adv761x_enable_power(adv761x_dev *dev, int enable) {
    adv761x_writereg(dev, ADV761X_IO_MAP, ADV761X_IO_REG_0C, (!enable)<<5);
    dev->powered_on = enable;
}

void adv761x_set_default_rgb_range(adv761x_dev *dev, adv761x_rgb_range rng) {
    adv761x_writereg(dev, ADV761X_HDMI_MAP, ADV761X_HDMI_REG_47H, ((1<<2)|(rng<<1)));
}

void adv761x_set_pixelderep(adv761x_dev *dev, uint8_t pixelderep_mode) {
    adv761x_writereg(dev, ADV761X_HDMI_MAP, ADV761X_DEREP, pixelderep_mode ? ((1<<6)|(1<<4)|(pixelderep_mode-1)) : (1<<6));
}

void adv761x_set_spdif_mux(adv761x_dev *dev, int enable) {
    adv761x_writereg(dev, ADV761X_HDMI_MAP, ADV761X_DST_MAP_REG, (0x04 | (enable<<3)));
}

void adv761x_set_input_cs(adv761x_dev *dev) {
    uint8_t regval, cs;

    regval = adv761x_readreg(dev, ADV761X_IO_MAP, ADV761X_IO_REG_02);
    cs = dev->hdmi_mode ? 0xf : 0x1;

    adv761x_writereg(dev, ADV761X_IO_MAP, ADV761X_IO_REG_02, ((cs<<4)|(regval & 0xf)));
}

int adv761x_check_activity(adv761x_dev *dev) {
    uint8_t sync_activity, sync_active;
    int activity_change = 0;

    /*retval = adv761x_readreg(ADV761X_IO_BASE, 0x6f);
    printf("+5V det: 0x%lx\n", retval);

    retval = adv761x_readreg(ADV761X_IO_BASE, ADV761X_HDMI_LVL_RAW_STAT_3);
    printf("TMDS det: 0x%lx\n", retval);

    retval = adv761x_readreg(ADV761X_IO_BASE, 0x21);
    printf("HPA stat: 0x%lx\n", retval);

    retval = adv761x_readreg(ADV761X_HDMI_BASE, 0x05);
    printf("HDMI stat: 0x%lx\n", retval);

    retval = adv761x_readreg(ADV761X_HDMI_BASE, 0x07);
    printf("VF stat: 0x%lx\n", retval);

    retval = adv761x_readreg(ADV761X_HDMI_BASE, 0x09);
    printf("Height: %d\n", ((retval&0x1f)<<8) | adv761x_readreg(ADV761X_HDMI_BASE, 0x0a));

    retval = adv761x_readreg(ADV761X_HDMI_BASE, 0x83);
    printf("term stat: 0x%lx\n", retval);

    retval = adv761x_readreg(ADV761X_KSV_BASE, 0x76);
    printf("edid enable: 0x%lx\n", retval);*/

    // check V_LOCKED_RAW
    sync_activity = adv761x_readreg(dev, ADV761X_IO_MAP, ADV761X_HDMI_LVL_RAW_STAT_3);
    sync_active = !!(sync_activity & 0x2);

    if (sync_active != dev->sync_active) {
        activity_change = 1;
        memset(&dev->ss, 0, sizeof(adv761x_sync_status));
        dev->pclk_hz = 0;
        dev->pixelderep = 0;
        dev->pixelderep_ifr = 0;

        printf("advrx activity: 0x%lx\n", sync_activity);
    }

    dev->sync_active = sync_active;

    return activity_change;
}

int adv761x_get_sync_stats(adv761x_dev *dev) {
    int mode_changed = 0;
    adv761x_sync_status ss = {0};
    uint32_t pclk_hz;
    uint8_t pixelderep, pixelderep_ifr, hdmi_mode;
    uint8_t regval;

    ss.interlace_flag = !!(adv761x_readreg(dev, ADV761X_HDMI_MAP, ADV761X_FIELD1_HEIGHT_1) & (1<<5));

    if (!ss.interlace_flag) {
        ss.v_total = (((adv761x_readreg(dev, ADV761X_HDMI_MAP, ADV761X_FIELD0_TOT_HEIGHT_1) & 0x3f) << 8) | adv761x_readreg(dev, ADV761X_HDMI_MAP, ADV761X_FIELD0_TOT_HEIGHT_2))/2;
        ss.v_synclen = (((adv761x_readreg(dev, ADV761X_HDMI_MAP, ADV761X_FIELD0_VS_WIDTH_1) & 0x3f) << 8) | adv761x_readreg(dev, ADV761X_HDMI_MAP, ADV761X_FIELD0_VS_WIDTH_2))/2;
        ss.v_backporch = (((adv761x_readreg(dev, ADV761X_HDMI_MAP, ADV761X_FIELD0_VS_BPORCH_1) & 0x3f) << 8) | adv761x_readreg(dev, ADV761X_HDMI_MAP, ADV761X_FIELD0_VS_BPORCH_2))/2;
        ss.v_active = (((adv761x_readreg(dev, ADV761X_HDMI_MAP, ADV761X_FIELD0_HEIGHT_1) & 0x1f) << 8) | adv761x_readreg(dev, ADV761X_HDMI_MAP, ADV761X_FIELD0_HEIGHT_2));
    } else {
        ss.v_total = ((((adv761x_readreg(dev, ADV761X_HDMI_MAP, ADV761X_FIELD0_TOT_HEIGHT_1) & 0x3f) << 8) | adv761x_readreg(dev, ADV761X_HDMI_MAP, ADV761X_FIELD0_TOT_HEIGHT_2)) +
                      (((adv761x_readreg(dev, ADV761X_HDMI_MAP, ADV761X_FIELD1_TOT_HEIGHT_1) & 0x3f) << 8) | adv761x_readreg(dev, ADV761X_HDMI_MAP, ADV761X_FIELD1_TOT_HEIGHT_2)))/2;
        ss.v_synclen = ((((adv761x_readreg(dev, ADV761X_HDMI_MAP, ADV761X_FIELD0_VS_WIDTH_1) & 0x3f) << 8) | adv761x_readreg(dev, ADV761X_HDMI_MAP, ADV761X_FIELD0_VS_WIDTH_2)) +
                        (((adv761x_readreg(dev, ADV761X_HDMI_MAP, ADV761X_FIELD1_VS_WIDTH_1) & 0x3f) << 8) | adv761x_readreg(dev, ADV761X_HDMI_MAP, ADV761X_FIELD1_VS_WIDTH_2)))/4;
        ss.v_backporch = ((((adv761x_readreg(dev, ADV761X_HDMI_MAP, ADV761X_FIELD0_VS_BPORCH_1) & 0x3f) << 8) | adv761x_readreg(dev, ADV761X_HDMI_MAP, ADV761X_FIELD0_VS_BPORCH_2)) +
                          (((adv761x_readreg(dev, ADV761X_HDMI_MAP, ADV761X_FIELD1_VS_BPORCH_1) & 0x3f) << 8) | adv761x_readreg(dev, ADV761X_HDMI_MAP, ADV761X_FIELD1_VS_BPORCH_2)))/4;
        ss.v_active = ((((adv761x_readreg(dev, ADV761X_HDMI_MAP, ADV761X_FIELD0_HEIGHT_1) & 0x1f) << 8) | adv761x_readreg(dev, ADV761X_HDMI_MAP, ADV761X_FIELD0_HEIGHT_2)) +
                       (((adv761x_readreg(dev, ADV761X_HDMI_MAP, ADV761X_FIELD1_HEIGHT_1) & 0x1f) << 8) | adv761x_readreg(dev, ADV761X_HDMI_MAP, ADV761X_FIELD1_HEIGHT_2)))/2;
    }

    ss.h_total = ((adv761x_readreg(dev, ADV761X_HDMI_MAP, ADV761X_TOTAL_LINE_WIDTH_1) & 0x3f) << 8) | adv761x_readreg(dev, ADV761X_HDMI_MAP, ADV761X_TOTAL_LINE_WIDTH_2);
    ss.h_synclen = ((adv761x_readreg(dev, ADV761X_HDMI_MAP, ADV761X_HSYNC_PULSEWIDTH_1) & 0x1f) << 8) | adv761x_readreg(dev, ADV761X_HDMI_MAP, ADV761X_HSYNC_PULSEWIDTH_2);
    ss.h_backporch = ((adv761x_readreg(dev, ADV761X_HDMI_MAP, ADV761X_HSYNC_BACKPORCH_1) & 0x1f) << 8) | adv761x_readreg(dev, ADV761X_HDMI_MAP, ADV761X_HSYNC_BACKPORCH_2);
    ss.h_active = ((adv761x_readreg(dev, ADV761X_HDMI_MAP, ADV761X_LINE_WIDTH_1) & 0x1f) << 8) | adv761x_readreg(dev, ADV761X_HDMI_MAP, ADV761X_LINE_WIDTH_2);

    regval = adv761x_readreg(dev, ADV761X_HDMI_MAP, ADV761X_HDMI_REG_05H);
    ss.h_polarity = !!(regval & (1<<5));
    ss.v_polarity = !!(regval & (1<<4));
    hdmi_mode = regval >> 7;

    pixelderep = regval & 0xf;
    if (hdmi_mode)
        pixelderep_ifr = adv761x_readreg(dev, ADV761X_INFOFRAME_MAP, ADV761X_AVI_INFOFRAME_DB5) & 0xf;
    else
        pixelderep_ifr = 0;

    regval = adv761x_readreg(dev, ADV761X_HDMI_MAP, ADV761X_TMDSFREQ_2);
    pclk_hz = (((adv761x_readreg(dev, ADV761X_HDMI_MAP, ADV761X_TMDSFREQ_1) << 1) | (regval >> 7))*1000000 + ((1000000*(regval & 0x7f)) / 128)) / (pixelderep + 1);

    // check if input is deepcolor
    if (hdmi_mode) {
        regval = adv761x_readreg(dev, ADV761X_HDMI_MAP, ADV761X_FIELD1_HEIGHT_1) >> 6;
        if (regval == 1)
            pclk_hz = (pclk_hz*4)/5;
        else if (regval == 2)
            pclk_hz = (pclk_hz*2)/3;
    }

    // Enforce manual de-repetition to ensure LLC is min. 13MHz as ADV761x output is not stable otherwise
    if (dev->cfg.pixelderep_mode == 0) {
        if ((pixelderep_ifr > 0) && (pclk_hz < 13000000UL))
            adv761x_set_pixelderep(dev, 1);
        else if ((pixelderep_ifr != pixelderep) && ((pclk_hz/(pixelderep_ifr+1)) > 13100000UL))
            adv761x_set_pixelderep(dev, 0);
    }

    if (memcmp(&ss, &dev->ss, sizeof(adv761x_sync_status)) ||
        (pclk_hz < (dev->pclk_hz - PCLK_HZ_TOLERANCE)) ||
        (pclk_hz > (dev->pclk_hz + PCLK_HZ_TOLERANCE)) ||
        (pixelderep != dev->pixelderep) ||
        (pixelderep_ifr != dev->pixelderep_ifr) ||
        (hdmi_mode != dev->hdmi_mode))
    {
        mode_changed = 1;

        if (pclk_hz <= 100000000UL)
            adv761x_writereg(dev, ADV761X_IO_MAP, ADV761X_IO_REG_14, 0x6a);
        else
            adv761x_writereg(dev, ADV761X_IO_MAP, ADV761X_IO_REG_14, 0x6e);

        printf("advrx h_total: %u\n", ss.h_total);
        printf("advrx h_synclen: %u\n", ss.h_synclen);
        printf("advrx h_backporch: %u\n", ss.h_backporch);
        printf("advrx h_active: %u\n", ss.h_active);
        printf("advrx v_total: %u\n", ss.v_total);
        printf("advrx v_synclen: %u\n", ss.v_synclen);
        printf("advrx v_backporch: %u\n", ss.v_backporch);
        printf("advrx v_active: %u\n", ss.v_active);
        printf("advrx sync polarities: H(%c) V(%c)\n", (ss.h_polarity ? '+' : '-'), (ss.v_polarity ? '+' : '-'));
        printf("advrx interlace_flag: %u\n", ss.interlace_flag);
        printf("advrx pclk: %luHz\n", pclk_hz);
        printf("advrx pixelderep: %u (IFR: %u)\n", pixelderep, pixelderep_ifr);
        printf("advrx hdmi_mode: %u\n", hdmi_mode);
    }

    memcpy(&dev->ss, &ss, sizeof(adv761x_sync_status));
    dev->pclk_hz = pclk_hz;
    dev->pixelderep = pixelderep;
    dev->pixelderep_ifr = pixelderep_ifr;
    dev->hdmi_mode = hdmi_mode;

    return mode_changed;
}

HDMI_audio_sample_type_t adv761x_get_audio_sample_type(adv761x_dev *dev) {
    return !!(adv761x_readreg(dev, ADV761X_HDMI_MAP, ADV761X_IEC60958_DATA_1) & (1<<1));
}

HDMI_i2s_fs_t adv761x_get_i2s_fs(adv761x_dev *dev) {
    return (adv761x_readreg(dev, ADV761X_HDMI_MAP, ADV761X_IEC60958_DATA_4) & 0xf);
}

HDMI_audio_cc_t adv761x_get_audio_cc(adv761x_dev *dev) {
    return (adv761x_readreg(dev, ADV761X_INFOFRAME_MAP, ADV761X_AUD_INFOFRAME_DB1) & 0x7);
}

HDMI_audio_ca_t adv761x_get_audio_ca(adv761x_dev *dev) {
    return adv761x_readreg(dev, ADV761X_INFOFRAME_MAP, ADV761X_AUD_INFOFRAME_DB4);
}

void adv761x_update_config(adv761x_dev *dev, adv761x_config *cfg) {
    HDMI_audio_sample_type_t audio_sample_type = adv761x_get_audio_sample_type(dev);

    if (cfg->default_rgb_range != dev->cfg.default_rgb_range)
        adv761x_set_default_rgb_range(dev, cfg->default_rgb_range);
    if (cfg->pixelderep_mode != dev->cfg.pixelderep_mode)
        adv761x_set_pixelderep(dev, cfg->pixelderep_mode);
    if (audio_sample_type != dev->audio_sample_type) {
        adv761x_set_spdif_mux(dev, (audio_sample_type == IEC60958_SAMPLE_NONPCM));
        dev->audio_sample_type = audio_sample_type;
    }

    memcpy(&dev->cfg, cfg, sizeof(adv761x_config));
}
