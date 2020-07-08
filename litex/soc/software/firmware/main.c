// This file is Copyright (c) 2013-2014 Sebastien Bourdeauducq <sb@m-labs.hk>
// This file is Copyright (c) 2014-2019 Florent Kermarrec <florent@enjoy-digital.fr>
// This file is Copyright (c) 2015 Yann Sionneau <ys@m-labs.hk>
// This file is Copyright (c) 2015 whitequark <whitequark@whitequark.org>
// This file is Copyright (c) 2019 Ambroz Bizjak <ambrop7@gmail.com>
// This file is Copyright (c) 2019 Caleb Jamison <cbjamo@gmail.com>
// This file is Copyright (c) 2018 Dolu1990 <charles.papon.90@gmail.com>
// This file is Copyright (c) 2018 Felix Held <felix-github@felixheld.de>
// This file is Copyright (c) 2019 Gabriel L. Somlo <gsomlo@gmail.com>
// This file is Copyright (c) 2018 Jean-François Nguyen <jf@lambdaconcept.fr>
// This file is Copyright (c) 2018 Sergiusz Bazanski <q3k@q3k.org>
// This file is Copyright (c) 2016 Tim 'mithro' Ansell <mithro@mithis.com>

// License: BSD

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
#include <generated/git.h>

#include "sdcard.h"

/* General address space functions */

#ifdef SPI_DMA_INTERRUPT
void spi_dma_isr_init(void);
void spi_dma_isr_init(void)
{
	spi_dma_ev_pending_write(spi_dma_ev_pending_read());
  spi_dma_ev_enable_write(1);
  irq_setmask(irq_getmask() | (1 << SPI_DMA_INTERRUPT));
}

void spi_dma_irq(void);
void spi_dma_irq(void)
{
	printf("spi_dma_irq\n\r");
}

#endif

#ifdef CSR_SPI_BASE
void spidma_test(void);
void spidma_test(void)
{
#define SIZE 128

	unsigned int *tx_buff = (unsigned int *)(MAIN_RAM_BASE + 0xA00000);
	unsigned int *rx_buff = (unsigned int *)(MAIN_RAM_BASE + 0xA00000 + SIZE);

	/* initialize buffer */
	tx_buff[0] = 0x12345678;
	tx_buff[1] = 0xABCDEFAB;
	tx_buff[2] = 0x15252617;
	tx_buff[3] = 0x19127934;

	rx_buff[0] = 0;
	rx_buff[1] = 0;
	rx_buff[2] = 0;
	rx_buff[3] = 0;

	/* SPI loopback config */
	spi_loopback_write(1);

	/* initialize spi dma */
	spi_dma_spi_control_reg_address_write(CSR_SPI_CONTROL_ADDR);
	spi_dma_spi_status_reg_address_write(CSR_SPI_STATUS_ADDR);
	spi_dma_spi_mosi_reg_address_write(CSR_SPI_MOSI_ADDR);
	spi_dma_spi_miso_reg_address_write(CSR_SPI_MISO_ADDR);

	/* setup dma transfer */
	spi_dma_tx_src_addr_write(MAIN_RAM_BASE);
	spi_dma_rx_dst_addr_write(MAIN_RAM_BASE + 16);
	spi_dma_tx_len_write(SIZE);
	spi_dma_rx_ena_write(1);

	/* CS assert */
	spi_cs_write(1);

	/* start dma transfer */
	spi_dma_start_write(1);

	/* wait for dma done */
	while(!spi_dma_done_read());

	/* CS de-assert */
	spi_cs_write(0);

	printf("spi_dma_rx_ena_read %X\r\n", spi_dma_rx_ena_read());
	printf("DMA done! MISO read %X\r\n", spi_miso_read());

  printf("rx_buff[0]          %X\r\n", rx_buff[0]);
  printf("rx_buff[1]          %X\r\n", rx_buff[1]);
  printf("rx_buff[2]          %X\r\n", rx_buff[2]);
  printf("rx_buff[3]          %X\r\n", rx_buff[3]);
}
#endif /* CSR_SPI_BASE */

int main(int i, char **c)
{
	irq_setmask(0);
	irq_setie(1);
	uart_init();

#ifdef SPI_DMA_INTERRUPT
	spi_dma_isr_init();
#endif

	printf("Applciation \n");

#ifdef CSR_SPI_BASE
	spidma_test();
#endif

	while(1);

	return 0;
}
