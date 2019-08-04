#include <stdint.h>
#include <stdio.h>

#include "fatfs.h"

extern void HAL_Delay(uint32_t n);

void main_app (void)
{
    FATFS fs;
    FIL fil;
    char buffer[100];

    MX_FATFS_Init();

    extern void HAL_Delay(uint32_t n);
    HAL_Delay(100);

    printf("SD Card demo\n");

	/* Mount SD Card */
	if(f_mount(&fs, "", 0) != FR_OK)
    {
        printf("SD Card mount failed\n");
    }

    HAL_Delay(100);

	/* Open file to read */
	if(f_open(&fil, "first.txt", FA_READ) != FR_OK)
    {
        printf("SD Card open file 2 error\n");
    }

	while(f_gets(buffer, sizeof(buffer), &fil))
	{
		/* SWV output */
		printf("%s", buffer);
	}

	/* Close file */
	if(f_close(&fil) != FR_OK)
    {
        printf("SD Card close file 2 error\n");
    }

	/* Unmount SDCARD */
	if(f_mount(NULL, "", 1) != FR_OK)
    {
        printf("SD Card unmount error\n");
    }  
}