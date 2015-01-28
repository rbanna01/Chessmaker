#pragma once

#include <map>
#include <string>

class Board;
class Game;
class PieceType;

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
	bool ParsePlayers(rapidxml::xml_node<char> *playersNode, rapidxml::xml_document<char> *svgDoc);
	bool ParseRules(rapidxml::xml_node<char> *rulesNode);
	unsigned int LookupDirection(char *dirName);

	unsigned int maxDirection, allDirections;
	dirLookup_t directionLookups;

	Game *game;
};

