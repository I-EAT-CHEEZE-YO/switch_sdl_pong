/*
Pong for the Nintendo Switch Written in c++ Using libnx and SDL2 
Written By I_EAT_CHEEZE_YO a.k.a. Todd Forester a.k.a. CHEEZE a.k.a. CHEEZUS
5-2018
*/



//includes
#include <switch.h>
#include <random>
#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <math.h>
#include <stdio.h>
#include "main.hpp"

using namespace std;

const int upper = 0;
const int lower = 1;
const int symbol = 2;


const int alpha_a = 0;
const int alpha_b = 1;
const int alpha_c = 2;
const int alpha_d = 3;
const int alpha_e = 4;
const int alpha_f = 5;
const int alpha_g = 6;
const int alpha_h = 7;
const int alpha_i = 8;
const int alpha_j = 9;
const int alpha_k = 10;
const int alpha_l = 11;
const int alpha_m = 12;
const int alpha_n = 13;
const int alpha_o = 14;
const int alpha_p = 15;
const int alpha_q = 16;
const int alpha_r = 17;
const int alpha_s = 18;
const int alpha_t = 19;
const int alpha_u = 20;
const int alpha_v = 21;
const int alpha_w = 22;
const int alpha_x = 23;
const int alpha_y = 24;
const int alpha_z = 25;

const int colon = 14;
const int number_0 = 15;
const int number_1 = 16;
const int number_2 = 17;
const int number_3 = 18;
const int number_4 = 19;
const int number_5 = 20;
const int number_6 = 21;
const int number_7 = 22;
const int number_8 = 23;
const int number_9 = 24;
const int space = 25;


//states
const int SPLASH = 0;
const int MAIN_MENU = 1;
const int GAME_SINGLE_PLAYER = 2;
const int GAME_MULTI_PLAYER = 3;
const int DETACH_JOYCONS = 4;
const int P1WINS = 5;
const int P2WINS = 6;

int state = SPLASH; //starting state

Mix_Chunk* pong = NULL; //pong sound 
Mix_Chunk* score = NULL; //score sound

const int FONT_WIDTH = 16;
const int FONT_HEIGHT = 16;

SDL_Rect charRect[3][26];

//ball struct
typedef struct {
	float x;
	float y;
	float xVelocity;
	float yVelocity;
	float speed;
} ball;

//player struct
typedef struct {
	SDL_Rect pos;
	int score;
	int speed;
} player;

//calculate angle function
float calculateAngle(float y1, float y2, int height) {
	
	float rely = y1 + height/2 - y2;
	rely /= height/2.0;
	return rely * MAX_ANGLE;
	
}

void renderFont(SDL_Texture *tex, SDL_Renderer *ren, const SDL_Rect *sourceRect, int x, int y, int w, int h) {
	
	SDL_Rect dest;
	dest.x = x;
	dest.y = y;
	dest.w = w;
	dest.h = h;
	
	SDL_RenderCopy(ren, tex, sourceRect, &dest);
	
}

//function to render a texture to a surface at x, y, w, y
void renderTexture(SDL_Texture *tex, SDL_Renderer *ren, int x, int y, int w=-1, int h=-1) {
	
	SDL_Rect dest;
	dest.x = x;
	dest.y = y;
	dest.w = w;
	dest.h = h;
	
	if (w == -1 || h == -1) {
		SDL_QueryTexture(tex, NULL, NULL, &dest.w, &dest.h); //if w and h not set get texture w and h
	}

	SDL_RenderCopy(ren, tex, NULL, &dest); // render the texture

}

void PlaySound(Mix_Chunk* sound) { //function to play a sound 
	if (Mix_PlayChannel( -1, sound, 0) == -1) {
		return;
	}
}

void restartGame(player &player_01, player &player_02, ball &b) { //function to reset game variables
	
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
		player_02.speed = 10;
	} else {
		player_02.speed = 3.5;
	}
	
	player_01.pos.x = 0;
	player_02.pos.x = SCREEN_WIDTH - player_02.pos.w;
	player_01.pos.y = SCREEN_HEIGHT / 2 - player_01.pos.h / 2;
	player_02.pos.y = SCREEN_HEIGHT / 2 - player_02.pos.h / 2;
	
}

void drawLine(SDL_Renderer* renderer, int x, int y, int w, int h, Uint8 r = 0, Uint8 g = 255, Uint8 b = 0, Uint8 a = 255 ) { //funtion to draw a line x, y, w, h, with a rgb color of red, green, blue, alpha
	
	SDL_SetRenderDrawColor(renderer, r, g, b, a); //set the render color to r,g,b,a
	SDL_RenderDrawLine(renderer, x, y, w, h); //draw the line
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0); //set the render color back to transparent
	
}

void drawScores(int p1score, int p2score, SDL_Texture *fontTexture, SDL_Renderer *renderer) {
	
	//player 1
	int player_01_char[6] = {alpha_p, alpha_l, alpha_a, alpha_y, alpha_e, alpha_r};
	
	for (int x=0; x<6; x++) {
		renderFont(fontTexture, renderer, &charRect[upper][player_01_char[x]], x * FONT_WIDTH, 0 , FONT_WIDTH, FONT_HEIGHT);
	}
	
	renderFont(fontTexture, renderer, &charRect[symbol][number_1], 96, 0, FONT_WIDTH, FONT_HEIGHT);
	renderFont(fontTexture, renderer, &charRect[symbol][colon], 112, 0, FONT_WIDTH, FONT_HEIGHT);
	renderFont(fontTexture, renderer, &charRect[symbol][space], 128, 0, FONT_WIDTH, FONT_HEIGHT);
	
	renderFont(fontTexture, renderer, &charRect[symbol][number_0], 144, 0, FONT_WIDTH, FONT_HEIGHT);
	
	if (p1score == 0) {
		renderFont(fontTexture, renderer, &charRect[symbol][number_0], 160, 0, FONT_WIDTH, FONT_HEIGHT);
	}
	if (p1score == 1) {
		renderFont(fontTexture, renderer, &charRect[symbol][number_1], 160, 0, FONT_WIDTH, FONT_HEIGHT);
	}
	if (p1score == 2) {
		renderFont(fontTexture, renderer, &charRect[symbol][number_2], 160, 0, FONT_WIDTH, FONT_HEIGHT);
	}
	if (p1score == 3) {
		renderFont(fontTexture, renderer, &charRect[symbol][number_3], 160, 0, FONT_WIDTH, FONT_HEIGHT);
	}
	if (p1score == 4) {
		renderFont(fontTexture, renderer, &charRect[symbol][number_4], 160, 0, FONT_WIDTH, FONT_HEIGHT);
	}
	if (p1score == 5) {
		renderFont(fontTexture, renderer, &charRect[symbol][number_5], 160, 0, FONT_WIDTH, FONT_HEIGHT);
	}
	if (p1score == 6) {
		renderFont(fontTexture, renderer, &charRect[symbol][number_6], 160, 0, FONT_WIDTH, FONT_HEIGHT);
	}
	if (p1score == 7) {
		renderFont(fontTexture, renderer, &charRect[symbol][number_7], 160, 0, FONT_WIDTH, FONT_HEIGHT);
	}
	if (p1score == 8) {
		renderFont(fontTexture, renderer, &charRect[symbol][number_8], 160, 0, FONT_WIDTH, FONT_HEIGHT);
	}
	if (p1score == 9) {
		renderFont(fontTexture, renderer, &charRect[symbol][number_9], 160, 0, FONT_WIDTH, FONT_HEIGHT);
	}
	
	//player 2
	for (int x=0; x<6; x++) {
		renderFont(fontTexture, renderer, &charRect[upper][player_01_char[x]], 1104 + x * FONT_WIDTH, 0, FONT_WIDTH, FONT_HEIGHT);
	}
	
	renderFont(fontTexture, renderer, &charRect[symbol][number_2], 1200, 0, FONT_WIDTH, FONT_HEIGHT);
	renderFont(fontTexture, renderer, &charRect[symbol][colon], 1216, 0, FONT_WIDTH, FONT_HEIGHT);
	renderFont(fontTexture, renderer, &charRect[symbol][space], 1232, 0, FONT_WIDTH, FONT_HEIGHT);
	renderFont(fontTexture, renderer, &charRect[symbol][number_0], 1248, 0, FONT_WIDTH, FONT_HEIGHT);
	
	if (p2score == 0) {
		renderFont(fontTexture, renderer, &charRect[symbol][number_0], 1264, 0, FONT_WIDTH, FONT_HEIGHT);
	}
	if (p2score == 1) {
		renderFont(fontTexture, renderer, &charRect[symbol][number_1], 1264, 0, FONT_WIDTH, FONT_HEIGHT);
	}
	if (p2score == 2) {
		renderFont(fontTexture, renderer, &charRect[symbol][number_2], 1264, 0, FONT_WIDTH, FONT_HEIGHT);
	}
	if (p2score == 3) {
		renderFont(fontTexture, renderer, &charRect[symbol][number_3], 1264, 0, FONT_WIDTH, FONT_HEIGHT);
	}
	if (p2score == 4) {
		renderFont(fontTexture, renderer, &charRect[symbol][number_4], 1264, 0, FONT_WIDTH, FONT_HEIGHT);
	}
	if (p2score == 5) {
		renderFont(fontTexture, renderer, &charRect[symbol][number_5], 1264, 0, FONT_WIDTH, FONT_HEIGHT);
	}
	if (p2score == 6) {
		renderFont(fontTexture, renderer, &charRect[symbol][number_6], 1264, 0, FONT_WIDTH, FONT_HEIGHT);
	}
	if (p2score == 7) {
		renderFont(fontTexture, renderer, &charRect[symbol][number_7], 1264, 0, FONT_WIDTH, FONT_HEIGHT);
	}
	if (p2score == 8) {
		renderFont(fontTexture, renderer, &charRect[symbol][number_8], 1264, 0, FONT_WIDTH, FONT_HEIGHT);
	}
	if (p2score == 9) {
		renderFont(fontTexture, renderer, &charRect[symbol][number_9], 1264, 0, FONT_WIDTH, FONT_HEIGHT);
	}
	
}

int main(int argc, char **argv) {
	
	for (int y=0; y<3; y++) {
		for (int x=0; x<26; x++) {
		
		charRect[y][x].x = x * FONT_WIDTH;
		charRect[y][x].y = y * FONT_HEIGHT;
		charRect[y][x].w = FONT_WIDTH;
		charRect[y][x].h = FONT_HEIGHT;
		
	}
}
	
	romfsInit();
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
	
	uint prevTime = SDL_GetTicks(); // Get ticks
	bool quit = false; // quit variable. if true quits and exits application
	bool paused = false; //if the game is paused or not
	int frames = 0; // frames
	float fps; //fps
	int splashTimer = 0; //splash screen timer
	int mainMenuIndex = 0; //main menu image index
	
    //Setup window
    SDL_Window* window = SDL_CreateWindow(nullptr, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 0, 0, SDL_WINDOW_FULLSCREEN_DESKTOP);
	
	//Setup renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
	
	//Load Images
	SDL_Surface* paddleImage = IMG_Load("romfs:/res/image/paddle.png");
	SDL_Surface* ballImage = IMG_Load("romfs:/res/image/ball.png");
	
	SDL_Surface* font16X16 = IMG_Load("romfs:/res/font/font_16x16.png");
	SDL_Texture* fontTexture = SDL_CreateTextureFromSurface(renderer, font16X16);
	
	SDL_Surface* p1winsImage = IMG_Load("romfs:/res/image/player_1_wins.png");
	SDL_Surface* p2winsImage = IMG_Load("romfs:/res/image/player_2_wins.png");
	
	SDL_Texture* p1winsTexture = SDL_CreateTextureFromSurface(renderer, p1winsImage);
	SDL_Texture* p2winsTexture = SDL_CreateTextureFromSurface(renderer, p2winsImage);
	
	SDL_Surface* splashImage = IMG_Load("romfs:/res/image/splash.png");
	SDL_Texture* splashTexture = SDL_CreateTextureFromSurface(renderer, splashImage);
	
	SDL_Surface* mainMenuImage_01 = IMG_Load("romfs:/res/image/main_menu_one_player.jpg");
	SDL_Surface* mainMenuImage_02 = IMG_Load("romfs:/res/image/main_menu_two_player.jpg");
	SDL_Surface* mainMenuImage_03 = IMG_Load("romfs:/res/image/main_menu_quit.jpg");
	
	SDL_Surface* pausedImage = IMG_Load("romfs:/res/image/paused.png");
	SDL_Texture* pausedTexture = SDL_CreateTextureFromSurface(renderer, pausedImage);
	
	SDL_Texture* mainMenuTexture_01 = SDL_CreateTextureFromSurface(renderer, mainMenuImage_01);
	SDL_Texture* mainMenuTexture_02 = SDL_CreateTextureFromSurface(renderer, mainMenuImage_02);
	SDL_Texture* mainMenuTexture_03 = SDL_CreateTextureFromSurface(renderer, mainMenuImage_03);
	
	SDL_Surface* detachImage = IMG_Load("romfs:/res/image/detach_joycons.png");
	SDL_Texture* detachTexture = SDL_CreateTextureFromSurface(renderer, detachImage);
	
	//convert Images to Textures
	SDL_Texture* paddleTexture = SDL_CreateTextureFromSurface(renderer, paddleImage);
	SDL_Texture* ballTexture = SDL_CreateTextureFromSurface(renderer, ballImage);
	
	SDL_Texture* mainMenuTextures[3] = {mainMenuTexture_01, mainMenuTexture_02, mainMenuTexture_03};
	
	pong = Mix_LoadWAV("romfs:/res/audio/pong.wav");
	score = Mix_LoadWAV("romfs:/res/audio/score.wav");
	
	while(appletMainLoop() && !quit) { //main game loop
		
		++frames; //increase frames counter
		uint currTime = SDL_GetTicks(); //current time since last tick
		float elapsed = (currTime - prevTime); //elapsed time
		
		if (elapsed > 100) { //if elapsed time id greater than 100
			
			fps = round(frames / (elapsed / 1000.0)); //round fps
			frames = 0; //reset frames
			prevTime = currTime; //set previous time to current time
			
		}
		
		//Scan all the inputs. This should be done once for each frame
		hidScanInput();
		
		//if splash 
		if (state == SPLASH) {
			splashTimer ++; //increase splash timer
			
			if (splashTimer >= 50) { //if splash timer is greater than 75
				state = MAIN_MENU; //change state to main menu
			}
			
			SDL_RenderClear(renderer); //clear the renderer
			renderTexture(splashTexture, renderer, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT); //render the splash screen
			SDL_RenderPresent(renderer); //show renderer on screen

		}
		
		if (state == P1WINS) {
			if (hidGetHandheldMode() == 1) { //if joycons connected to switch
			
				u64 kDown = hidKeysDown(CONTROLLER_P1_AUTO);
			
				if (kDown & KEY_B) {
					restartGame(player_01, player_02, b);
					state = MAIN_MENU;
				}
				
			} else if (hidGetHandheldMode() == 0) { //if joycons detached from switch
				
				u64 player_01_kDown = hidKeysDown(CONTROLLER_P1_AUTO);
				
				if (player_01_kDown & KEY_DDOWN || player_01_kDown & KEY_A) {
					restartGame(player_01, player_02, b);
					state = MAIN_MENU;
				}
			}
			
			SDL_RenderClear(renderer); //Clear the screen
			renderTexture(p1winsTexture, renderer, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT); //Draw the player 1 wins screen 
			SDL_RenderPresent(renderer); //Display the rendered image
		
		}
		
		if (state == P2WINS) {
			if (hidGetHandheldMode() == 1) { //if joycons connected to switch
			
				u64 kDown = hidKeysDown(CONTROLLER_P1_AUTO);
			
				if (kDown & KEY_B) {
					restartGame(player_01, player_02, b);
					state = MAIN_MENU;
				}
				
			} else if (hidGetHandheldMode() == 0) { //if joycons detached from switch
				
				u64 player_01_kDown = hidKeysDown(CONTROLLER_P1_AUTO);

				if (player_01_kDown & KEY_DDOWN || player_01_kDown & KEY_A) {
					restartGame(player_01, player_02, b);
					state = MAIN_MENU;
				}
			}
			
			SDL_RenderClear(renderer); //Clear the screen
			renderTexture(p2winsTexture, renderer, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT); //Draw the player 2 wins screen 
			SDL_RenderPresent(renderer); //Display the rendered image
		
		}
		
		if (state == MAIN_MENU) { //if state is main menu
			
			if (hidGetHandheldMode() == 1) { //if joycons connected to switch
			
				u64 kDown = hidKeysDown(CONTROLLER_P1_AUTO);
			
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
			u64 kDown = hidKeysDown(CONTROLLER_P1_AUTO);
			
			if (hidGetHandheldMode() == 1 && kDown & KEY_B) {
				state = MAIN_MENU;
			}
			
			if (hidGetHandheldMode() == 0) {
				paused = true;
				state = GAME_MULTI_PLAYER;
			}
			
			SDL_RenderClear(renderer);
			renderTexture(detachTexture, renderer, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
			SDL_RenderPresent(renderer);
			
		}
		
		if (state == GAME_SINGLE_PLAYER) { //If Single player mode
			
			if (!paused) {
				
				player_02.speed = 3.5; //set ai speed
			
				if (hidGetHandheldMode() == 1) { //if joycons connected to switch
			
					u64 kDown = hidKeysDown(CONTROLLER_P1_AUTO); //Variable for player one buttons pressed
					u64 kHeld = hidKeysHeld(CONTROLLER_P1_AUTO); //variable for player one buttons held 
			
					if (kDown & KEY_PLUS){ //If plus pressed
						paused = !paused;
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
				
					if (player_01_kDown & KEY_MINUS || player_01_kDown & KEY_PLUS) {
						paused = !paused;
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
		
				if (b.y < 17) {
					b.y = 17;
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
		
				if (player_01.pos.y < 17) player_01.pos.y = 17;
				if (player_01.pos.y + player_01.pos.h > SCREEN_HEIGHT) player_01.pos.y = SCREEN_HEIGHT - player_01.pos.h;
				if (player_02.pos.y < 17) player_02.pos.y = 17;
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
				
				if (player_01.score == 10) {
					restartGame(player_01, player_02, b);
					state = P1WINS;
				}
				
				if (player_02.score == 10) { 
					restartGame(player_01, player_02, b);
					state = P2WINS;
				}
		
				SDL_RenderClear(renderer);
				drawLine(renderer, 0, 17, SCREEN_WIDTH, 17 ,0, 255, 0, 255);
				drawLine(renderer, 0, SCREEN_HEIGHT - 1, SCREEN_WIDTH, SCREEN_HEIGHT - 1 ,0, 255, 0, 255);
				drawLine(renderer, SCREEN_WIDTH / 2, 17, SCREEN_WIDTH / 2, SCREEN_HEIGHT ,0, 255, 0, 255);
				drawScores(player_01.score, player_02.score, fontTexture, renderer);
				SDL_RenderCopy(renderer, paddleTexture, NULL, &player_01.pos);
				SDL_RenderCopy(renderer, paddleTexture, NULL, &player_02.pos);
				renderTexture(ballTexture, renderer, b.x, b.y, BALL_WIDTH, BALL_HEIGHT);
				SDL_RenderPresent(renderer);
			} else if (paused) {
				
				if (hidGetHandheldMode() == 1) { //if joycons connected to switch
			
					u64 kDown = hidKeysDown(CONTROLLER_P1_AUTO); //Variable for player one buttons pressed
			
					if (kDown & KEY_PLUS){ //If plus pressed
						paused = !paused; 
					}
					
					if (kDown & KEY_B) {
						restartGame(player_01, player_02, b);
						state = MAIN_MENU;
						paused = false;
					}
		
				} else if (hidGetHandheldMode() == 0) { //if joycon detached from switch
				
					u64 kDown = hidKeysDown(CONTROLLER_P1_AUTO); //Variable for player one buttons pressed
				
					if (kDown & KEY_B) {
						restartGame(player_01, player_02, b);
						state = MAIN_MENU;
						paused = false;
					}
				}
				
				SDL_RenderClear(renderer);
				renderTexture(pausedTexture, renderer, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
				SDL_RenderPresent(renderer);
			
			}
		}
		
		if (state == GAME_MULTI_PLAYER) {
			
			if (!paused) {
			
				player_02.speed = 10; //set player 2 speed
			
				if (hidGetHandheldMode() == 1) { //if joycons connected to switch
				
					state = DETACH_JOYCONS;
				
					} else if (hidGetHandheldMode() == 0) { //if joycon detached from switch
				
					u64 player_01_kDown = hidKeysDown(CONTROLLER_P1_AUTO); //Variable for player one buttons pressed
					u64 player_01_kHeld = hidKeysHeld(CONTROLLER_P1_AUTO); //variable for player one buttons held 	
				
					u64 player_02_kDown = hidKeysDown(CONTROLLER_P1_AUTO);
					u64 player_02_kHeld = hidKeysHeld(CONTROLLER_P1_AUTO);
				
					if (player_01_kDown & KEY_MINUS || player_02_kDown & KEY_PLUS) {
						paused = !paused;
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

				if (b.xVelocity > BALL_MAXSPEED) {
					b.xVelocity = BALL_MAXSPEED;
				}
				if (b.yVelocity > BALL_MAXSPEED) {
					b.yVelocity = BALL_MAXSPEED;
				}
		
				b.x += b.xVelocity;
				b.y += b.yVelocity;
		
				if (b.y < 17) {
					b.y = 17;
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
		
				if (player_01.pos.y < 17) player_01.pos.y = 17;
				if (player_01.pos.y + player_01.pos.h > SCREEN_HEIGHT) player_01.pos.y = SCREEN_HEIGHT - player_01.pos.h;
				if (player_02.pos.y < 17) player_02.pos.y = 17;
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
				
				if (player_01.score == 10) {
					restartGame(player_01, player_02, b);
					state = P1WINS;
				}
				
				if (player_02.score == 10) { 
					restartGame(player_01, player_02, b);
					state = P2WINS;
				}
		
				SDL_RenderClear(renderer);
				drawLine(renderer, 0, 17, SCREEN_WIDTH, 17 ,0, 255, 0, 255);
				drawLine(renderer, 0, 17, SCREEN_WIDTH, 17 ,0, 255, 0, 255);
				drawLine(renderer, 0, SCREEN_HEIGHT - 1, SCREEN_WIDTH, SCREEN_HEIGHT - 1 ,0, 255, 0, 255);
				drawLine(renderer, SCREEN_WIDTH / 2, 17, SCREEN_WIDTH / 2, SCREEN_HEIGHT ,0, 255, 0, 255);
				drawScores(player_01.score, player_02.score, fontTexture, renderer);
				SDL_RenderCopy(renderer, paddleTexture, NULL, &player_01.pos);
				SDL_RenderCopy(renderer, paddleTexture, NULL, &player_02.pos);
				renderTexture(ballTexture, renderer, b.x, b.y, BALL_WIDTH, BALL_HEIGHT);
				SDL_RenderPresent(renderer);
			
			} else if (paused) {
				
				if (hidGetHandheldMode() == 1) { //if joycons connected to switch
			
					u64 kDown = hidKeysDown(CONTROLLER_P1_AUTO); //Variable for player one buttons pressed
			
					state = DETACH_JOYCONS;
					
					if (kDown & KEY_B) {
						restartGame(player_01, player_02, b);
						state = MAIN_MENU;
						paused = false;
					}
		
				} else if (hidGetHandheldMode() == 0) { //if joycon detached from switch
				
					u64 kDown = hidKeysDown(CONTROLLER_P1_AUTO); //Variable for player one buttons pressed
					
					if (kDown & KEY_PLUS || kDown & KEY_MINUS) {
						paused = !paused;
					}
					
					if (kDown & KEY_A || kDown & KEY_DLEFT) {
						restartGame(player_01, player_02, b);
						state = MAIN_MENU;
						paused = false;
					}
				}
				
				SDL_RenderClear(renderer);
				renderTexture(pausedTexture, renderer, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
				drawScores(player_01.score, player_02.score, fontTexture, renderer);
				SDL_RenderPresent(renderer);
				
			}
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