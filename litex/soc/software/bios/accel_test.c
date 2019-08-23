#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <uart.h>
#include <console.h>
#include <generated/csr.h>
#include <generated/mem.h>
#include "spi.h"

#define AXIS_NUM 12 // should be 3*n

uint16_t buff[512];
volatile uint16_t sample_num;

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

void accel_write_reg(uint8_t addr, uint8_t value)
{
    spi_csn_active();
    spi_byte_transfer(0x0A);
    spi_byte_transfer(addr);
    spi_byte_transfer(value);
    spi_csn_inactive();
}

void read_sample_num(void)
{
    sample_num = accel_read_reg(13);
    sample_num = sample_num << 8;
    sample_num |= accel_read_reg(12);
}

void accel_read_fifo(uint32_t size)
{
    uint32_t j;
    uint16_t value;

    for(j = 0; j < size; j++)
    {
        spi_csn_active();
        spi_byte_transfer(0x0D);
        value = spi_byte_transfer(0x00);
        value = value << 8;
        value |= spi_byte_transfer(0x00);
        spi_csn_inactive();

        buff[j] = value;
        printf("%4X ", buff[j]);

        if((j % 3)==0)
        {
            printf("\n");
        }
    }
}

void gpio_irq(void)
{
    read_sample_num();

    if(sample_num >= AXIS_NUM)
    {
      accel_read_fifo(AXIS_NUM);
    }
}

void accel_test(void)
{
    printf("Accel test program start\n");

    printf("ID             = %X\n", accel_read_reg(0));
    printf("FIFO_ENTRIES_L = %X\n", accel_read_reg(12));
    printf("FIFO_ENTRIES_H = %X\n", accel_read_reg(13));
    printf("POWR_CTRL      = %X\n", accel_read_reg(45));

    accel_write_reg(31, 0x52); // SOFT_RESET   = Soft reset (0x52)

    /* Set mode fifo streamming */
    printf("Setting FIFO stream mode\n");
    accel_write_reg(40, 0x02);     // FIFO_CONTROL = Stream mode
    accel_write_reg(41, AXIS_NUM); // FIFO_SAMPLES = 60 (should be = 3N)
    accel_write_reg(44, 3);        // FILTER_CTL   => ODR = 0 (25 Hz)
    accel_write_reg(42, 0x84);     // INTMAP1 with FIFO_WATERMARK interrupt (active low)
    accel_write_reg(43, 0x84);     // INTMAP2 with FIFO_WATERMARK interrupt (active low)
    accel_write_reg(45, 0x02);     // POWER_CTL    = start measure

    while(readchar_nonblock() == 0);
    accel_write_reg(45, 0x00);     // POWER_CTL    = Stop measure

    printf("Accel test program end\n");
}
