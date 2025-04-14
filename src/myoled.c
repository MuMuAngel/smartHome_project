/*
 * Copyright (c) 2015, Vladimir Komendantskiy
 * MIT License
 *
 * SSD1306 demo of block and font drawing.
 */

//
// fixed for OrangePiZero by HypHop
//

#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>

#include "oled.h"
#include "font.h"
#include "myoled.h"

#define FILE_NAME "/dev/i2c-3"

#define STR_CMP(str1, str2) (strcmp(str1, str2) == 0) // compare two string
struct display_info disp;
struct oled_str
{
	char dev_name[128];
	char msg[128];
};
// clear screen
void oled_clear_local(struct display_info *disp, uint8_t x, uint8_t y)
{
	for (int k = 0; k < 100; k++)
	{
		for (int i = 0; i < disp->font.width; i++)
		{
			for (int j = 0; j < disp->font.height; j++)
			{
				oled_putpixel(disp, x + i, y + j, 0);
			}
		}
		x += disp->font.width + disp->font.spacing;
	}
	disp->font = font1;
	oled_send_buffer(disp);
}

int myoled_init(void)
{
	int e;
	memset(&disp, 0, sizeof(disp));
	disp.address = OLED_I2C_ADDR;
	disp.font = font2;

	e = oled_open(&disp, FILE_NAME);
	e = oled_init(&disp);
	oled_clear(&disp);
	return e;
}

int oled_show(void *arg)
{
	int i;
	struct oled_str *str = NULL;
	// str = (struct oled_str *)malloc(sizeof(struct oled_str));
	if (arg != NULL)
	{
		str = (struct oled_str *)arg;
	}
	unsigned char buf[512];
	if (STR_CMP(str->dev_name, "DRled"))
	{
		memset(buf, 0, sizeof(buf));
		sprintf(buf, "%s: %s", str->dev_name, str->msg);
		oled_clear_local(&disp, 0, 0);
		oled_putstrto(&disp, 0, 0, buf);
		disp.font = font1;
	}
	else if (STR_CMP(str->dev_name, "BRled"))
	{
		memset(buf, 0, sizeof(buf));
		sprintf(buf, "%s: %s", str->dev_name, str->msg);
		oled_clear_local(&disp, 0, 10);
		oled_putstrto(&disp, 0, 10, buf);
		disp.font = font1;
	}
	else if (STR_CMP(str->dev_name, "fan"))
	{
		memset(buf, 0, sizeof(buf));
		sprintf(buf, "%s: %s", str->dev_name, str->msg);
		oled_clear_local(&disp, 0, 20);
		oled_putstrto(&disp, 0, 20, buf);
		disp.font = font1;
	}
	else if (STR_CMP(str->dev_name, "beep"))
	{
		memset(buf, 0, sizeof(buf));
		sprintf(buf, "%s: %s", str->dev_name, str->msg);
		oled_clear_local(&disp, 0, 30);
		oled_putstrto(&disp, 0, 30, buf);
		disp.font = font1;
	}
	else if (STR_CMP(str->dev_name, "lock"))
	{
		memset(buf, 0, sizeof(buf));
		sprintf(buf, "%s: %s", str->dev_name, str->msg);
		oled_clear_local(&disp, 0, 40);
		oled_putstrto(&disp, 0, 40, buf);
		disp.font = font1;
	}

	// oled_putstrto(&disp, 0, 10, "hello mumu");
	// disp.font = font2;
	oled_send_buffer(&disp);
#if 0
	//putstrto(disp, 0, 0, "Spnd spd  2468 rpm");
	//	oled_putstrto(disp, 0, 9+1, "Spnd cur  0.46 A");
	oled_putstrto(&disp, 0, 10, "Welcome to My world");
	disp.font = font1;
	oled_putstrto(&disp, 0, 20, "hello mumu");
	disp.font = font2;
	//	oled_putstrto(disp, 0, 18+2, "Spnd tmp    53 C");
	//	oled_putstrto(disp, 0, 27+3, "DrvX tmp    64 C");
	//	oled_putstrto(disp, 0, 54, "Total cur  2.36 A");
	oled_send_buffer(&disp);

	//oled_putpixel(disp, 60, 45);
	//oled_putstr(disp, 1, "hello");
#endif
	return 0;
}
