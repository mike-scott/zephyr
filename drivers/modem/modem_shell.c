/** @file
 * @brief Modem shell module
 *
 * Provide some modem shell commands that can be useful to applications.
 */

/*
 * Copyright (c) 2018 Open Source Foundries Ltd.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr.h>
#include <stdlib.h>
#include <string.h>
#include <device.h>
#include <shell/shell.h>

#include <misc/printk.h>

#include <drivers/modem/modem_receiver.h>

#define MODEM_SHELL_MODULE "modem"

int modem_shell_cmd_send(int argc, char *argv[])
{
	struct mdm_receiver_context *mdm_ctx;
	int ret, i, count = 0, arg = 1;

	/* list */
	if (!argv[arg]) {
		printk("Modem receivers:\n");

		for (i = 0; i < CONFIG_MODEM_RECEIVER_MAX_CONTEXTS; i++) {
			mdm_ctx = mdm_receiver_context_from_id(i);
			if (mdm_ctx) {
				count++;
				printk("\t%d: %s\n", i,
				       mdm_ctx->uart_dev->config->name);
			}
		}

		if (!count) {
			printk("None found.\n");
		}

		return 0;
	}

	/* <index> of modem receiver */
	i = atoi(argv[arg]);
	mdm_ctx = mdm_receiver_context_from_id(i);
	if (!mdm_ctx) {
		printk("Modem receiver not found!");
		return 0;
	}

	for (i = arg + 1; i < argc; i++) {
		ret = mdm_receiver_send(mdm_ctx, argv[i], strlen(argv[i]));
		if (ret < 0) {
			printk("Error sending '%s': %d\n", argv[i], ret);
			return 0;
		}

		if (i == argc - 1) {
			ret = mdm_receiver_send(mdm_ctx, "\r\n", 2);
		} else {
			ret = mdm_receiver_send(mdm_ctx, " ", 1);
		}

		if (ret < 0) {
			printk("Error sending (CRLF or space): %d\n", ret);
			return 0;
		}
	}

	return 0;
}

static struct shell_cmd modem_commands[] = {
	/* Keep the commands in alphabetical order */
	{ "send", modem_shell_cmd_send, "\n\tModem receiver command utility\n"
		"send\n\tList registered modem receivers\n"
		"send <index> <command>\n\tSend an AT <command> to registered "
		"modem receiver <index> (normally 0)" },
	{ NULL, NULL, NULL }
};

SHELL_REGISTER(MODEM_SHELL_MODULE, modem_commands);
