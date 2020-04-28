#ifndef __MAIN_H__
#define __MAIN_H__

#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "TM4C123.h"
#include "gpio_port.h"
#include "ps2.h"
#include "lcd.h"
#include "serial_debug.h"
#include "timers.h"
//#include "tank.h"
#include "images.h"
#include "driver_defines.h"
#include "validate.h"
#include "print.h"

extern void init_hardware(void);
extern void initialize_serial_debug(void);
extern void put_string(char *data);

enum direction{up,down,left,right};

typedef struct
{
	int xPos;
	int yPos;
	enum direction direction;
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
