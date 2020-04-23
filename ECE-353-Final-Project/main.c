
#include "main.h"

//*****************************************************************************
//*****************************************************************************
void DisableInterrupts(void)
{
  __asm {
         CPSID  I
  }
}

//*****************************************************************************
//*****************************************************************************
void EnableInterrupts(void)
{
  __asm {
    CPSIE  I
  }
}


int main(void)
{
	bool alert_move = false;
	bool game_over = false;
	tank player;
	player.xPos = 40;
	player.yPos = 40;
	
		//initialize_serial_debug();
			
		//put_string("\n\r******************************\n\r");
		//put_string("ECE353 Final Project Spring 2020\n\r");
		//put_string("Jennifer Kaiser, Andrew Smart, Matthew Beyer");
	//	put_string("\n\r");
	//	put_string("******************************\n\r");  

		init_hardware();
		
	
	
	while(!game_over){
		if(alert_move){
			lcd_draw_image(player.xPos/8,tankWidthPixels,player.yPos/8,tankHeightPixels,tankBitmaps,LCD_COLOR_GREEN,LCD_COLOR_BLACK);
		}
		//check x,y ps2 positions
		if (ps2_get_x() > 0x7FF){
			player.xPos -= 1;
			alert_move = true;
		}else if(ps2_get_x() < 0x700){
			player.xPos += 1;
			alert_move = true;
		}
		
		if (ps2_get_y() > 0x7FF){
			player.yPos -= 1;
			alert_move = true;
		}else if(ps2_get_y() < 0x700){
			player.yPos += 1;
			alert_move = true;
		}
		
	}
	
}  

//*****************************************************************************
// Initializes all of the peripherls used in HW3
//*****************************************************************************
void init_hardware(void)
{
  lcd_config_gpio();
  lcd_config_screen();
  lcd_clear_screen(LCD_COLOR_BLACK);
  ps2_initialize();
  init_serial_debug(false,false);
  // Update the Space Shipt 60 times per second.
  //gp_timer_config_32(TIMER2_BASE,TIMER_TAMR_TAMR_PERIOD, 1000000, false, true);
  //gp_timer_config_32(TIMER3_BASE,TIMER_TAMR_TAMR_PERIOD, 500000, false, true);
  //gp_timer_config_32(TIMER4_BASE,TIMER_TAMR_TAMR_PERIOD, 50000, false, true);
}


