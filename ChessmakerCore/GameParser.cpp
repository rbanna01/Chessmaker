#include <stdio.h>
#include <map>
#include <utility>

#include "GameParser.h"
#include "Board.h"
#include "Cell.h"
#include "Conditions.h"
#include "Distance.h"
#include "EndOfGame.h"
#include "Game.h"
#include "GameState.h"
#include "MoveDefinition.h"
#include "Piece.h"
#include "PieceType.h"
#include "Player.h"
#include "TurnOrder.h"
#include "rapidxml\rapidxml.hpp"
#include "rapidxml\rapidxml_print.hpp"

using namespace rapidxml;

Game* GameParser::Parse(char *definition, std::string *svgOutput)
{
	xml_document<> doc;
	doc.parse<0>(definition); // 0 means default parse flags

	game = new Game();
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
	
	node = node->next_sibling("pieces");
	if (node == 0)
	{
		printf("Can't find \"pieces\" node in game definition\n");
		delete game;
		return 0;
	}
	if (!ParsePieceTypes(node, defsNodes))
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
	if (!ParsePlayers(node, &svgDoc))
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
	if (!ParseRules(node))
	{
		delete game;
		return 0;
	}
	
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


struct TempCellLink
{
public:
	TempCellLink(Cell *c, unsigned int d, char *r)
	{
		fromCell = c;
		direction = d;
		destinationCellRef = r;
	}

	Cell *fromCell;
	unsigned int direction;
	char* destinationCellRef;
};


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

	cellsByRef.clear();
	std::list<TempCellLink> links;

	node = boardNode->first_node();
	while (node != 0)
	{
		if (strcmp(node->name(), "cell") == 0)
		{
			attr = node->first_attribute("id");
			Cell *cell = &cells[iCell++];

			char *ref = attr->value();
			strncpy(cell->reference, ref, CELL_REF_LENGTH);
			cellsByRef.insert(std::pair<char*, Cell*>(ref, cell));
			
			// parse cell links, and store them until all cells have been loaded, so we can resolve the names
			xml_node<> *link = node->first_node("link");
			while (link != 0)
			{
				attr = link->first_attribute("dir");
				char *dir = attr->value();

				attr = link->first_attribute("to");
				ref = attr->value();

				links.push_back(TempCellLink(cell, LookupDirection(dir), ref));

				link = link->next_sibling("link");
			}

			// load cell into SVG
			xml_node<> *cellSVG = svgDoc->allocate_node(node_element, "path");
			svgRoot->append_node(cellSVG);

			ref = svgDoc->allocate_string(cell->reference);
			attr = svgDoc->allocate_attribute("id", ref);
			cellSVG->append_attribute(attr);

			// determine css class
			char *val = svgDoc->allocate_string("cell ", 28);
			attr = node->first_attribute("fill");
			strcat(val, attr->value());

			attr = node->first_attribute("border");
			if (attr != 0)
			{
				strcat(val, " ");
				strcat(val, attr->value());
				strcat(val, "Stroke");
			}
			attr = svgDoc->allocate_attribute("class", val);
			cellSVG->append_attribute(attr); // !

			// copy svg path as-is
			attr = node->first_attribute("path");
			val = svgDoc->allocate_string(attr->value());
			attr = svgDoc->allocate_attribute("d", val);
			cellSVG->append_attribute(attr); // !

			// save off each cell's position. Get the first two numbers from the path.
			// "M100 60 m-20 -20" should become "100" and "60"
			char *firstSpace = strchr(val, ' ');
			cell->coordX = atoi(val + 1);
			cell->coordY = atoi(firstSpace + 1);
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
			strcat(val, attr->value());
			strcat(val, "Stroke");
			attr = svgDoc->allocate_attribute("class", val);
			lineSVG->append_attribute(attr);
        }

		node = node->next_sibling();
	}
	board->cells = cells;

	// look through links, resolve the named cell for each, and then call cell.AddLink() for each.
	while (!links.empty())
	{
		TempCellLink link = links.front();
		links.pop_front();
		
		auto it = cellsByRef.find(link.destinationCellRef);
		if (it == cellsByRef.end())
		{
			// todo: report invalid link destination cell error somehow
			continue;
		}

		Cell *dest = it->second;
		link.fromCell->AddLink(link.direction, dest);
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

bool GameParser::ParsePieceTypes(xml_node<> *piecesNode, xml_node<> *svgDefsNode)
{
	pieceTypesByName.clear();

	// parse each piece type
	xml_node<> *node = piecesNode->first_node("piece");
	while (node != 0)
	{
		PieceType *type = new PieceType();
		char *capturedAs = ParsePieceType(node, svgDefsNode, type);
		
		auto info = std::make_tuple(type, capturedAs);
		pieceTypesByName.insert(std::make_pair(type->name, info));
		
		node = node->next_sibling("piece");
	}

	// resolve references to other types, also populate piece type list
	for (auto it = pieceTypesByName.begin(); it != pieceTypesByName.end(); it++)
	{
		PieceType *type = std::get<0>(it->second);
		game->allPieceTypes.push_back(type);

		char *capturedAs = std::get<1>(it->second);
		if (capturedAs != 0)
		{
			auto it2 = pieceTypesByName.find(capturedAs);
			if (it2 == pieceTypesByName.end())
			{
				// todo: report error somehow - captured as type is not defined
			}
			else
				type->capturedAs = std::get<0>(it2->second); 
		}

		/*
		todo: resolve promotion opportunity options
		for (var i = 0; i < type.promotionOpportunities.length; i++)
			type.promotionOpportunities[i].resolveOptions(definitions);
		*/
	}
	
	return true;
}


char *GameParser::ParsePieceType(xml_node<> *pieceNode, xml_node<> *svgDefsNode, PieceType *type)
{
	xml_attribute<> *attr = pieceNode->first_attribute("name");
	strncpy(type->name, attr->value(), TYPE_NAME_LENGTH);

	attr = pieceNode->first_attribute("notation");
	if (attr != 0)
		strncpy(type->notation, attr->value(), TYPE_NOTATION_LENGTH);

	attr = pieceNode->first_attribute("value");
	if (attr != 0)
		type->value = atoi(attr->value());

	xml_document<> *svgDoc = svgDefsNode->document();
	char *capturedAs = 0;

	xml_node<> *node = pieceNode->first_node();
	while (node != 0)
	{
		if (strcmp(node->name(), "capturedAs") == 0)
		{
			capturedAs = node->value();
		}
		else if (strcmp(node->name(), "moves") == 0)
		{
			xml_node<> *moveNode = node->first_node();
			while (moveNode != 0)
			{
				MoveDefinition *move = ParseMove(moveNode, true);
				if (move != 0)
					type->moves.push_back(move);

				moveNode = moveNode->next_sibling();
			}
		}/*
		else if (strcmp(node->name(), "special") == 0)
		{
			var specials = childNode.childNodes;
			for (var j = 0; j<specials.length; j++)
				var special = specials[j];

			if (special.tagName == "royal") // consider: while these properties should remain on pieces IN CODE (for game logic's sake) - shouldn't royalty in the DEFINITION be handled via victory conditions? lose when any/all pieces of given type are checkmated/captured/are in check/aren't in check? loading code could then apply royal / antiroyal values
				type.royalty = PieceType.RoyalState.Royal;
			else if (special.tagName == "anti_royal")
				type.royalty = PieceType.RoyalState.AntiRoyal;
			else if (special.tagName == "immobilize")
				type.immobilizations.push(Immobilization.parse(this));
			else // consider: other special types: blocks (as per immobilize, but instead prevents pieces entering a square), kills (kills pieces in target squares without expending a move)
				throw "Unexpected node name in piece's \"special\" tag: " + this.tagName;;
		}
		else if (strcmp(node->name(), "promotion") == 0)
		{
			var promos = childNode.childNodes;
			for (var j = 0; j<promos.length; j++)
				type.promotionOpportunities.push(PromotionOpportunity.parse(promos[j]);
		}*/
		else if (strcmp(node->name(), "appearance") == 0)
		{
			char *playerName = node->first_attribute("player")->value();

			char *defID = svgDoc->allocate_string(type->name, TYPE_NAME_LENGTH + PLAYER_NAME_LENGTH + 1);
			strcat(defID, "_");
			strcat(defID, playerName);

			xml_node<> *def = svgDoc->allocate_node(node_element, "g");
			svgDefsNode->append_node(def);

			attr = svgDoc->allocate_attribute("class", "piece");
			def->append_attribute(attr);
			
			attr = svgDoc->allocate_attribute("id", defID);
			def->append_attribute(attr);

			attr = node->first_attribute("transform");
			if (attr != 0)
			{
				char *trans = attr->value();
				attr = svgDoc->allocate_attribute("transform", trans);
				def->append_attribute(attr);
			}
			
			// move the piece appearance SVG into the def node
			xml_node<> *appNode = node->first_node();
			while (appNode != 0)
			{
				xml_node<> *moveNode = appNode;
				appNode = appNode->next_sibling();
				node->remove_node(moveNode);

				def->append_node(moveNode);
			}
			
			// save off this appearance, to store later once players are imported
			pieceAppearances.push_back(std::make_tuple(type, playerName, defID));
		}

		node = node->next_sibling();
	}

	return capturedAs;
}


MoveDefinition *GameParser::ParseMove(xml_node<char> *moveNode, bool isTopLevel)
{
	if (strcmp(moveNode->name(), "slide") == 0)
		return ParseMove_Slide(moveNode);
	if (strcmp(moveNode->name(), "leap") == 0)
		return ParseMove_Leap(moveNode);
	if (strcmp(moveNode->name(), "hop") == 0)
		return ParseMove_Hop(moveNode);
	if (strcmp(moveNode->name(), "shoot") == 0)
		return ParseMove_Shoot(moveNode);
	if (strcmp(moveNode->name(), "moveLike") == 0)
		return ParseMove_MoveLike(moveNode);
	if (isTopLevel)
	{
		if (strcmp(moveNode->name(), "sequence") == 0)
			return ParseMove_Sequence(moveNode);
	}
	else
	{
		if (strcmp(moveNode->name(), "repeat") == 0)
			return ParseMove_Repeat(moveNode);
		if (strcmp(moveNode->name(), "whenPossible") == 0)
			return ParseMove_WhenPossible(moveNode);
		if (strcmp(moveNode->name(), "referencePiece") == 0)
			return ParseMove_ReferencePiece(moveNode);
	}

	// todo: report unexpected move type error
	return 0;
}

MoveDefinition *GameParser::ParseMove_Slide(xml_node<char> *moveNode)
{
	Conditions *conditions = ParseConditions(moveNode->first_node("conditions"));

	xml_attribute<> *attr = moveNode->first_attribute("piece");
	char *pieceRef = attr == 0 ? "self" : attr->value();

	unsigned int dir = LookupDirection(moveNode->first_attribute("dir")->value());

	Distance *dist = ParseDistance(moveNode->first_attribute("dist")->value());

	attr = moveNode->first_attribute("distMax");
	Distance *distMax = attr == 0 ? 0 : ParseDistance(attr->value());

	attr = moveNode->first_attribute("when");
	MoveDefinition::When_t when = attr == 0 ? MoveDefinition::Any : ParseWhen(attr->value());

	return new Slide(pieceRef, conditions, when, dir, dist, distMax);
}


MoveDefinition *GameParser::ParseMove_Leap(xml_node<char> *moveNode)
{
	Conditions *conditions = ParseConditions(moveNode->first_node("conditions"));

	xml_attribute<> *attr = moveNode->first_attribute("piece");
	char *pieceRef = attr == 0 ? "self" : attr->value();

	unsigned int dir = LookupDirection(moveNode->first_attribute("dir")->value());

	Distance *dist = ParseDistance(moveNode->first_attribute("dist")->value());

	attr = moveNode->first_attribute("distMax");
	Distance *distMax = attr == 0 ? 0 : ParseDistance(attr->value());

	attr = moveNode->first_attribute("secondDist");
	Distance *secondDist = attr == 0 ? &Distance::Zero : ParseDistance(attr->value());

	attr = moveNode->first_attribute("secondDir");
	unsigned int secondDir = attr == 0 ? 0 : LookupDirection(attr->value());

	attr = moveNode->first_attribute("when");
	MoveDefinition::When_t when = attr == 0 ? MoveDefinition::Any : ParseWhen(attr->value());

	return new Leap(pieceRef, conditions, when, dir, dist, distMax, secondDir, secondDist);
}


MoveDefinition *GameParser::ParseMove_Hop(xml_node<char> *moveNode)
{
	Conditions *conditions = ParseConditions(moveNode->first_node("conditions"));

	xml_attribute<> *attr = moveNode->first_attribute("piece");
	char *pieceRef = attr == 0 ? "self" : attr->value();

	unsigned int dir = LookupDirection(moveNode->first_attribute("dir")->value());

	Distance *distToHurdle = ParseDistance(moveNode->first_attribute("distToHurdle")->value());

	attr = moveNode->first_attribute("distToHurdleMax");
	Distance *distToHurdleMax = attr == 0 ? 0 : ParseDistance(attr->value());

	Distance *distAfterHurdle = ParseDistance(moveNode->first_attribute("distAfterHurdle")->value());

	attr = moveNode->first_attribute("distAfterHurdleMax");
	Distance *distAfterHurdleMax = attr == 0 ? 0 : ParseDistance(attr->value());

	attr = moveNode->first_attribute("when");
	MoveDefinition::When_t when = attr == 0 ? MoveDefinition::Any : ParseWhen(attr->value());

	attr = moveNode->first_attribute("captureHurdle");
	bool captureHurdle = attr == 0 || (strcmp(attr->value(), "true") == 0);

	return new Hop(pieceRef, conditions, when, dir, distToHurdle, distToHurdleMax, distAfterHurdle, distAfterHurdleMax, captureHurdle);
}


MoveDefinition *GameParser::ParseMove_Shoot(xml_node<char> *moveNode)
{
	Conditions *conditions = ParseConditions(moveNode->first_node("conditions"));

	xml_attribute<> *attr = moveNode->first_attribute("piece");
	char *pieceRef = attr == 0 ? "self" : attr->value();

	unsigned int dir = LookupDirection(moveNode->first_attribute("dir")->value());

	Distance *dist = ParseDistance(moveNode->first_attribute("dist")->value());

	attr = moveNode->first_attribute("distMax");
	Distance *distMax = attr == 0 ? 0 : ParseDistance(attr->value());

	attr = moveNode->first_attribute("secondDist");
	Distance *secondDist = attr == 0 ? &Distance::Zero : ParseDistance(attr->value());

	attr = moveNode->first_attribute("secondDir");
	unsigned int secondDir = attr == 0 ? 0 : LookupDirection(attr->value());

	return new Shoot(pieceRef, conditions, MoveDefinition::Capturing, dir, dist, distMax, secondDir, secondDist);
}


MoveDefinition *GameParser::ParseMove_MoveLike(xml_node<char> *moveNode)
{
	Conditions *conditions = ParseConditions(moveNode->first_node("conditions"));

	char *pieceRef = moveNode->first_attribute("other")->value();
	
	xml_attribute<> *attr = moveNode->first_attribute("when");
	MoveDefinition::When_t when = attr == 0 ? MoveDefinition::Any : ParseWhen(attr->value());
	
	return new MoveLike(pieceRef, conditions, when);
}


MoveDefinition *GameParser::ParseMove_Sequence(xml_node<char> *moveNode)
{
	Sequence *move = new Sequence();

	xml_node<> *node = moveNode->first_node();
	while (node != 0)
	{
		MoveDefinition *child = ParseMove(node, false);
		move->contents.push_back(child);

		node = node->next_sibling();
	}

	return move;
}


MoveDefinition *GameParser::ParseMove_Repeat(xml_node<char> *moveNode)
{
	int min = atoi(moveNode->first_attribute("min")->value());
	int max = atoi(moveNode->first_attribute("max")->value());

	Repeat *move = new Repeat(min, max);

	xml_node<> *node = moveNode->first_node();
	while (node != 0)
	{
		MoveDefinition *child = ParseMove(node, false);
		move->contents.push_back(child);

		node = node->next_sibling();
	}

	return move;
}


MoveDefinition *GameParser::ParseMove_WhenPossible(xml_node<char> *moveNode)
{
	WhenPossible *move = new WhenPossible();

	xml_node<> *node = moveNode->first_node();
	while (node != 0)
	{
		MoveDefinition *child = ParseMove(node, false);
		move->contents.push_back(child);

		node = node->next_sibling();
	}

	return move;
}


MoveDefinition *GameParser::ParseMove_ReferencePiece(xml_node<char> *moveNode)
{
	xml_attribute<> *attr = moveNode->first_attribute("name");
	char *name = attr == 0 ? 0 : attr->value();

	attr = moveNode->first_attribute("type");
	char *type = attr == 0 ? "any" : attr->value();

	attr = moveNode->first_attribute("owner");
	Player::Relationship_t relat = ParseRelationship(attr == 0 ? 0 : attr->value());

	attr = moveNode->first_attribute("dir");
	unsigned int dir = attr == 0 ? 0 : LookupDirection(attr->value());

	moveNode->first_attribute("dist");
	Distance *dist = attr == 0 ? 0 : ParseDistance(attr->value());

	return new ReferencePiece(name, type, relat, dir, dist);
}


Conditions *GameParser::ParseConditions(xml_node<char> *node)
{
	if (node == 0)
		return 0;

	// todo: implement this
	return 0;
}

Distance *GameParser::ParseDistance(char *val)
{
	if (val == 0)
		return 0;

	if (strcmp(val, "any") == 0)
		return &Distance::Any;

	int len = strlen(val);
	if (len >= 3 && val[0] == 'm' && val[1] == 'a' && val[2] == 'x')
		return new Distance(Distance::Max, atoi(val+3));
	else if (len >= 4 && val[0] == 'p' && val[1] == 'r' && val[2] == 'e' && val[3] == 'v')
		return new Distance(Distance::Prev, atoi(val + 4));
	else
		return new Distance(Distance::None, atoi(val));
}


MoveDefinition::When_t GameParser::ParseWhen(char *val)
{
	if (val == 0)
		return MoveDefinition::Any;

	if (strcmp(val, "any") == 0)
		MoveDefinition::Any;
	if (strcmp(val, "move") == 0)
		MoveDefinition::Moving;
	if (strcmp(val, "capture") == 0)
		MoveDefinition::Capturing;

	// todo: report unexpected value
	return MoveDefinition::Any;
}


Player::Relationship_t GameParser::ParseRelationship(char *val)
{
	if (val == 0)
		return Player::Any;

	if (strcmp(val, "self") == 0)
		Player::Self;
	if (strcmp(val, "enemy") == 0)
		Player::Enemy;
	if (strcmp(val, "ally") == 0)
		Player::Ally;

	// todo: report unexpected value
	return Player::Any;
}


bool GameParser::ParsePlayers(xml_node<> *setupNode, xml_document<> *svgDoc)
{
	xml_node<> *svgRoot = svgDoc->first_node();

	xml_node<> *playerNode = setupNode->first_node();
	while (playerNode != 0)
	{
		char *playerName = playerNode->first_attribute("name")->value();
		char *forwardDir = playerNode->first_attribute("forwardDirection")->value();

		Player *player = new Player(game, playerName, LookupDirection(forwardDir));
		game->players.push_back(player);

		// link piece appearances up to actual player now that this exists
		for (auto it = pieceAppearances.begin(); it != pieceAppearances.end(); it++)
		{
			auto tuple = *it;

			if (strcmp(std::get<1>(tuple), playerName) != 0)
				continue;

			PieceType *type = std::get<0>(tuple);
			char *defID = std::get<2>(tuple);

			char *defRef = new char[TYPE_NAME_LENGTH + PLAYER_NAME_LENGTH + 2];
			strncpy(defRef, "#", TYPE_NAME_LENGTH + PLAYER_NAME_LENGTH + 2);
			strcat(defRef, defID);

			type->appearances.insert(std::make_pair(player->id, defRef));

			//pieceAppearances.erase(it++);
		}

		xml_node<> *pieceNode = playerNode->first_node();
		while (pieceNode != 0)
		{
			char *position = pieceNode->first_attribute("location")->value();
			Piece::State_t state = Piece::OnBoard;

			char *typeName = pieceNode->first_attribute("type")->value();
			auto it = pieceTypesByName.find(typeName);
			if (it == pieceTypesByName.end())
			{
				// todo: report error somehow
				return false;
			}
			PieceType *type = std::get<0>(it->second);

			if (strcmp(position, "held") == 0)
			{
				Piece *piece = new Piece(player, type, 0, Piece::Held, player);
				player->piecesHeld.insert(std::pair<int, Piece*>(piece->uniqueID, piece));
			}
			else
			{
				auto it = cellsByRef.find(position);
				if (it == cellsByRef.end())
				{
					// todo: report error somehow
					return false;
				}
				Cell *cell = it->second;

				Piece *piece = new Piece(player, type, cell, Piece::OnBoard, 0);
				player->piecesOnBoard.insert(std::pair<int, Piece*>(piece->uniqueID, piece));

				if (cell->piece == 0)
					cell->piece = piece;
				else
					; // todo: report multiple-pieces-in-one-cell

				// generate piece image
				xml_node<> *image = svgDoc->allocate_node(node_element, "use");

				char *val = svgDoc->allocate_string(0, 8);
				sprintf(val, "p%d", piece->uniqueID);
				image->append_attribute(svgDoc->allocate_attribute("id", val));

				val = svgDoc->allocate_string("piece ", PLAYER_NAME_LENGTH + 6);
				strcat(val, player->name);
				image->append_attribute(svgDoc->allocate_attribute("class", val));

				val = svgDoc->allocate_string(0, 8);
				sprintf(val, "%d", cell->coordX);
				image->append_attribute(svgDoc->allocate_attribute("x", val));

				val = svgDoc->allocate_string(0, 8);
				sprintf(val, "%d", cell->coordY);
				image->append_attribute(svgDoc->allocate_attribute("y", val));

				// assign correct piece appearance to this element
				auto it2 = piece->pieceType->appearances.find(player->id);
				if (it2 == piece->pieceType->appearances.end())
				{
					// todo: report error, piece has no appearance for this player
					return false;
				}
				val = svgDoc->allocate_string(it2->second, TYPE_NAME_LENGTH + PLAYER_NAME_LENGTH + 2);
				image->append_attribute(svgDoc->allocate_attribute("xmlns:xlink", "http://www.w3.org/1999/xlink"));
				image->append_attribute(svgDoc->allocate_attribute("xlink:href", val));

				svgRoot->append_node(image);
			}

			pieceNode = pieceNode->next_sibling();
		}

		playerNode = playerNode->next_sibling();
	}
	return true;
}


bool GameParser::ParseRules(xml_node<> *rulesNode)
{
	xml_node<> *node = rulesNode->first_node("turnOrder");
	TurnOrder *order = node == 0 ? TurnOrder::CreateDefault(game->players) : ParseTurnOrder(node);

	if (order == 0)
	{
		// report error parsing turn order
		return false;
	}
	else
		game->turnOrder = order;

	node = rulesNode->first_node("endOfGame");
	EndOfGame *end = node == 0 ? EndOfGame::CreateDefault() : ParseEndOfGame(node);

	if (end == 0)
	{
		// report error parsing end of game
		return false;
	}
	else
		game->endOfGame = end;

	return true;
}

TurnOrder *GameParser::ParseTurnOrder(xml_node<> *node)
{
	TurnOrder *turnOrder = new TurnOrder();

	ParseTurnRepeat(turnOrder, node);

	return turnOrder;
}

bool GameParser::ParseTurnRepeat(TurnRepeat *repeat, xml_node<> *repeatNode)
{
	xml_node<> *node = repeatNode->first_node();
	while (node != 0)
	{


		node = node->next_sibling();
	}

	// todo: implement this
	return false;
}


EndOfGame *GameParser::ParseEndOfGame(xml_node<> *node)
{
	// todo: implement this
	return 0;
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