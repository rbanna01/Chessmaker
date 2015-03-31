#include "GameState.h"
#include "Game.h"
#include "Move.h"
#include "MoveDefinition.h"
#include "Piece.h"

GameState::GameState(Game *game, Player *currentPlayer, int turnNumber)
{
	this->game = game;
	this->currentPlayer = currentPlayer;
	this->turnNumber = turnNumber;
}


GameState::~GameState()
{
	
}


std::list<Move*> *GameState::DeterminePossibleMoves()
{
	std::list<Move*> *moves = new std::list<Move*>();
	CalculateMovesForPlayer(currentPlayer, moves);
	return moves;
}


std::list<Move*> *GameState::DetermineThreatMoves()
{
	std::list<Move*> *moves = new std::list<Move*>();

	for (auto it = game->players.begin(); it != game->players.end(); it++)
	{
		Player *opponent = *it;
		if (currentPlayer->GetRelationship(opponent) != Player::Enemy)
			continue;

		CalculateMovesForPlayer(opponent, moves);
	}

	return moves;
}


// determine notation for each move (done all at once to ensure they are unique)
std::list<Move*> *GameState::PrepareMovesForTurn()
{
	std::map<char*, Move*, char_cmp> movesByNotation;
	std::list<Move*> *possibleMoves = DeterminePossibleMoves();

	for (auto it = possibleMoves->begin(); it != possibleMoves->end(); it++)
	{
		Move *move = *it;

        // ensure unique notation
		for (int detailLevel = 1; detailLevel <= MAX_NOTATION_DETAIL; detailLevel++)
		{
			char *notation = move->DetermineNotation(detailLevel);

			auto existingIt = movesByNotation.find(notation);
            if (existingIt != movesByNotation.end())
			{
                // another move uses this notation. Calculate a new, more-specific notation for that other move.
				Move *other = existingIt->second;
				char *otherNot = other->DetermineNotation(detailLevel + 1);

				// only save the other move with the more specific notation if we haven't done so already
				if (movesByNotation.find(otherNot) == movesByNotation.end())
					movesByNotation.insert(std::pair<char*, Move*>(otherNot, other));
            }
            else
			{
				movesByNotation.insert(std::pair<char*, Move*>(notation, move));
                break;
			}
        }
	}

    return possibleMoves;
}


void GameState::CalculateMovesForPlayer(Player *player, std::list<Move*> *output)
{
	std::set<Piece*> pieces(player->piecesOnBoard);
	for (auto it = pieces.begin(); it != pieces.end(); it++)
	{
		Piece *piece = *it;
		Move *moveTemplate = new Move(piece->GetOwner(), this, piece, piece->GetPosition());

        // get promotion possibilities
        //piece.pieceType.promotionOpportunities.each(function (op) {
        //    if (!op.mandatory && op.type != PromotionType.EndOfMove && op.isAvailable(moveTemplate, this.game))
        //        op.options.each(function (option) {
        //            output.push(new Promotion(piece.ownerPlayer, piece, option, this.moveNumber, op.type == PromotionType.CountsAsMove));
        //        });
        //});

        // and then get move possibilities
		auto moves = piece->GetType()->GetMoves();
		for (auto it = moves.begin(); it != moves.end(); it++)
		{
			MoveDefinition *moveDef = *it;
			std::list<Move*> *possibilities = moveDef->DetermineNextSteps(moveTemplate, piece, 0);
			for (auto it2 = possibilities->begin(); it2 != possibilities->end(); it2++)
			{
				Move *move = *it2;
        
				if (game->GetEndOfGame()->AnyIllegalMovesSpecified())
				{
                    GameState *subsequentState = move->Perform(false);
					EndOfGame::CheckType_t result = game->GetEndOfGame()->CheckEndOfTurn(this, move);
                    move->Reverse(false);
					if (result == EndOfGame::IllegalMove)
					{
						delete move;
						continue;
					}
                }

                output->push_back(move);
            }
			delete possibilities;
		}

		delete moveTemplate;
	}
	
    // now look at each held piece
    //if (player.piecesHeld.length == 0)
    //    continue;
    
    //pieces = player.piecesHeld.slice();
    //this.game.board.cells.items.each(function (coord, cell) {
    //    if (cell.value != Board.CellType.Normal)
    //        return;
    
    //    for (var i = 0; i < pieces.length; i++) {
    //        var move = new Move(piece.ownerPlayer, this, piece, null);
    //        move.addStep(MoveStep.CreateDrop(piece, coord, piece.ownerPlayer));

    //        if (this.game.rules.dropPiecesWhen == null || this.game.rules.dropPiecesWhen.isSatisfied(move, this))
    //            output.push(move);
    //    }
    //});
}