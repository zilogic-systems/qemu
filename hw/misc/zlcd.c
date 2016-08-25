/*
 * Zilogic Systems I2C Text LCD
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
#include "zlcd.h"

/*
 * I2C Callbacks
 */

static int data_mode;

static void zlcd_i2c_event(I2CSlave *i2c, enum i2c_event event)
{
    if (event == I2C_START_SEND)
        data_mode = 0;
}

static int zlcd_recv(I2CSlave *i2c)
{
    return 0;
}

static int zlcd_send(I2CSlave *i2c, uint8_t data)
{
    ZLCDState *s = ZLCD(i2c);
    static uint8_t address;
    uint8_t value;

    if (!data_mode) {
        address = data;
    } else {
        switch (address) {
        case 0x00:
            value = data;
            break;
        case 0x01:
        default:
            value = 0xFF;
            break;
        }

        if (s->chr)
            qemu_chr_fe_write(s->chr, &value, 1);
    }

    data_mode = !data_mode;

    return 0;
}

static void zlcd_reset(I2CSlave *i2c)
{
    /* pass */
}

static int zlcd_init(I2CSlave *i2c)
{
    ZLCDState *s = ZLCD(i2c);

    zlcd_reset(&s->i2c);

    return 0;
}

static void zlcd_initfn(Object *obj)
{
    /* pass */
}

/*
 * Char. Callbacks
 */

static int zlcd_can_rx(void *opaque)
{
    return false;
}

static void zlcd_rx(void *opaque, const uint8_t *buf, int size)
{
    /* pass */
}

static void zlcd_event(void *opaque, int event)
{
    /* pass */
}

static void zlcd_realize(DeviceState *dev, Error **errp)
{
    ZLCDState *s = ZLCD(dev);

    printf("zlcd_realize() called %p %p\n", s, s->chr);

    if (s->chr)
        qemu_chr_add_handlers(s->chr, zlcd_can_rx, zlcd_rx, zlcd_event, s);
}

static Property zlcd_properties[] = {
    DEFINE_PROP_CHR("chardev", ZLCDState, chr),
    DEFINE_PROP_END_OF_LIST(),
};

static void zlcd_class_init(ObjectClass *klass, void *data)
{
    DeviceClass *dc = DEVICE_CLASS(klass);
    I2CSlaveClass *k = I2C_SLAVE_CLASS(klass);

    k->init = zlcd_init;
    k->event = zlcd_i2c_event;
    k->recv = zlcd_recv;
    k->send = zlcd_send;
    dc->realize = zlcd_realize;
    dc->props = zlcd_properties;
}

static const TypeInfo zlcd_info = {
    .name          = TYPE_ZLCD,
    .parent        = TYPE_I2C_SLAVE,
    .instance_size = sizeof(ZLCDState),
    .instance_init = zlcd_initfn,
    .class_init    = zlcd_class_init,
};

static void zlcd_register_types(void)
{
    type_register_static(&zlcd_info);
}

type_init(zlcd_register_types)
