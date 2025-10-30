#include "raylib.h"

#define WINDOW_W 640
#define WINDOW_H 480

int main(void)
{
	InitWindow(WINDOW_W, WINDOW_H, "Nardy");
	SetTargetFPS(60);
	while (!WindowShouldClose()) {
		BeginDrawing();
		{
			// #da9d64
			ClearBackground((Color){218, 157, 100, 255});
		}
		EndDrawing();
	}
	return 0;
}
