/*
 * Copyright (c) 2015, Yanzi Networks AB.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * Original Authors:
 *         Joakim Eriksson <joakime@sics.se>
 *         Niclas Finne <nfi@sics.se>
 */

#ifndef LWM2M_ENGINE_H
#define LWM2M_ENGINE_H

#include "lwm2m_object.h"

#define ZOAP_RESPONSE_CODE_CLASS(x)	(x >> 5)
#define ZOAP_RESPONSE_CODE_DETAIL(x)	(x & 0x1F)

enum lwm2m_status {
	LWM2M_STATUS_OK,
	LWM2M_STATUS_ERROR,		/* Internal server error */
	LWM2M_STATUS_WRITE_ERROR,	/* Error from writer */
	LWM2M_STATUS_READ_ERROR,	/* Error from reader */
	LWM2M_STATUS_BAD_REQUEST,
	LWM2M_STATUS_UNAUTHORIZED,
	LWM2M_STATUS_FORBIDDEN,
	LWM2M_STATUS_NOT_FOUND,
	LWM2M_STATUS_OP_NOT_ALLOWED,
	LWM2M_STATUS_NOT_ACCEPTABLE,
	LWM2M_STATUS_NOT_IMPLEMENTED,
	LWM2M_STATUS_SERVICE_UNAVAIL
};

/* TODO: */
#define NOTIFY_OBSERVER(o, i, r)	engine_notify_observer(o, i, r)
#define NOTIFY_OBSERVER_PATH(path)	engine_notify_observer_path(path)

void  engine_add_object(struct lwm2m_engine_obj *obj);
void  engine_remove_object(struct lwm2m_engine_obj *obj);
int   engine_notify_observer(u16_t obj_id, u16_t obj_inst_id, u16_t res_id);
int   engine_notify_observer_path(struct lwm2m_obj_path *path);
void  engine_trigger_update(void);

#endif /* LWM2M_ENGINE_H */
