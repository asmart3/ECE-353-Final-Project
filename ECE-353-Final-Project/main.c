
#include "main.h"

//*****************************************************************************
// Global Vars
//*****************************************************************************
barrier barriers[5];
int numBarriers = 5;
int SPEED = 4; //lower number is faster

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

//Check Tank Collision with Barrier
bool checkCollision(int xPos, int yPos){
	int i;
	int rightSide,bottomSide;
	
	xPos /=SPEED;
	yPos /=SPEED;
	xPos -=16;
	yPos -=16;
	rightSide = xPos + upTankWidth;//+16;
	bottomSide = yPos + upTankHeight;//+16;
	
	for(i = 0; i<5;i++){
		if(!barriers[i].broken){
			//check x position
			if(((rightSide>=barriers[i].xPos)&&(rightSide<=(barriers[i].xPos+barriers[i].width))) || ((xPos <= (barriers[i].xPos + barriers[i].width)) && (xPos>=barriers[i].xPos))){
				//check y position
				if(((yPos>=barriers[i].yPos)&&(yPos<=(barriers[i].yPos+barriers[i].height))) || ((bottomSide <= (barriers[i].yPos + barriers[i].height)) && (bottomSide>=barriers[i].yPos))){
					return true;
				}
			}
		}
	}
	
	return false;
}


// PlayerMovement
void playerMove(tank *player,bool *alert_move){
	if (ps2_get_x() > 0x8A5){
			if((player->xPos/SPEED - (upTankWidth/2))>0){
				if(!checkCollision(player->xPos -1,player->yPos)){
					player->direction = left;
					player->xPos -= 1;
					*alert_move = true;
				}
				
			}
		}else if(ps2_get_x() < 0x700){
			if((player->xPos/SPEED + (upTankWidth/2))<COLS){
				if(!checkCollision(player->xPos+1,player->yPos)){
					player->direction = right;
					player->xPos += 1;
					*alert_move = true;
				}
			}
		}
		
		if (ps2_get_y() > 0x8A5){
			if((player->yPos/SPEED - (upTankWidth/2))>0){
				if(!checkCollision(player->xPos,player->yPos-1)){
					player->direction = up;
					player->yPos -= 1;
					*alert_move = true;
				}
			}
		}else if(ps2_get_y() < 0x700){
			if((player->yPos/SPEED + (upTankWidth/2))<ROWS){
				if(!checkCollision(player->xPos,player->yPos+1)){
					player->direction = down;
					player->yPos += 1;
					*alert_move = true;
				}
			}
		}
	
}


void initBarriers(){
	int i;
	barrier block ={40,40,40,40,false};
	barrier block2 ={20,20,20,20,true};
	barrier block3 ={180,220,35,35,false};
	
	barriers[0] = block; 
	barriers[1] = block3;
	for(i = 2;i<numBarriers;i++){
		barriers[i] = block2;
	}
	
	
}

void drawBarriers(){
	int i;
	for(i=0;i<numBarriers;i++){
		if(!barriers[i].broken)
			lcd_draw_box(barriers[i].xPos,barriers[i].width,barriers[i].yPos,barriers[i].height,LCD_COLOR_RED,LCD_COLOR_BLACK,2);
	}
	
		
}

int main(void)
{
	bool alert_move = false;
	bool game_over = false;
	tank player;
	player.xPos = 100 * SPEED;
	player.yPos = 100 * SPEED;
	player.direction = up;
	
		//initialize_serial_debug();
			
		//put_string("\n\r******************************\n\r");
		//put_string("ECE353 Final Project Spring 2020\n\r");
		//put_string("Jennifer Kaiser, Andrew Smart, Matthew Beyer");
	//	put_string("\n\r");
	//	put_string("******************************\n\r");  

		init_hardware();
		lcd_draw_image(player.xPos/SPEED,upTankWidth,player.yPos/SPEED,upTankHeight,upTank,LCD_COLOR_GREEN,LCD_COLOR_BLACK);
		initBarriers();
	
	while(!game_over){
		
		
		
		if(alert_move){
			switch(player.direction){
				case up:
					lcd_draw_image(player.xPos/SPEED,upTankWidth,player.yPos/SPEED,upTankHeight,upTank,LCD_COLOR_GREEN,LCD_COLOR_BLACK);
					break;
				case down:
					lcd_draw_image(player.xPos/SPEED,downTankWidth,player.yPos/SPEED,downTankHeight,downTank,LCD_COLOR_GREEN,LCD_COLOR_BLACK);
					break;
				case left:
					lcd_draw_image(player.xPos/SPEED,leftTankWidth,player.yPos/SPEED,leftTankHeight,leftTank,LCD_COLOR_GREEN,LCD_COLOR_BLACK);
					break;
				case right:
					lcd_draw_image(player.xPos/SPEED,rightTankWidth,player.yPos/SPEED,rightTankHeight,rightTank,LCD_COLOR_GREEN,LCD_COLOR_BLACK);
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


