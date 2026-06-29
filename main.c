// TODO: implement minor rules;
// TODO: implement aborting turns when no move is possible;
// TODO: implement win condition;
// TODO: implement window resizing;
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
	if (*dice1 == *dice2) {
		*dice1 = (*dice1 << 4) + *dice1;
		*dice2 = (*dice2 << 4) + *dice2;
	}
}

void DrawDie(unsigned char value, Rectangle die)
{
	const float dotRadius = 4.0f;
	switch (value & 0b1111) {
	case 1:
		DrawCircle(die.x + die.width * 0.50f, die.y + die.height * 0.50f, dotRadius, BLACK);
		break;
	case 2:
		DrawCircle(die.x + die.width * 0.25f, die.y + die.height * 0.25f, dotRadius, BLACK); // Top-left
		DrawCircle(die.x + die.width * 0.75f, die.y + die.height * 0.75f, dotRadius, BLACK); // Bottom-right
		break;
	case 3:
		DrawCircle(die.x + die.width * 0.25f, die.y + die.height * 0.25f, dotRadius, BLACK); // Top-left
		DrawCircle(die.x + die.width * 0.50f, die.y + die.height * 0.50f, dotRadius, BLACK); // Middle
		DrawCircle(die.x + die.width * 0.75f, die.y + die.height * 0.75f, dotRadius, BLACK); // Bottom-right
		break;
	case 4:
		DrawCircle(die.x + die.width * 0.25f, die.y + die.height * 0.25f, dotRadius, BLACK); // Top-left
		DrawCircle(die.x + die.width * 0.75f, die.y + die.height * 0.25f, dotRadius, BLACK); // Top-Right
		DrawCircle(die.x + die.width * 0.25f, die.y + die.height * 0.75f, dotRadius, BLACK); // Bottom-left
		DrawCircle(die.x + die.width * 0.75f, die.y + die.height * 0.75f, dotRadius, BLACK); // Bottom-right
		break;
	case 5:
		DrawCircle(die.x + die.width * 0.25f, die.y + die.height * 0.25f, dotRadius, BLACK); // Top-left
		DrawCircle(die.x + die.width * 0.75f, die.y + die.height * 0.25f, dotRadius, BLACK); // Top-Right
		DrawCircle(die.x + die.width * 0.50f, die.y + die.height * 0.50f, dotRadius, BLACK); // Middle
		DrawCircle(die.x + die.width * 0.25f, die.y + die.height * 0.75f, dotRadius, BLACK); // Bottom-left
		DrawCircle(die.x + die.width * 0.75f, die.y + die.height * 0.75f, dotRadius, BLACK); // Bottom-right
		break;
	case 6:
		DrawCircle(die.x + die.width * 0.25f, die.y + die.height * 0.25f, dotRadius, BLACK); // Top-left
		DrawCircle(die.x + die.width * 0.50f, die.y + die.height * 0.25f, dotRadius, BLACK); // Top-Mid
		DrawCircle(die.x + die.width * 0.75f, die.y + die.height * 0.25f, dotRadius, BLACK); // Top-Right
		DrawCircle(die.x + die.width * 0.25f, die.y + die.height * 0.75f, dotRadius, BLACK); // Bottom-left
		DrawCircle(die.x + die.width * 0.50f, die.y + die.height * 0.75f, dotRadius, BLACK); // Bottom-mid
		DrawCircle(die.x + die.width * 0.75f, die.y + die.height * 0.75f, dotRadius, BLACK); // Bottom-right
		break;
	default:
		break;
	}
}

enum MarkStatus {
	MARK_IDLE = 0,
	MARK_MOUSEON = 1,
	MARK_MOUSEDOWN = 2,
	MARK_SELECTED = 3,
	MARK_LEGAL = 4,
	MARK_ILLEGAL = 5
};

struct BoardMark {
	Vector2 v1;
	Vector2 v2;
	Vector2 v3;
	signed char pieces; // Negative value - black pieces, positive - white, zero - unoccupied.
	enum MarkStatus status;
};

enum Turn {
	TURN_BLACK_SELECT = 0, // For picking piece
	TURN_BLACK_MOVE = 1,
	TURN_BLACK_CHECKING = 2,
	TURN_BLACK_ABORTING = 3, // No legal move is possible, passing turn.
	TURN_WHITE_SELECT = 4,
	TURN_WHITE_MOVE = 5,
	TURN_WHITE_CHECKING = 6,
	TURN_WHITE_ABORTING = 7
};

#define COLOR_MARK_BASIC ((Color) {0x55, 0x31, 0x11, 0xFF})
#define COLOR_MARK_MOUSEON SKYBLUE
#define COLOR_MARK_MOUSEDOWN DARKBLUE
#define COLOR_MARK_SELECTED BLUE
#define COLOR_MARK_LEGAL GREEN
#define COLOR_MARK_ILLEGAL RED

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
		markArray[i].status = MARK_IDLE;
		// Bottom
		markArray[23 - i].v3 = (Vector2){leftmostX, WINDOW_H - PADDING};
		markArray[23 - i].v2 = (Vector2){leftmostX + trianglePad, (WINDOW_H / 2) + PADDING};
		markArray[23 - i].v1 = (Vector2){leftmostX + trianglePad * 2, WINDOW_H - PADDING};
		markArray[23 - i].status = MARK_IDLE;
	}
	// Right
	for (int i = 0; i < 6; i++) {
		float leftmostX = WINDOW_H / 2.0 + PADDING + trianglePad * 3.0 * i + trianglePad;
		// Top
		markArray[i + 6].v1 = (Vector2){leftmostX, PADDING};
		markArray[i + 6].v2 = (Vector2){leftmostX + trianglePad, (WINDOW_H / 2) - PADDING};
		markArray[i + 6].v3 = (Vector2){leftmostX + trianglePad * 2, PADDING};
		markArray[i + 6].status = MARK_IDLE;
		// Bottom
		markArray[17 - i].v3 = (Vector2){leftmostX, WINDOW_H - PADDING};
		markArray[17 - i].v2 = (Vector2){leftmostX + trianglePad, (WINDOW_H / 2) + PADDING};
		markArray[17 - i].v1 = (Vector2){leftmostX + trianglePad * 2, WINDOW_H - PADDING};
		markArray[17 - i].status = MARK_IDLE;
	}
}

// Will be adjusted in future to account for endgame.
int WhiteOffset(unsigned char die, int position)
{
	if (position - die > 0) return position - die;
	else return 24 + (position - die);
}

// Legal move check should occur every time enum Turn is about to be set to select phase.
bool LegalMoveCheckBlack(struct BoardMark *board, unsigned char rawDieA, unsigned char rawDieB)
{
	unsigned int dieA = rawDieA & 0b1111, dieB = rawDieB & 0b1111;
	for (int i = 0; i < 24; i++) {
		if (board[i].pieces < 0 && board[i].status != MARK_ILLEGAL) {
			if (i - dieA >= 0 && board[i - dieA].pieces <= 0 && dieA != 0) return true;
			if (i - dieB >= 0 && board[i - dieB].pieces <= 0 && dieB != 0) return true;
			// Endgame checks will be here
		}
	}
	return false;
}

bool LegalMoveCheckWhite(struct BoardMark *board, unsigned char rawDieA, unsigned char rawDieB)
{
	unsigned int dieA = rawDieA & 0b1111, dieB = rawDieB & 0b1111;
	for (int i = 0; i < 24; i++) {
		if (board[i].pieces > 0 && board[i].status != MARK_ILLEGAL) {
			int posA = WhiteOffset(dieA, i), posB = WhiteOffset(dieB, i);
			if (dieA != 0 && !(i > 12 && (i - dieA) < 12) && board[posA].pieces >= 0) return true;
			if (dieB != 0 && !(i > 12 && (i - dieB) < 12) && board[posB].pieces >= 0) return true;
			// Endgame checks will be here.
		}
	}
	return false;
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
	struct BoardMark markings[24];
	for (int i = 0; i < 24; i++) {
		markings[i].pieces = 0;
	}
	markings[11].pieces = 15;
	markings[23].pieces = -15;

	Color statusColor[6];
	statusColor[MARK_IDLE] = COLOR_MARK_BASIC;
	statusColor[MARK_MOUSEON] = COLOR_MARK_MOUSEON;
	statusColor[MARK_MOUSEDOWN] = COLOR_MARK_MOUSEDOWN;
	statusColor[MARK_SELECTED] = COLOR_MARK_SELECTED;
	statusColor[MARK_LEGAL] = COLOR_MARK_LEGAL;
	statusColor[MARK_ILLEGAL] = COLOR_MARK_ILLEGAL;
	updateMarkPosition(markings);

	unsigned char dieA, dieB;
	int dieAPos, dieBPos;
	ThrowDice(&dieA, &dieB);

	int selectedMark;
	enum Turn currentTurn = 0;

	float abortTimer; // For banner, notifying that no turn is possible. After some time turn is passed.
	while (!WindowShouldClose()) {
		Vector2 mouseXY = GetMousePosition();
		float frameTime = GetFrameTime();
		switch (currentTurn) {
		case TURN_BLACK_SELECT:
			if (!dieA && !dieB) {
				currentTurn = TURN_WHITE_CHECKING;
				ThrowDice(&dieA, &dieB);
				break;
			}
			for (int i = 0; i < 24; i++) {
				if (markings[i].pieces < 0 && CheckCollisionPointTriangle(mouseXY, markings[i].v1, markings[i].v2, markings[i].v3)) {
					if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) markings[i].status = MARK_MOUSEDOWN;
					else if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
						currentTurn = TURN_BLACK_MOVE;
						markings[i].status = MARK_SELECTED;
						selectedMark = i;
						if (dieA != 0) {
							if (markings[i - (dieA & 0b1111)].pieces > 0) markings[i - (dieA & 0b1111)].status = MARK_ILLEGAL;
							else markings[i - (dieA & 0b1111)].status = MARK_LEGAL;
						}
						if (dieB != 0) {
							if (markings[i - (dieB & 0b1111)].pieces > 0) markings[i - (dieB & 0b1111)].status = MARK_ILLEGAL;
							else markings[i - (dieB & 0b1111)].status = MARK_LEGAL;
						}
						break;
					}
					else markings[i].status = MARK_MOUSEON; // Mouse on
				}
				else markings[i].status = MARK_IDLE;
			}
			break;
		case TURN_BLACK_MOVE:
			dieAPos = selectedMark - (dieA & 0b1111), dieBPos = selectedMark - (dieB & 0b1111);
			if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
				// Die 1
				if (dieA && markings[dieAPos].status == MARK_LEGAL &&
						CheckCollisionPointTriangle(mouseXY, markings[dieAPos].v1, markings[dieAPos].v2, markings[dieAPos].v3)) {
					markings[selectedMark].pieces++;
					markings[dieAPos].pieces--;
					dieA >>= 4;
					markings[selectedMark].status = MARK_IDLE;
					markings[dieAPos].status = MARK_IDLE;
					markings[dieBPos].status = MARK_IDLE;
					currentTurn = TURN_BLACK_CHECKING;
				}
				// Die 2
				else if (dieB && markings[dieBPos].status == MARK_LEGAL &&
						CheckCollisionPointTriangle(mouseXY, markings[dieBPos].v1, markings[dieBPos].v2, markings[dieBPos].v3)) {
					markings[selectedMark].pieces++;
					markings[dieBPos].pieces--;
					dieB >>= 4;
					markings[selectedMark].status = MARK_IDLE;
					markings[dieAPos].status = MARK_IDLE;
					markings[dieBPos].status = MARK_IDLE;
					currentTurn = TURN_BLACK_CHECKING;
				}
				// Cancel movement phase
				else {
					markings[selectedMark].status = MARK_IDLE;
					markings[dieAPos].status = MARK_IDLE;
					markings[dieBPos].status = MARK_IDLE;
					currentTurn = TURN_BLACK_CHECKING;
				}
			}
			break;
		case TURN_BLACK_CHECKING:
			if (LegalMoveCheckBlack(markings, dieA, dieB))
				currentTurn = TURN_BLACK_SELECT;
			else {
				const float resetTime = 1.0f;
				abortTimer = resetTime;
				currentTurn = TURN_BLACK_ABORTING;
			}
			break;
		case TURN_BLACK_ABORTING:
			abortTimer -= frameTime;
			if (abortTimer <= 0.0f) {
				ThrowDice(&dieA, &dieB);
				currentTurn = TURN_WHITE_CHECKING;
			}
			break;
		case TURN_WHITE_SELECT:
			if (!dieA && !dieB) {
				currentTurn = TURN_BLACK_CHECKING;
				ThrowDice(&dieA, &dieB);
				break;
			}
			for (int i = 0; i < 24; i++) {
				if (markings[i].pieces > 0 && CheckCollisionPointTriangle(mouseXY, markings[i].v1, markings[i].v2, markings[i].v3)) {
					if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) markings[i].status = MARK_MOUSEDOWN;
					else if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
						dieAPos = WhiteOffset(dieA & 0b1111, i), dieBPos = WhiteOffset(dieB & 0b1111, i);
						currentTurn = TURN_WHITE_MOVE;
						markings[i].status = MARK_SELECTED;
						selectedMark = i;
						if (dieA != 0 && !(i > 12 && (i - dieA) < 12)) { //Second part is needed to ensure white pieces don't loop.
							if (markings[dieAPos].pieces < 0) markings[dieAPos].status = MARK_ILLEGAL;
							else markings[dieAPos].status = MARK_LEGAL;
						}
						if (dieB != 0 && !(i > 12 && (i - dieB) < 12)) {
							if (markings[dieBPos].pieces < 0) markings[dieBPos].status = MARK_ILLEGAL;
							else markings[dieBPos].status = MARK_LEGAL;
						}
						break;
					}
					else markings[i].status = MARK_MOUSEON; // Mouse on
				}
				else markings[i].status = MARK_IDLE;
			}
			break;
		case TURN_WHITE_MOVE:
			if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
				// Die 1
				if (dieA && markings[dieAPos].status == MARK_LEGAL &&
						CheckCollisionPointTriangle(mouseXY, markings[dieAPos].v1, markings[dieAPos].v2, markings[dieAPos].v3)) {
					markings[selectedMark].pieces--;
					markings[dieAPos].pieces++;
					dieA >>= 4;
					markings[selectedMark].status = MARK_IDLE;
					markings[dieAPos].status = MARK_IDLE;
					markings[dieBPos].status = MARK_IDLE;
					currentTurn = TURN_WHITE_CHECKING;
				}
				// Die 2
				else if (dieB && markings[dieBPos].status == MARK_LEGAL &&
						CheckCollisionPointTriangle(mouseXY, markings[dieBPos].v1, markings[dieBPos].v2, markings[dieBPos].v3)) {
					markings[selectedMark].pieces--;
					markings[dieBPos].pieces++;
					dieB >>= 4;
					markings[selectedMark].status = MARK_IDLE;
					markings[dieAPos].status = MARK_IDLE;
					markings[dieBPos].status = MARK_IDLE;
					currentTurn = TURN_WHITE_CHECKING;
				}
				else {
					markings[selectedMark].status = MARK_IDLE;
					markings[dieAPos].status = MARK_IDLE;
					markings[dieBPos].status = MARK_IDLE;
					currentTurn = TURN_WHITE_CHECKING;
				}
			}
			break;
		case TURN_WHITE_CHECKING:
			if (LegalMoveCheckWhite(markings, dieA, dieB))
				currentTurn = TURN_WHITE_SELECT;
			else {
				const float resetTime = 1.0f;
				abortTimer = resetTime;
				currentTurn = TURN_WHITE_ABORTING;
			}
			break;
		case TURN_WHITE_ABORTING:
			abortTimer -= frameTime;
			if (abortTimer <= 0.0f) {
				ThrowDice(&dieA, &dieB);
				currentTurn = TURN_BLACK_CHECKING;
			}
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
				DrawTriangle(markings[i].v1, markings[i].v2, markings[i].v3, statusColor[markings[i].status]);
				// Drawing pieces
				if (markings[i].pieces != 0) {
					float pieceRadius = 6.0f;
					int x = (int) markings[i].v2.x;
					int y = (int) markings[i].v1.y;
					if (i < 12) y += pieceRadius;
					else  y -= pieceRadius;
					Color pieceColor = markings[i].pieces > 0 ? WHITE : BLACK;
					for (int j = 0; j < abs(markings[i].pieces); j++) {
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
			int turnPad;
			if (currentTurn < TURN_WHITE_SELECT) { // Black turn
				turnPad = (WINDOW_W - WINDOW_H - MeasureText("Black turn", 24)) / 2;
				DrawText("Black turn", WINDOW_H + turnPad, 120, 24, BLACK);
			}
			else {
				turnPad = (WINDOW_W - WINDOW_H - MeasureText("White turn", 24)) / 2;
				DrawText("White turn", WINDOW_H + turnPad, 120, 24, BLACK);
			}
			// Dice
			Rectangle dieLeft = {500.0f, 50.0f, 50.0f, 50.0f};
			Rectangle dieRight = {570.0f, 50.0f, 50.0f, 50.0f};
			DrawRectangleRec(dieLeft, WHITE);
			DrawRectangleLinesEx(dieLeft, 2.0f, BLACK);
			DrawDie(dieA, dieLeft);
			DrawRectangleRec(dieRight, WHITE);
			DrawRectangleLinesEx(dieRight, 2.0f, BLACK);
			DrawDie(dieB, dieRight);
			if (currentTurn == TURN_WHITE_ABORTING || currentTurn == TURN_BLACK_ABORTING) {
				// Pop-up window will be here.
				int boxX = WINDOW_W / 4, boxY = WINDOW_H / 4;
				int boxW = WINDOW_W / 2, boxH = WINDOW_H / 2;
				DrawRectangle(boxX, boxY, boxW, boxH, RED);
				int messagePad = (boxX - MeasureText("Passing turn", 32)) / 2;
				DrawText("Passing turn", boxX + messagePad, boxY + 40, 32, YELLOW);
				int messagePad2 = (boxX - MeasureText("No legal move is possible.", 24)) / 2;
				DrawText("No legal move is possible.", boxX + messagePad2, boxY + 80, 24, WHITE);
			}
		}
		EndDrawing();
	}
	return 0;
}
