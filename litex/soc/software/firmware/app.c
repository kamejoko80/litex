#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "fatfs.h"

/* This variable contains sample set */
volatile int g_sample[3];

void convert_to_sample_set(char *str)
{
    char *p = str;    
    int i = 0;

    /* Find x, y, z number from string line */    
    while (*p) 
    {
        if (isdigit(*p) || ((*p=='-'||*p=='+') && isdigit(*(p+1)))) 
        {
            g_sample[i] = strtol(p, &p, 10);
            printf("%d ", g_sample[i]);
            
            if(i < 3) 
            {
                i++;
            } 
            else 
            {
                break;
            }
        } 
        else 
        {
            p++;
        }
    }    
}

void accel_data_read(void)
{
    FATFS fs;
    FIL fil;
    char buffer[100];
    int i = 0;

    // int x, y, z;
    
    char *substr;
    
	/* Mount SD Card */
	if(f_mount(&fs, "", 0) != FR_OK)
    {
        printf("SD Card mount failed\n");
    }

	/* Open file to read */
	if(f_open(&fil, "ACCEL.csv", FA_READ) != FR_OK)
    {
        printf("Open file error\n");
    }

	while(f_gets(buffer, sizeof(buffer), &fil))
	{
        substr = (char *)strchr(buffer, ',');
        convert_to_sample_set(substr);
        printf("\n");
    
        i++;
        if(i >= 20)
        {
            break;
        }
	}

	/* Close file */
	if(f_close(&fil) != FR_OK)
    {
        printf("Close file error\n");
    }

	/* Unmount SDCARD */
	if(f_mount(NULL, "", 1) != FR_OK)
    {
        printf("SD Card unmount error\n");
    }    
}

void main_app (void)
{
    /* Init Fatfs */
    MX_FATFS_Init();
    
    printf("SD Card demo\n");

    accel_data_read();

}