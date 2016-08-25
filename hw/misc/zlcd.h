/*
 * Zilogic Systems I2C based text LCD
 *
 * Copyright (C) 2016 Zilogic Systems <code@zilogic.com>
 *
 * Based on tmp105.h.
 *
 * Copyright (C) 2012 Alex Horn <alex.horn@cs.ox.ac.uk>
 * Copyright (C) 2008-2012 Andrzej Zaborowski <balrogg@gmail.com>
 *
 * This work is licensed under the terms of the GNU GPL, version 2 or
 * later. See the COPYING file in the top-level directory.
 */
#ifndef QEMU_ZLCD_H
#define QEMU_ZLCD_H

#include "hw/i2c/i2c.h"

#define TYPE_ZLCD "zlcd"
#define ZLCD(obj) OBJECT_CHECK(ZLCDState, (obj), TYPE_ZLCD)

/**
 * ZLCDState:
 * @chr: Character device to send LCD data
 */
typedef struct ZLCDState {
    /*< private >*/
    I2CSlave i2c;
    /*< public >*/

    CharDriverState *chr;
} ZLCDState;

#endif
