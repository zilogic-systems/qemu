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
#ifndef QEMU_ZKEY_H
#define QEMU_ZKEY_H

#include "hw/i2c/i2c.h"
#include "qemu/fifo8.h"

#define TYPE_ZKEY "zkey"
#define ZKEY(obj) OBJECT_CHECK(ZKeyState, (obj), TYPE_ZKEY)

/**
 * ZKeyState:
 * @chr: Character device to receive keys from
 * @interrupt: Interrupt line to raise interrupts
 */
typedef struct ZKeyState {
    /*< private >*/
    I2CSlave i2c;
    /*< public >*/

    CharDriverState *chr;
    qemu_irq interrupt;

    Fifo8 fifo;
} ZKeyState;

#endif
