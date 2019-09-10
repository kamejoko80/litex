#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <crc.h>
#include <uart.h>
#include <console.h>
#include <generated/csr.h>
#include <generated/mem.h>
#include "fatfs.h"
#include "boot.h"

#ifdef CSR_SPI_MASTER_01_BASE
void spi_01_init(void)
{
    /* IE=0, IPOL=0, CPHA=0, CPOL=0 */
    spi_master_01_config_write(0x00);
}

uint8_t spi_01_byte_transfer(uint8_t byte)
{
    spi_master_01_tx_data_write(byte);
    spi_master_01_start_write(1);
    while(!spi_master_01_done_read());
    return spi_master_01_rx_data_read();
}

void spi_01_csn_active(void)
{
    spi_master_01_csn_write(0);
}

void spi_01_csn_inactive(void)
{
    spi_master_01_csn_write(1);
}
#endif

void main_app (void)
{
   printf("SPI flash programmer\n");
   return;
}
