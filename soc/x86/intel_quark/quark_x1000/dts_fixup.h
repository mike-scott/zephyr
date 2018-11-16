#define DT_UART_NS16550_PORT_0_BASE_ADDR	DT_NS16550_9000F000_BASE_ADDRESS
#define CONFIG_UART_NS16550_PORT_0_BAUD_RATE	DT_NS16550_9000F000_CURRENT_SPEED
#define CONFIG_UART_NS16550_PORT_0_NAME		DT_NS16550_9000F000_LABEL
#define DT_UART_NS16550_PORT_0_IRQ		DT_NS16550_9000F000_IRQ_0
#define CONFIG_UART_NS16550_PORT_0_IRQ_PRI	DT_NS16550_9000F000_IRQ_0_PRIORITY
#define DT_UART_NS16550_PORT_0_IRQ_FLAGS	DT_NS16550_9000F000_IRQ_0_SENSE
#define DT_UART_NS16550_PORT_0_CLK_FREQ	DT_NS16550_9000F000_CLOCK_FREQUENCY

#define DT_UART_NS16550_PORT_1_BASE_ADDR	DT_NS16550_9000B000_BASE_ADDRESS
#define CONFIG_UART_NS16550_PORT_1_BAUD_RATE	DT_NS16550_9000B000_CURRENT_SPEED
#define CONFIG_UART_NS16550_PORT_1_NAME		DT_NS16550_9000B000_LABEL
#define DT_UART_NS16550_PORT_1_IRQ		DT_NS16550_9000B000_IRQ_0
#define CONFIG_UART_NS16550_PORT_1_IRQ_PRI	DT_NS16550_9000B000_IRQ_0_PRIORITY
#define DT_UART_NS16550_PORT_1_IRQ_FLAGS	DT_NS16550_9000B000_IRQ_0_SENSE
#define DT_UART_NS16550_PORT_1_CLK_FREQ	DT_NS16550_9000B000_CLOCK_FREQUENCY

#define DT_PHYS_RAM_ADDR			CONFIG_SRAM_BASE_ADDRESS

#define DT_PHYS_LOAD_ADDR			CONFIG_FLASH_BASE_ADDRESS

#define DT_RAM_SIZE				CONFIG_SRAM_SIZE

#define DT_ROM_SIZE				CONFIG_FLASH_SIZE

#define DT_IOAPIC_BASE_ADDRESS		DT_INTEL_IOAPIC_FEC00000_BASE_ADDRESS

#define DT_I2C_0_IRQ			DT_SNPS_DESIGNWARE_I2C_90007000_IRQ_0
#define DT_I2C_0_IRQ_FLAGS			DT_SNPS_DESIGNWARE_I2C_90007000_IRQ_0_SENSE
#define DT_I2C_0_BASE_ADDR			DT_SNPS_DESIGNWARE_I2C_90007000_BASE_ADDRESS
#define CONFIG_I2C_0_NAME			DT_SNPS_DESIGNWARE_I2C_90007000_LABEL
#define DT_I2C_0_BITRATE			DT_SNPS_DESIGNWARE_I2C_90007000_CLOCK_FREQUENCY

#define DT_SPI_0_BASE_ADDRESS		DT_INTEL_INTEL_SPI_90009000_BASE_ADDRESS
#define DT_SPI_0_IRQ			DT_INTEL_INTEL_SPI_90009000_IRQ_0
#define DT_SPI_0_IRQ_FLAGS			DT_INTEL_INTEL_SPI_90009000_IRQ_0_SENSE
#define CONFIG_SPI_0_IRQ_PRI			DT_INTEL_INTEL_SPI_90009000_IRQ_0_PRIORITY
#define CONFIG_SPI_0_NAME			DT_INTEL_INTEL_SPI_90009000_LABEL

#define DT_SPI_1_BASE_ADDRESS		DT_INTEL_INTEL_SPI_90008000_BASE_ADDRESS
#define DT_SPI_1_IRQ			DT_INTEL_INTEL_SPI_90008000_IRQ_0
#define DT_SPI_1_IRQ_FLAGS			DT_INTEL_INTEL_SPI_90008000_IRQ_0_SENSE
#define CONFIG_SPI_1_IRQ_PRI			DT_INTEL_INTEL_SPI_90008000_IRQ_0_PRIORITY
#define CONFIG_SPI_1_NAME			DT_INTEL_INTEL_SPI_90008000_LABEL
