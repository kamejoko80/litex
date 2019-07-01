#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <console.h>
#include <string.h>
#include <uart.h>
#include <system.h>
#include <id.h>
#include <irq.h>
#include <crc.h>

#include <generated/csr.h>
#include <generated/mem.h>

#include "spi.h"

#ifdef SPI_MASTER_BASE

void spi_init(void)
{
    SPI_DIV   = 0x00;  // fclk/2
    SPI_CTRL  = (1 << ASS) | (1 << LSB);
    SPI_CTRL |= 0x08;
    SPI_SS    = 0x01;  // ss = 0

    printf("SPI_DIV  = %X\r\n", SPI_DIV);
    printf("SPI_TX0  = %X\r\n", SPI_TX0);
    printf("SPI_CTRL = %X\r\n", SPI_CTRL);
    printf("SPI_SS   = %X\r\n", SPI_SS);
}

void spi_send_byte(uint8_t data)
{
    SPI_TX0   = data;
    SPI_CTRL |= (1 << GO_BSY);
    while(SPI_CTRL & (1 << GO_BSY));
}

#endif /* SPI_MASTER_BASE */
