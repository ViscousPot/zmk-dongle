/*
 * Preshared BLE identities and LTKs so both dongles look like one device
 * to the halves. All four firmwares (dongle x N, left, right) ship with
 * matching key material and skip the SMP pairing handshake entirely.
 *
 * Generated 2026-05-14. Regenerating ANY key here requires re-flashing
 * settings_reset on every device before flashing new firmware, since the
 * peer-side bond will reference the old material.
 */

#ifndef PROTEUS_BONDS_H
#define PROTEUS_BONDS_H

#include <stdint.h>
#include <zephyr/bluetooth/addr.h>

/* MAC F0:01:DC:00:00:00 — top two bits of MSB are 11 => static random */
#define PROTEUS_ADDR_DONGLE_INIT { \
	.type = BT_ADDR_LE_RANDOM, \
	.a = { .val = { 0x00, 0x00, 0x00, 0xDC, 0x01, 0xF0 } } \
}

/* MAC F0:01:5E:00:00:01 */
#define PROTEUS_ADDR_LEFT_INIT { \
	.type = BT_ADDR_LE_RANDOM, \
	.a = { .val = { 0x01, 0x00, 0x00, 0x5E, 0x01, 0xF0 } } \
}

/* MAC F0:01:71:00:00:02 */
#define PROTEUS_ADDR_RIGHT_INIT { \
	.type = BT_ADDR_LE_RANDOM, \
	.a = { .val = { 0x02, 0x00, 0x00, 0x71, 0x01, 0xF0 } } \
}

#define PROTEUS_LTK_DONGLE_LEFT_INIT { \
	0x67, 0xe3, 0x19, 0x6c, 0x07, 0x61, 0x63, 0x3b, \
	0xad, 0xe7, 0xcb, 0x60, 0x7c, 0x58, 0x05, 0x5c  \
}

#define PROTEUS_LTK_DONGLE_RIGHT_INIT { \
	0xb4, 0x44, 0xc3, 0xa1, 0xe1, 0x50, 0x35, 0xc3, \
	0xfc, 0x50, 0x7b, 0x1d, 0xe6, 0x16, 0x76, 0x21  \
}

struct proteus_peer {
	bt_addr_le_t addr;
	uint8_t ltk[16];
};

#endif /* PROTEUS_BONDS_H */
