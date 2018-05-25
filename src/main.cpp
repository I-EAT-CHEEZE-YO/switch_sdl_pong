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
const int GAME = 3;

int state = SPLASH;

Mix_Chunk *pong = NULL;

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

void PlaySound() {
	if (Mix_PlayChannel( -1, pong, 0) == -1) {
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
	player_02.speed = 3.5;
	
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
	player_02.speed = 3.5; // player 2 speed
	
	player_01.pos.x = 0; //player 1 x 
	player_02.pos.x = SCREEN_WIDTH - player_02.pos.w; //player 2 x
	player_01.pos.y = SCREEN_HEIGHT / 2 - player_01.pos.h / 2; //player 1 y
	player_02.pos.y = SCREEN_HEIGHT / 2 - player_02.pos.h / 2; //player 2 y
	
	uint prevTime = SDL_GetTicks(); // Hm
	bool quit = false; // quit variable. if true quits and exits application
	int frames = 0; // frames
	float fps; //fps
	int splashTimer = 0; //splash screen timer
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
	
	
	SDL_Surface* splashImage = IMG_Load("assets/image/splash.jpg");
	SDL_Texture* splashTexture = SDL_CreateTextureFromSurface(renderer, splashImage);
	
	SDL_Surface* mainMenuImage_01 = IMG_Load("assets/image/main_menu_one_player.jpg");
	SDL_Surface* mainMenuImage_02 = IMG_Load("assets/image/main_menu_two_player.jpg");
	SDL_Surface* mainMenuImage_03 = IMG_Load("assets/image/main_menu_quit.jpg");
	
	SDL_Texture* mainMenuTexture_01 = SDL_CreateTextureFromSurface(renderer, mainMenuImage_01);
	SDL_Texture* mainMenuTexture_02 = SDL_CreateTextureFromSurface(renderer, mainMenuImage_02);
	SDL_Texture* mainMenuTexture_03 = SDL_CreateTextureFromSurface(renderer, mainMenuImage_03);
	
	//convert Images to Textures
	SDL_Texture* paddleTexture = SDL_CreateTextureFromSurface(renderer, paddleImage);
	SDL_Texture* ballTexture = SDL_CreateTextureFromSurface(renderer, ballImage);
	
	SDL_Texture* mainMenuTextures[3] = {mainMenuTexture_01, mainMenuTexture_02, mainMenuTexture_03};
	
	pong = Mix_LoadWAV("assets/audio/pong.wav");
	
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
		
		u32 kDown = hidKeysDown(CONTROLLER_P1_AUTO);
		u32 kHeld = hidKeysHeld(CONTROLLER_P1_AUTO);
		
		if (state == SPLASH) {
			splashTimer ++;
			
			SDL_RenderClear(renderer);
			renderTexture(splashTexture, renderer, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
			SDL_RenderPresent(renderer);
			
			if (splashTimer >= 25) {
				state = MAINMENU;
			}
		}
		
		if (state == MAINMENU) {
			
			if (kDown & KEY_UP) {
				if (mainMenuIndex > 0) {
					mainMenuIndex -= 1;
					PlaySound();
				}
			}
			if (kDown & KEY_DOWN) {
				if (mainMenuIndex < 2) {
					mainMenuIndex += 1;
					PlaySound();
				}
			}
			if (kDown & KEY_A) {
				if (mainMenuIndex == 0) {
					state = GAME;
				}
				if (mainMenuIndex == 2) {
					quit = true;
					break;
				}
			}
			
			SDL_RenderClear(renderer);
			renderTexture(mainMenuTextures[mainMenuIndex], renderer, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
			SDL_RenderPresent(renderer);			
			
		}
		
		if (state == GAME) {
			
			if (kDown & KEY_PLUS){
				restartGame(player_01, player_02, b);
				state = MAINMENU;
			}
			
			if (kDown & KEY_MINUS) {
				restartGame(player_01, player_02, b);
			}
		
			if (kHeld & KEY_UP) {
				player_01.pos.y -= player_01.speed;
			}
		
			if (kHeld & KEY_DOWN) {
				player_01.pos.y += player_01.speed;
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
				PlaySound();
			}
			if (b.y + BALL_HEIGHT >= SCREEN_HEIGHT) {
				b.y = SCREEN_HEIGHT - BALL_HEIGHT - 1;
				b.yVelocity *= -1;
				PlaySound();
			}
			if (b.x < 0) {
				player_02.score += 1;
				b.x = player_01.pos.x + player_01.pos.w;
				b.y = player_01.pos.y + player_01.pos.h/2;
				b.xVelocity = BALL_INIT_SPEED;
				b.speed = BALL_INIT_SPEED;
				PlaySound();
			}
			if (b.x + BALL_WIDTH >= SCREEN_WIDTH) {
				player_01.score += 1;
				b.x = player_02.pos.x - BALL_WIDTH;
				b.y = player_02.pos.y + player_02.pos.h / 2;
				b.xVelocity = -1 * BALL_INIT_SPEED;
				b.speed = BALL_INIT_SPEED;
				PlaySound();
			}
		
			if (player_01.pos.y < 0) player_01.pos.y = 0;
			if (player_01.pos.y + player_01.pos.h > SCREEN_HEIGHT) player_01.pos.y = SCREEN_HEIGHT - player_01.pos.h;
			if (player_02.pos.y < 0) player_02.pos.y = 0;
			if (player_02.pos.y + player_02.pos.h > SCREEN_HEIGHT) player_02.pos.y = SCREEN_HEIGHT - player_02.pos.h;
		
			b_rect.x = (int) b.x;
			b_rect.y = (int) b.y;
		
			if (SDL_HasIntersection(&player_01.pos, &b_rect)) {
				PlaySound();
				b.x = player_01.pos.x + player_01.pos.w;
				b.speed = b.speed * BALL_ACCELERATE;
			
				float angle = calculateAngle(player_01.pos.y, b.y, player_01.pos.h);
				b.xVelocity = b.speed * cos(angle);
				b.yVelocity = ((b.yVelocity>0)? -1 : 1) * b.speed * sin(angle);
			}
		
			if (SDL_HasIntersection(&player_02.pos, &b_rect)) {
				PlaySound();
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
	Mix_CloseAudio();
	IMG_Quit();
	SDL_Quit();
	return 0;
}
