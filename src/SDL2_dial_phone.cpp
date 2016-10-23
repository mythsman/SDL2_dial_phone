#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_audio.h>
#include <SDL2/SDL_mixer.h>
#include <stdio.h>
#include <math.h>

const int windowWidth = 736;
const int windowHeight = 736;
const double speed = 2;
const SDL_Rect rect = { 0, 0, 736, 736 };
SDL_Window *window;
SDL_Renderer *render;
SDL_Texture *textureButtom;
SDL_Texture *textureRoundel;
SDL_Texture *texturePointer;
SDL_Point roundelCenterPoint;
Mix_Chunk *ding, *cala;

//play specified music
void playDing() {
	if (Mix_PlayChannel(-1, ding, 0) == -1) {
		printf("Mix_PlayChannel: %s\n", Mix_GetError());
	}
}

void playCala() {
	if (Mix_PlayChannel(-1, cala, 0) == -1) {
		printf("Mix_PlayChannel: %s\n", Mix_GetError());
	}
}

//get the position of mouse
int getPosition(int x, int y) {
	if ((x - 467) * (x - 467) + (y - 366) * (y - 366) <= 400)
		return 1;
	if ((x - 444) * (x - 444) + (y - 310) * (y - 310) <= 400)
		return 2;
	if ((x - 395) * (x - 395) + (y - 275) * (y - 275) <= 400)
		return 3;
	if ((x - 337) * (x - 337) + (y - 275) * (y - 275) <= 400)
		return 4;
	if ((x - 287) * (x - 287) + (y - 309) * (y - 309) <= 400)
		return 5;
	if ((x - 266) * (x - 266) + (y - 358) * (y - 358) <= 400)
		return 6;
	if ((x - 275) * (x - 275) + (y - 414) * (y - 414) <= 400)
		return 7;
	if ((x - 313) * (x - 313) + (y - 457) * (y - 457) <= 400)
		return 8;
	if ((x - 368) * (x - 368) + (y - 471) * (y - 471) <= 400)
		return 9;
	if ((x - 422) * (x - 422) + (y - 461) * (y - 461) <= 400)
		return 0;
	return -1;
}

//calculate the angle(signed)
double getAngle(SDL_Point p1, SDL_Point center, SDL_Point p2) {
	double x1 = p1.x - center.x, y1 = p1.y - center.y;
	double x2 = p2.x - center.x, y2 = p2.y - center.y;
	double ans = acos((x1 * x2 + y1 * y2) / hypot(x1, y1) / hypot(x2, y2))
			/ M_PI * 180;
	if (x1 * y2 - x2 * y1 < 0)
		ans = -ans;
	return ans;
}

int main(int argc, char** argv) {

//init
	SDL_Init(SDL_INIT_EVERYTHING);
	IMG_Init(IMG_INIT_PNG);
	window = SDL_CreateWindow("DiaPhone", SDL_WINDOWPOS_CENTERED,
	SDL_WINDOWPOS_CENTERED, windowWidth, windowHeight, SDL_WINDOW_SHOWN);
	if (window == nullptr) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Window init failure : %s",
				SDL_GetError());
	}
	render = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
	if (render == nullptr) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Render init failure : %s",
				SDL_GetError());
	}

//load pictures
	textureButtom = IMG_LoadTexture(render, "resources/pictures/buttom.png");
	textureRoundel = IMG_LoadTexture(render, "resources/pictures/roundel.png");
	texturePointer = IMG_LoadTexture(render, "resources/pictures/pointer.png");
	roundelCenterPoint.x = 368;
	roundelCenterPoint.y = 368;
	SDL_RenderClear(render);
	SDL_RenderCopy(render, textureButtom, NULL, &rect);
	SDL_RenderCopy(render, textureRoundel, NULL, &rect);
	SDL_RenderCopy(render, texturePointer, NULL, &rect);
	SDL_RenderPresent(render);

//load audio
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024) == -1) {
		printf("Mix_OpenAudio: %s\n", Mix_GetError());
		exit(2);
	}

	ding = Mix_LoadWAV("resources/audio/ding.wav");
	cala = Mix_LoadWAV("resources/audio/cala.wav");

//main loop
	bool quit = false;
	int number = -1;
	int clickNumber = -1;
	bool dragging = false;
	bool mouseDown = false;
	bool angleFixed = false;
	char phoneNumber[100] = { 0 };
	double angle = 0;
	double accAngle = 0;
	SDL_Point rawPoint;
	while (quit == false) {
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_QUIT:
				SDL_Log("exit");
				quit = true;
				break;
			case SDL_MOUSEMOTION:
				number = getPosition(event.button.x, event.button.y);
				if (!mouseDown) {
					if (number != -1) {
						SDL_SetCursor(
								SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND));
					} else if (number == -1) {
						SDL_FreeCursor(SDL_GetCursor());
						SDL_SetCursor(
								SDL_CreateSystemCursor(
										SDL_SYSTEM_CURSOR_ARROW));
					}
				} else if (dragging) {
					SDL_Point nowPoint;
					nowPoint.x = event.button.x;
					nowPoint.y = event.button.y;
					double deltaAngle = getAngle(rawPoint, roundelCenterPoint,
							nowPoint);
					SDL_RenderCopy(render, textureButtom, NULL, &rect);
					SDL_RenderCopyEx(render, textureRoundel, NULL, NULL, angle,
							&roundelCenterPoint, SDL_FLIP_NONE);
					angle += deltaAngle;
					accAngle += deltaAngle;
					angleFixed = false;

					if (angle > 32 * (clickNumber == 0 ? 10 : clickNumber)) {
						angle = 32 * (clickNumber == 0 ? 10 : clickNumber);
						angleFixed = true;
					} else if (angle < 0) {
						angle = 0;
					}
					while (accAngle > 25 && !angleFixed) {
						accAngle -= 25;
						playCala();
					}
					SDL_RenderCopy(render, texturePointer, NULL, &rect);
					SDL_RenderPresent(render);
					rawPoint = nowPoint;
				}
				break;
			case SDL_MOUSEBUTTONDOWN:
				SDL_Log("Mouse down on (%d,%d)", event.button.x,
						event.button.y);
				number = getPosition(event.button.x, event.button.y);
				if (number != -1) {
					clickNumber = number;
					dragging = true;
					SDL_Log("Click on number %d", clickNumber);
				}
				mouseDown = true;
				rawPoint.x = event.button.x;
				rawPoint.y = event.button.y;
				break;
			case SDL_MOUSEBUTTONUP:
				SDL_Log("Mouse up on (%d,%d)", event.button.x, event.button.y);
				mouseDown = false;
				dragging = false;
				number = getPosition(event.button.x, event.button.y);
				if (number == -1) {
					SDL_FreeCursor(SDL_GetCursor());
					SDL_SetCursor(
							SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW));
				} else {
					SDL_FreeCursor(SDL_GetCursor());
					SDL_SetCursor(
							SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND));
				}
				if (angleFixed) {
					phoneNumber[strlen(phoneNumber)] = clickNumber + '0';
					SDL_Log("Current number is %s", phoneNumber);
					angleFixed = false;
					playDing();

				}
				while (angle > 0) {
					double dx = angle / 50;
					if (dx < 0.3)
						dx = 0.3;
					angle -= dx * speed;
					SDL_RenderCopy(render, textureButtom, NULL, &rect);
					SDL_RenderCopyEx(render, textureRoundel, NULL, NULL, angle,
							&roundelCenterPoint, SDL_FLIP_NONE);
					SDL_RenderCopy(render, texturePointer, NULL, &rect);
					SDL_RenderPresent(render);
				}
				break;
			default:
				break;
			}
		}
	}
//free resources
	Mix_FreeChunk(ding);
	Mix_FreeChunk(cala);
	Mix_Quit();
	SDL_DestroyTexture(textureButtom);
	SDL_DestroyTexture(textureRoundel);
	SDL_DestroyTexture(texturePointer);
	SDL_DestroyRenderer(render);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

