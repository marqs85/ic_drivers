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

const unsigned char tv_std_id_arr[] = { Si2177_ATV_VIDEO_MODE_PROP_VIDEO_SYS_M, Si2177_ATV_VIDEO_MODE_PROP_VIDEO_SYS_B, Si2177_ATV_VIDEO_MODE_PROP_VIDEO_SYS_I };
const char* const tv_std_name_arr[] = { "NTSC M", "PAL B/G/H", "PAL I" };
const uint8_t cvbs_gain_arr[] = {87, 200};
const uint8_t cvbs_offset = 25;

const si2177_config si2177_cfg_default = {
    .ch_idx = 0,
    /* ref. Si2177_L1_User_Properties.c */
    .audio_sys = Si2177_ATV_AUDIO_MODE_PROP_AUDIO_SYS_DEFAULT,
    .audio_demod_mode = Si2177_ATV_AUDIO_MODE_PROP_DEMOD_MODE_FM1-Si2177_ATV_AUDIO_MODE_PROP_DEMOD_MODE_AM
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
        if ((dev->cfg.audio_sys != cfg->audio_sys) || (dev->cfg.audio_demod_mode != cfg->audio_demod_mode))
            si2177_set_audiomode(dev, cfg->audio_sys, cfg->audio_demod_mode);
        if ((dev->ch.freq != cfg->chlist[cfg->ch_idx].freq) || (dev->ch.tv_system != cfg->chlist[cfg->ch_idx].tv_system))
            si2177_tune(dev, &cfg->chlist[cfg->ch_idx]);
        if (dev->ch.ft_offset != cfg->chlist[cfg->ch_idx].ft_offset)
            si2177_fine_tune(dev, &cfg->chlist[cfg->ch_idx]);
        if (dev->cfg.cvbs_gain_sel != cfg->cvbs_gain_sel)
            si2177_set_cvbs_params(dev, cvbs_gain_arr[cfg->cvbs_gain_sel], cvbs_offset);

        memcpy(&dev->cfg, cfg, sizeof(si2177_config));
        memcpy(&dev->ch, &cfg->chlist[cfg->ch_idx], sizeof(si2177_channel));
    }
}

int si2177_channelscan(si2177_dev *dev, si2177_channel *chlist, uint8_t tv_std_id_idx, uint32_t start_freq, uint32_t stop_freq) {
    int retval, i;

    if (!dev->powered_on)
        return -1;

    // Ensure user offset does not affect scan
    Si2177_L1_FINE_TUNE(&dev->l1_ctx, 1, 1, 0);

    printf("Channelscan %s (%uMHz-%uMHz) - atv_rsq_rssi_threshold.lo: %u, atv_rsq_rssi_threshold.hi: %u, atv_rsq_snr_threshold.lo: %u, atv_rsq_snr_threshold.hi: %u\n",
            tv_std_name_arr[tv_std_id_idx], start_freq/1000000, stop_freq/1000000,
            dev->l1_ctx.prop->atv_rsq_rssi_threshold.lo, dev->l1_ctx.prop->atv_rsq_rssi_threshold.hi, dev->l1_ctx.prop->atv_rsq_snr_threshold.lo, dev->l1_ctx.prop->atv_rsq_snr_threshold.hi);

    if (tv_std_id_arr[tv_std_id_idx] == Si2177_ATV_VIDEO_MODE_PROP_VIDEO_SYS_M) {
        retval = Si2177_ATV_Channel_Scan_M(&dev->l1_ctx, start_freq, stop_freq,
                                dev->l1_ctx.prop->atv_rsq_rssi_threshold.lo,
                                dev->l1_ctx.prop->atv_rsq_rssi_threshold.hi,
                                dev->l1_ctx.prop->atv_rsq_snr_threshold.lo,
                                dev->l1_ctx.prop->atv_rsq_snr_threshold.hi);
    } else {
        retval = Si2177_ATV_Channel_Scan_PAL(&dev->l1_ctx, tv_std_id_arr[tv_std_id_idx], start_freq, stop_freq,
                                dev->l1_ctx.prop->atv_rsq_rssi_threshold.lo,
                                dev->l1_ctx.prop->atv_rsq_rssi_threshold.hi,
                                dev->l1_ctx.prop->atv_rsq_snr_threshold.lo,
                                dev->l1_ctx.prop->atv_rsq_snr_threshold.hi);
    }

    // Restore params which are modified by scan
    dev->l1_ctx.prop->atv_video_mode.video_sys = tv_std_id_arr[tv_std_id_idx];
    dev->l1_ctx.prop->atv_video_mode.color     = Si2177_ATV_VIDEO_MODE_PROP_COLOR_PAL_NTSC;
    dev->l1_ctx.prop->atv_audio_mode.audio_sys = dev->cfg.audio_sys;
    dev->l1_ctx.prop->atv_afc_range.range_khz  = 1000;
    Si2177_L1_SetProperty2(&dev->l1_ctx, Si2177_ATV_VIDEO_MODE_PROP);
    Si2177_L1_SetProperty2(&dev->l1_ctx, Si2177_ATV_AUDIO_MODE_PROP);
    Si2177_L1_SetProperty2(&dev->l1_ctx, Si2177_ATV_AFC_RANGE_PROP);

    if (retval != NO_Si2177_ERROR) {
        printf("Scan failed\n");
        return -1;
    }

    printf("%u Channels Found \n", dev->l1_ctx.ChannelListSize);
    printf("Num\tType\tChannel \n");
    for (i=0; i < dev->l1_ctx.ChannelListSize; i++) {
        printf("%d\t%s\t%ld\n",i, dev->l1_ctx.ChannelType[i], dev->l1_ctx.ChannelList[i]);
        chlist[i].freq = dev->l1_ctx.ChannelList[i];
        chlist[i].ft_offset = 0;
        chlist[i].tv_system = tv_std_id_arr[tv_std_id_idx];
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

int si2177_fine_tune(si2177_dev *dev, si2177_channel *ch) {
    unsigned char retval;

    if (!dev->powered_on) {
        return -1;
    } else if (!ch->freq) {
        printf("Invalid channel\n");
        return -1;
    }

    printf("Fine-tuning with offset %dkHz\n", ch->ft_offset/2);

    retval = Si2177_L1_FINE_TUNE(&dev->l1_ctx, 1, 1, ch->ft_offset);

    if (retval != NO_Si2177_ERROR) {
        printf("Fine-tune failed\n");
        return -1;
    }

    return 0;
}

int si2177_set_audiomode(si2177_dev *dev, uint8_t audio_sys, uint8_t demod_mode) {
    dev->l1_ctx.prop->atv_audio_mode.audio_sys = audio_sys;
    dev->l1_ctx.prop->atv_audio_mode.demod_mode = demod_mode+Si2177_ATV_AUDIO_MODE_PROP_DEMOD_MODE_AM;

    Si2177_L1_SetProperty2(&dev->l1_ctx, Si2177_ATV_AUDIO_MODE_PROP);
    si2177_tune(dev, &dev->cfg.chlist[dev->cfg.ch_idx]);

    return 0;
}

int si2177_set_cvbs_params(si2177_dev *dev, uint8_t gain, uint8_t offset) {
    dev->l1_ctx.prop->atv_cvbs_out.amp = gain;
    dev->l1_ctx.prop->atv_cvbs_out.offset = offset;
    Si2177_L1_SetProperty2(&dev->l1_ctx, Si2177_ATV_CVBS_OUT_PROP);

    return 0;
}

#if 0
int si2177_set_video_eq(si2177_dev *dev, uint8_t eq_slope) {
    dev->l1_ctx.prop->atv_video_equalizer.slope = eq_slope-128;
    Si2177_L1_SetProperty2(&dev->l1_ctx, Si2177_ATV_VIDEO_EQUALIZER_PROP);

    return 0;
}
#endif
