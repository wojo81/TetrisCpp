#pragma once
#include <vector>
#include <array>
#include <Wincon.h>

const int WIDTH= 10;
const int HEIGHT= 20;

const int LEFT= 0;
const int DOWN= 1;
const int RIGHT= 2;

struct Position {
	int x, y;

	Position(int x, int y) : x(x), y(y) {}
	
	Position() : Position(0, 0) {}
	
	Position operator+(Position otherPosition) {
	    Position position(x + otherPosition.x, y + otherPosition.y);
	    return position;
	}
	
	bool operator==(Position other) {
		return x == other.x && y == other.y;
	}
	
	bool isHigherThan(int y) {
	    return this->y < y;
	}
	
	bool isLowerThan(int y) {
	    return this->y > y;
	}

	bool isAt(int y) {
		return this->y == y;
	}
	
	bool isInBounds() {
	    return x >= 0 && x < WIDTH && y < HEIGHT;
	}

	void set(int x, int y) {
	    this->x= x;
	    this->y= y;
	}
	
	void set(Position position) {
	    set(position.x, position.y);
	}
	
	void move(const int DIRECTION) {
		switch (DIRECTION) {
		//Left
		case 0:
			x--;
			break;
		//Down
		case 1:
			y++;
			break;
		//Right
		case 2:
			x++;
			break;
		}
	}
};

class Block : public Position {

private:
    char character;
    int color;

public:

	Block(int x, int y, char character, int color) : Position(x, y) {
	    this->character= character;
	    this->color= color;
	}

    Block(Position position) : Block(position.x, position.y, ' ', 0) {}

	Block() : Block(0, 0, ' ', 0) {}

	bool collidesWithAny(std::vector<Block> otherBlocks) {
		if(isInBounds()) {
    		for(Block otherBlock : otherBlocks) {
    			if (operator==(otherBlock)) {
    				return true;
    			}
    		}
			return false;
		}
		return true;
	}

	static bool canMove(Block block, const int DIRECTION, std::vector<Block> otherBlocks) {
		block.move(DIRECTION);
		return !block.collidesWithAny(otherBlocks);
	}

	char getCharacter() {
		return character;
	}

	int getColor() {
		return color;
	}
};

class Tetromino {

private:
	std::array<std::array<Position, 4>, 4> rotations;
	std::array<Block, 4> blocks;
	Position topLeft;
	int rotationIndex;

public:

	Tetromino(int x, int y) {
		rotationIndex= 0;
		topLeft= Position(x, y);
	    int randNum= rand() % 6;
        char character= '#';
        int color= FOREGROUND_BLUE;

		std::array<Position, 4> arr;
		switch(randNum) {
		/*

        |#       |# # #      |  # #      |
        |#       |#          |    #      |    #
        |# #     |           |    #      |# # #
        
        */
		case 0:
		    arr[0]= Position(0, 0); arr[1]= Position(0, 1);
            arr[2]= Position(0, 2); arr[3]= Position(1, 2);
            rotations[0]= arr;
            
            arr[0]= Position(0, 0); arr[1]= Position(1, 0);
            arr[2]= Position(2, 0); arr[3]= Position(0, 1);
            rotations[1]= arr;
            
            arr[0]= Position(1, 0); arr[1]= Position(2, 0);
            arr[2]= Position(2, 1); arr[3]= Position(2, 2);
            rotations[2]= arr;
            
            arr[0]= Position(2, 1); arr[1]= Position(0, 2);
            arr[2]= Position(1, 2); arr[3]= Position(2, 2);
            rotations[3]= arr;
            
            
            character= '#';
            color= BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_INTENSITY |
                   FOREGROUND_BLUE | FOREGROUND_INTENSITY;
            
            break;
            
        /*
        
		|        |  #   |# #     |    #
		|# #     |# #   |  # #   |  # # 
		|  # #   |#     |        |  #
		
		*/
        case 1:
			arr[0]= Position(0, 1); arr[1]= Position(1, 1);
            arr[2]= Position(1, 2); arr[3]= Position(2, 2);
            rotations[0]= arr;
            
            arr[0]= Position(1, 0); arr[1]= Position(0, 1);
            arr[2]= Position(1, 1); arr[3]= Position(0, 2);
            rotations[1]= arr;
            
            arr[0]= Position(0, 0); arr[1]= Position(1, 0);
            arr[2]= Position(1, 1); arr[3]= Position(2, 1);
            rotations[2]= arr;
            
            arr[0]= Position(2, 0); arr[1]= Position(1, 1);
            arr[2]= Position(2, 1); arr[3]= Position(1, 2);
            rotations[3]= arr;
            
            character= '+';
            color= BACKGROUND_BLUE | BACKGROUND_INTENSITY |
                   FOREGROUND_GREEN | FOREGROUND_INTENSITY;
            
            break;
        
        /*
        
        |        |#     |  # #  |  #
        |  # #   |# #   |# #    |  # #
        |# #     |  #   |       |    #
        
        */
        case 2:
            arr[0]= Position(1, 1); arr[1]= Position(2, 1);
            arr[2]= Position(0, 2); arr[3]= Position(1, 2);
            rotations[0]= arr;
            
            arr[0]= Position(0, 0); arr[1]= Position(0, 1);
            arr[2]= Position(1, 1); arr[3]= Position(1, 2);
            rotations[1]= arr;
            
            arr[0]= Position(1, 0); arr[1]= Position(2, 0);
            arr[2]= Position(0, 1); arr[3]= Position(1, 1);
            rotations[2]= arr;
            
            arr[0]= Position(1, 0); arr[1]= Position(1, 1);
            arr[2]= Position(2, 1); arr[3]= Position(2, 2);
            rotations[3]= arr;
            
            character= '=';
            color= BACKGROUND_RED | BACKGROUND_INTENSITY |
                   FOREGROUND_BLUE | FOREGROUND_INTENSITY;
            
            break;
            
        /*
        
        |        |#      |# # #   |    #
        |  #     |# #    |  #     |  # #
        |# # #   |#      |        |    #
        
        */
        case 3:
            arr[0]= Position(1, 1); arr[1]= Position(0, 2);
            arr[2]= Position(1, 2); arr[3]= Position(2, 2);
            rotations[0]= arr;
            
            arr[0]= Position(0, 0); arr[1]= Position(0, 1);
            arr[2]= Position(1, 1); arr[3]= Position(0, 2);
            rotations[1]= arr;
            
            arr[0]= Position(0, 0); arr[1]= Position(1, 0);
            arr[2]= Position(2, 0); arr[3]= Position(1, 1);
            rotations[2]= arr;
            
            arr[0]= Position(2, 0); arr[1]= Position(1, 1);
            arr[2]= Position(2, 1); arr[3]= Position(2, 2);
            rotations[3]= arr;
            
            character= '|';
            color= BACKGROUND_GREEN | BACKGROUND_INTENSITY |
                   FOREGROUND_BLUE | FOREGROUND_INTENSITY;
            
            break;
            
        /*
        
        |      |# #    |  # #   |
        |# #   |# #    |  # #   |  # #
        |# #   |       |        |  # #
        
        */
        
        case 4:
            arr[0]= Position(0, 1); arr[1]= Position(1, 1);
            arr[2]= Position(0, 2); arr[3]= Position(1, 2);
            rotations[0]= arr;
            
            arr[0]= Position(0, 0); arr[1]= Position(1, 0);
            arr[2]= Position(0, 1); arr[3]= Position(1, 1);
            rotations[1]= arr;
            
            arr[0]= Position(1, 0); arr[1]= Position(2, 0);
            arr[2]= Position(1, 1); arr[3]= Position(2, 1);
            rotations[2]= arr;
            
            arr[0]= Position(1, 1); arr[1]= Position(2, 1);
            arr[2]= Position(1, 2); arr[3]= Position(2, 2);
            rotations[3]= arr;
            
            character= 'X';
            color= BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_INTENSITY |
                   FOREGROUND_BLUE | FOREGROUND_INTENSITY;
            
            break;
            
        /*
        
        |  #      |          |    #     |    
        |  #      |# # # #   |    #     | 
        |  #      |          |    #     |# # # #
        |  #      |          |    #     |
        
        */
            
        case 5:
            arr[0]= Position(1, 0); arr[1]= Position(1, 1);
            arr[2]= Position(1, 2); arr[3]= Position(1, 3);
            rotations[0]= arr;
            
            arr[0]= Position(0, 1); arr[1]= Position(1, 1);
            arr[2]= Position(2, 1); arr[3]= Position(3, 1);
            rotations[1]= arr;
            
            arr[0]= Position(2, 0); arr[1]= Position(2, 1);
            arr[2]= Position(2, 2); arr[3]= Position(2, 3);
            rotations[2]= arr;
            
            arr[0]= Position(0, 2); arr[1]= Position(1, 2);
            arr[2]= Position(2, 2); arr[3]= Position(3, 2);
            rotations[3]= arr;
            
            character= '#';
            color= BACKGROUND_BLUE | BACKGROUND_RED | BACKGROUND_INTENSITY |
                   FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
            
            break;
		}
		
		for(int i= 0; i < 4; i++) {
			blocks[i]= Block(0, 0, character, color);
		}
		
		map();
	}

	Tetromino(Position position) : Tetromino(position.x, position.y) {}

	Tetromino() : Tetromino(0, 0) {}
	
	void set(int x, int y) {
	    topLeft.set(x, y);
	    map();
	}

	void set(Position position) {
		set(position.x, position.y);
	}

	int getY() {
		return topLeft.y;
	}
	
	std::array<Block, 4> getBlocks() {
	    return blocks;
	}
	
	void move(const int DIRECTION) {
	    topLeft.move(DIRECTION);
		for (int i = 0; i < blocks.size(); i++) {
			blocks[i].move(DIRECTION);
		}
	}

	bool move(const int DIRECTION, std::vector<Block> otherBlocks) {
		for (Block block : blocks) {
			if(!Block::canMove(block, DIRECTION, otherBlocks)) {
				return false;
			}
		}

        move(DIRECTION);
		return true;
	}
	
	void map() {
	    for(int i= 0; i < blocks.size(); i++) {
	        blocks[i].set(topLeft + rotations[rotationIndex][i]);
	    }
	}
	
	bool map(std::vector<Block> otherBlocks) {
	    for(Position position : rotations[rotationIndex]) {
            Block block(topLeft + position);
            if(block.collidesWithAny(otherBlocks)) {
                return false;
            }
	    }
	    
	    map();
	    return true;
	}
	
	bool rotateRight(std::vector<Block> otherBlocks) {
	    rotationIndex++;
		if(rotationIndex == rotations.size()) {
			rotationIndex= 0;
		}
	    if(!map(otherBlocks)) {
			if(rotationIndex == 0) {
				rotationIndex= rotations.size();
			}
			rotationIndex--;
	        return false;
	    }
	    return true;
	}
	
	bool rotateLeft(std::vector<Block> otherBlocks) {
		if (rotationIndex == 0) {
			rotationIndex = rotations.size();
		}
		rotationIndex--;
	    if(!map(otherBlocks)) {
			rotationIndex++;
			if (rotationIndex == rotations.size()) {
				rotationIndex = 0;
			}
	        return false;
	    }
	    return true;
	}
};
