#include <stdlib.h>
#include "raylib.h"

#define WINDOW_W 640
#define WINDOW_H 480
#define PADDING 16.0

int main(void)
{
	InitWindow(WINDOW_W, WINDOW_H, "Nardy");
	SetTargetFPS(60);
	
	/* Array starts with top-left corner and goes through board clockwise.
	Positive values means white pieces, negayive - black, zero - unoccupied.
	So on board it like :
	( )  ( )( )( )( )( )( )( )( )( )( )(15W)
	(15b)( )( )( )( )( )( )( )( )( )( )( )
	*/
	signed char board[24] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 15,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -15};
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
			const float trianglePad = (WINDOW_H / 2.0 - PADDING * 2.0) / 19.0;
			// Left
			for (int i = 0; i < 6; i++) {
				float leftmostX = PADDING + trianglePad * 3.0 * i + trianglePad;
				// Top
				Vector2 v1 = {leftmostX, PADDING};
				Vector2 v2 = {leftmostX + trianglePad, (WINDOW_H / 2) - PADDING};
				Vector2 v3 = {leftmostX + trianglePad * 2, PADDING};
				DrawTriangle(v1, v2, v3, (Color) {0x55, 0x31, 0x11, 0xFF});
				// Drawing pieces
				if (board[i] != 0) {
					Color pieceColor = board[i] > 0 ? WHITE : BLACK;
					for (int j = 0; j < abs(board[i]); j++){
						DrawCircle(leftmostX + trianglePad,
								PADDING + trianglePad + trianglePad * 2 * j,
								trianglePad, pieceColor);
					}
				}
				// Bottom
				v1.y = WINDOW_H - PADDING;
				v2.y = WINDOW_H / 2.0 + PADDING;
				v3.y = WINDOW_H - PADDING;
				DrawTriangle(v1, v3, v2, (Color) {0x55, 0x31, 0x11, 0xFF});
				if (board[i+12] != 0) {
					int temp = i + 12;
					Color pieceColor = board[temp] > 0 ? WHITE : BLACK;
					for (int j = 0; j < abs(board[temp]); j++)
						DrawCircle(leftmostX + trianglePad,
								WINDOW_H - PADDING - trianglePad - trianglePad * 2 * j,
								trianglePad, pieceColor);
				}
			}
			// Right
			for (int i = 0; i < 6; i++) {
				float leftmostX = WINDOW_H / 2.0 + PADDING + trianglePad * 3.0 * i + trianglePad;
				// Top
				Vector2 v1 = {leftmostX, PADDING};
				Vector2 v2 = {leftmostX + trianglePad, (WINDOW_H / 2) - PADDING};
				Vector2 v3 = {leftmostX + trianglePad * 2, PADDING};
				DrawTriangle(v1, v2, v3, (Color) {0x55, 0x31, 0x11, 0xFF});
				// Bottom
				v1.y = WINDOW_H - PADDING;
				v2.y = WINDOW_H / 2.0 + PADDING;
				v3.y = WINDOW_H - PADDING;
				DrawTriangle(v1, v3, v2, (Color) {0x55, 0x31, 0x11, 0xFF});
			}
			// Side bar
			DrawRectangle(WINDOW_H, 0, WINDOW_W - WINDOW_H, WINDOW_H, GRAY);
			int titlePad = (WINDOW_W - WINDOW_H - MeasureText("Nardy", 32)) / 2;
			DrawText("Nardy", WINDOW_H + titlePad, 0, 32, BLACK);
		}
		EndDrawing();
	}
	return 0;
}
