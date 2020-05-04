
#include "main.h"

//*****************************************************************************
// Global Vars
//*****************************************************************************

//bariers that will be drawn one the screen
barrier barriers[6];
int numBarriers = 6;
//speed that the enemy tank and player's tank is controlled by
//a lower speed will result in a faster tank
int SPEED = 1; 
int ENEMYSPEED = 3;
//direction of the player's tank
volatile PS2_DIR_t TANK_DIRECTION = PS2_DIR_CENTER;
//variables to check if each enemy is dead and if they should be redrawn
volatile bool enemy1dead = true;
volatile bool enemy2dead = true;
volatile bool enemy3dead = true;
// indicates if game is over
volatile bool game_over = false; 
// indicates if game is paused
volatile bool paused = false; 
// indicates if game is paused
volatile bool printPauseMsg = false; 
// indicates the score of the current game
uint8_t score = 0; 

// player/enemy tanks
tank player;
tank enemy1;
tank enemy2;
tank enemy3;
//array that holds integers that correlate to the amount an enemy should move
static const uint16_t   MOVE_AMOUNT[] = {100, 150, 100, 100, 125, 150, 175, 200};

//true if enemy needs to move/be redrawn
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
/*******************************************************************************
* Function Name: gameOver
********************************************************************************
* Summary
* If the game has ended, this function will clear the lcd screen, 
* 	print to putty that the game has ended & print out the player's score/high
* 	score. It will also store a new high score to eeprom
*
* Parameters
* game_over: true if game has ended either because an enemy has hit
* 	the player's tank, or if a bullet has hit the player's tank
*
* Return
*  True: if game has ended
*	 False: if game has not ended
*******************************************************************************/ 
bool gameOver(bool game_over){
	uint8_t HighScore;
	
	if (!game_over) {
	return false;
	} else {
		DisableInterrupts();
		//prints to putty that the game has ended and the score(s)
		printf("\n\rFinal Score: ");
		printf("%d\n\r",score);
		put_string("Game Over.\n\r");
		EnableInterrupts();
		//clears the screen and pin for the led
		lcd_clear_screen(LCD_COLOR_RED);
		lp_io_clear_pin(BLUE_M);
		//stores high score
		eeprom_byte_read(EEPROM_I2C_BASE,EADDR,&HighScore);
		printf("High Score:");
		//checks if the player's score was greater than the new high score
		if(score > HighScore){
			eeprom_byte_write(EEPROM_I2C_BASE,EADDR,score);
			printf(" %d\n\r",score);
		}
		else
			printf(" %d\n\r",HighScore);
		return true;
	}
}
/*******************************************************************************
* Function Name: checkCollision
********************************************************************************
* Summary
* Fuction to tell if a tank (enemy or player) has hit one of the barriers
*
* Parameters
* xPos: the x-position of the tank
* yPos: the y-position of the tank
* ifEnemy: true if function is being used to detect an enemie's collision with
* a barrier
*
* Return
*  True: if a collision between the barrier and the tank has occured
*	 False: if no collision betwenn the barrier and the tank has occurred
*******************************************************************************/

//Check Tank Collision with Barrier
bool checkCollision(int xPos, int yPos, bool ifEnemy){
	int i;
	int rightSide,bottomSide;
	//sets up the x and y positions depending on if the tank is an enemy
	if (ifEnemy) {
		xPos /=ENEMYSPEED;
		yPos /=ENEMYSPEED;
	} else {
		xPos /=SPEED;
		yPos /=SPEED;
	}
	//gets the middle position of the tank
	xPos -=16;
	yPos -=16;
	//gets the bottom and right side of the tank
	rightSide = xPos + upTankWidth;//+16;
	bottomSide = yPos + upTankHeight;//+16;
	//loop to see if the tank hits any of the barriers
	for(i = 0; i<numBarriers; i++){
		if(!barriers[i].broken){
			//checks x position
			if(((rightSide>=barriers[i].xPos)&&(rightSide<=(barriers[i].xPos+barriers[i].width))) || ((xPos <= (barriers[i].xPos + barriers[i].width)) && (xPos>=barriers[i].xPos))){
				//checks y position
				if(((yPos>=barriers[i].yPos)&&(yPos<=(barriers[i].yPos+barriers[i].height))) || ((bottomSide <= (barriers[i].yPos + barriers[i].height)) && (bottomSide>=barriers[i].yPos))){
					//barrier was hit by the tank
					return true;
				}
			}
		}
	}
	//barrier was not hit by the tank
	return false;
}

/*******************************************************************************
* Function Name: newDirection
********************************************************************************
* Summary
* 	Gives the enemy tank a new random direction to move in
*
* Parameters
* enemy1: struct that contains information about enemy's direction
* directionEnemy1: true if enemy1 needs new direction
* directionEnemy2: true if enemy2 needs new direction
* directionEnemy3: true if enemy3 needs new direction
*
* Return
*  Nothings
*******************************************************************************/
//generates a random new direction depending on which tank needs a new direction
void newDirection(tank *enemy1, bool directionEnemy1, bool directionEnemy2, bool directionEnemy3){
	//creates a new tank so that direction previous can be checked against direction new
	tank checkDirection1;
	int randomnumber;
	int randomnumber2;
	int randomnumber3;
  
	//sets up the checker's direction
	checkDirection1.direction = enemy1->direction;
	//while loop to make sure new random direction is not the same as it was previous
	while (checkDirection1.direction  ==  enemy1->direction){
		//random number direction where: 0 = left, 1 = right, 2 = up, 3 = right
		randomnumber = rand() % 4;
		randomnumber2 = rand() % 4;
		randomnumber3 = rand() % 4;
		//switches the enemie's direction to a new direction
		if (directionEnemy1){
			switch(randomnumber){
					case 0:
						//left direction
						enemy1->direction = left;
						break;
					case 1:
						//right direction
						enemy1->direction = right;	
						break;
					case 2:
						//up direction
						enemy1->direction = up;	
						break;
					case 3:
						//down direction
						enemy1->direction = down;
						break;
				}		
		}
		//changes enemy2's direction
		if (directionEnemy2){
			switch(randomnumber2){
					case 0:
						//left direction
						enemy1->direction = left;
						break;
					case 1:
						//right direction
						enemy1->direction = right;	
						break;
					case 2:
						//up direction
						enemy1->direction = up;	
						break;
					case 3:
						//down direction
						enemy1->direction = down;
						break;
				}		
		}
		//changes enemy3's direction
		if (directionEnemy3){
			switch(randomnumber3){
					case 0:
						//left direction
						enemy1->direction = left;
						break;
					case 1:
						//right direction
						enemy1->direction = right;	
						break;
					case 2:
						//up direction
						enemy1->direction = up;	
						break;
					case 3:
						//down direction
						enemy1->direction = down;
						break;
				}		
		}
	}
}

/*******************************************************************************
* Function Name: checkEnemydead
********************************************************************************
* Summary
* 	Checks if any of the enemies have died. If enemy has died, function will
* clear the screen where they died, and will redraw the enemy in their spawn
*
* Parameters
* enemy1: struct that contains information about enemy1's x and y position
* enemy2: struct that contains information about enemy2's x and y position
* enemy3: struct that contains information about enemy3's x and y position
* 
*
* Return
*  Nothings
*******************************************************************************/
void checkEnemydead(tank *enemy1, tank *enemy2, tank *enemy3){
	//checks that enemy1 has died
	if (enemy1dead){
		// clears the dead tank
		lcd_draw_image(enemy1->xPos/ENEMYSPEED,upTankWidth,enemy1->yPos/ENEMYSPEED,upTankHeight,upTank,LCD_COLOR_BLACK,LCD_COLOR_BLACK);
		//changes enemy1's positon to the lower right corner
		enemy1->xPos = 220*ENEMYSPEED;
		enemy1->yPos = 300*ENEMYSPEED;
		//gets a new random direction for enemy1
		newDirection(enemy1, true, false, false);
		//redraws the enemy1 in the spawn
		lcd_draw_image(enemy1->xPos/ENEMYSPEED,upTankWidth,enemy1->yPos/ENEMYSPEED,upTankHeight,upTank,LCD_COLOR_BLUE,LCD_COLOR_BLACK);
		//enemy1 is now alive
		enemy1dead = false;
	}
	//checks that enemy2 has died
	if (enemy2dead){
		// clears the now dead tank
		lcd_draw_image(enemy2->xPos/ENEMYSPEED,upTankWidth,enemy2->yPos/ENEMYSPEED,upTankHeight,upTank,LCD_COLOR_BLACK,LCD_COLOR_BLACK);
		//changes enemy2's positon to the top right corner
		enemy2->xPos = 220*ENEMYSPEED;
		enemy2->yPos = 20*ENEMYSPEED;
		newDirection(enemy2, false, true, false);
		//redraws enemy2 in it's spawn
		lcd_draw_image(enemy2->xPos/ENEMYSPEED,downTankWidth,enemy2->yPos/ENEMYSPEED,downTankHeight,downTank,LCD_COLOR_MAGENTA,LCD_COLOR_BLACK);
		//enemy2 is now alive
		enemy2dead = false;
	}
	//checks that enemy3 is dead
	if (enemy3dead){
		// clears the now dead tank
		lcd_draw_image(enemy3->xPos/ENEMYSPEED,upTankWidth,enemy3->yPos/ENEMYSPEED,upTankHeight,upTank,LCD_COLOR_BLACK,LCD_COLOR_BLACK);
		//changes enemy3's positon to the lower left corner
		enemy3->xPos = 20*ENEMYSPEED;
		enemy3->yPos = 300*ENEMYSPEED;
		newDirection(enemy2, false, false, true);
		//redraws enemy3 in it's spawn
		lcd_draw_image(enemy3->xPos/ENEMYSPEED,upTankWidth,enemy3->yPos/ENEMYSPEED,upTankHeight,upTank,LCD_COLOR_RED,LCD_COLOR_BLACK);
		//enemy3 is now alive
		enemy3dead = false;
	}
}
/*******************************************************************************
* Function Name: tankCollision
********************************************************************************
* Summary
* Checks if any of the tanks hit eachother. If two enemy tanks collide, they
* 	will respawn in their respective spawns. If the player hits any of the
*		enemy tanks, the player dies and it is game over.
* 
*
* Parameters
* player: struct that contains information about player's x and y position
* enemy1: struct that contains information about enemy's x and y position
* enemy2: struct that contains information about enemy's x and y position
* enemy3: struct that contains information about enemy's x and y position
* checkPlayer: true if checking the player hit any of the enemy's tanks
* checkEnemy1: true if checking the enemy1 hit any of the other tanks
* checkEnemy2: true if checking the enemy2 hit any of the other tanks
* checkEnemy3: true if checking the enemy3 hit any of the other tanks
*
* Return
*  True: if given tank hit another tank
*	 False: if given tank did not hit any other tank
*******************************************************************************/
bool tankCollision(tank *player, tank *enemy1, tank *enemy2, tank *enemy3, bool checkPlayer, bool checkEnemy1, bool checkEnemy2, bool checkEnemy3){
	//sets up a checker tank
	tank checker;
	//bools change when different enemies collide
	bool needNewDirection1 = false; //given tank hit enemy1
	bool needNewDirection2 = false;	//given tank hit enemy2
	bool needNewDirection3 = false;	//given tank hit enemy3
	bool needNewDirection4 = false;	//given tank hit player
	int downTankWidth1 = downTankWidth;
	int downTankWidth2 = downTankWidth;
	int downTankWidth3 = downTankWidth;
	//sets tank checker's cordinates to appropriate tank
	if(checkPlayer){
		//checking the player's tank against other enemy tanks
		checker.xPos = player->xPos/ENEMYSPEED;
		checker.yPos = player->yPos/ENEMYSPEED;
	} else if(checkEnemy1) {
		//cheching enemy1's tank against other tanks
		checker.xPos = enemy1->xPos/ENEMYSPEED;
		checker.yPos = enemy1->yPos/ENEMYSPEED;
	} else if(checkEnemy2) {
		//cheching enemy2's tank against other tanks
		checker.xPos = enemy2->xPos/ENEMYSPEED;
		checker.yPos = enemy2->yPos/ENEMYSPEED;
	} else if(checkEnemy3) {
		//cheching enemy3's tank against other tanks
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
	
	//checks to see if given tank touches enemy2 at any point
	if (!checkEnemy2) {
		if (checker.xPos - downTankWidth2/2 >= enemy2->xPos/ENEMYSPEED - downTankWidth2/2 && checker.xPos - downTankWidth2/2 <= enemy2->xPos/ENEMYSPEED + upTankWidth/2 ){
			// left-most x coordinate of the tank is within the x coordinate range of the enemy2
			if (checker.yPos- downTankWidth2/2 >= enemy2->yPos/ENEMYSPEED - downTankWidth2/2 && checker.yPos - downTankWidth2/2 <= enemy2->yPos/ENEMYSPEED + upTankWidth/2 ){
				//tanks are touching, need a new direction
				needNewDirection2 = true;
				//player hit an enemy tank and will need to be dead
				if (checkPlayer) {
					needNewDirection4 = true;
				}
			}
			else if (checker.yPos+ upTankWidth/2 >= enemy2->yPos/ENEMYSPEED - downTankWidth2/2 && checker.yPos+ upTankWidth/2 <= enemy2->yPos/ENEMYSPEED + upTankWidth/2 ){
				//tanks are touching, need a new direction
				needNewDirection2 = true;
				//player hit an enemy tank and will need to be dead
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
				//player hit an enemy tank and will need to be dead
				if (checkPlayer) {
					needNewDirection4 = true;
				}
			}
			else if (checker.yPos + upTankWidth/2 >= enemy2->yPos/ENEMYSPEED - downTankWidth2/2 && checker.yPos + upTankWidth/2 <= enemy2->yPos/ENEMYSPEED + upTankWidth/2 ){
				// down-most y coordinate of given tank is within y coordinate range of enemy1, down/right side of ship is touching enemy2
				//tanks are touching, need a new direction
				needNewDirection2 = true;
				//player hit an enemy tank and will need to be dead
				if (checkPlayer) {
					needNewDirection4 = true;
				}
			}
		}
	}
	if (!checkEnemy3) {
		//checks given tank agaisnt enemy3 tank
		if (checker.xPos - downTankWidth3/2  >= enemy3->xPos/ENEMYSPEED - downTankWidth3/2 && checker.xPos- downTankWidth3/2 <= enemy3->xPos/ENEMYSPEED + upTankWidth/2 ){
			// left-most x coordinate of the given tank is within the x coordinate range of the enemy3
			if (checker.yPos - downTankWidth3/2 >= enemy3->yPos/ENEMYSPEED - downTankWidth3/2 && checker.yPos - downTankWidth3/2 <= enemy3->yPos/ENEMYSPEED + upTankWidth/2 ){
				//tanks are touching, need a new direction
				needNewDirection3 = true;
				//player hit an enemy tank and will need to be dead
				if (checkPlayer) {
					needNewDirection4 = true;
				}
			}
			else if (checker.yPos + upTankWidth/2  >= enemy3->yPos/ENEMYSPEED - downTankWidth3/2 && checker.yPos + upTankWidth/2 <= enemy3->yPos/ENEMYSPEED + upTankWidth/2 ){
				//tanks are touching, need a new direction
				needNewDirection3 = true;
				//player hit an enemy tank and will need to be dead
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
				//player hit an enemy tank and will need to be dead
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

	
	
	//resets enemy1 to the spawn if it was touching another tank
	if (needNewDirection1 == true) {
		enemy1dead = true;
		checkEnemydead(enemy1, enemy2, enemy3);
	}
	//resets enemy2 to the spawn if it was touching another tank
	if (needNewDirection2 == true) {
		enemy2dead = true;
		checkEnemydead(enemy1, enemy2, enemy3);
	}
	//resets enemy1 to the spawn if it was touching another tank
	if (needNewDirection3 == true) {
		enemy3dead = true;
		checkEnemydead(enemy1, enemy2, enemy3);
	}
	//returns true if given tank was touching another tank
	if (needNewDirection1 | needNewDirection2 | needNewDirection3) {
		return true;
	}
	//checks if player was touching an enemy tank
	if (needNewDirection4) {
		DisableInterrupts();
		//ends the game
		game_over = true;
		paused = true;
		EnableInterrupts();
		return true;
	}
	//tanks were not touching
	return false;
}


/*******************************************************************************
* Function Name: getTankMovement
********************************************************************************
* Summary
* 	Gets enemy tanks random movement
* 
* Parameters
*  enemy: enemies tank
*
* Return
*  New move count of enemy tank
*
*******************************************************************************/
// helps give the enemy tanks random movement
uint16_t getTankMovement(tank *enemy) {
	// tells enemy tank to move a certain amount of pixels
	return MOVE_AMOUNT[rand() % 8]; 
} 
/*******************************************************************************
* Function Name: enemy1Move
********************************************************************************
* Summary 
*	Moves enemy in the required direction and checks for collisions
* 
*
* Parameters
* player: struct that contains information about player's x position,y position
*			and the direction the player is heading in
* enemy1: struct that contains information about enemy's x and y position,
* 	direction, and number of moves
* enemy2: struct that contains information about enemy's x and y position
* 	direction, and number of moves
* enemy3: struct that contains information about enemy's x and y position
* 	direction, and number of moves
* *alert_enemy1: true if enemy1 needs to be redrawn
* *alert_enemy2: true if enemy2 needs to be redrawn
* *alert_enemy3: true if enemy3 needs to be redrawn
*
* Return
*  Nothing
*	 
*******************************************************************************/

void enemy1Move(tank *player, tank *enemy1, tank *enemy2, tank *enemy3, bool *alert_enemy1, bool *alert_enemy2, bool *alert_enemy3){
	tank directionCheck;
	//checks for a collision between enemy1 and any other tank
	if (tankCollision(player, enemy1, enemy2, enemy3, false, true, false, false)){
		enemy1dead = true;
		//enemy1 is dead and needs to respawn
		checkEnemydead(enemy1, enemy2, enemy3);
	}
	//checks for a collision between enemy2 and any other tank
	if (tankCollision(player, enemy1, enemy2, enemy3, false, false, true, false)) {
		enemy2dead = true;
		//enemy2 is dead and needs to respawn
		checkEnemydead(enemy1, enemy2, enemy3);
	}
	//checks for a collision between enemy3 and any other tank
	if (tankCollision(player, enemy1, enemy2, enemy3, false, false, false, true)){
		enemy3dead = true;
		//enemy3 is dead and needs to respawn
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
	//checks for collison of enemy1 and other tanks
	if (!tankCollision(player, enemy1, enemy2, enemy3, false, true, false,false)) {
		//checks for direction of enemy1
		if (enemy1->direction == left) {
			//checks for left edge of screen and barrier collison
			if(((enemy1->xPos/ENEMYSPEED - (upTankWidth/2))>0) && (!checkCollision(enemy1->xPos -1,enemy1->yPos, true))){
				//moves enemy1 left one pixel
				enemy1->xPos -= 1;
				enemy1->moves--; // decrements moves remaining
			} else {
				//gets new direction and moves for enemy1
				newDirection(enemy1, true, false, false);
				enemy1->moves = getTankMovement(enemy1);
			}
		}
		//checks for direction of enemy1
		if (enemy1->direction == right) {
			//checks for right edge of screen and barrier collison
			if(((enemy1->xPos/ENEMYSPEED + (upTankWidth/2))<COLS) && (!checkCollision(enemy1->xPos+1,enemy1->yPos, true))){
				//moves enemy1 right one pixel
				enemy1->xPos += 1;
				// decrements moves remaining
				enemy1->moves--; // decrements moves remaining
			} else {
				//gets new direction and moves for enemy1
				newDirection(enemy1, true, false, false);
				enemy1->moves = getTankMovement(enemy1);
			}
		}
		//checks for direction of enemy1
		if (enemy1->direction == up) {
			//checks for left edge of screen and barrier collison
			if(((enemy1->yPos/ENEMYSPEED - (upTankWidth/2))>0) && (!checkCollision(enemy1->xPos,enemy1->yPos-1, true))){
				//moves enemy1 up one pixel
				enemy1->yPos -= 1;
				// decrements moves remaining
				enemy1->moves--; 
			} else {
				//gets new direction and moves for enemy1
				newDirection(enemy1, true, false, false);
				enemy1->moves = getTankMovement(enemy1);
			}
		}
		//checks for direction of enemy1
		if (enemy1->direction == down) {
			//checks for left edge of screen and barrier collison
			if(((enemy1->yPos/ENEMYSPEED + (upTankWidth/2))<ROWS) && (!checkCollision(enemy1->xPos,enemy1->yPos+1, true))){
				//moves enemy1 down one pixel
				enemy1->yPos += 1;
				// decrements moves remaining
				enemy1->moves--; 
			} else {
				//gets new direction and moves for enemy1
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
			// decrements moves remaining
			enemy2->moves--; 
		} else {
			//gets new direction and moves for enemy2
			newDirection(enemy2, false, true, false);
			enemy2->moves = getTankMovement(enemy2);
		}
	}
	//checks for direction of enemy2
	if (enemy2->direction == right) {
		//checks for right edge of screen and barrier collison
		if(((enemy2->xPos/ENEMYSPEED + (upTankWidth/2))<COLS) && (!checkCollision(enemy2->xPos+1,enemy2->yPos, true))){
			//moves enemy1 right one pixel
			enemy2->xPos += 1;
			// decrements moves remaining
			enemy2->moves--; 
		} else {
			//gets new direction and moves for enemy2
			newDirection(enemy2, true, false, false);
			enemy2->moves = getTankMovement(enemy2);
		}
	}
	//checks for direction of enemy2
	if (enemy2->direction == up) {
		//checks for left edge of screen and barrier collison
		if(((enemy2->yPos/ENEMYSPEED - (upTankWidth/2))>0) && (!checkCollision(enemy2->xPos,enemy2->yPos-1, true))){
			//moves enemy1 up one pixel
			enemy2->yPos -= 1;
			// decrements moves remaining
			enemy2->moves--;
		} else {
			//gets new direction and moves for enemy2
			newDirection(enemy2, false, true, false);
			enemy2->moves = getTankMovement(enemy2);
		}
	}
	//checks for direction of enemy2
	if (enemy2->direction == down) {
		//checks for left edge of screen and barrier collison
		if(((enemy2->yPos/ENEMYSPEED + (upTankWidth/2))<ROWS) && (!checkCollision(enemy2->xPos,enemy2->yPos+1, true))){
				//moves enemy1 down one pixel
			enemy2->yPos += 1;
			// decrements moves remaining
			enemy2->moves--; 
		} else {
			//gets new direction and moves for enemy2
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
			// decrements moves remaining
			enemy3->moves--; 
		} else {
			//gets new direction and moves for enemy3
			newDirection(enemy3, false, false, true);
			enemy3->moves = getTankMovement(enemy3);
		}
	}
	//checks for direction of enemy3
	if (enemy3->direction == right) {
		//checks for right edge of screen and barrier collison
		if(((enemy3->xPos/ENEMYSPEED + (upTankWidth/2))<COLS) && (!checkCollision(enemy3->xPos+1,enemy3->yPos, true))){
			//moves enemy1 right one pixel
			enemy3->xPos += 1;
			// decrements moves remaining
			enemy3->moves--;
		} else {
			//gets new direction and moves for enemy3
			newDirection(enemy3, false, false, true);
			enemy3->moves = getTankMovement(enemy3);
		}
	}
	//checks for direction of enemy3
	if (enemy3->direction == up) {
		//checks for left edge of screen and barrier collison
		if(((enemy3->yPos/ENEMYSPEED - (upTankWidth/2))>0) && (!checkCollision(enemy3->xPos,enemy3->yPos-1, true))){
				//moves enemy1 up one pixel
			enemy3->yPos -= 1;
			// decrements moves remaining
			enemy3->moves--; 
		} else {
			//gets new direction and moves for enemy3
			newDirection(enemy3, false, false, true);
			enemy3->moves = getTankMovement(enemy3);
		}
	}
	//checks for direction of enemy3
	if (enemy3->direction == down) {
		//checks for left edge of screen and barrier collison
		if(((enemy3->yPos/ENEMYSPEED + (upTankWidth/2))<ROWS) && (!checkCollision(enemy3->xPos,enemy3->yPos+1, true))){
				//moves enemy1 down one pixel
			enemy3->yPos += 1;
			// decrements moves remaining
			enemy3->moves--; 
		} else {
			//gets new direction and moves for enemy3
			newDirection(enemy3, false, false, true);
			enemy3->moves = getTankMovement(enemy3);
		}
	}
	//alerts the program to redraw the enemy tanks
	*alert_enemy1 = true;
	*alert_enemy2 = true;
	*alert_enemy3 = true;
}
/*******************************************************************************
* Function Name: playerMove
********************************************************************************
* Summary 
*	Moves the player in the required direction and checks for collisions
* 
*
* Parameters
* player: struct that contains information about player's x position,y position
*			and the direction the player is heading in
* enemy1: struct that contains information about enemy's x and y position,
* 	direction, and number of moves
* enemy2: struct that contains information about enemy's x and y position
* 	direction, and number of moves
* enemy3: struct that contains information about enemy's x and y position
* 	direction, and number of moves
* *alert_move: true if player needs to be redrawn
* TANK_DIRECTION: the direction the player's tank needs to move in
*
* Return
*  Nothing
*	 
*******************************************************************************/
// PlayerMovement
void playerMove(tank *player,bool *alert_move, volatile PS2_DIR_t TANK_DIRECTION, tank *enemy1, tank *enemy2, tank *enemy3){
	//checks for a tank collision between the player's tank and the enemie's tanks
	if (!tankCollision(player, enemy2, enemy2, enemy3, true, false, false, false)){
		//no collision so checks the tanks direction
		if (TANK_DIRECTION == PS2_DIR_LEFT){
			//checks for left edge of screen
				if((player->xPos/SPEED - (upTankWidth/2))>0){
					//checks for collision of barrier
					if(!checkCollision(player->xPos -1,player->yPos, false)){
						//moves player one pixel left
						player->direction = left;
						player->xPos -= 1;
						//alerts program to redraw player
						*alert_move = true;
					} 
				}
			}else if(TANK_DIRECTION == PS2_DIR_RIGHT){
				//checks for right edge of screen
				if((player->xPos/SPEED + (upTankWidth/2))<COLS){
					//checks for collision of barrier
					if(!checkCollision(player->xPos+1,player->yPos, false)){
						//moves player one pixel right
						player->direction = right;
						player->xPos += 1;
						//alerts program to redraw player
						*alert_move = true;
					}
				}
			}
			
			if (TANK_DIRECTION == PS2_DIR_UP){
				//checks for top edge of screen
				if((player->yPos/SPEED - (upTankWidth/2))>0){
					//checks for collision of barrier
					if(!checkCollision(player->xPos,player->yPos-1, false)){
						//moves player one pixel up
						player->direction = up;
						player->yPos -= 1;
						//alerts program to redraw player
						*alert_move = true;
					}
				}
			}else if(TANK_DIRECTION == PS2_DIR_DOWN){
				//checks for bottom of screen
				if((player->yPos/SPEED + (upTankWidth/2))<ROWS){
					//checks for collision of barrier
					if(!checkCollision(player->xPos,player->yPos+1, false)){
						//moves player one pixel down
						player->direction = down;
						player->yPos += 1;
						//alerts program to redraw player
						*alert_move = true;
					}
				}
			}
		} else if (tankCollision(player, enemy2, enemy2, enemy3, true, false, false, false)){
			//player tank hit enemy tank so game over
			game_over = true;
			paused = true;
			DisableInterrupts();
			EnableInterrupts();
		}
	
}

/*******************************************************************************
* Function Name: initBarriers()
********************************************************************************
* Summary 
*	Initilizes the barriers
* 
*
* Parameters
* 	None
*
* Return
*  Nothing
*	 
*******************************************************************************/
void initBarriers(){
	int i;
	//sets barriers x-position, y-position, length, width, and whether the barrier
	//is broken
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
/*******************************************************************************
* Function Name: drawBarriers()
********************************************************************************
* Summary 
*	Draws the barriers onto the screen
*
* Parameters
* 	None
*
* Return
*  Nothing
*	 
*******************************************************************************/
void drawBarriers(){
	int i;
	//loops over each barrier
	for(i=0;i<numBarriers;i++){
		if(!barriers[i].broken)
			//draws each barrier
			lcd_draw_box(barriers[i].xPos,barriers[i].width,barriers[i].yPos,barriers[i].height,LCD_COLOR_RED,LCD_COLOR_BLACK,2);
	}	
}
/*******************************************************************************
* Function Name: updateBullets
********************************************************************************
* Summary 
*		Draws the bullet in the needed direction and checks if bullet hits edge of
* screen
* 
*
* Parameters
* 	enemy1: struct that contains x and y position of enemy1
* 	enemy2: struct that contains x and y position of enemy2
* 	enemy3: struct that contains x and y position of enemy3
*
* Return
*  Nothing
*	 
*******************************************************************************/
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

/*******************************************************************************
* Function Name: shoot
********************************************************************************
* Summary 
*		Shoots the bullet out of the tank
* 
*
* Parameters
* *t: struct that contains information about tank's x position,y position
*			and the direction the tank is heading in
*
*	playerShoot: true if the player's tank is the one shooting
* *b: struct that contains the direction of the bullet and the x and y position
*
* Return
*  Nothing
*	 
*******************************************************************************/

void shoot(tank *t, bullet *b, bool playerShoot) {
	// only shoots if bullet is inactive
	if(!b->active) {
		
		// sets up the bullet direction
		b->direction = t->direction;
		if (playerShoot) {
			// sets up the bullet position if the player is the one shooting
			b->xPos = t->xPos/SPEED;
			b->yPos = t->yPos/SPEED;
			} else { 
				// sets up the bullet position if the enemy is the one shooting
				b->xPos = t->xPos/ENEMYSPEED;
				b->yPos = t->yPos/ENEMYSPEED;
			}
		//bullet is now active
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
/*******************************************************************************
* Function Name: disableBarrier
********************************************************************************
* Summary 
*		Disables a barrier since it was shot
* 
*
* Parameters
* i : indicator of which barrier needs to be disabled
*
*	playerShoot: true if the player's tank is the one shooting
* *b: struct that contains the direction of the bullet and the x and y position
*
* Return
*  Nothing
*	 
*******************************************************************************/

void disableBarrier(int i) {
	// indicates that barrier is disabled
	barriers[i].broken = true; 
	// clears the barrier off the screen
	lcd_draw_box(barriers[i].xPos,barriers[i].width,barriers[i].yPos,barriers[i].height,LCD_COLOR_BLACK,LCD_COLOR_BLACK,2);
}
/*******************************************************************************
* Function Name: updateBullets
********************************************************************************
* Summary 
*		Allows the bullets to have movement and checks if bullet hits a tank
* 
*
* Parameters
* b: struct that contains the direction of the bullet and the x and y position
*
* Return
*  Nothing
*	 
*******************************************************************************/
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
			//player was hit
			paused = true;
			DisableInterrupts();
			game_over = true;
			EnableInterrupts();
		}
	}
	// checks if a bullet has hit an enemy, which kills the enemy and calls checkEnemyDead to respawn him
	if(((b->xPos-2>=enemy1.xPos/ENEMYSPEED-16)&&(b->xPos-2<=enemy1.xPos/ENEMYSPEED+16)) || ((b->xPos+2<=enemy1.xPos/ENEMYSPEED-16)&&(b->xPos+2>=enemy1.xPos/ENEMYSPEED+16))) {
		if(((b->yPos-2>=enemy1.yPos/ENEMYSPEED-16)&&(b->yPos-2<=enemy1.yPos/ENEMYSPEED+16)) || ((b->yPos+2<=enemy1.yPos/ENEMYSPEED-16)&&(b->yPos+2>=enemy1.yPos/ENEMYSPEED+16))) {
			//enemy1 was hit and is now dead
			enemy1dead = true;
			checkEnemydead(&enemy1, &enemy2, &enemy3);
			// deactivates bullet
			b->active = false; 
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
			//enemy2 was hit and is now dead
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
			//enemy3 was hit and is now dead
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
/*******************************************************************************
* Function Name: LCD_print
********************************************************************************
* Summary 
*		Prints the high score to the LCD
* 
*
* Parameters
*  None
*
* Return
*  Nothing
*	 
*******************************************************************************/
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
	//alerts the tanks to be redrawn if true
	bool alert_move = false;
	bool alert_enemy1 = false;
	bool alert_enemy2 = false;
	bool alert_enemy3 = false;
	//alerts the program that a button has been pressed when true
	bool up_trigger = false;
	bool left_trigger = false;
	bool right_trigger = false;
	bool down_trigger = false;
	//sets the player's x and y positions
	player.xPos = 100 * SPEED;
	player.yPos = 100 * SPEED;
	//sets the player's direction
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
		
		//prints to the lcd
		LCD_print();
		
		io_expander_write_reg(MCP23017_GPIOA_R,0xFF);
		//for push button
		while(!io_expander_trigger) {
			//no button was pushed, so continues to draw same image on screen
			lcd_draw_image(player.xPos/SPEED,upTankWidth,player.yPos/SPEED,upTankHeight,upTank,LCD_COLOR_GREEN,LCD_COLOR_BLACK);
		}
		//button was pushed and interrupt was generated
		if (io_expander_trigger && (!left_trigger || !right_trigger || !up_trigger || !down_trigger)) {
			//gives 8 bit unsigned number-> for buttons need to check first four bits->active low
			int value_read = io_expander_read_reg(MCP23017_GPIOB_R);
			//depending one which button was pushed, sets the button pushed to true
			up_trigger = !(value_read & (1 << DIR_BTN_UP_PIN));
			left_trigger = !(value_read & (1 << DIR_BTN_LEFT_PIN));
			right_trigger = !(value_read & (1 << DIR_BTN_RIGHT_PIN));
			down_trigger = !(value_read & (1 << DIR_BTN_DOWN_PIN));
		}
		//trigger is no longer needed
		io_expander_trigger = false;
		
		//remove High score from LCD
		lcd_draw_rectangle(10,83,10,11,LCD_COLOR_BLACK);
		
		//prints running to putty
		put_string("Running...\n\r");

	  //to draw the enemies
		checkEnemydead(&enemy1, &enemy2, &enemy3);
		lcd_draw_image(player.xPos/SPEED,upTankWidth,player.yPos/SPEED,upTankHeight,upTank,LCD_COLOR_GREEN,LCD_COLOR_BLACK);
		
		// draws the barriers
		initBarriers();
		drawBarriers();
	//checkes if the game is over
	while(!gameOver(game_over)){
		//checks if space button was pushed
		if(paused) {
			input = fgetc_nb(stdin);
			if(input == ' ') {
				paused = false;
				input = NULL;
				//space was not pushed, so game is not paused
				put_string("\n\rRunning...\n\r");
			}
		}
		//checks if space button was pushed
		while(!paused) {
			input = fgetc_nb(stdin); 
			if(input == ' ') {
				paused = true;
				input = NULL;
				//space was pushed, so pauses game
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
			//draws the enemy1 tank according to direction
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
		
			//draws the enemy2 tank according to direction
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
			
			//draws the enemy3 tank according to direction
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
			
			//uses interupts to move the player and player
			if(TIMER2_ALERT){
				enemy1Move(&player, &enemy1, &enemy2, &enemy3, &alert_enemy1, &alert_enemy2, &alert_enemy3);
				playerMove(&player, &alert_move, PS2_DIR, &enemy1, &enemy2, &enemy3);
				TIMER2_ALERT = false;
			}
			
			//game over after 30 seconds of game time
			if(TIMER3_ALERT){
				game_over = true;
				paused = true;
				//gameOver(game_over);
				TIMER3_ALERT = false;
			}
			
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
