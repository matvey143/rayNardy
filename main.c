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
		DrawCircle(dice.x + dice.width * 0.5f, dice.y + dice.height * 0.5f, dotRadius, BLACK);
		break;
	case 2:
		DrawCircle(dice.x + dice.width * 0.25f, dice.y + dice.height * 0.25f, dotRadius, BLACK); // Top-left
		DrawCircle(dice.x + dice.width * 0.75f, dice.y + dice.height * 0.75f, dotRadius, BLACK); // Bottom-right
		break;
	case 3:
		DrawCircle(dice.x + dice.width * 0.25f, dice.y + dice.height * 0.25f, dotRadius, BLACK); // Top-left
		DrawCircle(dice.x + dice.width * 0.5f, dice.y + dice.height * 0.5f, dotRadius, BLACK); // Middle
		DrawCircle(dice.x + dice.width * 0.75f, dice.y + dice.height * 0.75f, dotRadius, BLACK); // Bottom-right
		break;
	case 4:
		DrawCircle(dice.x + dice.width * 0.25f,	dice.y + dice.height * 0.25f, dotRadius, BLACK); // Top-left
		DrawCircle(dice.x + dice.width * 0.75f,	dice.y + dice.height * 0.25f, dotRadius, BLACK); // Top-Right
		DrawCircle(dice.x + dice.width * 0.25f,	dice.y + dice.height * 0.75f, dotRadius, BLACK); // Bottom-left
		DrawCircle(dice.x + dice.width * 0.75f,	dice.y + dice.height * 0.75f, dotRadius, BLACK); // Bottom-right
		break;
	case 5:
		DrawCircle(dice.x + dice.width * 0.25f,	dice.y + dice.height * 0.25f, dotRadius, BLACK); // Top-left
		DrawCircle(dice.x + dice.width * 0.75f, dice.y + dice.height * 0.25f, dotRadius, BLACK); // Top-Right
		DrawCircle(dice.x + dice.width * 0.5f, dice.y + dice.height * 0.5f,	dotRadius, BLACK); // Middle
		DrawCircle(dice.x + dice.width * 0.25f, dice.y + dice.height * 0.75f, dotRadius, BLACK); // Bottom-left
		DrawCircle(dice.x + dice.width * 0.75f, dice.y + dice.height * 0.75f, dotRadius, BLACK); // Bottom-right
		break;
	case 6:
		DrawCircle(dice.x + dice.width * 0.25f,	dice.y + dice.height * 0.25f, dotRadius, BLACK); // Top-left
		DrawCircle(dice.x + dice.width * 0.5f, dice.y + dice.height * 0.25f, dotRadius, BLACK); // Top-Mid
		DrawCircle(dice.x + dice.width * 0.75f,	dice.y + dice.height * 0.25f, dotRadius, BLACK); // Top-Right
		DrawCircle(dice.x + dice.width * 0.25f,	dice.y + dice.height * 0.75f, dotRadius, BLACK); // Bottom-left
		DrawCircle(dice.x + dice.width * 0.5f, dice.y + dice.height * 0.75f, dotRadius, BLACK); // Bottom-mid
		DrawCircle(dice.x + dice.width * 0.75f,	dice.y + dice.height * 0.75f, dotRadius, BLACK); // Bottom-right
		break;
	default:
		break;
	}
}

enum MarkStatus {
	MARK_UNSELECTED = 0,
	MARK_SELECTED = 1
};

struct BoardMark {
	Vector2 v1;
	Vector2 v2;
	Vector2 v3;
	Color color; // Changes when it is highlighted and to show possible turns.
	enum MarkStatus status;
};

enum Turn {
	TURN_BLACK_SELECT = 0, // For picking piece
	TURN_BLACK_MOVE = 1,
	TURN_WHITE_SELECT = 2,
	TURN_BLACK_MOVE = 3
};

#define MARK_COLOR_BASIC ((Color) {0x55, 0x31, 0x11, 0xFF})
#define MARK_COLOR_MOUSEON SKYBLUE

// Assumes 24 space board
void updateMarkPosition(struct BoardMark *markArray)
{
	const float trianglePad = (WINDOW_H / 2.0 - PADDING * 2.0) / 19.0;
	// Left
	for (int i = 0; i < 6; i++) {
		float leftmostX = PADDING + trianglePad * 3.0 * i + trianglePad;
		// Top
		markArray[i].v1 = (Vector2){leftmostX, PADDING};
		markArray[i].v2 = (Vector2){leftmostX + trianglePad, (WINDOW_H / 2) - PADDING};
		markArray[i].v3 = (Vector2){leftmostX + trianglePad * 2, PADDING};
		markArray[i].color = MARK_COLOR_BASIC;
		markArray[i].status = MARK_UNSELECTED;
		// Bottom
		markArray[23 - i].v3 = (Vector2){leftmostX, WINDOW_H - PADDING};
		markArray[23 - i].v2 = (Vector2){leftmostX + trianglePad, (WINDOW_H / 2) + PADDING};
		markArray[23 - i].v1 = (Vector2){leftmostX + trianglePad * 2, WINDOW_H - PADDING};
		markArray[23 - i].color = MARK_COLOR_BASIC;
		markArray[23 - i].status = MARK_UNSELECTED;
	}
	// Right
	for (int i = 0; i < 6; i++) {
		float leftmostX = WINDOW_H / 2.0 + PADDING + trianglePad * 3.0 * i + trianglePad;
		// Top
		markArray[i + 6].v1 = (Vector2){leftmostX, PADDING};
		markArray[i + 6].v2 = (Vector2){leftmostX + trianglePad, (WINDOW_H / 2) - PADDING};
		markArray[i + 6].v3 = (Vector2){leftmostX + trianglePad * 2, PADDING};
		markArray[i + 6].color = MARK_COLOR_BASIC;
		markArray[i + 6].status = MARK_UNSELECTED;
		// Bottom
		markArray[17 - i].v3 = (Vector2){leftmostX, WINDOW_H - PADDING};
		markArray[17 - i].v2 = (Vector2){leftmostX + trianglePad, (WINDOW_H / 2) + PADDING};
		markArray[17 - i].v1 = (Vector2){leftmostX + trianglePad * 2, WINDOW_H - PADDING};
		markArray[17 - i].color = MARK_COLOR_BASIC;
		markArray[17 - i].status = MARK_UNSELECTED;
	}
}

int main(void)
{
	srand(time(NULL));
	InitWindow(WINDOW_W, WINDOW_H, "Nardy");
	SetTargetFPS(60);

	/* Array starts with top-left corner and goes through board clockwise.
	Positive values means white pieces, negative - black, zero - unoccupied.
	So on board it like :
	(   )( )( )( )( )( )( )( )( )( )( )(15W)
	(15b)( )( )( )( )( )( )( )( )( )( )(   )
	*/
	signed char board[24] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 15,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -15};
	struct BoardMark markings[24];
	updateMarkPosition(markings);
	unsigned char dieA, dieB;
	enum Turn currentTurn = 0;
	ThrowDice(&dieA, &dieB);
	while (!WindowShouldClose()) {
		Vector2 mouseXY = GetMousePosition();
		switch (currentTurn) {
		case TURN_BLACK_SELECT:
			for (int i = 0; i < 24; i++) {
				if (markings[i].status == MARK_UNSELECTED && board[i] < 0 &&
						CheckCollisionPointTriangle(mouseXY, markings[i].v1, markings[i].v2, markings[i].v3)) {
					markings[i].color = MARK_COLOR_MOUSEON;
					markings[i].status = MARK_SELECTED;
				}
				else if (markings[i].status == MARK_SELECTED && board[i] < 0 &&
						!CheckCollisionPointTriangle(mouseXY, markings[i].v1, markings[i].v2, markings[i].v3)) {
					markings[i].color = MARK_COLOR_BASIC;
					markings[i].status = MARK_UNSELECTED;
				}
			}
			break;
		case TURN_BLACK_MOVE:
			break;
		}
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
			for (int i = 0; i < 24; i++) {
				// Top
				DrawTriangle(markings[i].v1, markings[i].v2, markings[i].v3, markings[i].color);
				// Drawing pieces
				if (board[i] != 0) {
					float pieceRadius = 6.0f;
					int x = (int) markings[i].v2.x;
					int y = (int) markings[i].v1.y;
					if (i < 12) y += pieceRadius;
					else  y -= pieceRadius;
					Color pieceColor = board[i] > 0 ? WHITE : BLACK;
					for (int j = 0; j < abs(board[i]); j++) {
						//int y = PADDING + pieceRadius + pieceRadius * 2 * j;
						DrawCircle(x, y, pieceRadius, pieceColor);
						DrawCircleLines(x, y, pieceRadius, DARKGRAY);
						if (i < 12) y += pieceRadius * 2;
						else y -= pieceRadius * 2;
					}
				}
			}
			// Side bar
			DrawRectangle(WINDOW_H, 0, WINDOW_W - WINDOW_H, WINDOW_H, GRAY);
			int titlePad = (WINDOW_W - WINDOW_H - MeasureText("Nardy", 32)) / 2;
			DrawText("Nardy", WINDOW_H + titlePad, 0, 32, BLACK);
			// Dice
			Rectangle dieLeft = {500.0f, 50.0f, 50.0f, 50.0f};
			Rectangle dieRight = {570.0f, 50.0f, 50.0f, 50.0f};
			DrawRectangleRec(dieLeft, WHITE);
			DrawRectangleLinesEx(dieLeft, 2.0f, BLACK);
			DrawDice(dieA, dieLeft);
			DrawRectangleRec(dieRight, WHITE);
			DrawRectangleLinesEx(dieRight, 2.0f, BLACK);
			DrawDice(dieB, dieRight);
		}
		EndDrawing();
	}
	return 0;
}
