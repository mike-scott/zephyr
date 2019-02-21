/*
 * Copyright (c) 2019 Foundries.io
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef LWM2M_SETTINGS_H_
#define LWM2M_SETTINGS_H_

#include "lwm2m_object.h"
#include "lwm2m_engine.h"

#define LWM2M_SETTINGS_PREFIX "lwm2m"

int lwm2m_try_persist_setting(u16_t obj_id, u16_t obj_inst_id, u16_t res_id,
			      struct lwm2m_engine_obj_field *obj_field,
			      void *data_ptr, size_t data_len);

int lwm2m_settings_init(void);

#endif /* LWM2M_SETTINGS_H_ */
