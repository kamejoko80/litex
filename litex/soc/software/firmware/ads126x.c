#include <stdio.h>
#include "ads126x.h"

void spi_array_init(void);
void spi_array_select_chip(uint8_t mask);
void spi_array_deselect_chip(void);
void spi_array_transfer_byte(uint8_t data);
uint8_t spi_array_read_rx_data(uint8_t chip);

uint8_t adc_data[3];  

#ifdef CSR_SPI_ARRAY_BASE

void spi_array_init(void)
{
    spi_array_config_write(0x00);
}

void spi_array_select_chip(uint8_t mask)
{
    spi_array_csn_write(~mask);
}

void spi_array_deselect_chip(void)
{
    spi_array_csn_write(0xFF);
}

void spi_array_transfer_byte(uint8_t data)
{
    spi_array_tx_data_write(data);
    spi_array_start_write(1);
    while(!spi_array_done_read());
}

uint8_t spi_array_read_rx_data(uint8_t chip)
{
    switch (chip)
    {
        case 0:
            return spi_array_rx_data_0_read();
        break;
        case 1:
            return spi_array_rx_data_1_read();
        break;
        case 2:
            return spi_array_rx_data_2_read();
        break;
        case 3:
            return spi_array_rx_data_3_read();
        break;
        case 4:
            return spi_array_rx_data_4_read();
        break;
        case 5:
            return spi_array_rx_data_5_read();
        break;
        case 6:
            return spi_array_rx_data_6_read();
        break;
        case 7:
            return spi_array_rx_data_7_read();
        break;
        default:
            return 0;
        break;
    }
}

#else /* CSR_SPI_ARRAY_BASE */

void spi_array_init(void)
{
}

void spi_array_select_chip(uint8_t mask)
{
}

void spi_array_deselect_chip(void)
{
}

void spi_array_transfer_byte(uint8_t data)
{
}

uint8_t spi_array_read_rx_data(uint8_t chip)
{
    return 0;
}

#endif /* CSR_SPI_ARRAY_BASE */

void ads126x_init(void)
{
    spi_array_init();
}

void ads126x_read_reg(uint8_t addr)
{
    /* send command read reg */
    spi_array_transfer_byte(0x20 + (addr & 0x1F));
    /* send echo byte */
    spi_array_transfer_byte(0x00);
    /* shift out register data */
    spi_array_transfer_byte(0x00);
}

void ads126x_read_adc_data(uint8_t chip)
{
    /* send command read adc data */
    spi_array_transfer_byte(0x12);
    /* send echo byte */
    spi_array_transfer_byte(0x00);
    /* shift out msb data */
    spi_array_transfer_byte(0x00);
    adc_data[2] = spi_array_read_rx_data(chip);
    /* shift out mid data */
    spi_array_transfer_byte(0x00);
    adc_data[1] = spi_array_read_rx_data(chip);
    /* shift out lsb data */
    spi_array_transfer_byte(0x00);
    adc_data[0] = spi_array_read_rx_data(chip);
}

void ads126x_print_adc_data(void)
{
    printf("msb %X\r\n", adc_data[2]);
    printf("mid %X\r\n", adc_data[1]); 
    printf("lsb %X\r\n", adc_data[0]);
}

uint8_t ads126x_read_rx_data(uint8_t chip)
{
    return spi_array_read_rx_data(chip);
}

void ads126x_select_chip(uint8_t mask)
{
    spi_array_select_chip(mask);
}

void ads126x_deselect_chip(void)
{
    spi_array_select_chip(0x00);
}
