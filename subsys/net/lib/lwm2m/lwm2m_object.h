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

#ifndef LWM2M_OBJECT_H_
#define LWM2M_OBJECT_H_

/* stdint conversions */
#include <zephyr/types.h>
#include <stddef.h>
#include <net/net_ip.h>
#include <net/zoap.h>
#include <net/lwm2m.h>
#include <misc/printk.h>
#include <kernel.h>

/* #####/###/#####/### + NULL */
#define MAX_RESOURCE_LEN	20

/* operations */
#define LWM2M_OP_NONE		0
#define LWM2M_OP_READ		1
#define LWM2M_OP_DISCOVER	2
#define LWM2M_OP_WRITE		3
#define LWM2M_OP_WRITE_ATTR	4
#define LWM2M_OP_EXECUTE	5
#define LWM2M_OP_CREATE 	6
#define LWM2M_OP_DELETE 	7

/* operation permission bits */
#define LWM2M_OP_BIT(op)	(1 << (op - 1))

/* resource permissions */
#define LWM2M_PERM_R		LWM2M_OP_BIT(LWM2M_OP_READ)
#define LWM2M_PERM_W		LWM2M_OP_BIT(LWM2M_OP_WRITE)
#define LWM2M_PERM_X		LWM2M_OP_BIT(LWM2M_OP_EXECUTE)
#define LWM2M_PERM_RW		(LWM2M_OP_BIT(LWM2M_OP_READ) | \
				 LWM2M_OP_BIT(LWM2M_OP_WRITE))
#define LWM2M_PERM_RWX		(LWM2M_OP_BIT(LWM2M_OP_READ) | \
				 LWM2M_OP_BIT(LWM2M_OP_WRITE) | \
				 LWM2M_OP_BIT(LWM2M_OP_EXECUTE))

/* resource types */
#define LWM2M_RES_TYPE_NONE	0
#define LWM2M_RES_TYPE_OPAQUE	1
#define LWM2M_RES_TYPE_STRING	2
#define LWM2M_RES_TYPE_UINT64	3
#define LWM2M_RES_TYPE_U64	3
#define LWM2M_RES_TYPE_UINT	4
#define LWM2M_RES_TYPE_U32	4
#define LWM2M_RES_TYPE_U16	5
#define LWM2M_RES_TYPE_U8	6
#define LWM2M_RES_TYPE_INT64	7
#define LWM2M_RES_TYPE_S64	7
#define LWM2M_RES_TYPE_INT	8
#define LWM2M_RES_TYPE_S32	8
#define LWM2M_RES_TYPE_S16	9
#define LWM2M_RES_TYPE_S8	10
#define LWM2M_RES_TYPE_FLOAT32	11
#define LWM2M_RES_TYPE_BOOL	12
#define LWM2M_RES_TYPE_TIME	13

/* remember that we have already output a value - can be between two block's */
#define WRITER_OUTPUT_VALUE      1
#define WRITER_RESOURCE_INSTANCE 2

struct lwm2m_engine_obj;
struct lwm2m_engine_context;

/* path representing object instances */
struct lwm2m_obj_path {
	u16_t obj_id;
	u16_t obj_inst_id;
	u16_t res_id;
	u16_t res_inst_id;
	u8_t  level;  /* 0/1/2/3 = 3 = resource */
};

#define OBJ_FIELD(res_id, perm, type, multi_max) \
	{ res_id, LWM2M_PERM_ ## perm, LWM2M_RES_TYPE_ ## type, multi_max }

#define OBJ_FIELD_DATA(res_id, perm, type) \
	OBJ_FIELD(res_id, perm, type, 1)

#define OBJ_FIELD_MULTI_DATA(res_id, perm, type, multi_max) \
	OBJ_FIELD(res_id, perm, type, multi_max)

#define OBJ_FIELD_EXECUTE(res_id) \
	OBJ_FIELD(res_id, X, NONE, 0)

struct lwm2m_engine_obj_field {
	u16_t  res_id;
	u8_t   permissions;
	u8_t   data_type;
	u8_t   multi_max_count;
};

typedef struct lwm2m_engine_obj_inst *
	(*lwm2m_engine_obj_create_cb_t)(u16_t obj_inst_id);
typedef int (*lwm2m_engine_obj_delete_cb_t)(u16_t obj_inst_id);

struct lwm2m_engine_obj {
	sys_snode_t node;
	u16_t obj_id;
	struct lwm2m_engine_obj_field *fields;
	u16_t field_count;
	u16_t instance_count;
	u16_t max_instance_count;
	lwm2m_engine_obj_create_cb_t create_cb;
	lwm2m_engine_obj_delete_cb_t delete_cb;
};

#define INIT_OBJ_RES(res_var, index_var, id_val, multi_var, \
		     data_val, data_val_len, r_cb, w_cb, ex_cb) \
	res_var[index_var].res_id = id_val; \
	res_var[index_var].multi_count_var = multi_var; \
	res_var[index_var].data_ptr = data_val; \
	res_var[index_var].data_len = data_val_len; \
	res_var[index_var].read_cb = r_cb; \
	res_var[index_var].write_cb = w_cb; \
	res_var[index_var++].execute_cb = ex_cb

#define INIT_OBJ_RES_MULTI_DATA(res_var, index_var, id_val, multi_var, \
				data_val, data_val_len) \
	res_var[index_var].res_id = id_val; \
	res_var[index_var].multi_count_var = multi_var; \
	res_var[index_var].data_ptr = data_val; \
	res_var[index_var++].data_len = data_val_len

#define INIT_OBJ_RES_DATA(res_var, index_var, id_val, data_val, data_val_len) \
	INIT_OBJ_RES_MULTI_DATA(res_var, index_var, id_val, NULL, \
				data_val, data_val_len)

#define INIT_OBJ_RES_DUMMY(res_var, index_var, id_val) \
	INIT_OBJ_RES_MULTI_DATA(res_var, index_var, id_val, NULL, NULL, 0)

#define INIT_OBJ_RES_EXECUTE(res_var, index_var, id_val, ex_cb) \
	res_var[index_var].execute_cb = ex_cb; \
	INIT_OBJ_RES_MULTI_DATA(res_var, index_var, id_val, NULL, NULL, 0)

struct lwm2m_engine_res_inst {
	char path[MAX_RESOURCE_LEN]; /* 3/0/0 */
	u16_t  res_id;
	u8_t   *multi_count_var;
	void  *data_ptr;
	size_t data_len;
	/* runtime field attributes (WRITE_ATTR) */

	/* callbacks set by user code on obj instance */
	lwm2m_engine_rw_cb_t	    read_cb;
	lwm2m_engine_rw_cb_t	    write_cb;
	lwm2m_engine_exec_cb_t	    execute_cb;
};

struct lwm2m_engine_obj_inst {
	sys_snode_t node;
	char path[MAX_RESOURCE_LEN]; /* 3/0 */
	struct lwm2m_engine_obj *obj;
	u16_t obj_inst_id;
	struct lwm2m_engine_res_inst *resources;
	u16_t resource_count;
};

struct lwm2m_output_context {
	struct zoap_packet *out_zpkt;
	u8_t writer_flags;	/* flags for reader/writer */
	u8_t *outbuf;
	u16_t outsize;
	u32_t outlen;
	u8_t mark_pos_ri;	/* mark pos for last resource instance */
	const struct lwm2m_writer *writer;
};

struct lwm2m_input_context {
	struct zoap_packet *in_zpkt;
	u8_t *inbuf;
	u16_t insize;
	s32_t inpos;
	u16_t last_value_len;
	const struct lwm2m_reader *reader;
};

/* LWM2M format writer for the various formats supported */
struct lwm2m_writer {
	size_t (*put_begin)(struct lwm2m_output_context *out,
			      struct lwm2m_obj_path *path);
	size_t (*put_end)(struct lwm2m_output_context *out,
			    struct lwm2m_obj_path *path);
	size_t (*put_begin_ri)(struct lwm2m_output_context *out,
				 struct lwm2m_obj_path *path);
	size_t (*put_end_ri)(struct lwm2m_output_context *out,
			       struct lwm2m_obj_path *path);
	size_t (*put_int32)(struct lwm2m_output_context *out,
			      struct lwm2m_obj_path *path,
			      s32_t value);
	size_t (*put_int64)(struct lwm2m_output_context *out,
			      struct lwm2m_obj_path *path,
			      s64_t value);
	size_t (*put_string)(struct lwm2m_output_context *out,
			       struct lwm2m_obj_path *path,
			       const char *value, size_t strlen);
	size_t (*put_float32fix)(struct lwm2m_output_context *out,
				   struct lwm2m_obj_path *path,
				   s32_t value, int bits);
	size_t (*put_bool)(struct lwm2m_output_context *out,
			     struct lwm2m_obj_path *path,
			     int value);
};

struct lwm2m_reader {
	size_t (*get_int32)(struct lwm2m_input_context *in,
			     s32_t *value);
	size_t (*get_int64)(struct lwm2m_input_context *in,
			     s64_t *value);
	size_t (*get_string)(struct lwm2m_input_context *in,
			     u8_t *value, size_t strlen);
	size_t (*get_float32fix)(struct lwm2m_input_context *in,
				  s32_t *value, int bits);
	size_t (*get_bool)(struct lwm2m_input_context *in,
			    int *value);
};

/* LWM2M engine context */
struct lwm2m_engine_context {
	struct lwm2m_input_context *in;
	struct lwm2m_output_context *out;
	struct lwm2m_obj_path *path;
	u8_t operation;
};

/* inline multi-format write / read functions */

static inline size_t engine_put_begin(struct lwm2m_output_context *out,
					struct lwm2m_obj_path *path)
{
	if (out->writer->put_begin) {
		return out->writer->put_begin(out, path);
	}

	return 0;
}

static inline size_t engine_put_end(struct lwm2m_output_context *out,
				      struct lwm2m_obj_path *path)
{
	if (out->writer->put_end) {
		return out->writer->put_end(out, path);
	}

	return 0;
}

static inline size_t engine_put_begin_ri(struct lwm2m_output_context *out,
					   struct lwm2m_obj_path *path)
{
	if (out->writer->put_begin_ri) {
		return out->writer->put_begin_ri(out, path);
	}

	return 0;
}

static inline size_t engine_put_end_ri(struct lwm2m_output_context *out,
					 struct lwm2m_obj_path *path)
{
	if (out->writer->put_end_ri) {
		return out->writer->put_end_ri(out, path);
	}

	return 0;
}

static inline size_t engine_put_int32(struct lwm2m_output_context *out,
					struct lwm2m_obj_path *path,
					s32_t value)
{
	return out->writer->put_int32(out, path, value);
}

static inline size_t engine_put_int64(struct lwm2m_output_context *out,
					struct lwm2m_obj_path *path,
					s64_t value)
{
	return out->writer->put_int64(out, path, value);
}

static inline size_t engine_put_string(struct lwm2m_output_context *out,
					 struct lwm2m_obj_path *path,
					 const char *value, size_t strlen)
{
	return out->writer->put_string(out, path, value, strlen);
}

static inline size_t engine_put_float32fix(struct lwm2m_output_context *out,
					     struct lwm2m_obj_path *path,
					     s32_t value, int bits)
{
	return out->writer->put_float32fix(out, path, value, bits);
}

static inline size_t engine_put_bool(struct lwm2m_output_context *out,
				       struct lwm2m_obj_path *path,
				       int value)
{
	return out->writer->put_bool(out, path, value);
}

static inline size_t engine_get_int32(struct lwm2m_input_context *in,
				       s32_t *value)
{
	return in->reader->get_int32(in, value);
}

static inline size_t engine_get_int64(struct lwm2m_input_context *in,
				       s64_t *value)
{
	return in->reader->get_int64(in, value);
}

static inline size_t engine_get_string(struct lwm2m_input_context *in,
					u8_t *value, size_t strlen)
{
	return in->reader->get_string(in, value, strlen);
}

static inline size_t engine_get_float32fix(struct lwm2m_input_context *in,
					    s32_t *value, int bits)
{
	return in->reader->get_float32fix(in, value, bits);
}

static inline size_t engine_get_bool(struct lwm2m_input_context *in,
				      int *value)
{
	return in->reader->get_bool(in, value);
}

#endif /* LWM2M_OBJECT_H_ */
