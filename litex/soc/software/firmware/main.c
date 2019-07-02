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

int main(int i, char **c)
{
	irq_setmask(0);
	irq_setie(1);
	uart_init();

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


	while(1);

	return 0;
}
