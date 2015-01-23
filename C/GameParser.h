#pragma once

#include <string>

class Board;
class Game;

namespace rapidxml {
	template<class Ch> class xml_node;
	template<class Ch> class xml_document;
}

class GameParser
{
public:
	static Game* Parse(char *definition, std::string *svgOutput);
	
private:
	GameParser() {}
	~GameParser() {}

	static bool ParseCellsAndGenerateSVG(Board *board, rapidxml::xml_node<char> *boardNode, rapidxml::xml_document<char> *svgDoc);
	static bool ParseDirections(Board *board, rapidxml::xml_node<char> *dirsNode);
	static bool ParsePieceTypes(rapidxml::xml_node<char> *piecesNode);
	static bool ParsePlayers(rapidxml::xml_node<char> *playersNode, Game *game, rapidxml::xml_document<char> *svgDoc);
	static bool ParseRules(rapidxml::xml_node<char> *rulesNode, Game *game);
	static int LookupDirection(char *dirName);
};

