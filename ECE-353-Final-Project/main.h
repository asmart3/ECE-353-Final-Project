#ifndef __MAIN_H__
#define __MAIN_H__

#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include "TM4C123.h"
#include "gpio_port.h"
#include "ps2.h"
#include "lcd.h"
#include "serial_debug.h"
//#include "timers.h"
//#include "tank.h"
#include "images.h"

extern void init_hardware(void);


typedef struct
{
	int xPos;
	int yPos;
	int direction;
	int delay; // time since last shot
}	tank;

typedef struct
{
	int xPos;
	int yPos;
	int width;
	int height;
	bool broken;
} barrier;
#endif
