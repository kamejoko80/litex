#ifndef __ADS126X_H
#define __ADS126X_H

#include <generated/csr.h>

#define ADS126X_CHIP0 0x01
#define ADS126X_CHIP1 0x02
#define ADS126X_CHIP2 0x04
#define ADS126X_CHIP3 0x08
#define ADS126X_CHIP4 0x10
#define ADS126X_CHIP5 0x20
#define ADS126X_CHIP6 0x40
#define ADS126X_CHIP7 0x80

void ads126x_init(void);
void ads126x_read_reg(uint8_t addr);
uint8_t ads126x_read_rx_data(uint8_t chip);
void ads126x_select_chip(uint8_t mask);
void ads126x_deselect_chip(void);
void ads126x_read_adc_data(uint8_t chip);
void ads126x_print_adc_data(void);

#endif /*__ADS126X_H */