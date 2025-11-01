#include "raylib.h"

#define WINDOW_W 640
#define WINDOW_H 480
#define PADDING 16.0

int main(void)
{
	InitWindow(WINDOW_W, WINDOW_H, "Nardy");
	SetTargetFPS(60);
	while (!WindowShouldClose()) {
		BeginDrawing();
		{
			// Board
			ClearBackground((Color){0xDA, 0x9D, 0x64, 0xFF});
			// Board borders
			DrawRectangleLinesEx((Rectangle){0.0, 0.0, (float)WINDOW_H / 2, (float)WINDOW_H},
					PADDING, (Color) {0x6F, 0x4F, 0x2F, 0xFF});
			DrawRectangleLinesEx((Rectangle){(float)WINDOW_H / 2, 0.0, (float)WINDOW_H / 2, (float)WINDOW_H},
					PADDING, (Color) {0x6F, 0x4F, 0x2F, 0xFF});
			// Board markings
			const float trianglePad = (WINDOW_H / 2.0 - PADDING * 2.0) / 24.0;
			// Top
			for (int i = 0; i < 6; i++) {
				float leftmostX = PADDING + trianglePad * 3.0 * i + trianglePad;
				DrawTriangle((Vector2) {leftmostX, PADDING},
						(Vector2) {leftmostX + trianglePad * 2, PADDING},
						(Vector2) {leftmostX + trianglePad, PADDING},
						(Color) {0x55, 0x31, 0x11, 0xFF});
			}
			// Bottom
			for (int i = 0; i < 6; i++) {}
		}
		EndDrawing();
	}
	return 0;
}
