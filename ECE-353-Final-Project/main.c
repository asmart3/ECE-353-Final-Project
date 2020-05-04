
#include "main.h"

//*****************************************************************************
// Global Vars
//*****************************************************************************
barrier barriers[6];
int numBarriers = 6;
int SPEED = 1; //lower number is faster
int ENEMYSPEED = 3;
//direction of the player's tank
volatile PS2_DIR_t TANK_DIRECTION = PS2_DIR_CENTER;

volatile bool enemy1dead = true;
volatile bool enemy2dead = true;
volatile bool enemy3dead = true;
volatile bool game_over = false; // indicates if game is over
volatile bool paused = false; // indicates if game is paused
volatile bool printPauseMsg = false; // indicates if game is paused
uint8_t score = 0; // indicates the score of the current game

// player/enemy tanks
tank player;
tank enemy1;
tank enemy2;
tank enemy3;

static const uint16_t   MOVE_AMOUNT[] = {100, 150, 100, 100, 125, 150, 175, 200};


bool alert_move = false;

//player/enemy bullets
bullet bullet_array[3];
bullet playerShot;
bullet enemy1Shot;
bullet enemy2Shot;
bullet enemy3Shot;
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

bool gameOver(bool game_over){
	uint8_t HighScore;
	
	if (!game_over) {
	return false;
	} else {
		DisableInterrupts();
		printf("\n\rFinal Score: ");
		printf("%d\n\r",score);
		put_string("Game Over.\n\r");
		EnableInterrupts();
		lcd_clear_screen(LCD_COLOR_BLACK);
		lp_io_clear_pin(BLUE_M);
		//store new high score
		eeprom_byte_read(EEPROM_I2C_BASE,EADDR,&HighScore);
		printf("High Score:");
		
		if(score > HighScore){
			eeprom_byte_write(EEPROM_I2C_BASE,EADDR,score);
			printf(" %d\n\r",score);
		}
		else
			printf(" %d\n\r",HighScore);
		return true;
	}
}


//Check Tank Collision with Barrier
bool checkCollision(int xPos, int yPos, bool ifEnemy){
	int i;
	int rightSide,bottomSide;
	if (ifEnemy) {
		xPos /=ENEMYSPEED;
		yPos /=ENEMYSPEED;
	} else {
		xPos /=SPEED;
		yPos /=SPEED;
	}
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
//resets the enemies postions so that they can be redrawn
void checkEnemydead(tank *enemy1, tank *enemy2, tank *enemy3){
	if (enemy1dead){
		// clears the now dead tank
		lcd_draw_image(enemy1->xPos/ENEMYSPEED,upTankWidth,enemy1->yPos/ENEMYSPEED,upTankHeight,upTank,LCD_COLOR_BLACK,LCD_COLOR_BLACK);
		//changes enemy1's positon to the lower right corner
		enemy1->xPos = 220*ENEMYSPEED;
		enemy1->yPos = 300*ENEMYSPEED;
		newDirection(enemy1, true, false, false);
		//redraws the enemy1
		lcd_draw_image(enemy1->xPos/ENEMYSPEED,upTankWidth,enemy1->yPos/ENEMYSPEED,upTankHeight,upTank,LCD_COLOR_BLUE,LCD_COLOR_BLACK);
		enemy1dead = false;
	}
	if (enemy2dead){
		// clears the now dead tank
		lcd_draw_image(enemy2->xPos/ENEMYSPEED,upTankWidth,enemy2->yPos/ENEMYSPEED,upTankHeight,upTank,LCD_COLOR_BLACK,LCD_COLOR_BLACK);
		//changes enemy2's positon to the top right corner
		enemy2->xPos = 220*ENEMYSPEED;
		enemy2->yPos = 20*ENEMYSPEED;
		newDirection(enemy2, false, true, false);
		//redraws enemy2
		lcd_draw_image(enemy2->xPos/ENEMYSPEED,downTankWidth,enemy2->yPos/ENEMYSPEED,downTankHeight,downTank,LCD_COLOR_MAGENTA,LCD_COLOR_BLACK);
		enemy2dead = false;
	}
	if (enemy3dead){
		// clears the now dead tank
		lcd_draw_image(enemy3->xPos/ENEMYSPEED,upTankWidth,enemy3->yPos/ENEMYSPEED,upTankHeight,upTank,LCD_COLOR_BLACK,LCD_COLOR_BLACK);
		//changes enemy3's positon to the lower left corner
		enemy3->xPos = 20*ENEMYSPEED;
		enemy3->yPos = 300*ENEMYSPEED;
		newDirection(enemy2, false, false, true);
		//redraws enemy3
		lcd_draw_image(enemy3->xPos/ENEMYSPEED,upTankWidth,enemy3->yPos/ENEMYSPEED,upTankHeight,upTank,LCD_COLOR_RED,LCD_COLOR_BLACK);
		enemy3dead = false;
	}
}

bool tankCollision(tank *player, tank *enemy1, tank *enemy2, tank *enemy3, bool checkPlayer, bool checkEnemy1, bool checkEnemy2, bool checkEnemy3){
	tank checker;
	bool needNewDirection1 = false;
	bool needNewDirection2 = false;
	bool needNewDirection3 = false;
	bool needNewDirection4 = false;
	int downTankWidth1 = downTankWidth;
	int downTankWidth2 = downTankWidth;
	int downTankWidth3 = downTankWidth;
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
				//player hit an enemy tank
				if (checkPlayer) {
					needNewDirection4 = true;
				}
			}
			else if (checker.yPos+ upTankWidth/2 >= enemy1->yPos/ENEMYSPEED - downTankWidth1/2 && checker.yPos + upTankWidth/2 <= enemy1->yPos/ENEMYSPEED + upTankWidth/2  ){
				//tanks are touching, need a new direction
				needNewDirection1 = true;
				//player hit an enemy tank
				if (checkPlayer) {
					needNewDirection4 = true;
				}
			}
		}
		else if (checker.xPos + upTankWidth/2  >= enemy1->xPos/ENEMYSPEED - downTankWidth1/2 && checker.xPos + upTankWidth/2 <= enemy1->xPos/ENEMYSPEED + upTankWidth/2 ){
			// right-most x coordinate of the given tank is within the x coordinate range of enemy1
			if(checker.yPos- downTankWidth1/2 >= enemy1->yPos/ENEMYSPEED - downTankWidth1/2 && checker.yPos- downTankWidth1/2 <= enemy1->yPos/ENEMYSPEED + upTankWidth/2 ){
				//tanks are touching, need a new direction
				needNewDirection1 = true;
				//player hit an enemy tank
				if (checkPlayer) {
					needNewDirection4 = true;
				}
			}
				// up-most y coordinate of given tank is within y coordinate range of enemy1, up/right side of ship is touching given tank
			else if (checker.yPos+ upTankWidth/2  >= enemy1->yPos/ENEMYSPEED - downTankWidth1/2 && checker.yPos + upTankWidth/2 <= enemy1->yPos/ENEMYSPEED + upTankWidth/2 ){
				// down-most y coordinate of player tank is within y coordinate range of enemy1, down/right side of ship is touching enemy1
				//tanks are touching, need a new direction
				needNewDirection1 = true;
				//player hit an enemy tank
				if (checkPlayer) {
					needNewDirection4 = true;
				}
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
				//player hit an enemy tank
				if (checkPlayer) {
					needNewDirection4 = true;
				}
			}
			else if (checker.yPos+ upTankWidth/2 >= enemy2->yPos/ENEMYSPEED - downTankWidth2/2 && checker.yPos+ upTankWidth/2 <= enemy2->yPos/ENEMYSPEED + upTankWidth/2 ){
				//tanks are touching, need a new direction
				needNewDirection2 = true;
				//player hit an enemy tank
				if (checkPlayer) {
					needNewDirection4 = true;
				}
			}
		}
		else if (checker.xPos + upTankWidth/2  >= enemy2->xPos/ENEMYSPEED - downTankWidth2/2 && checker.xPos + upTankWidth/2 <= enemy2->xPos/ENEMYSPEED + upTankWidth/2 ){
			// right-most x coordinate of the given tank is within the x coordinate range of enemy2
			if(checker.yPos - downTankWidth2/2 >= enemy2->yPos/ENEMYSPEED - downTankWidth2/2 && checker.yPos - downTankWidth2/2 <= enemy2->yPos/ENEMYSPEED + upTankWidth/2 ){
				// up-most y coordinate of given tank is within y coordinate range of enemy2, up/right side of ship is touching enemy2
				//tanks are touching, need a new direction
				needNewDirection2 = true;
				//player hit an enemy tank
				if (checkPlayer) {
					needNewDirection4 = true;
				}
			}
			else if (checker.yPos + upTankWidth/2 >= enemy2->yPos/ENEMYSPEED - downTankWidth2/2 && checker.yPos + upTankWidth/2 <= enemy2->yPos/ENEMYSPEED + upTankWidth/2 ){
				// down-most y coordinate of given tank is within y coordinate range of enemy1, down/right side of ship is touching enemy2
				//tanks are touching, need a new direction
				needNewDirection2 = true;
				//player hit an enemy tank
				if (checkPlayer) {
					needNewDirection4 = true;
				}
			}
		}
	}
	if (!checkEnemy3) {
		if (checker.xPos - downTankWidth3/2  >= enemy3->xPos/ENEMYSPEED - downTankWidth3/2 && checker.xPos- downTankWidth3/2 <= enemy3->xPos/ENEMYSPEED + upTankWidth/2 ){
			// left-most x coordinate of the given tank is within the x coordinate range of the enemy3
			if (checker.yPos - downTankWidth3/2 >= enemy3->yPos/ENEMYSPEED - downTankWidth3/2 && checker.yPos - downTankWidth3/2 <= enemy3->yPos/ENEMYSPEED + upTankWidth/2 ){
				//tanks are touching, need a new direction
				needNewDirection3 = true;
				//player hit an enemy tank
				if (checkPlayer) {
					needNewDirection4 = true;
				}
			}
			else if (checker.yPos + upTankWidth/2  >= enemy3->yPos/ENEMYSPEED - downTankWidth3/2 && checker.yPos + upTankWidth/2 <= enemy3->yPos/ENEMYSPEED + upTankWidth/2 ){
				//tanks are touching, need a new direction
				needNewDirection3 = true;
				//player hit an enemy tank
				if (checkPlayer) {
					needNewDirection4 = true;
				}
			}
		}
		else if (checker.xPos + upTankWidth/2  >= enemy3->xPos/ENEMYSPEED - downTankWidth3/2 && checker.xPos + upTankWidth/2 <= enemy3->xPos/ENEMYSPEED + upTankWidth/2 ){
			// right-most x coordinate of the given tank is within the x coordinate range of enemy3
			if(checker.yPos - downTankWidth3/2 >= enemy3->yPos/ENEMYSPEED - downTankWidth3/2 && checker.yPos- downTankWidth3/2 <= enemy3->yPos/ENEMYSPEED + upTankWidth/2 ){
				// up-most y coordinate of given tank is within y coordinate range of enemy3, up/right side of ship is touching enemy3
				//tanks are touching, need a new direction
				needNewDirection3 = true;
				//player hit an enemy tank
				if (checkPlayer) {
					needNewDirection4 = true;
				}
			}	
			else if (checker.yPos + upTankWidth/2 >= enemy3->yPos/ENEMYSPEED - downTankWidth3/2 && checker.yPos + upTankWidth/2 <= enemy3->yPos/ENEMYSPEED + upTankWidth/2 ){
				// down-most y coordinate of player tank is within y coordinate range of given tank, down/right side of ship is touching enemy3
				//tanks are touching, need a new direction
				needNewDirection3 = true;
				//player hit an enemy tank
				if (checkPlayer) {
					needNewDirection4 = true;
				}
			}
		}
	}

	
	
	//completely changes enemies direction to the opposite direction
	if (needNewDirection1 == true) {
		enemy1dead = true;
		checkEnemydead(enemy1, enemy2, enemy3);
	}
	if (needNewDirection2 == true) {
		enemy2dead = true;
		checkEnemydead(enemy1, enemy2, enemy3);
	}
	//switches enemy 3's direction to the opposite direction
	if (needNewDirection3 == true) {
		enemy3dead = true;
		checkEnemydead(enemy1, enemy2, enemy3);
	}
	if (needNewDirection1 | needNewDirection2 | needNewDirection3) {
		return true;
	}
	if (needNewDirection4) {
		DisableInterrupts();
		//put_string("Ran into enemy tank.\n\r");
		game_over = true;
		paused = true;
		//gameOver(true);
		EnableInterrupts();
		return true;
	}
	return false;
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
		enemy1dead = true;
		checkEnemydead(enemy1, enemy2, enemy3);
	}
	if (tankCollision(player, enemy1, enemy2, enemy3, false, false, true, false)) {
		enemy2dead = true;
		checkEnemydead(enemy1, enemy2, enemy3);
	}
	if (tankCollision(player, enemy1, enemy2, enemy3, false, false, false, true)){
		enemy3dead = true;
		checkEnemydead(enemy1, enemy2, enemy3);
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
			if(((enemy1->xPos/ENEMYSPEED - (upTankWidth/2))>0) && (!checkCollision(enemy1->xPos -1,enemy1->yPos, true))){
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
			if(((enemy1->xPos/ENEMYSPEED + (upTankWidth/2))<COLS) && (!checkCollision(enemy1->xPos+1,enemy1->yPos, true))){
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
			if(((enemy1->yPos/ENEMYSPEED - (upTankWidth/2))>0) && (!checkCollision(enemy1->xPos,enemy1->yPos-1, true))){
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
			if(((enemy1->yPos/ENEMYSPEED + (upTankWidth/2))<ROWS) && (!checkCollision(enemy1->xPos,enemy1->yPos+1, true))){
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
		if(((enemy2->xPos/ENEMYSPEED - (upTankWidth/2))>0) && (!checkCollision(enemy2->xPos -1,enemy2->yPos, true))){
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
		if(((enemy2->xPos/ENEMYSPEED + (upTankWidth/2))<COLS) && (!checkCollision(enemy2->xPos+1,enemy2->yPos, true))){
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
		if(((enemy2->yPos/ENEMYSPEED - (upTankWidth/2))>0) && (!checkCollision(enemy2->xPos,enemy2->yPos-1, true))){
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
		if(((enemy2->yPos/ENEMYSPEED + (upTankWidth/2))<ROWS) && (!checkCollision(enemy2->xPos,enemy2->yPos+1, true))){
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
		if(((enemy3->xPos/ENEMYSPEED - (upTankWidth/2))>0) && (!checkCollision(enemy3->xPos -1,enemy3->yPos, true))){
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
		if(((enemy3->xPos/ENEMYSPEED + (upTankWidth/2))<COLS) && (!checkCollision(enemy3->xPos+1,enemy3->yPos, true))){
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
		if(((enemy3->yPos/ENEMYSPEED - (upTankWidth/2))>0) && (!checkCollision(enemy3->xPos,enemy3->yPos-1, true))){
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
		if(((enemy3->yPos/ENEMYSPEED + (upTankWidth/2))<ROWS) && (!checkCollision(enemy3->xPos,enemy3->yPos+1, true))){
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
void playerMove(tank *player,bool *alert_move, volatile PS2_DIR_t TANK_DIRECTION, tank *enemy1, tank *enemy2, tank *enemy3){
	if (!tankCollision(player, enemy2, enemy2, enemy3, true, false, false, false)){
		if (TANK_DIRECTION == PS2_DIR_LEFT){
			//checks for left edge of screen
				if((player->xPos/SPEED - (upTankWidth/2))>0){
					//checks for collision of barrier
					if(!checkCollision(player->xPos -1,player->yPos, false)){
						player->direction = left;
						player->xPos -= 1;
						*alert_move = true;
					} 
				}
			}else if(TANK_DIRECTION == PS2_DIR_RIGHT){
				//checks for right edge of screen
				if((player->xPos/SPEED + (upTankWidth/2))<COLS){
					//checks for collision of barrier
					if(!checkCollision(player->xPos+1,player->yPos, false)){
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
					if(!checkCollision(player->xPos,player->yPos-1, false)){
						player->direction = up;
						player->yPos -= 1;
						*alert_move = true;
					}
				}
			}else if(TANK_DIRECTION == PS2_DIR_DOWN){
				//checks for bottom of screen
				if((player->yPos/SPEED + (upTankWidth/2))<ROWS){
					//checks for collision of barrier
					if(!checkCollision(player->xPos,player->yPos+1, false)){
						player->direction = down;
						player->yPos += 1;
						*alert_move = true;
					}
				}
			}
		} else if (tankCollision(player, enemy2, enemy2, enemy3, true, false, false, false)){
			game_over = true;
			paused = true;
			DisableInterrupts();
			//put_string("Ran into an enemy tank.\n\r");
			//gameOver(game_over);
			EnableInterrupts();
		}
	
}


void initBarriers(){
	int i;
	barrier block ={40,40,40,40,false};
	barrier block2 ={150,40,40,40,false};
	barrier block3 ={100,140,40,40,false};
	barrier block4 = {0,120,20,70,false};
	barrier block5 = {220,120,20,70,false};
	barrier block6 = {50, 290, 140, 30, false};
	
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
				if(bullet_array[i].xPos < 1 || bullet_array[i].xPos > COLS-1) {
					bullet_array[i].active = false;
				}
				if(bullet_array[i].yPos < 1 || bullet_array[i].yPos > 320) {
					bullet_array[i].active = false;
				}
				
				
				
		}
	}
}



// shoots bullets out of tanks
void shoot(tank *t, bullet *b, bool playerShoot) {
	// only shoots if bullet is inactive
	if(!b->active) {
		
		// sets up the bullet
		b->direction = t->direction;
		if (playerShoot) {
			b->xPos = t->xPos/SPEED;
			b->yPos = t->yPos/SPEED;
			} else { 
				b->xPos = t->xPos/ENEMYSPEED;
				b->yPos = t->yPos/ENEMYSPEED;
			}
		b->active = true;
		
		// bullet is drawn in a different spot depending on where the tank is facing
		switch(b->direction) {
			case up:
				b->yPos = b->yPos-20;
				b->xPos = b->xPos-2;
				break;
			case down:
				b->yPos = b->yPos+20;
				b->xPos = b->xPos-2;
				break;
			case left:
				b->xPos = b->xPos-21;
				break;
			case right:
				b->xPos = b->xPos+19;
				break;
		}
		// draws bullet
		lcd_draw_image(b->xPos, bulletVecWidth, b->yPos, bulletVecHeight, bulletVec, LCD_COLOR_RED, LCD_COLOR_BLACK);
	}
}

// disables a barrier
void disableBarrier(int i) {
	barriers[i].broken = true; // indicates that barrier is disabled
	// clears the barrier off the screen
	lcd_draw_box(barriers[i].xPos,barriers[i].width,barriers[i].yPos,barriers[i].height,LCD_COLOR_BLACK,LCD_COLOR_BLACK,2);
}

// allows bullets to have movement
void moveBullet(bullet *b) {
	int i;
	// clears old bullets so that they aren't left on the screen
	lcd_draw_image(b->xPos, bulletVecWidth, b->yPos, bulletVecHeight, bulletVec, LCD_COLOR_BLACK, LCD_COLOR_BLACK);
	// moves bullet depending on direction
	switch(b->direction) {
		case up:
			b->yPos = b->yPos - 2;
			break;
		case down:
			b->yPos = b->yPos + 2;
			break;
		case left:
			b->xPos = b->xPos - 2;
			break;
		case right:
			b->xPos = b->xPos + 2;
			break;
	}
	// redraws image
	lcd_draw_image(b->xPos, bulletVecWidth, b->yPos, bulletVecHeight, bulletVec, LCD_COLOR_RED, LCD_COLOR_BLACK);
	
	// checks if a bullet has hit the player, which will cause the game to end
	if(((b->xPos-2>=player.xPos-16)&&(b->xPos-2<=player.xPos+16)) || ((b->xPos+2<=player.xPos-16)&&(b->xPos+2>=player.xPos+16))) {
		if(((b->yPos-2>=player.yPos-16)&&(b->yPos-2<=player.yPos+16)) || ((b->yPos+2<=player.yPos-16)&&(b->yPos+2>=player.yPos+16))) {
			paused = true;
			DisableInterrupts();
			game_over = true;
			//gameOver(true);
			EnableInterrupts();
		}
	}
	// checks if a bullet has hit an enemy, which kills the enemy and calls checkEnemyDead to respawn him
	if(((b->xPos-2>=enemy1.xPos/ENEMYSPEED-16)&&(b->xPos-2<=enemy1.xPos/ENEMYSPEED+16)) || ((b->xPos+2<=enemy1.xPos/ENEMYSPEED-16)&&(b->xPos+2>=enemy1.xPos/ENEMYSPEED+16))) {
		if(((b->yPos-2>=enemy1.yPos/ENEMYSPEED-16)&&(b->yPos-2<=enemy1.yPos/ENEMYSPEED+16)) || ((b->yPos+2<=enemy1.yPos/ENEMYSPEED-16)&&(b->yPos+2>=enemy1.yPos/ENEMYSPEED+16))) {
			enemy1dead = true;
			checkEnemydead(&enemy1, &enemy2, &enemy3);
			b->active = false; // deactivates bullet
			// updates the score and prints it to serial debugger
			if(b == &playerShot) {
				score++;
				printf("%d\n\r",score);
			}
		}
	}
	// checks if a bullet has hit an enemy, which kills the enemy and calls checkEnemyDead to respawn him
	if(((b->xPos-2>=enemy2.xPos/ENEMYSPEED-16)&&(b->xPos-2<=enemy2.xPos/ENEMYSPEED+16)) || ((b->xPos+2<=enemy2.xPos/ENEMYSPEED-16)&&(b->xPos+2>=enemy2.xPos/ENEMYSPEED+16))) {
		if(((b->yPos-2>=enemy2.yPos/ENEMYSPEED-16)&&(b->yPos-2<=enemy2.yPos/ENEMYSPEED+16)) || ((b->yPos+2<=enemy2.yPos/ENEMYSPEED-16)&&(b->yPos+2>=enemy2.yPos/ENEMYSPEED+16))) {
			enemy2dead = true;
			checkEnemydead(&enemy1, &enemy2, &enemy3);
			b->active = false; // deactivates bullet
			// updates the score and prints it to serial debugger
			if(b == &playerShot) {
				score++;
				DisableInterrupts();
				printf("%d\n\r",score);
				EnableInterrupts();
			}
		}
	}
	// checks if a bullet has hit an enemy, which kills the enemy and calls checkEnemyDead to respawn him
	if(((b->xPos-2>=enemy3.xPos/ENEMYSPEED-16)&&(b->xPos-2<=enemy3.xPos/ENEMYSPEED+16)) || ((b->xPos+2<=enemy3.xPos/ENEMYSPEED-16)&&(b->xPos+2>=enemy3.xPos/ENEMYSPEED+16))) {
		if(((b->yPos-2>=enemy3.yPos/ENEMYSPEED-16)&&(b->yPos-2<=enemy3.yPos/ENEMYSPEED+16)) || ((b->yPos+2<=enemy3.yPos/ENEMYSPEED-16)&&(b->yPos+2>=enemy3.yPos/ENEMYSPEED+16))) {
			enemy3dead = true;
			checkEnemydead(&enemy1, &enemy2, &enemy3);
			b->active = false; // deactivates bullet
			// updates the score and prints it to serial debugger
			if(b == &playerShot) {
				score++;
				printf("%d\n\r",score);
			}
		}
	}
	
	//check if hit edge of screen, deactivates bullet if it has
	if(b->xPos < 0 || b->xPos > 240) {
		b->active = false;
	}
	if(b->yPos < 1 || b->yPos > 320) {
		b->active = false;
	}
	
	
	// checks if bullet has hit a barrier
	for(i = 0; i<6; i++){
		if(!barriers[i].broken){
			//check x position
			if(((b->xPos>=barriers[i].xPos)&&(b->xPos<=(barriers[i].xPos+barriers[i].width))) || ((b->xPos <= (barriers[i].xPos + barriers[i].width)) && (b->xPos>=barriers[i].xPos))){
				//check y position
				if(((b->yPos>=barriers[i].yPos)&&(b->yPos<=(barriers[i].yPos+barriers[i].height))) || ((b->yPos <= (barriers[i].yPos + barriers[i].height)) && (b->yPos>=barriers[i].yPos))){
					b->active = false;
					//if(b == &playerShot)
						disableBarrier(i);
				}
			}
		}
	}
	
	// if bullet was deactivated, it gets cleared off the screen
	if(!b->active)
		lcd_draw_image(b->xPos, bulletVecWidth, b->yPos, bulletVecHeight, bulletVec, LCD_COLOR_BLACK, LCD_COLOR_BLACK);
}

void LCD_print(){
		uint8_t HighScore;
		int digits;
		eeprom_byte_read(EEPROM_I2C_BASE,EADDR,&HighScore);
		digits = (int)HighScore;
		//High
		lcd_draw_char(10,6,10,11,fontBitMap+110,LCD_COLOR_WHITE,LCD_COLOR_BLACK,0x00);
		lcd_draw_char(18,1,10,11,fontBitMap+176,LCD_COLOR_WHITE,LCD_COLOR_BLACK,0x00);
		lcd_draw_char(20,5,10,11,fontBitMap+154,LCD_COLOR_WHITE,LCD_COLOR_BLACK,0x00);
		lcd_draw_char(27,5,10,11,fontBitMap+165,LCD_COLOR_WHITE,LCD_COLOR_BLACK,0x00);
		
		//Score
		lcd_draw_char(37,5,10,11,fontBitMap+121,LCD_COLOR_WHITE,LCD_COLOR_BLACK,0x00);
		lcd_draw_char(44,5,10,11,fontBitMap+132,LCD_COLOR_WHITE,LCD_COLOR_BLACK,0x00);
		lcd_draw_char(51,5,10,11,fontBitMap+187,LCD_COLOR_WHITE,LCD_COLOR_BLACK,0x00);
		lcd_draw_char(58,2,10,11,fontBitMap+198,LCD_COLOR_WHITE,LCD_COLOR_BLACK,0x00);
		lcd_draw_char(62,5,10,11,fontBitMap+143,LCD_COLOR_WHITE,LCD_COLOR_BLACK,0x00);
		
	//NUMBER
		lcd_draw_char(74,5,10,11,fontBitMap+(11*(digits/100)),LCD_COLOR_WHITE,LCD_COLOR_BLACK,0x00);
		lcd_draw_char(81,5,10,11,fontBitMap+(11*((digits%100)/10)),LCD_COLOR_WHITE,LCD_COLOR_BLACK,0x00);
		lcd_draw_char(88,5,10,11,fontBitMap+(11*((digits%100)%10)),LCD_COLOR_WHITE,LCD_COLOR_BLACK,0x00);

}


int main(void)
{
	int i;
	uint8_t touch_event;
	char input;
	bool alert_move = false;
	bool alert_enemy1 = false;
	bool alert_enemy2 = false;
	bool alert_enemy3 = false;
	bool up_trigger = false;
	bool left_trigger = false;
	bool right_trigger = false;
	bool down_trigger = false;
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

		//initialize eeprom value to 0
		//eeprom_byte_write(EEPROM_I2C_BASE,EADDR,0x00);
		
		LCD_print();
		
		io_expander_write_reg(MCP23017_GPIOA_R,0xFF);
		//for push button
		while(!io_expander_trigger) {
			lcd_draw_image(player.xPos/SPEED,upTankWidth,player.yPos/SPEED,upTankHeight,upTank,LCD_COLOR_GREEN,LCD_COLOR_BLACK);
		} 
		if (io_expander_trigger && (!left_trigger || !right_trigger || !up_trigger || !down_trigger)) {
			//gives 8 bit unsigned number-> for buttons need to check first four bits->active low
			int value_read = io_expander_read_reg(MCP23017_GPIOB_R);
			up_trigger = !(value_read & (1 << DIR_BTN_UP_PIN));
			left_trigger = !(value_read & (1 << DIR_BTN_LEFT_PIN));
			right_trigger = !(value_read & (1 << DIR_BTN_RIGHT_PIN));
			down_trigger = !(value_read & (1 << DIR_BTN_DOWN_PIN));
		}
		io_expander_trigger = false;
		
		//remove High score from LCD
		lcd_draw_rectangle(10,83,10,11,LCD_COLOR_BLACK);
		
		put_string("Running...\n\r");

	  //to draw the enemies
		checkEnemydead(&enemy1, &enemy2, &enemy3);
		lcd_draw_image(player.xPos/SPEED,upTankWidth,player.yPos/SPEED,upTankHeight,upTank,LCD_COLOR_GREEN,LCD_COLOR_BLACK);
		
		// draws the barriers
		initBarriers();
		drawBarriers();
	
	while(!gameOver(game_over)){
		if(paused) {
			input = fgetc_nb(stdin);
			if(input == ' ') {
				paused = false;
				input = NULL;
				put_string("\n\rRunning...\n\r");
			}
		}
		
		while(!paused) {
			input = fgetc_nb(stdin); 
			if(input == ' ') {
				paused = true;
				input = NULL;
				put_string("\n\rPaused.");
				break;
			}
			io_expander_write_reg(MCP23017_GPIOA_R,score);
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
						lcd_draw_image(enemy2.xPos/ENEMYSPEED,upTankWidth,enemy2.yPos/ENEMYSPEED,upTankHeight,upTank,LCD_COLOR_MAGENTA,LCD_COLOR_BLACK);
						break;
					case down:
						lcd_draw_image(enemy2.xPos/ENEMYSPEED,downTankWidth,enemy2.yPos/ENEMYSPEED,downTankHeight,downTank,LCD_COLOR_MAGENTA,LCD_COLOR_BLACK);
						break;
					case left:
						lcd_draw_image(enemy2.xPos/ENEMYSPEED,leftTankWidth,enemy2.yPos/ENEMYSPEED,leftTankHeight,leftTank,LCD_COLOR_MAGENTA,LCD_COLOR_BLACK);
						break;
					case right:
						lcd_draw_image(enemy2.xPos/ENEMYSPEED,rightTankWidth,enemy2.yPos/ENEMYSPEED,rightTankHeight,rightTank,LCD_COLOR_MAGENTA,LCD_COLOR_BLACK);
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
				enemy1Move(&player, &enemy1, &enemy2, &enemy3, &alert_enemy1, &alert_enemy2, &alert_enemy3);
				playerMove(&player, &alert_move, PS2_DIR, &enemy1, &enemy2, &enemy3);
				TIMER2_ALERT = false;
			}
			
			
			if(TIMER3_ALERT){
				game_over = true;
				paused = true;
				//gameOver(game_over);
				TIMER3_ALERT = false;
			}
			
			
			//barriers
			//drawBarriers();
			//updateBullets(&enemy1, &enemy2, &enemy3);
			
			
			// moves all of the bullets
			if(playerShot.active)
				moveBullet(&playerShot);
			if(enemy1Shot.active)
				moveBullet(&enemy1Shot);
			if(enemy2Shot.active)
				moveBullet(&enemy2Shot);
			if(enemy3Shot.active)
				moveBullet(&enemy3Shot);
			
			
			
			// forces enemy tanks to 'reload' before they can shoot again
			if(!enemy1Shot.active && enemy1Shot.waitTime != 300) {
				enemy1Shot.waitTime++;
			}
			else {
				shoot(&enemy1, &enemy1Shot, false);
				enemy1Shot.waitTime = 0;
			}
			if(!enemy2Shot.active && enemy2Shot.waitTime != 300) {
				enemy2Shot.waitTime++;
			}
			else {
				shoot(&enemy2, &enemy2Shot, false);
				enemy2Shot.waitTime = 0;
			}
			if(!enemy3Shot.active && enemy3Shot.waitTime != 300) {
				enemy3Shot.waitTime++;
			}
			else {
				shoot(&enemy3, &enemy3Shot, false);
				enemy3Shot.waitTime = 0;
			}
			
			
			// players tank shoots when player touches LCD screen
			touch_event = ft6x06_read_td_status();
			if(touch_event > 0 && touch_event < 3){
				//find next inactive bullet to send
				//for(i=0;i<3;i++){
				//	if(!bullet_array[i].active)
				//	{
				//		bullet_array[i].yPos = player.yPos;
				//		bullet_array[i].xPos = player.xPos;
				//		bullet_array[i].direction = player.direction;
				//		bullet_array[i].active = true;
				//		break;
				//	}
					
				//}
				shoot(&player, &playerShot, true);
			}
		}
		
	}
	lp_io_clear_pin(BLUE_M);

	
}  

void uart0_config_gpio(void)
{
  // ADD CODE
	gpio_enable_port(GPIOA_BASE);
	gpio_config_digital_enable(GPIOA_BASE, PA1);
	gpio_config_digital_enable(GPIOA_BASE, PA0);
	gpio_config_alternate_function(GPIOA_BASE, PA1);
	gpio_config_alternate_function(GPIOA_BASE, PA0);
	gpio_config_port_control(GPIOA_BASE, GPIO_PCTL_PA1_M, GPIO_PCTL_PA1_U0TX);
	gpio_config_port_control(GPIOA_BASE, GPIO_PCTL_PA0_M, GPIO_PCTL_PA0_U0RX);
}


//*****************************************************************************
// Initializes all of the peripherls used in HW3
//*****************************************************************************
void init_hardware(void)
{
	//for 30 seconds
	int setGameEnd = 50000000*30;
	DisableInterrupts();
	lp_io_init();
  lcd_config_gpio();
  lcd_config_screen();
  lcd_clear_screen(LCD_COLOR_BLACK);
  ps2_initialize();
  init_serial_debug(true,true);
	ft6x06_init();
	uart0_config_gpio();
	uart_init(UART0_BASE, true, true);
	io_expander_init();
	eeprom_init();
	EnableInterrupts();
  //
	config_timer1();
  gp_timer_config_32(TIMER2_BASE,TIMER_TAMR_TAMR_PERIOD, 1000000, false, true);
  gp_timer_config_32(TIMER3_BASE,TIMER_TAMR_TAMR_PERIOD, setGameEnd, false, true);
  gp_timer_config_32(TIMER4_BASE,TIMER_TAMR_TAMR_PERIOD, 50000, false, true);
}
