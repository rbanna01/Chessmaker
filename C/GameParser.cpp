#include <stdio.h>
#include <map>
#include <utility>

#include "GameParser.h"
#include "Board.h"
#include "Cell.h"
#include "EndOfGame.h"
#include "Game.h"
#include "GameState.h"
#include "PieceType.h"
#include "TurnOrder.h"
#include "rapidxml\rapidxml.hpp"
#include "rapidxml\rapidxml_print.hpp"

using namespace rapidxml;

Game* GameParser::Parse(char *definition, std::string *svgOutput)
{
	xml_document<> doc;
	doc.parse<0>(definition); // 0 means default parse flags

	Game *game = new Game();
	game->board = new Board(game);

	maxDirection = 2;
	allDirections = 0;
	directionLookups.clear();

	xml_node<> *node = doc.first_node()->first_node("board");
	if (node == 0)
	{
		printf("Can't find \"board\" node in game definition\n");
		delete game;
		return 0;
	}
	
	xml_document<char> svgDoc;
	if (!ParseCellsAndGenerateSVG(game->board, node, &svgDoc))
	{
		delete game;
		return 0;
	}
	
	xml_node<> *defsNodes = svgDoc.first_node()->first_node("defs");
	
	node = node->next_sibling("dirs");
	if (node == 0)
	{
		printf("Can't find \"dirs\" node in game definition\n");
		delete game;
		return 0;
	}
	if (!ParseDirections(game->board, node))
	{
		delete game;
		return 0;
	}
	/*
	node = node->next_sibling("pieces");
	if (node == 0)
	{
		printf("Can't find \"pieces\" node in game definition\n");
		delete game;
		return 0;
	}
	if (!ParsePieceTypes(node))
	{
		delete game;
		return 0;
	}

	node = node->next_sibling("setup");
	if (node == 0)
	{
		printf("Can't find \"setup\" node in game definition\n");
		delete game;
		return 0;
	}
	if (!ParsePlayers(node, game, &svgDoc))
	{
		delete game;
		return 0;
	}

	node = node->next_sibling("rules");
	if (node == 0)
	{
		printf("Can't find \"rules\" node in game definition\n");
		delete game;
		return 0;
	}
	if (!ParseRules(node, game))
	{
		delete game;
		return 0;
	}
	*/
	// todo: implement this player type stuff. AIs was an array of AI objects in the view.
	/*
	// this needs enhanced to also allow for remote players
	if (typeof AIs != 'undefined')
	for (var i = 0; i < game.players.length; i++) {
	var AI = AIs[i];
	if (AI == null)
	continue;

	var player = game.players[i];
	player.type = Player.Type.AI;
	player.AI = AIs[i];

	if (i >= AIs.length - 1)
	break;
	}
	*/
	
	// write svgDoc into svgOutput
	print(std::back_inserter(*svgOutput), svgDoc, print_no_indenting);

	return game;
}


bool GameParser::ParseCellsAndGenerateSVG(Board *board, xml_node<> *boardNode, xml_document<> *svgDoc)
{
	// create SVG root node
	xml_node<> *svgRoot = svgDoc->allocate_node(node_element, "svg");
	svgDoc->append_node(svgRoot);
	
	xml_attribute<> *attr = svgDoc->allocate_attribute("xmlns", "http://www.w3.org/2000/svg");
	svgRoot->append_attribute(attr);

	attr = svgDoc->allocate_attribute("id", "render");
	svgRoot->append_attribute(attr);

	attr = boardNode->first_attribute("viewBox");
	char *value = svgDoc->allocate_string(attr->value());

	attr = svgDoc->allocate_attribute("viewBox", value);
	svgRoot->append_attribute(attr);

	xml_node<> *node = svgDoc->allocate_node(node_element, "defs");
	svgRoot->append_node(node);

	// loop through all the cell nodes
	int iCell = 0;
	node = boardNode->first_node();
	while (node != 0)
	{
		if (strcmp(node->name(), "cell") == 0)
			iCell++;
		node = node->next_sibling();
	}

	Cell *cells = new Cell[iCell];
	iCell = 0;

	std::map<char*, Cell*> cellsByRef;
	std::list<std::tuple<Cell*, int, char*>> links;

	node = boardNode->first_node();
	while (node != 0)
	{
		if (strcmp(node->name(), "cell") == 0)
		{
			attr = node->first_attribute("id");
			Cell cell = cells[iCell];

			char *ref = attr->value();
			strncpy_s(cell.reference, CELL_REF_LENGTH, ref, CELL_REF_LENGTH);
			cellsByRef.insert(std::pair<char*, Cell*>(ref, &cell));
			
			// parse cell links, and store them until all cells have been loaded, so we can resolve the names
			xml_node<> *link = node->first_node("link");
			while (link != 0)
			{
				attr = link->first_attribute("dir");
				char *dir = attr->value();

				attr = link->first_attribute("to");
				ref = attr->value();

				links.insert(links.end(), std::tuple<Cell*, int, char*>(&cell, LookupDirection(dir), ref));

				link = link->next_sibling("link");
			}

			// load cell into SVG
			xml_node<> *cellSVG = svgDoc->allocate_node(node_element, "path");
			svgRoot->append_node(cellSVG);

			ref = svgDoc->allocate_string(cell.reference);
			attr = svgDoc->allocate_attribute("id", ref);
			cellSVG->append_attribute(attr);

			// determine css class
			char *val = svgDoc->allocate_string("cell ", 28);
			attr = node->first_attribute("fill");
			strcat_s(val, 28, attr->value());

			attr = node->first_attribute("border");
			if (attr != 0)
			{
				strcat_s(val, 28, " ");
				strcat_s(val, 28, attr->value());
				strcat_s(val, 28, "Stroke");
			}
			attr = svgDoc->allocate_attribute("class", val);
			cellSVG->append_attribute(attr); // !

			// copy svg path as-is
			attr = node->first_attribute("path");
			val = svgDoc->allocate_string(attr->value());
			attr = svgDoc->allocate_attribute("d", val);
			cellSVG->append_attribute(attr); // !

			// todo: save off each cell's position. Get the first two numbers from the path.
			// "M100 60 m-20 -20" should become "100" and "60"
			/*var seg = cell.pathSegList.getItem(0);
			cell.coordX = seg.x;
			cell.coordY = seg.y;*/

			iCell++;
		}
		else if (strcmp(node->name(), "line") == 0)
		{
			xml_node<> *lineSVG = svgDoc->allocate_node(node_element, "line");
			svgRoot->append_node(lineSVG);

			attr = node->first_attribute("x1");
			char *val = svgDoc->allocate_string(attr->value());
			attr = svgDoc->allocate_attribute("x1", val);
			lineSVG->append_attribute(attr);

			attr = node->first_attribute("x2");
			val = svgDoc->allocate_string(attr->value());
			attr = svgDoc->allocate_attribute("x2", val);
			lineSVG->append_attribute(attr);

			attr = node->first_attribute("y1");
			val = svgDoc->allocate_string(attr->value());
			attr = svgDoc->allocate_attribute("y1", val);
			lineSVG->append_attribute(attr);

			attr = node->first_attribute("y2");
			val = svgDoc->allocate_string(attr->value());
			attr = svgDoc->allocate_attribute("y2", val);
			lineSVG->append_attribute(attr);

			val = svgDoc->allocate_string("detail ", 20);
			attr = node->first_attribute("color");
			strcat_s(val, 20, attr->value());
			strcat_s(val, 20, "Stroke");
			attr = svgDoc->allocate_attribute("class", val);
			lineSVG->append_attribute(attr);
        }

		node = node->next_sibling();
	}
	board->cells = cells;

	// look through links, resolve the named cell for each, and then call cell.AddLink() for each.
	while (!links.empty())
	{
		std::tuple<Cell*, int, char*> link = links.front();
		links.pop_front();

		char *ref = std::get<2>(link);
		std::map<char*, Cell*>::iterator it = cellsByRef.find(ref);
		if (it == cellsByRef.end())
		{
			// todo: report invalid link destination cell error somehow
			continue;
		}

		Cell *cell = std::get<0>(link);
		Cell *dest = it->second;
		int dir = std::get<1>(link);
		cell->AddLink(dir, dest);
	}

	board->allAbsoluteDirections = allDirections;
	return true;
}


bool GameParser::ParseDirections(Board *board, xml_node<> *dirsNode)
{
	board->firstRelativeDirection = maxDirection << 1;

	xml_node<> *node = dirsNode->first_node();
	while (node != 0)
	{
		if (strcmp(node->name(), "relative") == 0)
		{
			char *name = node->first_attribute("name")->value();
			unsigned int id = LookupDirection(name); // this should always be new
			relativeDir_t values;

			xml_node<> *child = node->first_node();
			while (child != 0)
			{
				char *from = child->first_attribute("from")->value();
				char *to = child->first_attribute("to")->value();
				
				unsigned int fromID = LookupDirection(from); // this should never be new, and also < firstRelativeDirection
				unsigned int toID = LookupDirection(to); // this should never be new, and also < firstRelativeDirection

				values.insert(relativeDirValue_t(fromID, toID));

				child = child->next_sibling();
			}

			board->relativeDirections.insert(std::pair<unsigned int, relativeDir_t>(id, values));
		}
		else if (strcmp(node->name(), "group") == 0)
		{
			char *name = node->first_attribute("name")->value();
			int dirMask = 0;

			xml_node<> *child = node->first_node();
			while (child != 0)
			{
				char *dir = child->first_attribute("dir")->value();
				dirMask |= LookupDirection(dir); // this should never be new

				child = child->next_sibling();
			}

			directionLookups.insert(dirLookupEntry_t(name, dirMask));
		}

		node = node->next_sibling();
	}

	board->lastRelativeDirection = maxDirection;

	if (board->firstRelativeDirection == board->lastRelativeDirection)
		board->lastRelativeDirection = board->lastRelativeDirection >> 1; // there are none, so ensure they don't get looped over

	return true;
}


bool GameParser::ParsePieceTypes(xml_node<> *piecesNode)
{
	printf("ParsePieceTypes is not implemented\n");
	// todo: implement this
	return false;
}


bool GameParser::ParsePlayers(xml_node<> *setupNode, Game *game, xml_document<> *svgDoc)
{
	printf("ParsePlayers is not implemented\n");
	// todo: implement this
	return false;
}


bool GameParser::ParseRules(xml_node<> *rulesNode, Game *game)
{
	printf("ParseRules is not implemented\n");
	// todo: implement this
	return false;
}


unsigned int GameParser::LookupDirection(char *dirName)
{
	dirLookup_t::iterator it = directionLookups.find(dirName);

	if (it == directionLookups.end())
	{// a new direction, add it
		maxDirection = maxDirection << 1;
		allDirections |= maxDirection;
		directionLookups.insert(dirLookupEntry_t(dirName, maxDirection));

		return maxDirection;
	}

	return it->second;
}