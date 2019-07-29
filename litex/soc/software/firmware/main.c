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

#ifdef SPI_MASTER_BASE
uint16_t accel_read_reg(uint8_t addr);
uint16_t accel_read_reg(uint8_t addr)
{
    uint8_t reg;

    spi_csn_active();
    spi_byte_transfer(0x0B);
    spi_byte_transfer(addr);
    reg = spi_byte_transfer(0x00);
    spi_csn_inactive();

    return reg;
}

void accel_read_fifo(void);
void accel_read_fifo(void)
{
    uint8_t reg0;
    uint16_t j;

    spi_csn_active();
    spi_byte_transfer(0x0D);
    for(j = 0; j < 512; j++)
    {
        reg0 = spi_byte_transfer(0x00);
        if(reg0 != 0)
        {
            //printf("FIFO data[%2d] = %X\n", j, reg0);
        }
    }
    spi_csn_inactive();
}
#endif

int main(int i, char **c)
{
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

#ifdef SPI_MASTER_BASE
#if 1 /* Test spi slave loop back */

#if 1 /* Test accel behavior */
    uint8_t reg0 = 0, reg1 = 1;

    printf("Accel behavior test demo\n");
    printf("\n");
    printf("Read status registers\n");
    printf("Press anykey to quit the program\n");

    printf("FIFO_ENTRIES_L = %X\n", accel_read_reg(12));
    printf("FIFO_ENTRIES_H = %X\n", accel_read_reg(13));

    while(readchar_nonblock() == 0)
    {
        reg1 = accel_read_reg(11);
        
        if(reg0 != reg1)
        {
            printf("Status = 0x%X\n", reg1);
            reg0 = reg1;

            printf("FIFO_ENTRIES_L = %X\n", accel_read_reg(12));
            printf("FIFO_ENTRIES_H = %X\n", accel_read_reg(13));

            if(reg1 & 0x01) /* Data sample ready */
            {
                accel_read_fifo();
            }
        }
    }

#else /* Test registers, FIFO access */
    uint8_t reg0, reg1, reg2, reg3;
    uint32_t j;

    printf("Accel register access test demo\n");
    printf("\n");

    printf("Read ID registers\n");

    spi_csn_active();
    spi_byte_transfer(0x0B);
    spi_byte_transfer(0x00);
    reg0 = spi_byte_transfer(0x00);
    reg1 = spi_byte_transfer(0x00);
    reg2 = spi_byte_transfer(0x00);
    reg3 = spi_byte_transfer(0x00);
    printf("Reg0 = %X\n", reg0);
    printf("Reg1 = %X\n", reg1);
    printf("Reg2 = %X\n", reg2);
    printf("Reg3 = %X\n", reg3);
    printf("=============\n");
    spi_csn_inactive();

    printf("Write registers(single mode)\n");

    for(j = 0; j < 15; j++)
    {
        spi_csn_active();
        spi_byte_transfer(0x0A);
        spi_byte_transfer(32 + j);
        spi_byte_transfer(0xA5);
        spi_csn_inactive();
    }

    printf("Read back registers\n");

    spi_csn_active();
    spi_byte_transfer(0x0B);
    spi_byte_transfer(32);

    for(j = 0; j < 15; j++)
    {
       reg0 = spi_byte_transfer(0x00);
       printf("Reg[%d] = %X\n", 32+j, reg0);
    }

    spi_csn_inactive();


    printf("Write registers(burst mode)\n");
    spi_csn_active();
    spi_byte_transfer(0x0A);
    spi_byte_transfer(32);

    for(j = 0; j < 15; j++)
    {
        spi_byte_transfer(0x5A);
    }

    spi_csn_inactive();

    printf("Read back registers\n");

    spi_csn_active();
    spi_byte_transfer(0x0B);
    spi_byte_transfer(32);

    for(j = 0; j < 15; j++)
    {
       reg0 = spi_byte_transfer(0x00);
       printf("Reg[%d] = %X\n", 32+j, reg0);
    }

    spi_csn_inactive();

    /* Read IDs manytimes */
    for(j = 0; j < 20; j++)
    {
        spi_csn_active();
        spi_byte_transfer(0x0B);
        spi_byte_transfer(0x00);
        reg0 = spi_byte_transfer(0x00);
        reg1 = spi_byte_transfer(0x00);
        reg2 = spi_byte_transfer(0x00);
        reg3 = spi_byte_transfer(0x00);
        printf("Reg0 = %X\n", reg0);
        printf("Reg1 = %X\n", reg1);
        printf("Reg2 = %X\n", reg2);
        printf("Reg3 = %X\n", reg3);
        printf("=============\n");
        spi_csn_inactive();
    }

    printf("Done! Reading FIFO...\n");

    accel_read_fifo();

    printf("Done! press anykey to quit the program\n");

    while(readchar_nonblock() == 0);

#endif /* Test registers, FIFO access */

#else /* Test ADC128S102 */

    printf("ADC voltatge measurement demo\n");
    printf("Press any key to quit the program\n");
    printf("\n");

    printf("Volt\t[0]\t[1]\t[2]\t[3]\t[4]\t[5]\t[6]\t[7]\t[mV]\n");
    printf("        ===========================================================\n");
    printf("    \t");

    while(readchar_nonblock() == 0)
    {
        uint16_t adc;
        uint32_t voltage;

        for(int j = 0; j < 8; j++)
        {
            /* Read 2 times */
            adc = spi_adc_read(j);
            adc = spi_adc_read(j);
            voltage = (3300 * adc) / 0x7FF;
            printf("%d\t", voltage);
        }
        printf("\r                                                                   ");
        printf("\r    \t");
    }
#endif
#endif

#ifdef CSR_CTRL_BASE
    /* Reboot the board */
    reboot();
#endif

    return 0;
}
