#ifndef __inf_image_H
#define __inf_image_H

#include "stm32f4xx.h"
#include "lcd_spi_169.h"
#include "delay.h"

void display_attention(void);
void display_no(void);
void display_traffic_lights(void);
void display_cars(void);
void display_ao(void);
void display_tu(void);
void display_aotu(void);

#endif

