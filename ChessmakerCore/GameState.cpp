#include "GameState.h"
#include "Game.h"
#include "Move.h"
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


std::list<Move*> GameState::DeterminePossibleMoves()
{
	std::list<Move*> moves;
	CalculateMovesForPlayer(currentPlayer, moves);
	return moves;
}


std::list<Move*> GameState::DetermineThreatMoves()
{
	std::list<Move*> moves;

	auto it = game->players.begin();
	while (it != game->players.end())
	{
		Player *opponent = *it;
		if (currentPlayer->GetRelationship(opponent) != Player::Enemy)
			continue;

		CalculateMovesForPlayer(opponent, moves);
	}

	return moves;
}


// sort possible states (moves) by piece, determine notation for each move (done all at once to ensure they are unique), and return whether any moves are possible or not
bool GameState::PrepareMovesForTurn()
{
	movesByPiece.clear(); // urgh, i guess these need deleted? don't really want to do this tbh
	bool anyMoves = false;

	std::map<char*, Move*, char_cmp> movesByNotation;
	std::list<Move*> allMoves = DeterminePossibleMoves();

	auto it = allMoves.begin();
	while (it != allMoves.end())
	{
		anyMoves = true;
		Move *move = *it;

		// sort moves by piece, for ease of access in the UI
		auto pieceIt = movesByPiece.find(move->piece->GetID());
		if (pieceIt == movesByPiece.end())
		{
			// add a new one
			std::list<Move*> pieceMoves;
			pieceMoves.push_back(move);
			movesByPiece.insert(std::pair<int, std::list<Move*>>(move->piece->GetID(), pieceMoves));
		}
		else
		{
			std::list<Move*> pieceMoves = pieceIt->second;
			pieceMoves.push_back(move);
		}

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

    return anyMoves;
}


void GameState::CalculateMovesForPlayer(Player *player, std::list<Move*> output)
{
	std::set<Piece*> pieces(player->piecesOnBoard);
	auto it = pieces.begin();
	while (it != pieces.end())
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
        /*for (var j = 0; j < piece.pieceType.moves.length; j++) {
            var moveDef = piece.pieceType.moves[j];
            var possibilities = moveDef.appendValidNextSteps(moveTemplate, piece, null);
            for (var k = 0; k < possibilities.length; k++) {
                var move = possibilities[k];

                if (this.game.endOfGame.illegalMovesSpecified) {
                    move.perform(this.game, false);
                    var result = this.game.endOfGame.checkEndOfTurn(this, move);
                    move.reverse(this.game, false);
                    if (result == EndOfGame.Type.IllegalMove)
                        continue;
                }

                output.push(move);
            }
		}*/ // todo:implement this

		delete moveTemplate;
		it++;
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