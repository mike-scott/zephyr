/*
 * Copyright (c) 2019 Foundries.io
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#define LOG_MODULE_NAME net_lwm2m_setttings
#define LOG_LEVEL CONFIG_LWM2M_LOG_LEVEL

#include <logging/log.h>
LOG_MODULE_REGISTER(LOG_MODULE_NAME);

#include <kernel.h>
#include <errno.h>
#include <logging/log.h>
#include <settings/settings.h>

#include "lwm2m_settings.h"

/*
 * For setting names we use the modified path such as
 * light control on/off: lwm2m/3311-0-5850.
 * For the setting values we use strings, similar to how the values are
 * set via
 */

int lwm2m_try_persist_setting(u16_t obj_id, u16_t obj_inst_id, u16_t res_id,
			      struct lwm2m_engine_obj_field *obj_field,
			      void *data_ptr, size_t data_len)
{
	char setting_path[MAX_RESOURCE_LEN + sizeof(LWM2M_SETTINGS_PREFIX) + 1];
	int ret = 0;

	if (LWM2M_HAS_PERM(obj_field, BIT(LWM2M_FLAG_PERSIST))) {
		snprintk(setting_path, sizeof(setting_path), "%s/%u-%u-%u",
			 LWM2M_SETTINGS_PREFIX, obj_id, obj_inst_id, res_id);
		ret = settings_save_one(setting_path, data_ptr, data_len);
	}

	return ret;
}

static int set(int argc, char **argv, void *val_ctx)
{
	struct lwm2m_obj_path path;
	struct lwm2m_engine_res_inst *res = NULL;
	char pathstr[MAX_RESOURCE_LEN];
	int ret = 0;
	void *data_ptr;
	size_t data_len;
	u16_t len;
	u8_t data_flags;

	if (argc != 1) {
		return -ENOENT;
	}

	/* parse argv for LwM2M path */
	path.level = 3;
	path.res_inst_id = 0;
	ret = lwm2m_string_to_path(argv[0], &path, '-');
	if (ret < 0) {
		LOG_ERR("Invalid path from settings: %s", argv[0]);
		return ret;
	}

	/* look up resource obj */
	ret = lwm2m_path_to_objs(&path, NULL, NULL, &res);
	if (ret < 0) {
		return ret;
	}

	snprintk(pathstr, sizeof(pathstr), "%u/%u/%u",
		 path.obj_id, path.obj_inst_id, path.res_id);

	ret = lwm2m_engine_get_res_data(pathstr, &data_ptr, &len, &data_flags);
	if (ret < 0) {
		LOG_ERR("Error getting resource[%s] data pointer: %d",
			pathstr, ret);
		return ret;
	}

	data_len = len;
	if (LWM2M_HAS_RES_FLAG(res, LWM2M_RES_DATA_FLAG_RO)) {
		LOG_ERR("res data pointer is read-only");
		return -EACCES;
	}

	/* allow user to override data elements via callback */
	if (res->pre_write_cb) {
		data_ptr = res->pre_write_cb(path.obj_inst_id, &data_len);
	}

	if (!data_ptr) {
		LOG_ERR("res data pointer is NULL");
		return -EINVAL;
	}

	/* This actually sets the data buffer */
	ret = settings_val_read_cb(val_ctx, data_ptr, data_len);
	if (ret < 0) {
		return ret;
	}

	if (res->post_write_cb) {
		ret = res->post_write_cb(path.obj_inst_id, data_ptr, len,
					 false, 0);
	}

	return (len < 0) ? len : 0;
}

static struct settings_handler lwm2m_settings = {
	.name = "lwm2m",
	.h_set = set,
};

int lwm2m_settings_init(void)
{
	int err;

	err = settings_subsys_init();
	if (err) {
		LOG_ERR("settings_subsys_init failed (err %d)", err);
		return err;
	}

	err = settings_register(&lwm2m_settings);
	if (err) {
		LOG_ERR("settings_register failed (err %d)", err);
		return err;
	}

	return 0;
}
