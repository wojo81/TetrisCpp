#pragma comment(lib, "user32.lib")

#include <iostream>
#include <vector>
#include <array>
#include <Windows.h>
#include <time.h>
#include <math.h>

#include "Tetromino.h"

const Position STARTING(4, -4);
HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
const int CONSOLE_WIDTH = 80;
const int CONSOLE_HEIGHT = 30;

bool game = true;

int frameCounter = 0;
const int MAX_FRAMES = 6;
const int SLEEP_TIME = 50;
const Position BOARD_OFFSET(33, 4);
const Position NEXT_DISPLAY_OFFSET(50, 8);


struct Inputs {
	bool down, right, left, rotateRight, rotateLeft, paused;

	Inputs() {
		down = right = left = rotateLeft = rotateLeft = paused = false;
	}
};

struct Rendering {
	bool renderCurrent, renderBlocks, renderNext, renderScore;
	std::vector<Position> oldBlockPositions, oldTetrominoPositions;
	std::vector<int> flickeringLines;

	Rendering() {
		renderCurrent= renderBlocks= false;
		renderNext= renderScore= true;
		oldBlockPositions = oldTetrominoPositions = std::vector<Position>();
		flickeringLines= std::vector<int>();
	}
};

struct GameParts {
	Tetromino current, next;
	std::vector<Block> blocks;
	std::array<int, HEIGHT> lineCounters;
	int score;

	GameParts() {
		score= 0;
		next = Tetromino();
		blocks = std::vector<Block>();
		lineCounters = std::array<int, HEIGHT>();
		current = Tetromino(STARTING);
	}
};



void showConsoleCursor(bool show) {
    CONSOLE_CURSOR_INFO cursorInfo;
	GetConsoleCursorInfo(console, &cursorInfo);
	cursorInfo.bVisible= false;
    SetConsoleCursorInfo(console, &cursorInfo);
}

void input(Inputs &inputs) {
	inputs.left= GetAsyncKeyState('A') & 0x8000 && !inputs.left;
	inputs.down= GetAsyncKeyState('S') & 0x8000;
    inputs.right= GetAsyncKeyState('D') & 0x8000 && !inputs.right;
	inputs.rotateLeft= GetAsyncKeyState('Q') & 0x8000 && !inputs.rotateLeft;
	inputs.rotateRight= GetAsyncKeyState('E') & 0x8000 && !inputs.rotateRight;
    inputs.paused= GetAsyncKeyState(' ') & 0x8000;
}

void checkLines(Rendering &rendering, GameParts &gameParts) {
	std::vector<int> fullLines;

	for(int i = 0; i < gameParts.lineCounters.size(); i++) {
		if(gameParts.lineCounters[i] == WIDTH) {
			fullLines.push_back(i);
		}
	}

	if(!fullLines.empty()) {
	    SetConsoleTextAttribute(console, 0);

		for(int lineNumber : fullLines) {
			for(int i = 0; i < gameParts.blocks.size(); i++) {
				if (gameParts.blocks[i].isAt(lineNumber)) {
					gameParts.blocks.erase(gameParts.blocks.begin() + i);
					gameParts.lineCounters[lineNumber]--;
					i--;
				}
				else if (gameParts.blocks[i].isHigherThan(lineNumber)) {
				    rendering.oldBlockPositions.push_back(gameParts.blocks[i]);
					gameParts.blocks[i].move(DOWN);
					gameParts.lineCounters[gameParts.blocks[i].y - 1]--;
					gameParts.lineCounters[gameParts.blocks[i].y]++;
				}
			}
		}

		switch(fullLines.size()) {
		case 1:
			gameParts.score+= 50;
			break;
		case 2:
			gameParts.score+= 150;
			break;
		case 3:
			gameParts.score+= 250;
			break;
		case 4:
			gameParts.score+= 400;
		}

		rendering.renderBlocks = true;
		rendering.renderScore= true;
		rendering.flickeringLines= fullLines;
	}
}

void endGame() {
    game= false;
}

void logic(Inputs &inputs, Rendering &rendering, GameParts &gameParts) {
    frameCounter++;

    for(Block block : gameParts.current.getBlocks()) {
        rendering.oldTetrominoPositions.push_back(block);
    }

    if(frameCounter >= MAX_FRAMES) {
        if(!gameParts.current.move(DOWN, gameParts.blocks)) {
			rendering.renderNext= true;
			if(gameParts.current.getY() >= 0) {
				for(Block block : gameParts.current.getBlocks()) {
					gameParts.blocks.push_back(block);
					gameParts.lineCounters[block.y]++;
				}
				checkLines(rendering, gameParts);

				gameParts.current= gameParts.next;
				gameParts.current.set(STARTING);

				gameParts.next= Tetromino();
			} else {
                endGame();
			}
        } else {
            rendering.renderCurrent= true;
        }
        frameCounter= 0;
    } else {
        if(inputs.rotateRight) {
            gameParts.current.rotateRight(gameParts.blocks);
			rendering.renderCurrent= true;
        } else if(inputs.rotateLeft) {
            gameParts.current.rotateLeft(gameParts.blocks);
			rendering.renderCurrent= true;
        } else if(inputs.right) {
            gameParts.current.move(RIGHT, gameParts.blocks);
			rendering.renderCurrent= true;
        } else if(inputs.left) {
            gameParts.current.move(LEFT, gameParts.blocks);
			rendering.renderCurrent= true;
		} else if(inputs.paused) {
			system("pause>nul");
		}

        if(inputs.down) {
            rendering.renderCurrent= true;
            return;
        }
    }
	Sleep(SLEEP_TIME);
}

void gotoxy(int x, int y) {
	COORD c = { x, y };
	SetConsoleCursorPosition(console, c);
}

void gotoxy(Position position) {
	gotoxy(position.x, position.y);
}

void flickerLines(std::vector<int> lineNumbers) {
	if(!lineNumbers.empty()) {
		int color = 0;
		switch (lineNumbers.size()) {
		case 1:
			color = BACKGROUND_RED | BACKGROUND_INTENSITY;
			break;
		case 2:
			color = BACKGROUND_GREEN | BACKGROUND_INTENSITY;
			break;
		case 3:
			color = BACKGROUND_BLUE | BACKGROUND_INTENSITY;
		default:
			color = BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE | BACKGROUND_INTENSITY;
		}

		for (int i = 0; i < 5; i++) {
			SetConsoleTextAttribute(console, color);
			for (int j = 0; j < 2; j++) {
				for (int lineNumber : lineNumbers) {
					gotoxy(Position(0, lineNumber) + BOARD_OFFSET);
					for (int x = 0; x < WIDTH; x++) {
						std::cout << ' ';
					}
				}
				SetConsoleTextAttribute(console, 0);
				Sleep(100);
			}
		}
	}
}

void drawBlock(Block block, Position offSet) {
    if(block.isLowerThan(-1)) {
        gotoxy(block + offSet);
        SetConsoleTextAttribute(console, block.getColor());
        std::cout << block.getCharacter();
    }
}

void drawTetromino(Tetromino tetromino, Position offSet) {
    for(Block block : tetromino.getBlocks()) {
        drawBlock(block, offSet);
    }
}

void renderBlocks(Rendering & rendering, std::vector<Block> blocks)
{
	if (rendering.renderBlocks) {
		SetConsoleTextAttribute(console, 0);
		for (Position position : rendering.oldBlockPositions) {
			if (position.isInBounds()) {
				gotoxy(position + BOARD_OFFSET);
				std::cout << ' ';
			}
		}

		for (Block block : blocks) {
			if (block.isInBounds()) {
				drawBlock(block, BOARD_OFFSET);
			}
		}
	}
}

void renderCurrent(Rendering & rendering, Tetromino current)
{
	if (rendering.renderCurrent) {
		SetConsoleTextAttribute(console, 0);
		for (Position position : rendering.oldTetrominoPositions) {
			if (position.isInBounds() && position.isLowerThan(-1)) {
				gotoxy(position + BOARD_OFFSET);
				std::cout << ' ';
			}
		}

		for (Block block : current.getBlocks()) {
			if (block.isInBounds()) {
				drawBlock(block, BOARD_OFFSET);
			}
		}
	}
}

void renderNext(Rendering &rendering, Tetromino next) {
	if(rendering.renderNext) {
		SetConsoleTextAttribute(console, 0);
		for (int y = NEXT_DISPLAY_OFFSET.y; y < NEXT_DISPLAY_OFFSET.y + 6; y++) {
			gotoxy(NEXT_DISPLAY_OFFSET.x, y);
			for (int x = NEXT_DISPLAY_OFFSET.x; x < NEXT_DISPLAY_OFFSET.x + 8; x++) {
				std::cout << ' ';
			}
		}

		for (Block block :next.getBlocks()) {
			drawBlock(block, Position(NEXT_DISPLAY_OFFSET.x + 3, NEXT_DISPLAY_OFFSET.y + 1));
		}
	}
}

void renderScore(Rendering &rendering, int score) {

}

void clearRendering(Rendering & rendering) {
	rendering.renderCurrent = false;
	rendering.renderBlocks = false;
	rendering.renderNext= false;
	rendering.renderScore= false;
	rendering.oldTetrominoPositions.clear();
	rendering.oldBlockPositions.clear();
	rendering.flickeringLines.clear();
}

void render(Rendering &rendering, GameParts gameParts) {

	flickerLines(rendering.flickeringLines);
	renderBlocks(rendering, gameParts.blocks);
	renderCurrent(rendering, gameParts.current);
	renderNext(rendering, gameParts.next);
	renderScore(rendering, gameParts.score);
	clearRendering(rendering);

	gotoxy(0, 0);
	std::cout << gameParts.score;
}

void setWindowSize() {
	HWND tempConsole = GetConsoleWindow();
	RECT r;
	GetWindowRect(tempConsole, &r);
	MoveWindow(tempConsole, r.left, r.top, 8 * CONSOLE_WIDTH + 24, 18 * CONSOLE_HEIGHT + 9, TRUE);
}

void makeBoard() {
	for (int y = 0; y < CONSOLE_HEIGHT; y++) {
		for (int x = 0; x < CONSOLE_WIDTH; x++) {
			std::cout << "#";
		}
		if (y < CONSOLE_HEIGHT - 1) {
			std::cout << std::endl;
		}
	}

	for (int y = BOARD_OFFSET.y; y < BOARD_OFFSET.y + HEIGHT; y++) {
		gotoxy(BOARD_OFFSET.x, y);
		for (int x = BOARD_OFFSET.x; x < BOARD_OFFSET.x + WIDTH; x++) {
			std::cout << ' ';
		}
	}

	for (int y = NEXT_DISPLAY_OFFSET.y; y < NEXT_DISPLAY_OFFSET.y + 6; y++) {
		gotoxy(NEXT_DISPLAY_OFFSET.x, y);
		for (int x = NEXT_DISPLAY_OFFSET.x; x < NEXT_DISPLAY_OFFSET.x + 8; x++) {
			std::cout << ' ';
		}
	}

	gotoxy(BOARD_OFFSET.x + 1, BOARD_OFFSET.y - 1);
	SetConsoleTextAttribute(console,
		FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
	std::cout << "-TETRIS-";

	gotoxy(NEXT_DISPLAY_OFFSET.x + 2, NEXT_DISPLAY_OFFSET.y - 1);
	SetConsoleTextAttribute(console,
		FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);
	std::cout << "Next";
}

void setup() {
	setWindowSize();

	showConsoleCursor(false);
	SetConsoleTextAttribute(console, FOREGROUND_BLUE | FOREGROUND_INTENSITY);

	makeBoard();

	srand(time(NULL));
}

int main() {
	setup();
	Rendering rendering;
	Inputs inputs;
	GameParts gameParts;

	while (game) {
		input(inputs);
		logic(inputs, rendering, gameParts);
		render(rendering, gameParts);
	}

    return 0;
}