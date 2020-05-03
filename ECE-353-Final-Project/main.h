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
#include "ft6x06.h"
#include "images.h"
#include "driver_defines.h"
#include "validate.h"
#include "print.h"
#include "launchpad_io.h"
#include "interrupts.h"
#include "uart.h"
#include "io_expander.h"


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
	int moves;
	bool dead;
	int waitTime;
}	tank;

typedef struct
{
	int xPos;
	int yPos;
	int width;
	int height;
	bool broken;
} barrier;

typedef struct
{
	int xPos;
	int yPos;
	enum direction direction;
	bool active;	
	int waitTime;
}bullet;

typedef enum{
  PS2_DIR_UP,
  PS2_DIR_DOWN,
  PS2_DIR_LEFT,
  PS2_DIR_RIGHT,
  PS2_DIR_CENTER,
  PS2_DIR_INIT,
} PS2_DIR_t;


extern volatile PS2_DIR_t PS2_DIR;
extern volatile bool TIMER1_ALERT;
extern volatile bool TIMER2_ALERT;
extern volatile bool TIMER3_ALERT;
#endif
