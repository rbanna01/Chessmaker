#pragma once

#include "MoveConditions.h"
#include "MoveDefinition.h"

class Board;
class Distance;
class EndOfGame;
class Game;
class PieceType;
class Player;
class StateConditionGroup;
class TurnOrder;
class TurnRepeat;


namespace rapidxml {
	template<class Ch> class xml_node;
	template<class Ch> class xml_document;
}

typedef std::map<char*, unsigned int, char_cmp> dirLookup_t;
typedef std::pair<char*, unsigned int> dirLookupEntry_t;

class GameParser
{
public:
#ifdef NO_SVG
	Game* Parse(char *definition);
#else
	Game* Parse(char *definition, std::string *svgOutput);
#endif
	GameParser() {}
	~GameParser() {}
	
private:
#ifdef NO_SVG
	bool ParseCells(Board *board, rapidxml::xml_node<char> *boardNode);
	bool ParsePieceTypes(rapidxml::xml_node<char> *piecesNode);
	char *ParsePieceType(rapidxml::xml_node<char> *piecesNode, PieceType *type);
	bool ParsePlayers(rapidxml::xml_node<char> *playersNode);
#else
	bool ParseCellsAndGenerateSVG(Board *board, rapidxml::xml_node<char> *boardNode, rapidxml::xml_document<char> *svgDoc);
	bool ParsePieceTypes(rapidxml::xml_node<char> *piecesNode, rapidxml::xml_node<char> *svgDefsNode);
	char *ParsePieceType(rapidxml::xml_node<char> *piecesNode, rapidxml::xml_node<char> *svgDefsNode, PieceType *type);
	bool ParsePlayers(rapidxml::xml_node<char> *playersNode, rapidxml::xml_document<char> *svgDoc);
#endif
	bool ParseDirections(Board *board, rapidxml::xml_node<char> *dirsNode);
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
	MoveConditionGroup *ParseMoveConditions(rapidxml::xml_node<char> *node, Condition::GroupType_t type);
	StateConditionGroup *ParseStateConditions(rapidxml::xml_node<char> *node, Condition::GroupType_t type);
	Condition::NumericComparison_t ParseNumericComparison(char *value);

	Distance *ParseDistance(char *val);
	MoveDefinition::When_t ParseWhen(char *val);
	Player::Relationship_t ParseRelationship(char *val);
	
	bool ParseRules(rapidxml::xml_node<char> *rulesNode);
	TurnOrder *ParseTurnOrder(rapidxml::xml_node<char> *node);
	bool ParseTurnRepeat(TurnRepeat *repeat, rapidxml::xml_node<char> *repeatNode);
	EndOfGame *ParseEndOfGame(rapidxml::xml_node<char> *rootNode);
	Player *GetPlayerByName(char *name);

	unsigned int LookupDirection(char *dirName);

	unsigned int maxDirection, allDirections;
	dirLookup_t directionLookups;
	std::map<char*, std::tuple<PieceType*, char*>, char_cmp> pieceTypesByName;
	std::map<char*, Cell*, char_cmp> cellsByRef;
#ifndef NO_SVG
	std::list<std::tuple<PieceType*, char*, char*>> pieceAppearances;
#endif

	Game *game;
};

