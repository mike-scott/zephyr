/*
 * Copyright (c) 2018 Open Source Foundries Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <net/net_core.h>
#include <net/net_l2.h>
#include <net/net_if.h>
#include <net/net_pkt.h>

static enum net_verdict offload_recv(struct net_if *iface,
					struct net_pkt *pkt)
{
	return NET_CONTINUE;
}

static enum net_verdict offload_send(struct net_if *iface,
					struct net_pkt *pkt)
{
	net_if_queue_tx(iface, pkt);

	return NET_OK;
}

static inline u16_t offload_reserve(struct net_if *iface, void *unused)
{
	ARG_UNUSED(iface);
	ARG_UNUSED(unused);

	return 0;
}

NET_L2_INIT(OFFLOAD_IP_L2, offload_recv, offload_send, offload_reserve, NULL);
