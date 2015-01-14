#pragma once
#include <map>

class Game;
class Piece;

typedef enum { Local, AI, Remote } PlayerType_t;
typedef enum { Any, Self, Enemy, Ally } PlayerRelationship_t;

class Player
{
public:
	Player(Game *game, int id, PlayerType_t type, int forwardDir);
	~Player();

	int ResolveDirection(int dir, int prevDir);
	PlayerRelationship_t GetRelationship(Player *other);

private:
	Game *game;
	int id, forwardDir;
	PlayerType_t type;

	std::map<int, Piece*> piecesOnBoard, piecesHeld, piecesCaptured;

	friend class MoveStep;
};

