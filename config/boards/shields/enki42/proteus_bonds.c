/*
 * Pre-seed BLE identity + bond keys before bt_enable() so the device boots
 * "already paired" to its split-BLE peer(s) without ever running SMP.
 *
 * Layout assumptions verified against zephyr v3.5.0+zmk-fixes:
 *   subsys/bluetooth/host/keys.h :: struct bt_keys
 * with the Kconfig profile ZMK uses at v0.3:
 *   BT_SMP_SC_PAIR_ONLY=y  => no periph_ltk field
 *   BT_SIGNING=n           => no CSRK fields
 *   BT_PRIVACY=n           => IRK present in struct but unused; we store zeros
 *   BT_KEYS_OVERWRITE_OLDEST=n => no aging_counter
 * Storage length is 52 bytes; BUILD_ASSERT below catches any drift.
 */

#include <stdint.h>
#include <string.h>
#include <errno.h>

#include <zephyr/init.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/util.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/addr.h>
#include <zephyr/settings/settings.h>
#include <zephyr/logging/log.h>

#include "proteus_bonds.h"

LOG_MODULE_REGISTER(proteus_bonds, CONFIG_ZMK_LOG_LEVEL);

/* Mirrors the on-flash layout of `struct bt_keys` storage_start onward. */
struct proteus_bond_storage {
	uint8_t  enc_size;
	uint8_t  flags;
	uint16_t key_types;
	struct {
		uint8_t rand[8];
		uint8_t ediv[2];
		uint8_t val[16];
	} ltk;
	struct {
		uint8_t val[16];
		uint8_t rpa[6];
	} irk;
};
BUILD_ASSERT(sizeof(struct proteus_bond_storage) == 52,
	     "Bond storage layout no longer matches Zephyr bt_keys");

/* Mirrors enum/flag values from subsys/bluetooth/host/keys.h */
#define PROT_KEYS_LTK_P256  BIT(5)  /* SC LTK */
#define PROT_KEYS_AUTH      BIT(0)
#define PROT_KEYS_SC        BIT(4)

/* Per-shield identity + peer table */
#if defined(CONFIG_SHIELD_ENKI42_DONGLE)
static const bt_addr_le_t proteus_own_addr = PROTEUS_ADDR_DONGLE_INIT;
static const struct proteus_peer proteus_peers[] = {
	{ .addr = PROTEUS_ADDR_LEFT_INIT,  .ltk = PROTEUS_LTK_DONGLE_LEFT_INIT  },
	{ .addr = PROTEUS_ADDR_RIGHT_INIT, .ltk = PROTEUS_LTK_DONGLE_RIGHT_INIT },
};
#elif defined(CONFIG_SHIELD_ENKI42_LEFT)
static const bt_addr_le_t proteus_own_addr = PROTEUS_ADDR_LEFT_INIT;
static const struct proteus_peer proteus_peers[] = {
	{ .addr = PROTEUS_ADDR_DONGLE_INIT, .ltk = PROTEUS_LTK_DONGLE_LEFT_INIT },
};
#elif defined(CONFIG_SHIELD_ENKI42_RIGHT)
static const bt_addr_le_t proteus_own_addr = PROTEUS_ADDR_RIGHT_INIT;
static const struct proteus_peer proteus_peers[] = {
	{ .addr = PROTEUS_ADDR_DONGLE_INIT, .ltk = PROTEUS_LTK_DONGLE_RIGHT_INIT },
};
#else
#error "proteus_bonds requires one of SHIELD_ENKI42_{DONGLE,LEFT,RIGHT}"
#endif

static int seed_bond(const struct proteus_peer *peer)
{
	/* bt_settings_encode_key format: bt/keys/<addr_msb_first><type>
	 * For default identity (id 0) no /id suffix is appended.
	 */
	char path[24];
	int n = snprintk(path, sizeof(path),
			 "bt/keys/%02x%02x%02x%02x%02x%02x%u",
			 peer->addr.a.val[5], peer->addr.a.val[4],
			 peer->addr.a.val[3], peer->addr.a.val[2],
			 peer->addr.a.val[1], peer->addr.a.val[0],
			 peer->addr.type);
	if (n < 0 || n >= (int)sizeof(path)) {
		return -ENAMETOOLONG;
	}

	struct proteus_bond_storage s = {
		.enc_size  = 16,
		.flags     = PROT_KEYS_AUTH | PROT_KEYS_SC,
		.key_types = PROT_KEYS_LTK_P256,
	};
	memcpy(s.ltk.val, peer->ltk, sizeof(s.ltk.val));

	return settings_save_one(path, &s, sizeof(s));
}

static int proteus_bonds_init(void)
{
	int err;

	/* settings_save_one needs the subsystem up. Idempotent — bt_enable
	 * also calls this during bt_settings_init.
	 */
	err = settings_subsys_init();
	if (err) {
		LOG_ERR("settings_subsys_init: %d", err);
		return 0;
	}

	for (size_t i = 0; i < ARRAY_SIZE(proteus_peers); i++) {
		err = seed_bond(&proteus_peers[i]);
		if (err) {
			LOG_WRN("seed_bond[%u]: %d", (unsigned)i, err);
		}
	}

	/* Lock identity 0 to our fixed address. Setting bt_dev.id_count > 0
	 * here causes bt_init to set BT_DEV_PRESET_ID, which makes the
	 * later settings_load skip any bt/id record from a prior boot.
	 */
	bt_addr_le_t addr = proteus_own_addr;
	err = bt_id_create(&addr, NULL);
	if (err < 0) {
		LOG_ERR("bt_id_create: %d", err);
	}

	return 0;
}

/* Must run BEFORE bt_enable(). ZMK's BLE init is at
 * APPLICATION priority 50 (CONFIG_ZMK_BLE_INIT_PRIORITY). SYS_INIT
 * stringifies its third arg into a section name, so it must be a literal
 * integer — arithmetic on the Kconfig symbol won't preprocess correctly.
 */
BUILD_ASSERT(CONFIG_ZMK_BLE_INIT_PRIORITY == 50,
	     "ZMK_BLE_INIT_PRIORITY changed; update SYS_INIT literal below");
SYS_INIT(proteus_bonds_init, APPLICATION, 49);
