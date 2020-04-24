
#include "main.h"

//*****************************************************************************
// Global Vars
//*****************************************************************************
barrier barriers[1];


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




// PlayerMovement
void playerMove(tank *player,bool *alert_move){
	if (ps2_get_x() > 0x7FF){
			if((player->xPos/8 - (upTankWidth/2))>0){
				player->direction = left;
				player->xPos -= 1;
				*alert_move = true;
			}
		}else if(ps2_get_x() < 0x700){
			if((player->xPos/8 + (upTankWidth/2))<COLS){
				player->direction = right;
				player->xPos += 1;
				*alert_move = true;
			}
		}
		
		if (ps2_get_y() > 0x7FF){
			if((player->yPos/8 - (upTankWidth/2))>0){
				player->direction = up;
				player->yPos -= 1;
				*alert_move = true;
			}
		}else if(ps2_get_y() < 0x700){
			if((player->yPos/8 + (upTankWidth/2))<ROWS){
				player->direction = down;
				player->yPos += 1;
				*alert_move = true;
			}
		}
	
}


void initBarriers(){
	barrier block ={20,20,20,20,false};
	barriers[0] = block;
	
}

void drawBarriers(){
	lcd_draw_box(barriers[0].xPos,barriers[0].width,barriers[0].yPos,barriers[0].height,LCD_COLOR_RED,LCD_COLOR_BLACK,2);
	
		
}

int main(void)
{
	bool alert_move = false;
	bool game_over = false;
	tank player;
	player.xPos = 860;
	player.yPos = 860;
	player.direction = up;
	
		//initialize_serial_debug();
			
		//put_string("\n\r******************************\n\r");
		//put_string("ECE353 Final Project Spring 2020\n\r");
		//put_string("Jennifer Kaiser, Andrew Smart, Matthew Beyer");
	//	put_string("\n\r");
	//	put_string("******************************\n\r");  

		init_hardware();
		lcd_draw_image(player.xPos/8,upTankWidth,player.yPos/8,upTankHeight,upTank,LCD_COLOR_GREEN,LCD_COLOR_BLACK);
		initBarriers();
	
	while(!game_over){
		
		
		
		if(alert_move){
			switch(player.direction){
				case up:
					lcd_draw_image(player.xPos/8,upTankWidth,player.yPos/8,upTankHeight,upTank,LCD_COLOR_GREEN,LCD_COLOR_BLACK);
					break;
				case down:
					lcd_draw_image(player.xPos/8,downTankWidth,player.yPos/8,downTankHeight,downTank,LCD_COLOR_GREEN,LCD_COLOR_BLACK);
					break;
				case left:
					lcd_draw_image(player.xPos/8,leftTankWidth,player.yPos/8,leftTankHeight,leftTank,LCD_COLOR_GREEN,LCD_COLOR_BLACK);
					break;
				case right:
					lcd_draw_image(player.xPos/8,rightTankWidth,player.yPos/8,rightTankHeight,rightTank,LCD_COLOR_GREEN,LCD_COLOR_BLACK);
					break;
			}
				
		}
		//check x,y ps2 positions
		playerMove(&player,&alert_move);
		
		//barriers
		drawBarriers();
		
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


