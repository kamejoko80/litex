#include <stdio.h>
#include <stdlib.h>
#include <console.h>
#include <string.h>
#include <uart.h>
#include <system.h>
#include <id.h>
#include <irq.h>
#include <crc.h>

#include <generated/csr.h>
#include <generated/mem.h>

#include "boot.h"
#include "spi.h"

/* General address space functions */

#ifdef SPI_MASTER_INTERRUPT
void spi_master_isr_init(void);
void spi_master_isr_init(void)
{  
  spi_master_ev_pending_write(spi_master_ev_pending_read());
  spi_master_ev_enable_write(1);
  irq_setmask(irq_getmask() | (1 << SPI_MASTER_INTERRUPT));
}
#endif

#ifdef CSR_CTRL_BASE
static void reboot(void)
{
	ctrl_reset_write(1);
}
#endif

int main(int i, char **c)
{
    uint16_t adc;
    uint32_t voltage;

	irq_setmask(0);
	irq_setie(1);
	uart_init();

#ifdef SPI_MASTER_BASE
    spi_init(); 
#endif

#ifdef SPI_MASTER_INTERRUPT
    spi_master_isr_init();
#endif

	printf("\n");
	printf("\e[1m        __   _ __      _  __\e[0m\n");
	printf("\e[1m       / /  (_) /____ | |/_/\e[0m\n");
	printf("\e[1m      / /__/ / __/ -_)>  <\e[0m\n");
	printf("\e[1m     /____/_/\\__/\\__/_/|_|\e[0m\n");
	printf("\n");
	printf(" (c) Copyright 2012-2019 Enjoy-Digital\n");
	printf(" (c) Copyright 2007-2015 M-Labs Ltd\n");
	printf("\n");
	printf(" Firmware built on "__DATE__" "__TIME__"\n");
	printf("\n");

    printf("Read ADC\n");

    for(int i = 1; i <= 20; i++)
    {
        printf("=====================\n");
        for(int j = 0; j < 8; j++)
        {
            adc = spi_adc_read(j);
            voltage = (3300 * adc) / 0x7FF;
            printf("adc[%d] = %d\t[mV]\n", j, voltage);
        }
        printf("\n");
    }

    printf("Done!\n");

#ifdef CSR_CTRL_BASE
    printf("Press anykey to quit the program\n");
    uart_read();

    /* Reboot the board */
    reboot();
#endif

    return 0;
}
