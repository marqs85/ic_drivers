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
#include "si2177.h"
#include "sysconfig.h"
#include "i2c_opencores.h"

const si2177_config si2177_cfg_default = {
    .tv_std = 0,
    .ch_idx = 0
};

void si2177_get_default_cfg(si2177_config *cfg) {
    memcpy(cfg, &si2177_cfg_default, sizeof(si2177_config));
}

int si2177_init(si2177_dev *dev) {
    unsigned char retval;

    memcpy(&dev->cfg, &si2177_cfg_default, sizeof(si2177_config));
    memset(&dev->ch, 0, sizeof(si2177_channel));
    dev->powered_on = 0;

    retval = Si2177_L1_API_Init(&dev->l1_ctx, dev->i2c_addr);
    if (retval != NO_Si2177_ERROR) {
        printf("L1 Init failed\n");
        return -1;
    }

    retval = Si2177_Init(&dev->l1_ctx);
    if (retval != NO_Si2177_ERROR) {
        printf("Init failed\n");
        return -1;
    }

    dev->powered_on = 1;

    return 0;
}

void si2177_update_config(si2177_dev *dev, si2177_config *cfg) {
    if (dev->powered_on) {
        if (memcmp(&dev->ch, &cfg->chlist[cfg->ch_idx], sizeof(si2177_channel)))
            si2177_tune(dev, &cfg->chlist[cfg->ch_idx]);

        memcpy(&dev->cfg, cfg, sizeof(si2177_config));
        memcpy(&dev->ch, &cfg->chlist[cfg->ch_idx], sizeof(si2177_channel));
    }
}

int si2177_channelscan(si2177_dev *dev, si2177_channel *chlist) {
    int retval, i;

    if (!dev->powered_on)
        return -1;

    printf("Channelscan %s - atv_rsq_rssi_threshold.lo: %u, atv_rsq_rssi_threshold.hi: %u, atv_rsq_snr_threshold.lo: %u, atv_rsq_snr_threshold.hi: %u\n",
            dev->cfg.tv_std ? "PAL" : "NTSC",
            dev->l1_ctx.prop->atv_rsq_rssi_threshold.lo, dev->l1_ctx.prop->atv_rsq_rssi_threshold.hi, dev->l1_ctx.prop->atv_rsq_snr_threshold.lo, dev->l1_ctx.prop->atv_rsq_snr_threshold.hi);

    if (dev->cfg.tv_std == 0) {
        retval = Si2177_ATV_Channel_Scan_M(&dev->l1_ctx,Si2177_TUNER_TUNE_FREQ_CMD_FREQ_MIN,Si2177_TUNER_TUNE_FREQ_CMD_FREQ_MAX,
                                dev->l1_ctx.prop->atv_rsq_rssi_threshold.lo,
                                dev->l1_ctx.prop->atv_rsq_rssi_threshold.hi,
                                dev->l1_ctx.prop->atv_rsq_snr_threshold.lo,
                                dev->l1_ctx.prop->atv_rsq_snr_threshold.hi);
    } else {
        retval = Si2177_ATV_Channel_Scan_PAL(&dev->l1_ctx,Si2177_TUNER_TUNE_FREQ_CMD_FREQ_MIN,Si2177_TUNER_TUNE_FREQ_CMD_FREQ_MAX,
                                dev->l1_ctx.prop->atv_rsq_rssi_threshold.lo,
                                dev->l1_ctx.prop->atv_rsq_rssi_threshold.hi,
                                dev->l1_ctx.prop->atv_rsq_snr_threshold.lo,
                                dev->l1_ctx.prop->atv_rsq_snr_threshold.hi);
    }

    if (retval != NO_Si2177_ERROR) {
        printf("Scan failed\n");
        return -1;
    }

    printf("%u Channels Found \n", dev->l1_ctx.ChannelListSize);
    printf("Num\tType\tChannel \n");
    for (i=0; i < dev->l1_ctx.ChannelListSize; i++) {
        printf("%d\t%s\t%ld\n",i, dev->l1_ctx.ChannelType[i], dev->l1_ctx.ChannelList[i]);
        chlist[i].freq = dev->l1_ctx.ChannelList[i];
        chlist[i].tv_system = (dev->l1_ctx.ChannelType[i][0] == 'M') ? Si2177_ATV_VIDEO_MODE_PROP_VIDEO_SYS_M : Si2177_ATV_VIDEO_MODE_PROP_VIDEO_SYS_B;
    }

    // Enforce re-tune of selected channel after scan
    memset(&dev->ch, 0, sizeof(si2177_channel));

    return dev->l1_ctx.ChannelListSize;
}

int si2177_tune(si2177_dev *dev, si2177_channel *ch) {
    int retval;

    if (!dev->powered_on) {
        return -1;
    } else if (!ch->freq) {
        printf("Invalid channel\n");
        return -1;
    }

    printf("Tuning to system %u, freq %lu\n", ch->tv_system, ch->freq);

    retval = Si2177_ATVTune(&dev->l1_ctx,
                             ch->freq,
                             ch->tv_system,
                             Si2177_ATV_VIDEO_MODE_PROP_COLOR_PAL_NTSC,
                             dev->l1_ctx.prop->atv_video_mode.invert_signal);

    if (retval != NO_Si2177_ERROR) {
        printf("Tune failed\n");
        return -1;
    }

    return 0;
}
