
#include "main.h"

//*****************************************************************************
// Global Vars
//*****************************************************************************
barrier barriers[5];
int numBarriers = 5;
int SPEED = 4; //lower number is faster
//direction of the player's tank
volatile PS2_DIR_t TANK_DIRECTION = PS2_DIR_CENTER;

volatile bool enemy1dead = true;
volatile bool enemy2dead = true;
volatile bool enemy3dead = true;
static const uint16_t   MOVE_AMOUNT[] = {100, 150, 100, 100, 125, 150, 175, 200};

tank player;
bool alert_move = false;
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



bool tankCollision(tank *player, tank *enemy1, tank *enemy2, tank *enemy3, bool checkPlayer, bool checkEnemy1, bool checkEnemy2, bool checkEnemy3){
	tank checker;
	bool needNewDirection1 = false;
	bool needNewDirection2 = false;
	bool needNewDirection3 = false;
	bool needNewDirection4 = false;
	//sets tank checker's cordinates to appropriate tank
	if(checkPlayer){
		checker.xPos = player->xPos/SPEED;
		checker.yPos = player->yPos/SPEED;
	} else if(checkEnemy1) {
		checker.xPos = enemy1->xPos/SPEED;
		checker.yPos = enemy1->yPos/SPEED;
	} else if(checkEnemy2) {
		checker.xPos = enemy2->xPos/SPEED;
		checker.yPos = enemy2->yPos/SPEED;
	} else if(checkEnemy3) {
		checker.xPos = enemy3->xPos/SPEED;
		checker.yPos = enemy3->yPos/SPEED;
	}
	//goes through checking the given tank's coordinates against those of the player's tank
	if (checker.xPos - upTankWidth/2 >= player->xPos/SPEED - upTankWidth/2 && checker.xPos - upTankWidth/2 <= player->xPos/SPEED + upTankWidth/2 && checkPlayer == false){
		// left-most x coordinate of the given is within the x coordinate range of the player
		if (checker.yPos- upTankWidth/2 >= player->yPos/SPEED - upTankWidth/2 && checker.yPos - upTankWidth/2 <= player->yPos/SPEED + upTankWidth/2){
			//tanks are touching, need a new direction
			needNewDirection4 = true;
		} else if (checker.yPos+ upTankWidth/2 >= player->yPos/SPEED - upTankWidth/2 && checker.yPos + upTankWidth/2 <= player->yPos/SPEED + upTankWidth/2){
			//tanks are touching, need a new direction
			needNewDirection4 = true;
		}
	}
	else if (checker.xPos + upTankWidth/2 >= player->xPos/SPEED - upTankWidth/2 && checker.xPos + upTankWidth/2 <= player->xPos/SPEED + upTankWidth/2 && checkPlayer == false){
		// right-most x coordinate of the given tank is within the x coordinate range of player
		if(checker.yPos- upTankWidth/2 >= player->yPos/SPEED - upTankWidth/2 && checker.yPos- upTankWidth/2 <= player->yPos/SPEED + upTankWidth/2){
			//tanks are touching, need a new direction
			needNewDirection4 = true;
		}
		// up-most y coordinate of given tank is within y coordinate range of player, up/right side of ship is touching given tank
		else if (checker.yPos+ upTankWidth/2 >= player->yPos/SPEED - upTankWidth/2 && checker.yPos + upTankWidth/2 <= player->yPos/SPEED + upTankWidth/2){
			// down-most y coordinate of player tank is within y coordinate range of enemy1, down/right side of ship is touching enemy1
			//tanks are touching, need a new direction
			needNewDirection4 = true;
		}
			
	}
	if (!checkEnemy1) {
		//checks given tank against enemy1
		if (checker.xPos - upTankWidth/2 + 3  >= enemy1->xPos/SPEED - upTankWidth/2 && checker.xPos - upTankWidth/2 <= enemy1->xPos/SPEED + upTankWidth/2 +3 ){
			// left-most x coordinate of the given tank is within the x coordinate range of the enemy1
			if (checker.yPos- upTankWidth/2 + 3 >= enemy1->yPos/SPEED - upTankWidth/2 && checker.yPos - upTankWidth/2 <= enemy1->yPos/SPEED + upTankWidth/2 +3 ){
				//tanks are touching, need a new direction
				needNewDirection1 = true;
			}
			else if (checker.yPos+ upTankWidth/2 + 3>= enemy1->yPos/SPEED - upTankWidth/2 && checker.yPos + upTankWidth/2 <= enemy1->yPos/SPEED + upTankWidth/2 +3 ){
				//tanks are touching, need a new direction
				needNewDirection1 = true;
			}
		}
		else if (checker.xPos + upTankWidth/2 + 3 >= enemy1->xPos/SPEED - upTankWidth/2 && checker.xPos + upTankWidth/2 <= enemy1->xPos/SPEED + upTankWidth/2 +3){
			// right-most x coordinate of the given tank is within the x coordinate range of enemy1
			if(checker.yPos- upTankWidth/2 >= enemy1->yPos/SPEED - upTankWidth/2 && checker.yPos- upTankWidth/2 <= enemy1->yPos/SPEED + upTankWidth/2 +1){
				//tanks are touching, need a new direction
				needNewDirection1 = true;
			}
				// up-most y coordinate of given tank is within y coordinate range of enemy1, up/right side of ship is touching given tank
			else if (checker.yPos+ upTankWidth/2 + 3 >= enemy1->yPos/SPEED - upTankWidth/2 && checker.yPos + upTankWidth/2 <= enemy1->yPos/SPEED + upTankWidth/2 +3){
				// down-most y coordinate of player tank is within y coordinate range of enemy1, down/right side of ship is touching enemy1
				//tanks are touching, need a new direction
				needNewDirection1 = true;
			}
				
		}
	}
	
	//
	//
	//
	//checks to see if given tank touches enemy2 at any point
	if (!checkEnemy2) {
		if (checker.xPos - upTankWidth/2 + 6 >= enemy2->xPos/SPEED - upTankWidth/2 && checker.xPos - upTankWidth/2 <= enemy2->xPos/SPEED + upTankWidth/2 +6){
			// left-most x coordinate of the tank is within the x coordinate range of the enemy2
			if (checker.yPos- upTankWidth/2 + 6 >= enemy2->yPos/SPEED - upTankWidth/2 && checker.yPos - upTankWidth/2 <= enemy2->yPos/SPEED + upTankWidth/2 +6){
				//tanks are touching, need a new direction
				needNewDirection2 = true;
			}
			else if (checker.yPos+ upTankWidth/2 >= enemy2->yPos - upTankWidth/2 && checker.yPos+ upTankWidth/2 <= enemy2->yPos/SPEED + upTankWidth/2 +6){
				//tanks are touching, need a new direction
				needNewDirection2 = true;
			}
		}
		else if (checker.xPos + upTankWidth/2 + 6 >= enemy2->xPos/SPEED - upTankWidth/2 && checker.xPos + upTankWidth/2 <= enemy2->xPos/SPEED + upTankWidth/2 +6){
			// right-most x coordinate of the given tank is within the x coordinate range of enemy2
			if(checker.yPos - upTankWidth/2 >= enemy2->yPos/SPEED - upTankWidth/2 && checker.yPos - upTankWidth/2 <= enemy2->yPos/SPEED + upTankWidth/2 +6){
				// up-most y coordinate of given tank is within y coordinate range of enemy2, up/right side of ship is touching enemy2
				//tanks are touching, need a new direction
				needNewDirection2 = true;
			}
			else if (checker.yPos + upTankWidth/2 +6 >= enemy2->yPos/SPEED - upTankWidth/2 && checker.yPos + upTankWidth/2 <= enemy2->yPos/SPEED + upTankWidth/2 +6){
				// down-most y coordinate of given tank is within y coordinate range of enemy1, down/right side of ship is touching enemy2
				//tanks are touching, need a new direction
				needNewDirection2 = true;
			}
		}
	}
if (!checkEnemy3) {
		if (checker.xPos - upTankWidth/2 +9 >= enemy3->xPos/SPEED - upTankWidth/2 && checker.xPos- upTankWidth/2 <= enemy3->xPos/SPEED + upTankWidth/2 +9){
			// left-most x coordinate of the given tank is within the x coordinate range of the enemy3
			if (checker.yPos - upTankWidth/2 +9>= enemy3->yPos/SPEED - upTankWidth/2 && checker.yPos - upTankWidth/2 <= enemy3->yPos/SPEED + upTankWidth/2 +9){
				//tanks are touching, need a new direction
				needNewDirection3 = true;
			}
			else if (checker.yPos + upTankWidth/2 +9 >= enemy3->yPos - upTankWidth/2 && checker.yPos + upTankWidth/2 <= enemy3->yPos/SPEED + upTankWidth/2 +9){
				//tanks are touching, need a new direction
				needNewDirection3 = true;
			}
		}
		else if (checker.xPos + upTankWidth/2 +9 >= enemy3->xPos/SPEED - upTankWidth/2 && checker.xPos + upTankWidth/2 <= enemy3->xPos/SPEED + upTankWidth/2 +9){
			// right-most x coordinate of the given tank is within the x coordinate range of enemy3
			if(checker.yPos - upTankWidth/2 +9>= enemy3->yPos/SPEED - upTankWidth/2 && checker.yPos- upTankWidth/2 <= enemy3->yPos/SPEED + upTankWidth/2 +9){
				// up-most y coordinate of given tank is within y coordinate range of enemy3, up/right side of ship is touching enemy3
				//tanks are touching, need a new direction
				return needNewDirection3 = true;
			}	
			else if (checker.yPos + upTankWidth/2 +9 >= enemy3->yPos/SPEED - upTankWidth/2 && checker.yPos + upTankWidth/2 <= enemy3->yPos/SPEED + upTankWidth/2 +9){
				// down-most y coordinate of player tank is within y coordinate range of given tank, down/right side of ship is touching enemy3
				//tanks are touching, need a new direction
				needNewDirection3 = true;
			}
		}
	}
	//completely changes enemies direction to the opposite direction
	if (needNewDirection1 == true) {
		switch(enemy1->direction){
					enemy1->moves = 25;
					case right:
						enemy1->direction = left;
						break;
					case left:
						enemy1->direction = right;	
						break;
					case down:
						enemy1->direction = up;	
						break;
					case up:
						enemy1->direction = down;
						break;
				}	
	}
	if (needNewDirection2 == true) {
		switch(enemy2->direction){
					enemy2->moves = 25;
					case right:
						enemy2->direction = left;
						break;
					case left:
						enemy2->direction = right;	
						break;
					case down:
						enemy2->direction = up;	
						break;
					case up:
						enemy2->direction = down;
						break;
				}	
	}
	//switches enemy 3's direction to the opposite direction
	if (needNewDirection3 == true) {
		switch(enemy3->direction){
					enemy3->moves = 25;
					case right:
						enemy3->direction = left;
						break;
					case left:
						enemy3->direction = right;	
						break;
					case down:
						enemy3->direction = up;	
						break;
					case up:
						enemy3->direction = down;
						break;
				}	
	}
	if (needNewDirection1 | needNewDirection2 | needNewDirection3 | needNewDirection4) {
		return true;
	}
	return false;
}

//generates a random new direction depending on which tank needs a new direction
void newDirection(tank *enemy1, bool directionEnemy1, bool directionEnemy2, bool directionEnemy3){
	//random direction where: 0 = left, 1 = right, 2 = up, 3 = right
	tank checkDirection1;
	int randomnumber;
	int randomnumber2;
	int randomnumber3;
  
	
	checkDirection1.direction = enemy1->direction;
	//while loop to make sure new random direction is not the same as it was previous
	while (checkDirection1.direction  ==  enemy1->direction){
		randomnumber = rand() % 4;
		randomnumber2 = rand() % 4;
		randomnumber3 = rand() % 4;
		if (directionEnemy1){
			switch(randomnumber){
					case 0:
						enemy1->direction = left;
						break;
					case 1:
						enemy1->direction = right;	
						break;
					case 2:
						enemy1->direction = up;	
						break;
					case 3:
						enemy1->direction = down;
						break;
				}		
		}
		if (directionEnemy2){
			switch(randomnumber2){
					case 0:
						enemy1->direction = left;
						break;
					case 1:
						enemy1->direction = right;	
						break;
					case 2:
						enemy1->direction = up;	
						break;
					case 3:
						enemy1->direction = down;
						break;
				}		
		}
		if (directionEnemy3){
			switch(randomnumber3){
					case 0:
						enemy1->direction = left;
						break;
					case 1:
						enemy1->direction = right;	
						break;
					case 2:
						enemy1->direction = up;	
						break;
					case 3:
						enemy1->direction = down;
						break;
				}		
		}
	}
}

// helps give the enemy tanks random movement
uint16_t getTankMovement(tank *enemy) {
	int randomnumber = 200;
	return MOVE_AMOUNT[rand() % 8]; // tells enemy tank to move a certain amount of pixels
} 

//moves the enemy tanks and checks for collisions
void enemy1Move(tank *player, tank *enemy1, tank *enemy2, tank *enemy3, bool *alert_enemy1, bool *alert_enemy2, bool *alert_enemy3){
	tank directionCheck;
	//checks for a collision between enemy1 and any other tank
	if (tankCollision(player, enemy1, enemy2, enemy3, false, true, false, false)){
		//enemy1 changes into the opposite direction
		switch(enemy1->direction){
						enemy1->moves = getTankMovement(enemy1);
					case right:
						enemy1->direction = left;
						break;
					case left:
						enemy1->direction = right;
						break;
					case down:
						enemy1->direction = up;	
						break;
					case up:
						enemy1->direction = down;
						break;	
		}
	}
	if (tankCollision(player, enemy1, enemy2, enemy3, false, false, true, false)){
		switch(enemy2->direction){
						enemy2->moves = getTankMovement(enemy2);
					case right:
						enemy2->direction = left;
						break;
					case left:
						enemy2->direction = right;
						break;
					case down:
						enemy2->direction = up;
						break;
					case up:
						enemy2->direction = down;
						break;	
		}
	}
	if (tankCollision(player, enemy1, enemy2, enemy3, false, false, false, true)){
		switch(enemy3->direction){
						enemy3->moves = getTankMovement(enemy3);
					case right:
						enemy3->direction = left;
						break;
					case left:
						enemy3->direction = right;	
						break;
					case down:
						enemy3->direction = up;	
						break;
					case up:
						enemy3->direction = down;
						break;	
		}
	}
	
	// checks if enemy tanks still have moves remaining, changes direction if it does not
	if(enemy1->moves == 0 || enemy1->moves == NULL) {
		newDirection(enemy1, true, false, false);
		enemy1->moves = getTankMovement(enemy1);
	}
	if(enemy2->moves == 0 || enemy2->moves == NULL) {
		newDirection(enemy2, false, true, false);
		enemy2->moves = getTankMovement(enemy2);
	}
	if(enemy3->moves == 0 || enemy3->moves == NULL) {
		newDirection(enemy3, false, false, true);
		enemy3->moves = getTankMovement(enemy3);
	}
	
	
	//checks for direction of enemy1
	if (enemy1->direction == left) {
		//checks for left edge of screen and barrier collison
		if(((enemy1->xPos/SPEED - (upTankWidth/2))>0) && (!checkCollision(enemy1->xPos -1,enemy1->yPos))){
				//moves enemy1 left one pixel
			enemy1->xPos -= 1;
			enemy1->moves--; // decrements moves remaining
		} else {
			newDirection(enemy1, true, false, false);
			enemy1->moves = getTankMovement(enemy1);
		}
	}
	if (enemy1->direction == right) {
		//checks for right edge of screen and barrier collison
		if(((enemy1->xPos/SPEED + (upTankWidth/2))<COLS) && (!checkCollision(enemy1->xPos+1,enemy1->yPos))){
				//moves enemy1 right one pixel
			enemy1->xPos += 1;
			enemy1->moves--; // decrements moves remaining
		} else {
			newDirection(enemy1, true, false, false);
			enemy1->moves = getTankMovement(enemy1);
		}
	}
	if (enemy1->direction == up) {
		//checks for left edge of screen and barrier collison
		if(((enemy1->yPos/SPEED - (upTankWidth/2))>0) && (!checkCollision(enemy1->xPos,enemy1->yPos-1))){
				//moves enemy1 up one pixel
			enemy1->yPos -= 1;
			enemy1->moves--; // decrements moves remaining
		} else {
			newDirection(enemy1, true, false, false);
			enemy1->moves = getTankMovement(enemy1);
		}
	}
	if (enemy1->direction == down) {
		//checks for left edge of screen and barrier collison
		if(((enemy1->yPos/SPEED + (upTankWidth/2))<ROWS) && (!checkCollision(enemy1->xPos,enemy1->yPos+1))){
				//moves enemy1 down one pixel
			enemy1->yPos += 1;
			enemy1->moves--; // decrements moves remaining
		} else {
			newDirection(enemy1, true, false, false);
			enemy1->moves = getTankMovement(enemy1);
		}
	}
	
	
	//checks for direction of enemy2
	if (enemy2->direction == left) {
		//checks for left edge of screen and barrier collison
		if(((enemy2->xPos/SPEED - (upTankWidth/2))>0) && (!checkCollision(enemy2->xPos -1,enemy2->yPos))){
				//moves enemy1 left one pixel
			enemy2->xPos -= 1;
			enemy2->moves--; // decrements moves remaining
		} else {
			newDirection(enemy2, false, true, false);
			enemy2->moves = getTankMovement(enemy2);
		}
	}
	if (enemy2->direction == right) {
		//checks for right edge of screen and barrier collison
		if(((enemy2->xPos/SPEED + (upTankWidth/2))<COLS) && (!checkCollision(enemy2->xPos+1,enemy2->yPos))){
				//moves enemy1 right one pixel
			enemy2->xPos += 1;
			enemy2->moves--; // decrements moves remaining
		} else {
			newDirection(enemy2, true, false, false);
			enemy2->moves = getTankMovement(enemy2);
		}
	}
	if (enemy2->direction == up) {
		//checks for left edge of screen and barrier collison
		if(((enemy2->yPos/SPEED - (upTankWidth/2))>0) && (!checkCollision(enemy2->xPos,enemy2->yPos-1))){
				//moves enemy1 up one pixel
			enemy2->yPos -= 1;
			enemy2->moves--; // decrements moves remaining
		} else {
			newDirection(enemy2, false, true, false);
			enemy2->moves = getTankMovement(enemy2);
		}
	}
	if (enemy2->direction == down) {
		//checks for left edge of screen and barrier collison
		if(((enemy2->yPos/SPEED + (upTankWidth/2))<ROWS) && (!checkCollision(enemy2->xPos,enemy2->yPos+1))){
				//moves enemy1 down one pixel
			enemy2->yPos += 1;
			enemy2->moves--; // decrements moves remaining
		} else {
			newDirection(enemy2, false, true, false);
			enemy2->moves = getTankMovement(enemy2);
		}
	}
	

	//checks for direction of enemy3
	if (enemy3->direction == left) {
		//checks for left edge of screen and barrier collison
		if(((enemy3->xPos/SPEED - (upTankWidth/2))>0) && (!checkCollision(enemy3->xPos -1,enemy3->yPos))){
				//moves enemy1 left one pixel
			enemy3->xPos -= 1;
			enemy3->moves--; // decrements moves remaining
		} else {
			newDirection(enemy3, false, false, true);
			enemy3->moves = getTankMovement(enemy3);
		}
	}
	if (enemy3->direction == right) {
		//checks for right edge of screen and barrier collison
		if(((enemy3->xPos/SPEED + (upTankWidth/2))<COLS) && (!checkCollision(enemy3->xPos+1,enemy3->yPos))){
				//moves enemy1 right one pixel
			enemy3->xPos += 1;
			enemy3->moves--;
		} else {
			newDirection(enemy3, false, false, true);
			enemy3->moves = getTankMovement(enemy3);
		}
	}
	if (enemy3->direction == up) {
		//checks for left edge of screen and barrier collison
		if(((enemy3->yPos/SPEED - (upTankWidth/2))>0) && (!checkCollision(enemy3->xPos,enemy3->yPos-1))){
				//moves enemy1 up one pixel
			enemy3->yPos -= 1;
			enemy3->moves--; // decrements moves remaining
		} else {
			newDirection(enemy3, false, false, true);
			enemy3->moves = getTankMovement(enemy3);
		}
	}
	if (enemy3->direction == down) {
		//checks for left edge of screen and barrier collison
		if(((enemy3->yPos/SPEED + (upTankWidth/2))<ROWS) && (!checkCollision(enemy3->xPos,enemy3->yPos+1))){
				//moves enemy1 down one pixel
			enemy3->yPos += 1;
			enemy3->moves--; // decrements moves remaining
		} else {
			newDirection(enemy3, false, false, true);
			enemy3->moves = getTankMovement(enemy3);
		}
	}
	
	*alert_enemy1 = true;
	*alert_enemy2 = true;
	*alert_enemy3 = true;
}

// PlayerMovement
void playerMove(tank *player,bool *alert_move, volatile PS2_DIR_t TANK_DIRECTION){
	if (TANK_DIRECTION == PS2_DIR_LEFT){
		//checks for left edge of screen
			if((player->xPos/SPEED - (upTankWidth/2))>0){
				//checks for collision of barrier
				if(!checkCollision(player->xPos -1,player->yPos)){
					player->direction = left;
					player->xPos -= 1;
					*alert_move = true;
				}
				
			}
		}else if(TANK_DIRECTION == PS2_DIR_RIGHT){
			//checks for right edge of screen
			if((player->xPos/SPEED + (upTankWidth/2))<COLS){
				//checks for collision of barrier
				if(!checkCollision(player->xPos+1,player->yPos)){
					player->direction = right;
					player->xPos += 1;
					*alert_move = true;
				}
			}
		}
		
		if (TANK_DIRECTION == PS2_DIR_UP){
			//checks for top edge of screen
			if((player->yPos/SPEED - (upTankWidth/2))>0){
				//checks for collision of barrier
				if(!checkCollision(player->xPos,player->yPos-1)){
					player->direction = up;
					player->yPos -= 1;
					*alert_move = true;
				}
			}
		}else if(TANK_DIRECTION == PS2_DIR_DOWN){
			//checks for bottom of screen
			if((player->yPos/SPEED + (upTankWidth/2))<ROWS){
				//checks for collision of barrier
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

//resets the enemies postions so that they can be redrawn
void checkEnemydead(tank *enemy1, tank *enemy2, tank *enemy3){
	if (enemy1dead){
		//changes enemy1's positon to the lower right corner
		enemy1->xPos = 220*SPEED;
		enemy1->yPos = 300*SPEED;
		enemy1->direction = up;
		//redraws the enemy1
		lcd_draw_image(enemy1->xPos/SPEED,upTankWidth,enemy1->yPos/SPEED,upTankHeight,upTank,LCD_COLOR_BLUE,LCD_COLOR_BLACK);
		enemy1dead = false;
	}
	if (enemy2dead){
		//changes enemy2's positon to the top right corner
		enemy2->xPos = 220*SPEED;
		enemy2->yPos = 20*SPEED;
		enemy2->direction = down;
		//redraws enemy2
		lcd_draw_image(enemy2->xPos/SPEED,downTankWidth,enemy2->yPos/SPEED,downTankHeight,downTank,LCD_COLOR_BLUE,LCD_COLOR_BLACK);
		enemy2dead = false;
	}
	if (enemy3dead){
		//changes enemy3's positon to the lower left corner
		enemy3->xPos = 20*SPEED;
		enemy3->yPos = 300*SPEED;
		enemy3->direction = up;
		//redraws enemy3
		lcd_draw_image(enemy3->xPos/SPEED,upTankWidth,enemy3->yPos/SPEED,upTankHeight,upTank,LCD_COLOR_BLUE,LCD_COLOR_BLACK);
		enemy3dead = false;
	}
}

int main(void)
{
	bool alert_move = false;
	bool game_over = false;
	bool alert_enemy1 = false;
	bool alert_enemy2 = false;
	bool alert_enemy3 = false;
	tank player;
	tank enemy1;
	tank enemy2;
	tank enemy3;
	player.xPos = 100 * SPEED;
	player.yPos = 100 * SPEED;
	player.direction = up;
	init_hardware();
	
		
		put_string("\n\r******************************\n\r");
		put_string("ECE353 Final Project Spring 2020\n\r");
		put_string("Jennifer Kaiser, Andrew Smart, Matthew Beyer\n\r");
		put_string("******************************\n\r\n\r");  

		put_string("Running...");
	  //to draw the enemies
		checkEnemydead(&enemy1, &enemy2, &enemy3);
		lcd_draw_image(player.xPos/SPEED,upTankWidth,player.yPos/SPEED,upTankHeight,upTank,LCD_COLOR_GREEN,LCD_COLOR_BLACK);
		initBarriers();
	
	while(!game_over){
		
		//draws the players tank according to direction
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
		if(alert_enemy1){
			switch(enemy1.direction){
				case up:
					lcd_draw_image(enemy1.xPos/SPEED,upTankWidth,enemy1.yPos/SPEED,upTankHeight,upTank,LCD_COLOR_BLUE,LCD_COLOR_BLACK);
					break;
				case down:
					lcd_draw_image(enemy1.xPos/SPEED,downTankWidth,enemy1.yPos/SPEED,downTankHeight,downTank,LCD_COLOR_BLUE,LCD_COLOR_BLACK);
					break;
				case left:
					lcd_draw_image(enemy1.xPos/SPEED,leftTankWidth,enemy1.yPos/SPEED,leftTankHeight,leftTank,LCD_COLOR_BLUE,LCD_COLOR_BLACK);
					break;
				case right:
					lcd_draw_image(enemy1.xPos/SPEED,rightTankWidth,enemy1.yPos/SPEED,rightTankHeight,rightTank,LCD_COLOR_BLUE,LCD_COLOR_BLACK);
					break;
			}
				
		}
		if(alert_enemy2){
			switch(enemy2.direction){
				case up:
					lcd_draw_image(enemy2.xPos/SPEED,upTankWidth,enemy2.yPos/SPEED,upTankHeight,upTank,LCD_COLOR_GREEN,LCD_COLOR_BLACK);
					break;
				case down:
					lcd_draw_image(enemy2.xPos/SPEED,downTankWidth,enemy2.yPos/SPEED,downTankHeight,downTank,LCD_COLOR_GREEN,LCD_COLOR_BLACK);
					break;
				case left:
					lcd_draw_image(enemy2.xPos/SPEED,leftTankWidth,enemy2.yPos/SPEED,leftTankHeight,leftTank,LCD_COLOR_GREEN,LCD_COLOR_BLACK);
					break;
				case right:
					lcd_draw_image(enemy2.xPos/SPEED,rightTankWidth,enemy2.yPos/SPEED,rightTankHeight,rightTank,LCD_COLOR_GREEN,LCD_COLOR_BLACK);
					break;
			}
				
		}
		if(alert_enemy3){
			switch(enemy3.direction){
				case up:
					lcd_draw_image(enemy3.xPos/SPEED,upTankWidth,enemy3.yPos/SPEED,upTankHeight,upTank,LCD_COLOR_RED,LCD_COLOR_BLACK);
					break;
				case down:
					lcd_draw_image(enemy3.xPos/SPEED,downTankWidth,enemy3.yPos/SPEED,downTankHeight,downTank,LCD_COLOR_RED,LCD_COLOR_BLACK);
					break;
				case left:
					lcd_draw_image(enemy3.xPos/SPEED,leftTankWidth,enemy3.yPos/SPEED,leftTankHeight,leftTank,LCD_COLOR_RED,LCD_COLOR_BLACK);
					break;
				case right:
					lcd_draw_image(enemy3.xPos/SPEED,rightTankWidth,enemy3.yPos/SPEED,rightTankHeight,rightTank,LCD_COLOR_RED,LCD_COLOR_BLACK);
					break;
			}
				
		}
		//uses interupts to move the player
		if(TIMER2_ALERT){
			//check x,y ps2 positions
			playerMove(&player, &alert_move, PS2_DIR);
			TIMER2_ALERT = false;
		}
		
		if(TIMER3_ALERT){
			enemy1Move(&player, &enemy1, &enemy2, &enemy3, &alert_enemy1, &alert_enemy2, &alert_enemy3);
		}
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
  init_serial_debug(true,true);
  // Update the Space Shipt 60 times per second.
  gp_timer_config_32(TIMER2_BASE,TIMER_TAMR_TAMR_PERIOD, 1000000, false, true);
  gp_timer_config_32(TIMER3_BASE,TIMER_TAMR_TAMR_PERIOD, 500000, false, true);
  gp_timer_config_32(TIMER4_BASE,TIMER_TAMR_TAMR_PERIOD, 50000, false, true);
}


