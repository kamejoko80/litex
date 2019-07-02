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
    SPI_DIV   = 0x01;  // fclk/4 ~ 12MHz
    SPI_CTRL  = (1 << ASS) | (1 << TX_NEG);
    SPI_CTRL |= 0x10;  // 16 byte transfer
    SPI_SS    = 0x01;  // cns = 0

    printf("SPI_DIV  = %X\r\n", SPI_DIV);
    printf("SPI_CTRL = %X\r\n", SPI_CTRL);
    printf("SPI_SS   = %X\r\n", SPI_SS);
}

uint16_t spi_adc_read(uint8_t chanel)
{
    SPI_TX0   = (chanel << 11);
    SPI_CTRL |= (1 << GO_BSY);
    while(SPI_CTRL & (1 << GO_BSY));
    return SPI_RX0;
}

#endif /* SPI_MASTER_BASE */
