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
    SPI_DIV  = 0x00;  // fclk/2
    SPI_TX0  = 0x5A;  // TX = 0x5A
    SPI_CTRL = 0x208; // set 8 bit transfer
    SPI_SS   = 0x01;  // ss = 0 

    printf("SPI_DIV  = %X\r\n", SPI_DIV);
    printf("SPI_TX0  = %X\r\n", SPI_TX0);
    printf("SPI_CTRL = %X\r\n", SPI_CTRL);
    printf("SPI_SS   = %X\r\n", SPI_SS);
}

#endif /* SPI_MASTER_BASE */
