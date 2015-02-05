#pragma once

#include <map>
#include <string>

#include "MoveDefinition.h"

class Board;
class Conditions;
class Distance;
class EndOfGame;
class Game;
class PieceType;
class Player;
class TurnOrder;
class TurnRepeat;


namespace rapidxml {
	template<class Ch> class xml_node;
	template<class Ch> class xml_document;
}

struct char_cmp {
	bool operator () (const char *a, const char *b) const
	{
		return strcmp(a, b) < 0;
	}
};

typedef std::map<char*, unsigned int, char_cmp> dirLookup_t;
typedef std::pair<char*, unsigned int> dirLookupEntry_t;

class GameParser
{
public:
	Game* Parse(char *definition, std::string *svgOutput);
	GameParser() {}
	~GameParser() {}
	
private:
	bool ParseCellsAndGenerateSVG(Board *board, rapidxml::xml_node<char> *boardNode, rapidxml::xml_document<char> *svgDoc);
	bool ParseDirections(Board *board, rapidxml::xml_node<char> *dirsNode);
	bool ParsePieceTypes(rapidxml::xml_node<char> *piecesNode, rapidxml::xml_node<char> *svgDefsNode);
	char *ParsePieceType(rapidxml::xml_node<char> *piecesNode, rapidxml::xml_node<char> *svgDefsNode, PieceType *type);
	MoveDefinition *ParseMove(rapidxml::xml_node<char> *moveNode, bool isTopLevel);

	MoveDefinition *ParseMove_Slide(rapidxml::xml_node<char> *moveNode);
	MoveDefinition *ParseMove_Leap(rapidxml::xml_node<char> *moveNode);
	MoveDefinition *ParseMove_Hop(rapidxml::xml_node<char> *moveNode);
	MoveDefinition *ParseMove_Shoot(rapidxml::xml_node<char> *moveNode);
	MoveDefinition *ParseMove_MoveLike(rapidxml::xml_node<char> *moveNode);
	MoveDefinition *ParseMove_Sequence(rapidxml::xml_node<char> *moveNode);
	MoveDefinition *ParseMove_Repeat(rapidxml::xml_node<char> *moveNode);
	MoveDefinition *ParseMove_WhenPossible(rapidxml::xml_node<char> *moveNode);
	MoveDefinition *ParseMove_ReferencePiece(rapidxml::xml_node<char> *moveNode);
	Conditions *ParseConditions(rapidxml::xml_node<char> *node);
	Distance *ParseDistance(char *val);
	MoveDefinition::When_t ParseWhen(char *val);
	Player::Relationship_t ParseRelationship(char *val);
	
	bool ParsePlayers(rapidxml::xml_node<char> *playersNode, rapidxml::xml_document<char> *svgDoc);
	bool ParseRules(rapidxml::xml_node<char> *rulesNode);
	TurnOrder *ParseTurnOrder(rapidxml::xml_node<char> *node);
	bool ParseTurnRepeat(TurnRepeat *repeat, rapidxml::xml_node<char> *repeatNode);
	EndOfGame *ParseEndOfGame(rapidxml::xml_node<char> *node);
	Player *GetPlayerByName(char *name);

	unsigned int LookupDirection(char *dirName);

	unsigned int maxDirection, allDirections;
	dirLookup_t directionLookups;
	std::map<char*, std::tuple<PieceType*, char*>, char_cmp> pieceTypesByName;
	std::map<char*, Cell*, char_cmp> cellsByRef;
	std::list<std::tuple<PieceType*, char*, char*>> pieceAppearances;

	Game *game;
};

