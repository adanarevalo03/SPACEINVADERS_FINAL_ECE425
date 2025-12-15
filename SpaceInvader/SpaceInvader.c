#include "SpaceInvader.h"
#include "Nokia5110.h"
#include "GPTM0.h"
#include "ADC0.h"
#include "Sounds.h"
#include "GPTM1.h"
#include "MCP4725.h"
#include "Sprites.h"
#include "button.h"

enum GameScreen {
	MAIN_MENU,
	GAME,
	GAME_OVER
};

enum GameScreen currentGameScreen = MAIN_MENU;

static uint8_t readyToRender = 0;

void RenderScreen() {
	if (readyToRender) {
		Nokia5110_DisplayBuffer();
		readyToRender = 0;
	}
}

uint16_t *currentSound;
static uint8_t playSound = 0;
uint32_t sampleIndex = 0;
uint32_t numSamples = 0;

void OutputSound() {
	if (playSound && sampleIndex < numSamples)
		MCP4725_SetValue(currentSound[sampleIndex++]);
	else if(playSound && sampleIndex >= numSamples) {
		playSound = 0;
		sampleIndex = 0;
	}
}

void PlaySound(uint16_t *sound, uint32_t samples){
	currentSound = sound;
	numSamples = samples;
	sampleIndex = 0;
	playSound = 1;
	
	
	
}

void GameInit() {
	MCP4725_Init();
	ADC_Init();
	EduBase_Button_Init();
	
	// Initialize Timer 0
	Timer_0A_Interrupt_Init(RenderScreen);
	// Initialize Timer 1
	Timer_1A_Interrupt_Init(OutputSound);
	
	// Initialize Nokia 5110 display
	Nokia5110_Init();
  Nokia5110_ClearBuffer();
	Nokia5110_DisplayBuffer();
}

struct Entity{
	uint8_t alive;
	uint8_t x,y;
	uint8_t *sprite;
	uint8_t width, height;
	uint8_t spriteSize;
};

struct Entity player ={1, 0, 38, rocketship, 10, 10, 20};
struct Entity bullet = {0, 0, 0, shot, 1, 8, 1 };


struct Entity enemies[] = {
	{1, 0, 0, invader_2, 10, 10, 20},
	{1, 20, 0, invader_3, 10, 10, 20},
	{1, 40, 0, invader_2, 10, 10, 20},
	{1, 60, 0, invader_4, 10, 10, 20}
	
};


void GameLoop() {
	switch (currentGameScreen) {
		
	case MAIN_MENU:
		// draw the main menu
		// if button_pressed, currentGameScreen = GAME;
	
		Nokia5110_SetCursor(3, 2);
		Nokia5110_OutString("SPACE");
		Nokia5110_SetCursor(2, 3);
		Nokia5110_OutString("INVADERS");
		
		if(Get_EduBase_Button_Status()){
			currentGameScreen = GAME;
			
		}
		break;
	case GAME:
		
	//ADC_Sample()*84/4095
		
		if (readyToRender == 0) {
			
			player.x = ADC_Sample()*84/4095; // PLAYER MOVEMENT
			
			if(Get_EduBase_Button_Status() & 0x01){
				bullet.alive = 1;
				bullet.x = player.x+5;
				bullet.y = player.y;
				PlaySound(shoot, 4080);
			};
			
			
			for (int i =0; i <4; i++){
				if(enemies[i].alive && bullet.alive && bullet.x <= enemies[i].x+enemies[i].width && enemies[i].x <= bullet.x + bullet.width &&   bullet.y <= enemies[i].y +enemies[i].height && enemies[i].y <= bullet.y + bullet.height){
					enemies[i].alive = 0;
					bullet.alive = 0;
				
				}
			}
				
				
			bullet.y -= 2;
			
			static int enemyDirection = 1;
			
			for(int i = 0; i< 4; i++){
				if(enemies[i].alive){
					enemies[i].x += enemyDirection;
					if(enemies[i].x >= 74) enemyDirection = -1;
					if(enemies[i].x <= 0) enemyDirection = 1;
				}
			}
			
			int aliveEnemies = 0;
			for (int i = 0; i <4; i++){
				if(enemies[i].alive) aliveEnemies++;
			}
			if(aliveEnemies == 0){
				currentGameScreen = GAME_OVER;
				break;
			}
			
			Nokia5110_ClearBuffer();
			// Game logic (collisions, score updates, player movement, game over)
			// Draw entities to the screen buffer
			if(bullet.alive)
				Nokia5110_DrawImage(bullet.x, bullet.y, bullet.width, bullet.height, bullet.sprite, bullet.spriteSize);
			
			Nokia5110_DrawImage(player.x, player.y, player.width, player.height, player.sprite, player.spriteSize);
			
			for (int i =0; i <4; i++){
				if(enemies[i].alive)
					Nokia5110_DrawImage(enemies[i].x, enemies[i].y, enemies[i].width, enemies[i].height, enemies[i].sprite, enemies[i].spriteSize);
				//readyToRender = 1;
			}
			readyToRender = 1;
		}
		
		break;
	case GAME_OVER:
		// draw game over screen
		// ....currentGameScreen = MAIN_MENU;
		Nokia5110_ClearBuffer();
		Nokia5110_SetCursor(2, 1);
		Nokia5110_OutString("GAME OVER");
		Nokia5110_SetCursor(0,3);
		Nokia5110_OutString("Press Button");
		//Nokia5110_DisplayBuffer();
	
		
		if(Get_EduBase_Button_Status()){
			currentGameScreen = MAIN_MENU;
			
		}
		break;
		
//		Nokia5110_OutString("Score: ");
//		break;
	default:
			currentGameScreen = MAIN_MENU;
	break;
	}
}

/*
- Main menu
	- press button to start game
- Game screen:
	- Player, enemies, bullets
	- score
	- lives
	- round/level number
- End game screen
	- score
*/