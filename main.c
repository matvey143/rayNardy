#include <stdlib.h>
#include <time.h>
#include "raylib.h"

#define WINDOW_W 640
#define WINDOW_H 480
#define PADDING 16.0

void ThrowDice(unsigned char *dice1, unsigned char *dice2)
{
	// I do this crap only to get results for 2 dice with only 1 rand() call.
	unsigned int value = (unsigned int) rand();
	unsigned char *ptr = (unsigned char *) &value;
	*dice1 = (ptr[0] % 6) + 1;
	*dice2 = (ptr[1] % 6) + 1;
}

void DrawDice(unsigned char value, Rectangle dice)
{
	const float dotRadius = 4.0f;
	switch (value) {
	case 1:
		DrawCircle(dice.x + dice.width * 0.5f,
				dice.y + dice.height * 0.5f,
				dotRadius, BLACK);
		break;
	case 2:
		// Top-left
		DrawCircle(dice.x + dice.width * 0.25f,
				dice.y + dice.height * 0.25f,
				dotRadius, BLACK);
		// Bottom-right
		DrawCircle(dice.x + dice.width * 0.75f,
				dice.y + dice.height * 0.75f,
				dotRadius, BLACK);
		break;
	case 3:
		// Top-left
		DrawCircle(dice.x + dice.width * 0.25f,
				dice.y + dice.height * 0.25f,
				dotRadius, BLACK);
		// Middle
		DrawCircle(dice.x + dice.width * 0.5f,
				dice.y + dice.height * 0.5f,
				dotRadius, BLACK);
		// Bottom-right
		DrawCircle(dice.x + dice.width * 0.75f,
				dice.y + dice.height * 0.75f,
				dotRadius, BLACK);
		break;
	case 4:
		// Top-left
		DrawCircle(dice.x + dice.width * 0.25f,
				dice.y + dice.height * 0.25f,
				dotRadius, BLACK);
		// Top-Right
		DrawCircle(dice.x + dice.width * 0.75f,
				dice.y + dice.height * 0.25f,
				dotRadius, BLACK);
		// Bottom-left
		DrawCircle(dice.x + dice.width * 0.25f,
				dice.y + dice.height * 0.75f,
				dotRadius, BLACK);
		// Bottom-right
		DrawCircle(dice.x + dice.width * 0.75f,
				dice.y + dice.height * 0.75f,
				dotRadius, BLACK);
		break;
	case 5:
		// Top-left
		DrawCircle(dice.x + dice.width * 0.25f,
				dice.y + dice.height * 0.25f,
				dotRadius, BLACK);
		// Top-Right
		DrawCircle(dice.x + dice.width * 0.75f,
				dice.y + dice.height * 0.25f,
				dotRadius, BLACK);
		// Middle
		DrawCircle(dice.x + dice.width * 0.5f,
				dice.y + dice.height * 0.5f,
				dotRadius, BLACK);
		// Bottom-left
		DrawCircle(dice.x + dice.width * 0.25f,
				dice.y + dice.height * 0.75f,
				dotRadius, BLACK);
		// Bottom-right
		DrawCircle(dice.x + dice.width * 0.75f,
				dice.y + dice.height * 0.75f,
				dotRadius, BLACK);
		break;
	case 6:
		// Top-left
		DrawCircle(dice.x + dice.width * 0.25f,
				dice.y + dice.height * 0.25f,
				dotRadius, BLACK);
		// Top-Mid
		DrawCircle(dice.x + dice.width * 0.5f,
				dice.y + dice.height * 0.25f,
				dotRadius, BLACK);
		// Top-Right
		DrawCircle(dice.x + dice.width * 0.75f,
				dice.y + dice.height * 0.25f,
				dotRadius, BLACK);
		// Bottom-left
		DrawCircle(dice.x + dice.width * 0.25f,
				dice.y + dice.height * 0.75f,
				dotRadius, BLACK);
		// Bottom-mid
		DrawCircle(dice.x + dice.width * 0.5f,
				dice.y + dice.height * 0.75f,
				dotRadius, BLACK);
		// Bottom-right
		DrawCircle(dice.x + dice.width * 0.75f,
				dice.y + dice.height * 0.75f,
				dotRadius, BLACK);
		break;
	default:
		break;
	}
}

struct BoardMark {
	Vector2 v1;
	Vector2 v2;
	Vector3 v3;
	Color color; // Changes when it is highlighted and to show possible turns.
};

constexpr Color basicMarkColor = (Color) {0x55, 0x31, 0x11, 0xFF};

int main(void)
{
	srand(time(NULL));
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
	unsigned char dieA, dieB;
	ThrowDice(&dieA, &dieB);
	while (!WindowShouldClose()) {
		// Too much stuff is happening in rendering phase. It is probably best
		// to move some stuff outside of it
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
			const float pieceRadius = trianglePad * 0.60;
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
					for (int j = 0; j < abs(board[i]); j++) {
						int x = leftmostX + trianglePad;
						int y = PADDING + pieceRadius + pieceRadius * 2 * j;
						DrawCircle(x, y, pieceRadius, pieceColor);
					}
				}
			// Bottom
				v1.y = WINDOW_H - PADDING;
				v2.y = WINDOW_H / 2.0 + PADDING;
				v3.y = WINDOW_H - PADDING;
				DrawTriangle(v1, v3, v2, (Color) {0x55, 0x31, 0x11, 0xFF});
				// Drawing pieces
				if (board[23-i] != 0) {
					int temp = 23 - i;
					Color pieceColor = board[temp] > 0 ? WHITE : BLACK;
					for (int j = 0; j < abs(board[temp]); j++) {
						int x = leftmostX + trianglePad;
						int y = WINDOW_H - PADDING - pieceRadius - pieceRadius * 2 * j;
						DrawCircle(x, y, pieceRadius, pieceColor);
					}
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
				// Drawing pieces
				if (board[i+6] != 0) {
					int temp = i + 6;
					Color pieceColor = board[temp] > 0 ? WHITE : BLACK;
					for (int j = 0; j < abs(board[temp]); j++) {
						int x = leftmostX + trianglePad;
						int y = PADDING + pieceRadius + pieceRadius * 2 * j;
						DrawCircle(x, y, pieceRadius, pieceColor);
					}
				}
				// Bottom
				v1.y = WINDOW_H - PADDING;
				v2.y = WINDOW_H / 2.0 + PADDING;
				v3.y = WINDOW_H - PADDING;
				DrawTriangle(v1, v3, v2, (Color) {0x55, 0x31, 0x11, 0xFF});
				// Drawing pieces
				if (board[17-i] != 0) {
					int temp = 17 - i;
					Color pieceColor = board[temp] > 0 ? WHITE : BLACK;
					for (int j = 0; j < abs(board[temp]); j++) {
						int x = leftmostX + trianglePad;
						int y = WINDOW_H - PADDING - pieceRadius - pieceRadius * 2 * j;
						DrawCircle(x, y, pieceRadius, pieceColor);
					}
				}
			}

			// Side bar
			DrawRectangle(WINDOW_H, 0, WINDOW_W - WINDOW_H, WINDOW_H, GRAY);
			int titlePad = (WINDOW_W - WINDOW_H - MeasureText("Nardy", 32)) / 2;
			DrawText("Nardy", WINDOW_H + titlePad, 0, 32, BLACK);
			// Dice
			// Left
			Rectangle dieLeft = {500.0f, 50.0f, 50.0f, 50.0f};
			Rectangle dieRight = {570.0f, 50.0f, 50.0f, 50.0f};
			DrawRectangleRec(dieLeft, WHITE);
			DrawRectangleLinesEx(dieLeft, 2.0f, BLACK);
			DrawDice(dieA, dieLeft);
			// Here should be witch case for visualing dice value
			DrawRectangleRec(dieRight, WHITE);
			DrawRectangleLinesEx(dieRight, 2.0f, BLACK);
			DrawDice(dieB, dieRight);
		}
		EndDrawing();
	}
	return 0;
}
