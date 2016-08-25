/*
 * Zilogic Systems I2C Keypad
 *
 * Copyright (C) 2016 Zilogic Systems
 * Written by Vijay Kumar <vijaykumar@zilogic.com>
 * And Deepak <deepak@zilogic.com>
 *
 * Based on tmp105.c
 *
 * Copyright (C) 2008 Nokia Corporation
 * Written by Andrzej Zaborowski <andrew@openedhand.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 or
 * (at your option) version 3 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, see <http://www.gnu.org/licenses/>.
 */

#include "qemu/osdep.h"
#include "hw/hw.h"
#include "hw/i2c/i2c.h"
#include "sysemu/char.h"
#include "hw/char/serial.h"

#include "zkey.h"

/*
 * I2C Callbacks
 */

static int data_mode;
static uint8_t address;

static void zkey_i2c_event(I2CSlave *i2c, enum i2c_event event)
{
    if (event == I2C_START_SEND)
        data_mode = 0;
}

static int zkey_recv(I2CSlave *i2c)
{
    ZKeyState *s = ZKEY(i2c);
    uint8_t key;

    switch (address) {
    case 0:
        if (!fifo8_is_empty(&s->fifo)) {
            key = fifo8_pop(&s->fifo);
            qemu_set_irq(s->interrupt, !fifo8_is_empty(&s->fifo));
            return key;
        } else {
            return 0;
        }
    case 1:
        return fifo8_num_used(&s->fifo);
    default:
        return 0;
    }
}

static int zkey_send(I2CSlave *i2c, uint8_t data)
{
    if (!data_mode) {
        address = data;
    }

    data_mode = !data_mode;

    return 0;
}

static void zkey_reset(I2CSlave *i2c)
{
    /* pass */
}

static int zkey_init(I2CSlave *i2c)
{
    ZKeyState *s = ZKEY(i2c);

    zkey_reset(&s->i2c);

    return 0;
}

static void zkey_initfn(Object *obj)
{
    ZKeyState *s = ZKEY(obj);
    DeviceState *dev = DEVICE(obj);

    /* FIXME: Need to destroy when done. */
    fifo8_create(&s->fifo, 16);
    qdev_init_gpio_out_named(dev, &s->interrupt, "interrupt", 1);
}

/*
 * Char. Callbacks
 */

static int zkey_can_rx(void *opaque)
{
    return true;
}

static void zkey_rx(void *opaque, const uint8_t *buf, int size)
{
    ZKeyState *s = ZKEY(opaque);
    fifo8_push_all(&s->fifo, buf, size);
    qemu_set_irq(s->interrupt, !fifo8_is_empty(&s->fifo));
}

static void zkey_event(void *opaque, int event)
{
    /* pass */
}

static void zkey_realize(DeviceState *dev, Error **errp)
{
    ZKeyState *s = ZKEY(dev);

    printf("zkey_realize() called %p %p\n", s, s->chr);

    if (s->chr)
        qemu_chr_add_handlers(s->chr, zkey_can_rx, zkey_rx, zkey_event, s);
}

static Property zkey_properties[] = {
    DEFINE_PROP_CHR("chardev", ZKeyState, chr),
    DEFINE_PROP_END_OF_LIST(),
};

static void zkey_class_init(ObjectClass *klass, void *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);
    I2CSlaveClass *k = I2C_SLAVE_CLASS(klass);

    k->init = zkey_init;
    k->event = zkey_i2c_event;
    k->recv = zkey_recv;
    k->send = zkey_send;
    dc->realize = zkey_realize;
    dc->props = zkey_properties;
}

static const TypeInfo zkey_info = {
    .name          = TYPE_ZKEY,
    .parent        = TYPE_I2C_SLAVE,
    .instance_size = sizeof(ZKeyState),
    .instance_init = zkey_initfn,
    .class_init    = zkey_class_init,
};

static void zkey_register_types(void)
{
    type_register_static(&zkey_info);
}

type_init(zkey_register_types)
