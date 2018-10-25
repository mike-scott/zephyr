/*
 * Copyright (c) 2018 Foundries.io
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/*
 * Source material for IPSO Addressable Display object (3341):
 * http://www.openmobilealliance.org/tech/profiles/lwm2m/3341.xml
 */

#define LOG_MODULE_NAME net_ipso_display
#define LOG_LEVEL CONFIG_LWM2M_LOG_LEVEL

#include <logging/log.h>
LOG_MODULE_REGISTER(LOG_MODULE_NAME);

#include <stdint.h>
#include <init.h>
#include <net/lwm2m.h>

#include "lwm2m_object.h"
#include "lwm2m_engine.h"

/* Server resource IDs */
#define DISPLAY_TEXT_ID		5527
#define DISPLAY_X_COORD_ID	5528
#define DISPLAY_Y_COORD_ID	5529
#define DISPLAY_CLEAR_ID	5530
#define DISPLAY_CONTRAST_ID	5531
#define DISPLAY_MAX_X_COORD_ID	5545
#define DISPLAY_MAX_Y_COORD_ID	5546
#define DISPLAY_APP_TYPE_ID	5750
#define DISPLAY_DIMMER_ID	5851


#define DISPLAY_MAX_ID		9

#define MAX_INSTANCE_COUNT	1

#define DISPLAY_STRING_LEN	255

/* resource state variables */
static char text[MAX_INSTANCE_COUNT][DISPLAY_STRING_LEN];

static struct lwm2m_engine_obj display;
static struct lwm2m_engine_obj_field fields[] = {
	OBJ_FIELD_DATA(DISPLAY_TEXT_ID, RW, STRING),
	OBJ_FIELD_DATA(DISPLAY_X_COORD_ID, RW_OPT, S32),
	OBJ_FIELD_DATA(DISPLAY_Y_COORD_ID, RW_OPT, S32),
	OBJ_FIELD_DATA(DISPLAY_MAX_X_COORD_ID, R_OPT, S32),
	OBJ_FIELD_DATA(DISPLAY_MAX_Y_COORD_ID, R_OPT, S32),
	OBJ_FIELD_EXECUTE_OPT(DISPLAY_CLEAR_ID),
	OBJ_FIELD_DATA(DISPLAY_DIMMER_ID, RW_OPT, FLOAT32),
	OBJ_FIELD_DATA(DISPLAY_CONTRAST_ID, RW_OPT, FLOAT32),
	OBJ_FIELD_DATA(DISPLAY_APP_TYPE_ID, RW_OPT, STRING),
};

static struct lwm2m_engine_obj_inst inst[MAX_INSTANCE_COUNT];
static struct lwm2m_engine_res_inst res[MAX_INSTANCE_COUNT][DISPLAY_MAX_ID];

static struct lwm2m_engine_obj_inst *display_create(u16_t obj_inst_id)
{
	int index, i = 0;

	/* Check that there is no other instance with this ID */
	for (index = 0; index < MAX_INSTANCE_COUNT; index++) {
		if (inst[index].obj && inst[index].obj_inst_id == obj_inst_id) {
			LOG_ERR("Can not create instance - "
				"already existing: %u", obj_inst_id);
			return NULL;
		}
	}

	for (index = 0; index < MAX_INSTANCE_COUNT; index++) {
		if (!inst[index].obj) {
			break;
		}
	}

	if (index >= MAX_INSTANCE_COUNT) {
		LOG_ERR("Can not create instance - no more room: %u",
			obj_inst_id);
		return NULL;
	}

	/* Set default values */
	text[index][0] = '\0';

	/* initialize instance resource data */
	INIT_OBJ_RES_DATA(res[index], i, DISPLAY_TEXT_ID,
			  text[index], DISPLAY_STRING_LEN);
	INIT_OBJ_RES_DUMMY(res[index], i, DISPLAY_X_COORD_ID);
	INIT_OBJ_RES_DUMMY(res[index], i, DISPLAY_Y_COORD_ID);
	INIT_OBJ_RES_DUMMY(res[index], i, DISPLAY_MAX_X_COORD_ID);
	INIT_OBJ_RES_DUMMY(res[index], i, DISPLAY_MAX_Y_COORD_ID);
	INIT_OBJ_RES_DUMMY(res[index], i, DISPLAY_CLEAR_ID);
	INIT_OBJ_RES_DUMMY(res[index], i, DISPLAY_DIMMER_ID);
	INIT_OBJ_RES_DUMMY(res[index], i, DISPLAY_CONTRAST_ID);
	INIT_OBJ_RES_DUMMY(res[index], i, DISPLAY_APP_TYPE_ID);

	inst[index].resources = res[index];
	inst[index].resource_count = i;
	LOG_DBG("Create IPSO Display instance: %d", obj_inst_id);
	return &inst[index];
}

static int ipso_display_init(struct device *dev)
{
	int ret = 0;

	/* Set default values */
	(void)memset(inst, 0, sizeof(*inst) * MAX_INSTANCE_COUNT);
	(void)memset(res, 0, sizeof(struct lwm2m_engine_res_inst) *
			MAX_INSTANCE_COUNT * DISPLAY_MAX_ID);

	display.obj_id = IPSO_OBJECT_DISPLAY_ID;
	display.fields = fields;
	display.field_count = ARRAY_SIZE(fields);
	display.max_instance_count = MAX_INSTANCE_COUNT;
	display.create_cb = display_create;
	lwm2m_register_obj(&display);

	return ret;
}

SYS_INIT(ipso_display_init, APPLICATION, CONFIG_KERNEL_INIT_PRIORITY_DEFAULT);
