#include <switch.h>
#include <iostream>
#include <random>
#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <math.h>

#include "main.hpp"

using namespace std;

const int SPLASH = 0;
const int MAINMENU = 1;
const int GAME_SINGLE_PLAYER = 2;
const int GAME_MULTI_PLAYER = 3;
const int DETACH_JOYCONS = 4;
const int ATTACH_JOYCONS = 5;


int state = SPLASH;

Mix_Chunk* pong = NULL;
Mix_Chunk* score = NULL;

typedef struct {
	float x;
	float y;
	float xVelocity;
	float yVelocity;
	float speed;
} ball;

typedef struct {
	SDL_Rect pos;
	int score;
	int speed;
} player;

float calculateAngle(float y1, float y2, int height) {
	
	float rely = y1 + height/2 - y2;
	rely /= height/2.0;
	return rely * MAX_ANGLE;
	
}

void renderTexture(SDL_Texture *tex, SDL_Renderer *ren, int x, int y, int w=-1, int h=-1) {
	
	SDL_Rect dest;
	dest.x = x;
	dest.y = y;
	dest.w = w;
	dest.h = h;
	
	if (w == -1 || h == -1) {
		SDL_QueryTexture(tex, NULL, NULL, &dest.w, &dest.h);
	}

	SDL_RenderCopy(ren, tex, NULL, &dest);

}

void PlaySound(Mix_Chunk* sound) {
	if (Mix_PlayChannel( -1, sound, 0) == -1) {
		return;
	}
}

void restartGame(player &player_01, player &player_02, ball &b) {
	
	b.x = SCREEN_WIDTH / 2;
	b.y = SCREEN_HEIGHT / 2;
	b.speed = BALL_INIT_SPEED;
	b.xVelocity = (rand() % 2 == 0)? BALL_INIT_SPEED : -1 * BALL_INIT_SPEED;
	b.yVelocity = -0.5f;
	
	player_01.score = 0;
	player_02.score = 0;
	player_01.pos.w = 25;
	player_02.pos.w = 25;
	player_01.pos.h = 150;
	player_02.pos.h = 150;
	player_01.speed = 10;
	
	if (state == GAME_MULTI_PLAYER) {
		player_02.speed = 20; //WTF WHY?
	} else {
		player_02.speed = 3.5;
	}
	
	player_01.pos.x = 0;
	player_02.pos.x = SCREEN_WIDTH - player_02.pos.w;
	player_01.pos.y = SCREEN_HEIGHT / 2 - player_01.pos.h / 2;
	player_02.pos.y = SCREEN_HEIGHT / 2 - player_02.pos.h / 2;
	
}

int main(int argc, char **argv) //main loop
{
	
	SDL_Init(SDL_INIT_EVERYTHING); //init sdl
	IMG_Init(IMG_INIT_JPG); //init image lib
	
	Mix_OpenAudio(220550, MIX_DEFAULT_FORMAT, 2, 4096); //init audio mixer
	
	SDL_Rect b_rect; //ball rect object
	
	b_rect.w = BALL_WIDTH; //ball width
	b_rect.h = BALL_HEIGHT; // ball height
	
	player player_01; //create player 1 object
	player player_02; //create player 2 object
	
	ball b; //create ball object
	b.x = SCREEN_WIDTH / 2; //ball x position
	b.y = SCREEN_HEIGHT / 2; //ball y position
	b.speed = BALL_INIT_SPEED; //ball default speed
	b.xVelocity = (rand() % 2 == 0)? BALL_INIT_SPEED : -1 * BALL_INIT_SPEED; //ball x velocity
	b.yVelocity = -0.5f; //ball y velocity
	
	player_01.score = player_02.score = 0; //player 1 score
	player_01.pos.w = player_02.pos.w = 25; //player 1 width
	player_01.pos.h = player_02.pos.h = 150; // player 1 height
	player_01.speed = 10; // player 1 speed
	
	player_01.pos.x = 0; //player 1 x 
	player_02.pos.x = SCREEN_WIDTH - player_02.pos.w; //player 2 x
	player_01.pos.y = SCREEN_HEIGHT / 2 - player_01.pos.h / 2; //player 1 y
	player_02.pos.y = SCREEN_HEIGHT / 2 - player_02.pos.h / 2; //player 2 y
	
	uint prevTime = SDL_GetTicks(); // Hm
	bool quit = false; // quit variable. if true quits and exits application
	int frames = 0; // frames
	float fps; //fps
	int splashTimer = 0; //splash screen timer
	int showBarsAt = 50;
	int mainMenuIndex = 0; //main menu image index
	
    //Setup window
    SDL_Window* window = SDL_CreateWindow(nullptr, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 0, 0, SDL_WINDOW_FULLSCREEN_DESKTOP);
	
	//Setup renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);

	//Make an image-surface and create texture
	//SDL_Surface * image = IMG_Load("assets/image/image.jpg");
	
	//Load Images
	SDL_Surface* paddleImage = IMG_Load("assets/image/paddle.png");
	SDL_Surface* ballImage = IMG_Load("assets/image/ball.png");
	
	SDL_Surface* font16X16 = IMG_Load("assets/font/font_16x16.png");
	
	
	SDL_Surface* splashImage = IMG_Load("assets/image/splash.png");
	SDL_Texture* splashTexture = SDL_CreateTextureFromSurface(renderer, splashImage);
	
	SDL_Surface* barsImage = IMG_Load("assets/image/bars.png");
	SDL_Texture* barsTexture = SDL_CreateTextureFromSurface(renderer, barsImage);
	
	SDL_Surface* mainMenuImage_01 = IMG_Load("assets/image/main_menu_one_player.jpg");
	SDL_Surface* mainMenuImage_02 = IMG_Load("assets/image/main_menu_two_player.jpg");
	SDL_Surface* mainMenuImage_03 = IMG_Load("assets/image/main_menu_quit.jpg");
	
	SDL_Texture* mainMenuTexture_01 = SDL_CreateTextureFromSurface(renderer, mainMenuImage_01);
	SDL_Texture* mainMenuTexture_02 = SDL_CreateTextureFromSurface(renderer, mainMenuImage_02);
	SDL_Texture* mainMenuTexture_03 = SDL_CreateTextureFromSurface(renderer, mainMenuImage_03);
	
	SDL_Surface* detachImage = IMG_Load("assets/image/detach_joycons.png");
	SDL_Texture* detachTexture = SDL_CreateTextureFromSurface(renderer, detachImage);
	
	//convert Images to Textures
	SDL_Texture* paddleTexture = SDL_CreateTextureFromSurface(renderer, paddleImage);
	SDL_Texture* ballTexture = SDL_CreateTextureFromSurface(renderer, ballImage);
	
	SDL_Texture* mainMenuTextures[3] = {mainMenuTexture_01, mainMenuTexture_02, mainMenuTexture_03};
	
	pong = Mix_LoadWAV("assets/audio/pong.wav");
	score = Mix_LoadWAV("assets/audio/score.wav");
	
	//music = Mix_LoadWAV("assets/audio/sasquacoon.wav");
	
	while(appletMainLoop() && !quit) {
		
		++frames;
		uint currTime = SDL_GetTicks();
		float elapsed = (currTime - prevTime);
		
		if (elapsed > 100) {
			
			fps = round(frames / (elapsed / 1000.0));
			frames = 0;
			prevTime = currTime;
			
		}
		
		//Scan all the inputs. This should be done once for each frame
		hidScanInput();
		
		if (state == SPLASH) {
			splashTimer ++;
			
			if (splashTimer >= 75) {
				state = MAINMENU;
			}
			
			SDL_RenderClear(renderer);
			renderTexture(splashTexture, renderer, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
			if (splashTimer > showBarsAt) {
				renderTexture(barsTexture, renderer, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
			}
			SDL_RenderPresent(renderer);

		}
		
		if (state == MAINMENU) {
			
			if (hidGetHandheldMode() == 1) { //if joycons connected to switch
			
				u64 kDown = hidKeysDown(CONTROLLER_P1_AUTO);
				u64 kHeld = hidKeysHeld(CONTROLLER_P1_AUTO);
			
				if (kDown & KEY_LSTICK_UP || kDown & KEY_DUP) {
					if (mainMenuIndex > 0) {
						mainMenuIndex -= 1;
						PlaySound(pong);
					}
				}
				if (kDown & KEY_LSTICK_DOWN || kDown & KEY_DDOWN) {
					if (mainMenuIndex < 2) {
						mainMenuIndex += 1;
						PlaySound(pong);
					}
				}
				if (kDown & KEY_A) {
					if (mainMenuIndex == 0) {
						state = GAME_SINGLE_PLAYER;
					}
					if (mainMenuIndex == 1) {
						if (hidGetHandheldMode() == 1) {
							state = DETACH_JOYCONS;
						} else if (hidGetHandheldMode() == 0) {
							state = GAME_MULTI_PLAYER;
						}
					}
					if (mainMenuIndex == 2) {
						quit = true;
						break;
					}
				}
			} else if (hidGetHandheldMode() == 0) { //if joycons detached from switch
				
				u64 player_01_kDown = hidKeysDown(CONTROLLER_P1_AUTO);
				u64 player_01_kHeld = hidKeysHeld(CONTROLLER_P1_AUTO);
				
				if (player_01_kDown & KEY_LSTICK_RIGHT) {
					if (mainMenuIndex > 0) {
						mainMenuIndex -= 1;
						PlaySound(pong);
					}
				}
				if (player_01_kDown & KEY_LSTICK_LEFT) {
					if (mainMenuIndex < 2) {
						mainMenuIndex += 1;
						PlaySound(pong);
					}
				}
				if (player_01_kDown & KEY_DDOWN) {
					if (mainMenuIndex == 0) {
						state = GAME_SINGLE_PLAYER;
					}
					if (mainMenuIndex == 1) {
						state = GAME_MULTI_PLAYER;
					}
					if (mainMenuIndex == 2) {
						quit = true;
						break;
					}
				}
			}
			
			SDL_RenderClear(renderer); //Clear the screen
			renderTexture(mainMenuTextures[mainMenuIndex], renderer, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT); //Draw the menu 
			SDL_RenderPresent(renderer); //Display the rendered image
			
		}
		
		if (state == DETACH_JOYCONS) {
			
			if (hidGetHandheldMode() == 0) {
				state = GAME_MULTI_PLAYER;
			}
			
			SDL_RenderClear(renderer);
			renderTexture(detachTexture, renderer, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
			SDL_RenderPresent(renderer);
			
		}
		
		if (state == GAME_SINGLE_PLAYER) { //If Single player mode
		
			player_02.speed = 3.5; //set ai speed
			
			if (hidGetHandheldMode() == 1) { //if joycons connected to switch
			
				u64 kDown = hidKeysDown(CONTROLLER_P1_AUTO); //Variable for player one buttons pressed
				u64 kHeld = hidKeysHeld(CONTROLLER_P1_AUTO); //variable for player one buttons held 
			
				if (kDown & KEY_PLUS){ //If plus pressed
					restartGame(player_01, player_02, b); //reset the game variables
					state = MAINMENU; //return to main menu 
				}
		
				if (kHeld & KEY_DUP || kHeld & KEY_LSTICK_UP) { //if dpad up or left analog up held
					player_01.pos.y -= player_01.speed; //move player up
				}
		
				if (kHeld & KEY_DDOWN || kHeld & KEY_LSTICK_DOWN) { //if dpad down or left analog down held
					player_01.pos.y += player_01.speed; //move player down
				}
			} else if (hidGetHandheldMode() == 0) { //if joycon detached from switch
				
				u64 player_01_kDown = hidKeysDown(CONTROLLER_P1_AUTO); //Variable for player one buttons pressed
				u64 player_01_kHeld = hidKeysHeld(CONTROLLER_P1_AUTO); //variable for player one buttons held 	
				
				if (player_01_kDown & KEY_MINUS) {
					restartGame(player_01, player_02, b);
					state = MAINMENU;
				}
				if (player_01_kHeld & KEY_DUP || player_01_kHeld & KEY_LSTICK_RIGHT) {
					player_01.pos.y -= player_01.speed; //Move player up
				}
				if (player_01_kHeld & KEY_DDOWN || player_01_kHeld & KEY_LSTICK_LEFT) {
					player_01.pos.y += player_01.speed;
				}
			}
		
			if (b.y < player_02.pos.y + player_02.pos.h/2) {
				player_02.pos.y -= player_02.speed;
			}
			if (b.y > player_02.pos.y + player_02.pos.h/2) {
				player_02.pos.y += player_02.speed;
			}	
			if (b.xVelocity > BALL_MAXSPEED) {
				b.xVelocity = BALL_MAXSPEED;
			}
			if (b.yVelocity > BALL_MAXSPEED) {
				b.yVelocity = BALL_MAXSPEED;
			}
		
			b.x += b.xVelocity;
			b.y += b.yVelocity;
		
			if (b.y < 0) {
				b.y = 0;
				b.yVelocity *= -1;
				PlaySound(pong);
			}
			if (b.y + BALL_HEIGHT >= SCREEN_HEIGHT) {
				b.y = SCREEN_HEIGHT - BALL_HEIGHT - 1;
				b.yVelocity *= -1;
				PlaySound(pong);
			}
			if (b.x < 0) {
				player_02.score += 1;
				b.x = player_01.pos.x + player_01.pos.w;
				b.y = player_01.pos.y + player_01.pos.h/2;
				b.xVelocity = BALL_INIT_SPEED;
				b.speed = BALL_INIT_SPEED;
				PlaySound(score);
			}
			if (b.x + BALL_WIDTH >= SCREEN_WIDTH) {
				player_01.score += 1;
				b.x = player_02.pos.x - BALL_WIDTH;
				b.y = player_02.pos.y + player_02.pos.h / 2;
				b.xVelocity = -1 * BALL_INIT_SPEED;
				b.speed = BALL_INIT_SPEED;
				PlaySound(score);
			}
		
			if (player_01.pos.y < 0) player_01.pos.y = 0;
			if (player_01.pos.y + player_01.pos.h > SCREEN_HEIGHT) player_01.pos.y = SCREEN_HEIGHT - player_01.pos.h;
			if (player_02.pos.y < 0) player_02.pos.y = 0;
			if (player_02.pos.y + player_02.pos.h > SCREEN_HEIGHT) player_02.pos.y = SCREEN_HEIGHT - player_02.pos.h;
		
			b_rect.x = (int) b.x;
			b_rect.y = (int) b.y;
		
			if (SDL_HasIntersection(&player_01.pos, &b_rect)) {
				PlaySound(pong);
				b.x = player_01.pos.x + player_01.pos.w;
				b.speed = b.speed * BALL_ACCELERATE;
			
				float angle = calculateAngle(player_01.pos.y, b.y, player_01.pos.h);
				b.xVelocity = b.speed * cos(angle);
				b.yVelocity = ((b.yVelocity>0)? -1 : 1) * b.speed * sin(angle);
			}
		
			if (SDL_HasIntersection(&player_02.pos, &b_rect)) {
				PlaySound(pong);
				b.x = player_02.pos.x - BALL_WIDTH;
				b.speed = b.speed * BALL_ACCELERATE;
				float angle = calculateAngle(player_02.pos.y, b.y, player_02.pos.h);
				b.xVelocity = -1 * b.speed * cos(angle);
				b.yVelocity = ((b.yVelocity>0)? -1 : 1) * b.speed * sin(angle);
			}
		
			SDL_RenderClear(renderer);
			SDL_RenderCopy(renderer, paddleTexture, NULL, &player_01.pos);
			SDL_RenderCopy(renderer, paddleTexture, NULL, &player_02.pos);
			renderTexture(ballTexture, renderer, b.x, b.y, BALL_WIDTH, BALL_HEIGHT);
			SDL_RenderPresent(renderer);
		
		}
		
		if (state == GAME_MULTI_PLAYER) {
			
			player_02.speed = 10; //set player 2 speed
			
			if (hidGetHandheldMode() == 1) { //if joycons connected to switch
				
				state = DETACH_JOYCONS;
				
				} else if (hidGetHandheldMode() == 0) { //if joycon detached from switch
				
				u64 player_01_kDown = hidKeysDown(CONTROLLER_P1_AUTO); //Variable for player one buttons pressed
				u64 player_01_kHeld = hidKeysHeld(CONTROLLER_P1_AUTO); //variable for player one buttons held 	
				
				u64 player_02_kDown = hidKeysDown(CONTROLLER_P1_AUTO);
				u64 player_02_kHeld = hidKeysHeld(CONTROLLER_P1_AUTO);
				
				if (player_01_kDown & KEY_MINUS) {
					restartGame(player_01, player_02, b);
					state = MAINMENU;
				}
				if (player_01_kHeld & KEY_DUP || player_01_kHeld & KEY_LSTICK_RIGHT) {
					player_01.pos.y -= player_01.speed; //Move player up
				}
				if (player_01_kHeld & KEY_DDOWN || player_01_kHeld & KEY_LSTICK_LEFT) {
					player_01.pos.y += player_01.speed;
				}
				
				if (player_02_kHeld & KEY_Y || player_02_kHeld & KEY_RSTICK_LEFT) {
					player_02.pos.y -= player_02.speed;
				}
				if (player_02_kHeld & KEY_A || player_02_kHeld & KEY_RSTICK_RIGHT) {
					player_02.pos.y += player_02.speed;
				}
			}
		
			//if (b.y < player_02.pos.y + player_02.pos.h/2) {
				//player_02.pos.y -= player_02.speed;
			//}
			//if (b.y > player_02.pos.y + player_02.pos.h/2) {
				//player_02.pos.y += player_02.speed;
			//}	
			if (b.xVelocity > BALL_MAXSPEED) {
				b.xVelocity = BALL_MAXSPEED;
			}
			if (b.yVelocity > BALL_MAXSPEED) {
				b.yVelocity = BALL_MAXSPEED;
			}
		
			b.x += b.xVelocity;
			b.y += b.yVelocity;
		
			if (b.y < 0) {
				b.y = 0;
				b.yVelocity *= -1;
				PlaySound(pong);
			}
			if (b.y + BALL_HEIGHT >= SCREEN_HEIGHT) {
				b.y = SCREEN_HEIGHT - BALL_HEIGHT - 1;
				b.yVelocity *= -1;
				PlaySound(pong);
			}
			if (b.x < 0) {
				player_02.score += 1;
				b.x = player_01.pos.x + player_01.pos.w;
				b.y = player_01.pos.y + player_01.pos.h/2;
				b.xVelocity = BALL_INIT_SPEED;
				b.speed = BALL_INIT_SPEED;
				PlaySound(score);
			}
			if (b.x + BALL_WIDTH >= SCREEN_WIDTH) {
				player_01.score += 1;
				b.x = player_02.pos.x - BALL_WIDTH;
				b.y = player_02.pos.y + player_02.pos.h / 2;
				b.xVelocity = -1 * BALL_INIT_SPEED;
				b.speed = BALL_INIT_SPEED;
				PlaySound(score);
			}
		
			if (player_01.pos.y < 0) player_01.pos.y = 0;
			if (player_01.pos.y + player_01.pos.h > SCREEN_HEIGHT) player_01.pos.y = SCREEN_HEIGHT - player_01.pos.h;
			if (player_02.pos.y < 0) player_02.pos.y = 0;
			if (player_02.pos.y + player_02.pos.h > SCREEN_HEIGHT) player_02.pos.y = SCREEN_HEIGHT - player_02.pos.h;
		
			b_rect.x = (int) b.x;
			b_rect.y = (int) b.y;
		
			if (SDL_HasIntersection(&player_01.pos, &b_rect)) {
				PlaySound(pong);
				b.x = player_01.pos.x + player_01.pos.w;
				b.speed = b.speed * BALL_ACCELERATE;
			
				float angle = calculateAngle(player_01.pos.y, b.y, player_01.pos.h);
				b.xVelocity = b.speed * cos(angle);
				b.yVelocity = ((b.yVelocity>0)? -1 : 1) * b.speed * sin(angle);
			}
		
			if (SDL_HasIntersection(&player_02.pos, &b_rect)) {
				PlaySound(pong);
				b.x = player_02.pos.x - BALL_WIDTH;
				b.speed = b.speed * BALL_ACCELERATE;
				float angle = calculateAngle(player_02.pos.y, b.y, player_02.pos.h);
				b.xVelocity = -1 * b.speed * cos(angle);
				b.yVelocity = ((b.yVelocity>0)? -1 : 1) * b.speed * sin(angle);
			}
		
			SDL_RenderClear(renderer);
			SDL_RenderCopy(renderer, paddleTexture, NULL, &player_01.pos);
			SDL_RenderCopy(renderer, paddleTexture, NULL, &player_02.pos);
			renderTexture(ballTexture, renderer, b.x, b.y, BALL_WIDTH, BALL_HEIGHT);
			SDL_RenderPresent(renderer);
			
		}
	}
	
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	
	Mix_FreeChunk(pong);
	Mix_FreeChunk(score);
	Mix_CloseAudio();
	IMG_Quit();
	SDL_Quit();
	return 0;
}