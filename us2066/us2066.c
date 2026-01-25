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

#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include "us2066.h"
#include "i2c_opencores.h"

#define WRDELAY     20
#define CLEARDELAY  800


const us2066_config us2066_cfg_default = {
    .contrast = 127,
    .fade = 0
};

static void us2066_cmd(us2066_dev *dev, uint8_t cmd, uint16_t postdelay) {
    I2C_start(dev->i2cm_base, dev->i2c_addr, 0);
    I2C_write(dev->i2cm_base, 0x80, 0);
    I2C_write(dev->i2cm_base, cmd, 1);

    usleep(postdelay);
}

static void us2066_data(us2066_dev *dev, uint8_t data, uint16_t postdelay) {
    I2C_start(dev->i2cm_base, dev->i2c_addr, 0);
    I2C_write(dev->i2cm_base, 0xC0, 0);
    I2C_write(dev->i2cm_base, data, 1);

    usleep(postdelay);
}

static inline uint8_t us2066_get_fade_cmd(uint8_t cont, uint8_t fade) {
    if (fade == 0)
        return 0;
    else if (cont < 16)
        return (1<<5)|(15>>(cont/8));
    else
        return (1<<5)|((4-(cont>>5))+(3*fade)-1);
}

void us2066_init(us2066_dev *dev)
{
    memcpy(&dev->cfg, &us2066_cfg_default, sizeof(us2066_config));

    us2066_cmd(dev, 0x2A, WRDELAY);      // function set (extended command set)
    us2066_cmd(dev, 0x71, WRDELAY);      // function selection A
    us2066_data(dev, 0x00, WRDELAY);     // disable internal VDD regulator

    us2066_cmd(dev, 0x28, WRDELAY);      // function set (fundamental command set)
    us2066_cmd(dev, 0x08, WRDELAY);      // display off, cursor off, blink off
    us2066_cmd(dev, 0x2A, WRDELAY);      // function set (extended command set)
    us2066_cmd(dev, 0x79, WRDELAY);      // OLED command set enabled
    us2066_cmd(dev, 0xD5, WRDELAY);      // set display clock divide ratio/oscillator frequency
    us2066_cmd(dev, 0x70, WRDELAY);      // set display clock divide ratio/oscillator frequency
    us2066_cmd(dev, 0x78, WRDELAY);      // OLED command set disabled
    us2066_cmd(dev, 0x08, WRDELAY);      // extended function set (2-lines)
    us2066_cmd(dev, 0x06, WRDELAY);      // COM SEG direction
    us2066_cmd(dev, 0x72, WRDELAY);      // function selection B
    us2066_data(dev, 0x08, WRDELAY);     // ROM CGRAM selection (ROM C)

    us2066_cmd(dev, 0x2A, WRDELAY);      // function set (extended command set)
    us2066_cmd(dev, 0x79, WRDELAY);      // OLED command set enabled
    us2066_cmd(dev, 0xDA, WRDELAY);      // set SEG pins hardware configuration
    us2066_cmd(dev, 0x10, WRDELAY);      // set SEG pins hardware configuration
    us2066_cmd(dev, 0xDC, WRDELAY);      // function selection C
    us2066_cmd(dev, 0x00, WRDELAY);      // function selection C
    us2066_cmd(dev, 0x81, WRDELAY);      // set contrast control
    us2066_cmd(dev, us2066_cfg_default.contrast, WRDELAY);
    us2066_cmd(dev, 0x23, WRDELAY);      // set fade control
    us2066_cmd(dev, us2066_get_fade_cmd(us2066_cfg_default.contrast, us2066_cfg_default.fade), WRDELAY);
    us2066_cmd(dev, 0xD9, WRDELAY);      // set phase length
    us2066_cmd(dev, 0xF1, WRDELAY);      // set phase length
    us2066_cmd(dev, 0xDB, WRDELAY);      // set VCOMH deselect level
    us2066_cmd(dev, 0x40, WRDELAY);      // set VCOMH deselect level
    us2066_cmd(dev, 0x78, WRDELAY);      // OLED command set disabled

    us2066_cmd(dev, 0x28, WRDELAY);      // function set (fundamental command set)
    us2066_cmd(dev, 0x01, CLEARDELAY);   // clear display
    us2066_cmd(dev, 0x80, WRDELAY);      // set DDRAM address to 0x00
}

void us2066_get_default_cfg(us2066_config *cfg) {
    memcpy(cfg, &us2066_cfg_default, sizeof(us2066_config));
}

void us2066_display_on(us2066_dev *dev)
{
    us2066_cmd(dev, 0x0C, WRDELAY);      // display ON
}

void us2066_set_contrast_fade(us2066_dev *dev, uint8_t cont, uint8_t fade)
{
    us2066_cmd(dev, 0x2A, WRDELAY); // function set (extended command set)
    us2066_cmd(dev, 0x79, WRDELAY); // OLED command set enabled
    us2066_cmd(dev, 0x81, WRDELAY);
    us2066_cmd(dev, cont, WRDELAY);
    us2066_cmd(dev, 0x23, WRDELAY);
    us2066_cmd(dev, 0x00, WRDELAY);
    us2066_cmd(dev, 0x23, WRDELAY);
    us2066_cmd(dev, us2066_get_fade_cmd(cont, fade), WRDELAY);
    us2066_cmd(dev, 0x78, WRDELAY); // OLED command set disabled
    us2066_cmd(dev, 0x28, WRDELAY); // function set (fundamental command set)
}

void us2066_write(us2066_dev *dev, char *row1, char *row2)
{
    uint8_t i, rowlen;

    us2066_cmd(dev, 0x01, CLEARDELAY);   // clear display

    // reset fade status
    if (dev->cfg.fade != 0)
        us2066_set_contrast_fade(dev, dev->cfg.contrast, dev->cfg.fade);

    //ensure no empty row
    rowlen = strnlen(row1, US2066_ROW_LEN);
    if (rowlen == 0) {
        strncpy(row1, " ", US2066_ROW_LEN+1);
        rowlen++;
    }

    for (i=0; i<rowlen; i++)
        us2066_data(dev, row1[i], WRDELAY);

    // change to second row
    us2066_cmd(dev, 0xC0, WRDELAY);

    //ensure no empty row
    rowlen = strnlen(row2, US2066_ROW_LEN);
    if (rowlen == 0) {
        strncpy(row2, " ", US2066_ROW_LEN+1);
        rowlen++;
    }

    for (i=0; i<rowlen; i++)
        us2066_data(dev, row2[i], WRDELAY);
}

void us2066_update_config(us2066_dev *dev, us2066_config *cfg) {
    if ((cfg->contrast != dev->cfg.contrast) || (cfg->fade != dev->cfg.fade))
        us2066_set_contrast_fade(dev, cfg->contrast, cfg->fade);

    memcpy(&dev->cfg, cfg, sizeof(us2066_config));
}
