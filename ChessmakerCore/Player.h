#pragma once
#include <map>

class Game;
class Piece;

#define PLAYER_NAME_LENGTH 16

class Player
{
public:
	typedef enum { Local, AI, Remote } Type_t;
	typedef enum { Any, Self, Enemy, Ally } Relationship_t;

	Player(Game *game, int id, Type_t type, int forwardDir);
	~Player();

	unsigned int ResolveDirections(unsigned int dir, unsigned int prevDir);
	Relationship_t GetRelationship(Player *other);

private:
	Game *game;
	char name[PLAYER_NAME_LENGTH];
	int id, forwardDir;
	Type_t type;

	std::map<int, Piece*> piecesOnBoard, piecesHeld, piecesCaptured;

	friend class MoveStep;
};

