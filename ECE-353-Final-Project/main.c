
#include "main.h"

//*****************************************************************************
// Global Vars
//*****************************************************************************
barrier barriers[6];
int numBarriers = 6;
int SPEED = 1; //lower number is faster
int ENEMYSPEED = 1;

//direction of the player's tank
volatile PS2_DIR_t TANK_DIRECTION = PS2_DIR_CENTER;

volatile bool enemy1dead = true;
volatile bool enemy2dead = true;
volatile bool enemy3dead = true;


static const uint16_t   MOVE_AMOUNT[] = {100, 150, 100, 100, 125, 150, 175, 200};


bool alert_move = false;

//player bullets
bullet bullet_array[3];

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
	
	for(i = 0; i<6; i++){
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
	int downTankWidth1 = downTankWidth + 50;
	int downTankWidth2 = downTankWidth + 20;
	int downTankWidth3 = downTankWidth + 35;
	//sets tank checker's cordinates to appropriate tank
	if(checkPlayer){
		checker.xPos = player->xPos/ENEMYSPEED;
		checker.yPos = player->yPos/ENEMYSPEED;
	} else if(checkEnemy1) {
		checker.xPos = enemy1->xPos/ENEMYSPEED;
		checker.yPos = enemy1->yPos/ENEMYSPEED;
	} else if(checkEnemy2) {
		checker.xPos = enemy2->xPos/ENEMYSPEED;
		checker.yPos = enemy2->yPos/ENEMYSPEED;
	} else if(checkEnemy3) {
		checker.xPos = enemy3->xPos/ENEMYSPEED;
		checker.yPos = enemy3->yPos/ENEMYSPEED;
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
		if (checker.xPos - upTankWidth/2   >= enemy1->xPos/ENEMYSPEED - downTankWidth1/2 && checker.xPos - downTankWidth1/2 <= enemy1->xPos/ENEMYSPEED + upTankWidth/2 ){
			// left-most x coordinate of the given tank is within the x coordinate range of the enemy1
			if (checker.yPos- upTankWidth/2 >= enemy1->yPos/ENEMYSPEED - downTankWidth1/2 && checker.yPos - downTankWidth1/2 <= enemy1->yPos/ENEMYSPEED + upTankWidth/2  ){
				//tanks are touching, need a new direction
				needNewDirection1 = true;
			}
			else if (checker.yPos+ upTankWidth/2 >= enemy1->yPos/ENEMYSPEED - downTankWidth1/2 && checker.yPos + upTankWidth/2 <= enemy1->yPos/ENEMYSPEED + upTankWidth/2  ){
				//tanks are touching, need a new direction
				needNewDirection1 = true;
			}
		}
		else if (checker.xPos + upTankWidth/2  >= enemy1->xPos/ENEMYSPEED - downTankWidth1/2 && checker.xPos + upTankWidth/2 <= enemy1->xPos/ENEMYSPEED + upTankWidth/2 ){
			// right-most x coordinate of the given tank is within the x coordinate range of enemy1
			if(checker.yPos- downTankWidth1/2 >= enemy1->yPos/ENEMYSPEED - downTankWidth1/2 && checker.yPos- downTankWidth1/2 <= enemy1->yPos/ENEMYSPEED + upTankWidth/2 ){
				//tanks are touching, need a new direction
				needNewDirection1 = true;
			}
				// up-most y coordinate of given tank is within y coordinate range of enemy1, up/right side of ship is touching given tank
			else if (checker.yPos+ upTankWidth/2  >= enemy1->yPos/ENEMYSPEED - downTankWidth1/2 && checker.yPos + upTankWidth/2 <= enemy1->yPos/ENEMYSPEED + upTankWidth/2 ){
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
		if (checker.xPos - downTankWidth2/2 >= enemy2->xPos/ENEMYSPEED - downTankWidth2/2 && checker.xPos - downTankWidth2/2 <= enemy2->xPos/ENEMYSPEED + upTankWidth/2 ){
			// left-most x coordinate of the tank is within the x coordinate range of the enemy2
			if (checker.yPos- downTankWidth2/2 >= enemy2->yPos/ENEMYSPEED - downTankWidth2/2 && checker.yPos - downTankWidth2/2 <= enemy2->yPos/ENEMYSPEED + upTankWidth/2 ){
				//tanks are touching, need a new direction
				needNewDirection2 = true;
			}
			else if (checker.yPos+ upTankWidth/2 >= enemy2->yPos/ENEMYSPEED - downTankWidth2/2 && checker.yPos+ upTankWidth/2 <= enemy2->yPos/ENEMYSPEED + upTankWidth/2 ){
				//tanks are touching, need a new direction
				needNewDirection2 = true;
			}
		}
		else if (checker.xPos + upTankWidth/2  >= enemy2->xPos/ENEMYSPEED - downTankWidth2/2 && checker.xPos + upTankWidth/2 <= enemy2->xPos/ENEMYSPEED + upTankWidth/2 ){
			// right-most x coordinate of the given tank is within the x coordinate range of enemy2
			if(checker.yPos - downTankWidth2/2 >= enemy2->yPos/ENEMYSPEED - downTankWidth2/2 && checker.yPos - downTankWidth2/2 <= enemy2->yPos/ENEMYSPEED + upTankWidth/2 ){
				// up-most y coordinate of given tank is within y coordinate range of enemy2, up/right side of ship is touching enemy2
				//tanks are touching, need a new direction
				needNewDirection2 = true;
			}
			else if (checker.yPos + upTankWidth/2 >= enemy2->yPos/ENEMYSPEED - downTankWidth2/2 && checker.yPos + upTankWidth/2 <= enemy2->yPos/ENEMYSPEED + upTankWidth/2 ){
				// down-most y coordinate of given tank is within y coordinate range of enemy1, down/right side of ship is touching enemy2
				//tanks are touching, need a new direction
				needNewDirection2 = true;
			}
		}
	}
if (!checkEnemy3) {
		if (checker.xPos - downTankWidth3/2  >= enemy3->xPos/ENEMYSPEED - downTankWidth3/2 && checker.xPos- downTankWidth3/2 <= enemy3->xPos/ENEMYSPEED + upTankWidth/2 ){
			// left-most x coordinate of the given tank is within the x coordinate range of the enemy3
			if (checker.yPos - downTankWidth3/2 >= enemy3->yPos/ENEMYSPEED - downTankWidth3/2 && checker.yPos - downTankWidth3/2 <= enemy3->yPos/ENEMYSPEED + upTankWidth/2 ){
				//tanks are touching, need a new direction
				needNewDirection3 = true;
			}
			else if (checker.yPos + upTankWidth/2  >= enemy3->yPos/ENEMYSPEED - downTankWidth3/2 && checker.yPos + upTankWidth/2 <= enemy3->yPos/ENEMYSPEED + upTankWidth/2 ){
				//tanks are touching, need a new direction
				needNewDirection3 = true;
			}
		}
		else if (checker.xPos + upTankWidth/2  >= enemy3->xPos/ENEMYSPEED - downTankWidth3/2 && checker.xPos + upTankWidth/2 <= enemy3->xPos/ENEMYSPEED + upTankWidth/2 ){
			// right-most x coordinate of the given tank is within the x coordinate range of enemy3
			if(checker.yPos - downTankWidth3/2 >= enemy3->yPos/ENEMYSPEED - downTankWidth3/2 && checker.yPos- downTankWidth3/2 <= enemy3->yPos/ENEMYSPEED + upTankWidth/2 ){
				// up-most y coordinate of given tank is within y coordinate range of enemy3, up/right side of ship is touching enemy3
				//tanks are touching, need a new direction
				return needNewDirection3 = true;
			}	
			else if (checker.yPos + upTankWidth/2 >= enemy3->yPos/ENEMYSPEED - downTankWidth3/2 && checker.yPos + upTankWidth/2 <= enemy3->yPos/ENEMYSPEED + upTankWidth/2 ){
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
	if (tankCollision(player, enemy1, enemy2, enemy3, false, false, true, false)) {
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
	
	if (!tankCollision(player, enemy1, enemy2, enemy3, false, true, false,false)) {
		//checks for direction of enemy1
		if (enemy1->direction == left) {
			//checks for left edge of screen and barrier collison
			if(((enemy1->xPos/ENEMYSPEED - (upTankWidth/2))>0) && (!checkCollision(enemy1->xPos -1,enemy1->yPos))){
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
			if(((enemy1->xPos/ENEMYSPEED + (upTankWidth/2))<COLS) && (!checkCollision(enemy1->xPos+1,enemy1->yPos))){
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
			if(((enemy1->yPos/ENEMYSPEED - (upTankWidth/2))>0) && (!checkCollision(enemy1->xPos,enemy1->yPos-1))){
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
			if(((enemy1->yPos/ENEMYSPEED + (upTankWidth/2))<ROWS) && (!checkCollision(enemy1->xPos,enemy1->yPos+1))){
					//moves enemy1 down one pixel
				enemy1->yPos += 1;
				enemy1->moves--; // decrements moves remaining
			} else {
				newDirection(enemy1, true, false, false);
				enemy1->moves = getTankMovement(enemy1);
			}
		}
		
	} 
	
	//checks for direction of enemy2
	if (enemy2->direction == left) {
		//checks for left edge of screen and barrier collison
		if(((enemy2->xPos/ENEMYSPEED - (upTankWidth/2))>0) && (!checkCollision(enemy2->xPos -1,enemy2->yPos))){
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
		if(((enemy2->xPos/ENEMYSPEED + (upTankWidth/2))<COLS) && (!checkCollision(enemy2->xPos+1,enemy2->yPos))){
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
		if(((enemy2->yPos/ENEMYSPEED - (upTankWidth/2))>0) && (!checkCollision(enemy2->xPos,enemy2->yPos-1))){
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
		if(((enemy2->yPos/ENEMYSPEED + (upTankWidth/2))<ROWS) && (!checkCollision(enemy2->xPos,enemy2->yPos+1))){
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
		if(((enemy3->xPos/ENEMYSPEED - (upTankWidth/2))>0) && (!checkCollision(enemy3->xPos -1,enemy3->yPos))){
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
		if(((enemy3->xPos/ENEMYSPEED + (upTankWidth/2))<COLS) && (!checkCollision(enemy3->xPos+1,enemy3->yPos))){
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
		if(((enemy3->yPos/ENEMYSPEED - (upTankWidth/2))>0) && (!checkCollision(enemy3->xPos,enemy3->yPos-1))){
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
		if(((enemy3->yPos/ENEMYSPEED + (upTankWidth/2))<ROWS) && (!checkCollision(enemy3->xPos,enemy3->yPos+1))){
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
	barrier block2 ={150,40,40,40,false};
	barrier block3 ={100,140,40,40,false};
	barrier block4 = {0,120,20,70,false};
	barrier block5 = {220,120,20,70,false};
	barrier block6 = {50, 250, 140, 30, false};
	
	barriers[0] = block; 
	barriers[1] = block2;
	barriers[2] = block3;
	barriers[3] = block4;
	barriers[4] = block5;
	barriers[5] = block6;
	for(i = 6;i<numBarriers;i++){
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
		//clears tank image off the screen if shot
		lcd_draw_image(enemy1->xPos/ENEMYSPEED,upTankWidth,enemy1->yPos/ENEMYSPEED,upTankHeight,upTank,LCD_COLOR_BLACK,LCD_COLOR_BLACK);
		//changes enemy1's positon to the lower right corner
		enemy1->xPos = 220*ENEMYSPEED;
		enemy1->yPos = 300*ENEMYSPEED;
		enemy1->direction = up;
		//redraws the enemy1
		lcd_draw_image(enemy1->xPos/ENEMYSPEED,upTankWidth,enemy1->yPos/ENEMYSPEED,upTankHeight,upTank,LCD_COLOR_BLUE,LCD_COLOR_BLACK);
		enemy1dead = false;
	}
	if (enemy2dead){
		//clears the enemies image off the screen if shot
		lcd_draw_image(enemy2->xPos/ENEMYSPEED,downTankWidth,enemy2->yPos/ENEMYSPEED,downTankHeight,downTank,LCD_COLOR_BLACK,LCD_COLOR_BLACK);
		//changes enemy2's positon to the top right corner
		enemy2->xPos = 220*ENEMYSPEED;
		enemy2->yPos = 20*ENEMYSPEED;
		enemy2->direction = down;
		//redraws enemy2
		lcd_draw_image(enemy2->xPos/ENEMYSPEED,downTankWidth,enemy2->yPos/ENEMYSPEED,downTankHeight,downTank,LCD_COLOR_BLUE,LCD_COLOR_BLACK);
		enemy2dead = false;
	}
	if (enemy3dead){
		lcd_draw_image(enemy3->xPos/ENEMYSPEED,upTankWidth,enemy3->yPos/ENEMYSPEED,upTankHeight,upTank,LCD_COLOR_BLACK,LCD_COLOR_BLACK);
		//changes enemy3's positon to the lower left corner
		enemy3->xPos = 20*ENEMYSPEED;
		enemy3->yPos = 300*ENEMYSPEED;
		enemy3->direction = up;
		//redraws enemy3
		lcd_draw_image(enemy3->xPos/ENEMYSPEED,upTankWidth,enemy3->yPos/ENEMYSPEED,upTankHeight,upTank,LCD_COLOR_BLUE,LCD_COLOR_BLACK);
		enemy3dead = false;
	}
}

void checkEnemyhit (tank *enemy1, tank *enemy2, tank *enemy3, int bulletUsed) {
	int i, j, k, l;
	int checkXpos1;
	int checkYpos1;
	int checkXpos2;
	int checkYpos2;
	bool enemy1Hit;
	bool enemy2Hit;
	bool enemy3Hit;
	int checkXpos3;
	int checkYpos3;
	
	//gets the coordinates of each of the enemy tanks
	checkXpos1 = enemy1->xPos/SPEED;
	checkYpos1 = enemy1->yPos/SPEED;
	checkXpos2 = enemy2->xPos/SPEED;
	checkYpos2 = enemy2->yPos/SPEED;
	checkXpos3 = enemy3->xPos/SPEED;
	checkYpos3 = enemy3->yPos/SPEED;
	
	enemy1Hit = false;
	enemy2Hit = false;
	enemy3Hit = false;
	i = 1; 
	//loops to check if bullet hit enemy1
	while (i > 0 && !enemy1Hit) {
			j = upTankWidth;
			k = upTankHeight;
		//loop to check each x-position of the enemy1 tank against the position of the bullet
			while( j> 0 && !enemy1Hit) {
				if (checkXpos1 == bullet_array[bulletUsed].xPos) {
				//loop to check each y-position of the enemy1 tank against the position of the bullet
					while (k > 0 && !enemy1Hit) {
						if (checkYpos1 == bullet_array[bulletUsed].yPos) {
							enemy1Hit = true;
						} else {
							checkYpos1++;
							
						}
						k--;
					}
					//increments the x-position of enemy1 tank
				} else {
					checkXpos1++;
					}
				j--;
			}
			i--;
	}
	i = 1;
	//loops to check if bullet hit enemy2
	while (i > 0 && !enemy2Hit) {
			j = upTankWidth;
			k = upTankHeight;
		//loop to check each x-position of the enemy2 tank against the position of the bullet
			while( j> 0 && !enemy2Hit) {
				if (checkXpos2 == bullet_array[bulletUsed].xPos) {
				//loop to check each y-position of the enemy2 tank against the position of the bullet
					while (k > 0 && !enemy2Hit) {
						if (checkYpos2 == bullet_array[bulletUsed].yPos) {
							enemy2Hit = true;
						} else {
							checkYpos2++;
							
						}
						k--;
					}
					//increments the x-position of enemy2 tank
				} else {
					checkXpos2++;
					}
				j--;
			}
			i--;
	}
	i = 1;
	//loops to check if bullet hit enemy1
	while (i > 0 && !enemy3Hit) {
			j = upTankWidth;
			k = upTankHeight;
		//loop to check each x-position of the enemy2 tank against the position of the bullet
			while( j> 0 && !enemy3Hit) {
				if (checkXpos3 == bullet_array[bulletUsed].xPos) {
				//loop to check each y-position of the enemy2 tank against the position of the bullet
					while (k > 0 && !enemy3Hit) {
						if (checkYpos3 == bullet_array[bulletUsed].yPos) {
							enemy3Hit = true;
						} else {
							checkYpos3++;
							
						}
						k--;
					}
					//increments the x-position of enemy2 tank
				} else {
					checkXpos3++;
					}
				j--;
			}
			i--;
	}
	
	
	//so that enemy will be redrawn
	if (enemy1Hit) {
		enemy1dead = true;
	}
	//so that enemy will be redrawn
	if (enemy2Hit) {
		enemy2dead = true;
	}
	//so that enemy will be redrawn
	if (enemy3Hit) {
		enemy3dead = true;
	}
	
	
}


void updateBullets(tank *enemy1, tank *enemy2, tank *enemy3){
	int i;
	for(i = 0; i<3;i++){
		//if active, we need to move it in its direction, check if it hit the edge of screen, check if it hit enemy, and draw on lcd.
		if(bullet_array[i].active){
				//moves bullet in direction, and draws it in that direction.
				switch(bullet_array[i].direction){
					case up: 
							lcd_draw_line(bullet_array[i].xPos,bullet_array[i].yPos,bullet_array[i].xPos + 2,bullet_array[i].yPos + 6,LCD_COLOR_BLACK);
							bullet_array[i].yPos--;
							lcd_draw_line(bullet_array[i].xPos,bullet_array[i].yPos,bullet_array[i].xPos + 2,bullet_array[i].yPos + 6,LCD_COLOR_WHITE);
							break;
					case down:
							lcd_draw_line(bullet_array[i].xPos,bullet_array[i].yPos,bullet_array[i].xPos + 2,bullet_array[i].yPos + 6,LCD_COLOR_BLACK);
							bullet_array[i].yPos++;
							lcd_draw_line(bullet_array[i].xPos,bullet_array[i].yPos,bullet_array[i].xPos + 2,bullet_array[i].yPos + 6,LCD_COLOR_WHITE);
							break;
					case right:
							lcd_draw_line(bullet_array[i].xPos,bullet_array[i].yPos,bullet_array[i].xPos + 6,bullet_array[i].yPos + 2,LCD_COLOR_BLACK);
							bullet_array[i].xPos++;
							lcd_draw_line(bullet_array[i].xPos,bullet_array[i].yPos,bullet_array[i].xPos + 6,bullet_array[i].yPos + 2,LCD_COLOR_WHITE);
							break;
					case left:
							lcd_draw_line(bullet_array[i].xPos,bullet_array[i].yPos,bullet_array[i].xPos + 6,bullet_array[i].yPos + 2,LCD_COLOR_BLACK);
							bullet_array[i].xPos--;
							lcd_draw_line(bullet_array[i].xPos,bullet_array[i].yPos,bullet_array[i].xPos + 6,bullet_array[i].yPos + 2,LCD_COLOR_WHITE);
							break;
				}
				
				//check if hit edge of screen
				if(bullet_array[i].xPos < 1 || bullet_array[i].xPos > COLS-1)
					bullet_array[i].active = false;
				if(bullet_array[i].yPos < 1 || bullet_array[i].yPos > 320)
					bullet_array[i].active = false;
				
				//check if hit enemy
				if (bullet_array[0].active) {
					checkEnemyhit(enemy1, enemy2, enemy3, 0);
				}
				//check if hit enemy
				if (bullet_array[1].active) {
					checkEnemyhit(enemy1, enemy2, enemy3, 1);
				}
				//check if hit enemy
				if (bullet_array[2].active) {
					checkEnemyhit(enemy1, enemy2, enemy3, 2);
				}
				//will redraw the enemy if dead
				checkEnemydead(enemy1, enemy2, enemy3);
		}
	}
}



int main(void)
{
	int i;
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
	
	//player bullets initialize to not active.
	
	for(i = 0;i<3;i++)
		bullet_array[i].active = false;
	
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
					lcd_draw_image(enemy1.xPos/ENEMYSPEED,upTankWidth,enemy1.yPos/ENEMYSPEED,upTankHeight,upTank,LCD_COLOR_BLUE,LCD_COLOR_BLACK);
					break;
				case down:
					lcd_draw_image(enemy1.xPos/ENEMYSPEED,downTankWidth,enemy1.yPos/ENEMYSPEED,downTankHeight,downTank,LCD_COLOR_BLUE,LCD_COLOR_BLACK);
					break;
				case left:
					lcd_draw_image(enemy1.xPos/ENEMYSPEED,leftTankWidth,enemy1.yPos/ENEMYSPEED,leftTankHeight,leftTank,LCD_COLOR_BLUE,LCD_COLOR_BLACK);
					break;
				case right:
					lcd_draw_image(enemy1.xPos/ENEMYSPEED,rightTankWidth,enemy1.yPos/ENEMYSPEED,rightTankHeight,rightTank,LCD_COLOR_BLUE,LCD_COLOR_BLACK);
					break;
			}
				
		}
		if(alert_enemy2){
			switch(enemy2.direction){
				case up:
					lcd_draw_image(enemy2.xPos/ENEMYSPEED,upTankWidth,enemy2.yPos/ENEMYSPEED,upTankHeight,upTank,LCD_COLOR_GREEN,LCD_COLOR_BLACK);
					break;
				case down:
					lcd_draw_image(enemy2.xPos/ENEMYSPEED,downTankWidth,enemy2.yPos/ENEMYSPEED,downTankHeight,downTank,LCD_COLOR_GREEN,LCD_COLOR_BLACK);
					break;
				case left:
					lcd_draw_image(enemy2.xPos/ENEMYSPEED,leftTankWidth,enemy2.yPos/ENEMYSPEED,leftTankHeight,leftTank,LCD_COLOR_GREEN,LCD_COLOR_BLACK);
					break;
				case right:
					lcd_draw_image(enemy2.xPos/ENEMYSPEED,rightTankWidth,enemy2.yPos/ENEMYSPEED,rightTankHeight,rightTank,LCD_COLOR_GREEN,LCD_COLOR_BLACK);
					break;
			}
				
		}
		if(alert_enemy3){
			switch(enemy3.direction){
				case up:
					lcd_draw_image(enemy3.xPos/ENEMYSPEED,upTankWidth,enemy3.yPos/ENEMYSPEED,upTankHeight,upTank,LCD_COLOR_RED,LCD_COLOR_BLACK);
					break;
				case down:
					lcd_draw_image(enemy3.xPos/ENEMYSPEED,downTankWidth,enemy3.yPos/ENEMYSPEED,downTankHeight,downTank,LCD_COLOR_RED,LCD_COLOR_BLACK);
					break;
				case left:
					lcd_draw_image(enemy3.xPos/ENEMYSPEED,leftTankWidth,enemy3.yPos/ENEMYSPEED,leftTankHeight,leftTank,LCD_COLOR_RED,LCD_COLOR_BLACK);
					break;
				case right:
					lcd_draw_image(enemy3.xPos/ENEMYSPEED,rightTankWidth,enemy3.yPos/ENEMYSPEED,rightTankHeight,rightTank,LCD_COLOR_RED,LCD_COLOR_BLACK);
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
			TIMER3_ALERT = false;
		}
		//barriers
		drawBarriers();
		updateBullets(&enemy1, &enemy2, &enemy3);
		
		if(ft6x06_read_td_status()!=0){
			//find next inactive bullet to send
			for(i=0;i<3;i++){
				if(!bullet_array[i].active)
				{
					bullet_array[i].yPos = player.yPos;
					bullet_array[i].xPos = player.xPos;
					bullet_array[i].direction = player.direction;
					bullet_array[i].active = true;
					break;
				}
			}
		}
	}
	
}  




//*****************************************************************************
// Initializes all of the peripherls used in HW3
//*****************************************************************************
void init_hardware(void)
{
	lp_io_init();
  lcd_config_gpio();
  lcd_config_screen();
  lcd_clear_screen(LCD_COLOR_BLACK);
  ps2_initialize();
  init_serial_debug(true,true);
	ft6x06_init();
	EnableInterrupts();
  //
	config_timer1();
  gp_timer_config_32(TIMER2_BASE,TIMER_TAMR_TAMR_PERIOD, 1000000, false, true);
  gp_timer_config_32(TIMER3_BASE,TIMER_TAMR_TAMR_PERIOD, 500000, false, true);
  gp_timer_config_32(TIMER4_BASE,TIMER_TAMR_TAMR_PERIOD, 50000, false, true);
}


