zephyr_sources_ifdef(CONFIG_MODEM_RECEIVER modem_receiver.c)
zephyr_sources_ifdef(CONFIG_MODEM_SHELL	   modem_shell.c)
zephyr_sources_ifdef(CONFIG_MODEM_WCN14A2A wcn14a2a.c)
