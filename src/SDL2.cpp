#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include <string>
#include <cmath>

class DialPhone {
private:
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
public:

	void reportError(void *pointer) {
		if (pointer == nullptr) {
			SDL_Log(SDL_GetError());
		}
	}

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

	double getAngle(SDL_Point p1, SDL_Point center, SDL_Point p2) {
		double x1 = p1.x - center.x, y1 = p1.y - center.y;
		double x2 = p2.x - center.x, y2 = p2.y - center.y;
		double ans = std::acos(
				(x1 * x2 + y1 * y2) / std::hypot(x1, y1) / std::hypot(x2, y2))
				/ M_PI * 180;
		if (x1 * y2 - x2 * y1 < 0)
			ans = -ans;
		return ans;
	}

	DialPhone() {
		SDL_Init(SDL_INIT_EVERYTHING);
		IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG);
		window = SDL_CreateWindow("DiaPhone", SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED, windowWidth, windowHeight, SDL_WINDOW_SHOWN);
		reportError(window);
		render = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
		reportError(render);
		textureButtom = IMG_LoadTexture(render, "resources/buttom.png");
		textureRoundel = IMG_LoadTexture(render, "resources/roundel.png");
		texturePointer = IMG_LoadTexture(render, "resources/pointer.png");
		roundelCenterPoint.x = 368;
		roundelCenterPoint.y = 368;
	}
	void show() {
		SDL_RenderClear(render);
		SDL_RenderCopy(render, textureButtom, NULL, &rect);
		SDL_RenderCopy(render, textureRoundel, NULL, &rect);
		SDL_RenderCopy(render, texturePointer, NULL, &rect);
		SDL_RenderPresent(render);
		bool quit = false;
		int number = -1;
		int clickNumber = -1;
		bool dragging = false;
		bool mouseDown = false;
		bool angleFixed = false;
		std::string phoneNumber = "";
		double angle = 0;
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
									SDL_CreateSystemCursor(
											SDL_SYSTEM_CURSOR_HAND));
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
						double deltaAngle = getAngle(rawPoint,
								roundelCenterPoint, nowPoint);
						SDL_RenderCopy(render, textureButtom, NULL, &rect);
						SDL_RenderCopyEx(render, textureRoundel, NULL, NULL,
								angle, &roundelCenterPoint, SDL_FLIP_NONE);
						angle += deltaAngle;
						angleFixed = false;
						if (angle
								> 32 * (clickNumber == 0 ? 10 : clickNumber)) {
							angle = 32 * (clickNumber == 0 ? 10 : clickNumber);
							angleFixed = true;
						} else if (angle < 0) {
							angle = 0;
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
					SDL_Log("Mouse up on (%d,%d)", event.button.x,
							event.button.y);
					mouseDown = false;
					dragging = false;
					number = getPosition(event.button.x, event.button.y);
					if (number == -1) {
						SDL_FreeCursor(SDL_GetCursor());
						SDL_SetCursor(
								SDL_CreateSystemCursor(
										SDL_SYSTEM_CURSOR_ARROW));
					} else {
						SDL_FreeCursor(SDL_GetCursor());
						SDL_SetCursor(
								SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND));
					}
					if (angleFixed) {
						phoneNumber += (clickNumber + '0');
						std::cout << phoneNumber << std::endl;
					}
					while (angle > 0) {
						if (angle > speed) {
							double dx = angle / 80;
							if (dx < 0.3)
								dx = 0.3;
							angle -= dx * speed;
						} else {
							angle = 0;
						}
						SDL_RenderCopy(render, textureButtom, NULL, &rect);
						SDL_RenderCopyEx(render, textureRoundel, NULL, NULL,
								angle, &roundelCenterPoint, SDL_FLIP_NONE);
						SDL_RenderCopy(render, texturePointer, NULL, &rect);
						SDL_RenderPresent(render);
					}

					break;
				default:
					break;
				}
			}
		}
		SDL_Quit();
	}
	~DialPhone() {
		SDL_DestroyTexture(textureButtom);
		SDL_DestroyTexture(textureRoundel);
		SDL_DestroyTexture(texturePointer);
		SDL_DestroyRenderer(render);
		SDL_DestroyWindow(window);
	}
}
;
int main(int argc, char** argv) {
	DialPhone diaPhone;
	diaPhone.show();
}

