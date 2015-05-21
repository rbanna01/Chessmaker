#include <utility>
#include "Board.h"
#include "Game.h"
#include "GameState.h"
#include "Move.h"
#include "MoveConditions.h"
#include "MoveDefinition.h"
#include "MoveStep.h"

MoveDefinition::MoveDefinition(const char *pieceRef, MoveConditionGroup *conditions, When_t when, direction_t direction)
{
	strcpy(this->pieceRef, pieceRef);
	this->conditions = conditions;
	this->direction = direction;
	this->when = when;
	this->moveSelf = strcmp(pieceRef, "self") == 0;
}


MoveDefinition::~MoveDefinition()
{
	if (conditions != 0)
		delete conditions;
}


MoveStep *MoveDefinition::CreateCapture(Piece *target, Piece *capturedBy)
{
	Player *player = capturedBy->GetOwner();
	if (player->GetGame()->GetHoldCapturedPieces())
		return MoveStep::CreatePickup(target, target->GetPosition(), player);
	return MoveStep::CreateCapture(target, target->GetPosition());
}


std::list<Move*> *Slide::DetermineNextSteps(Move *baseMove, Piece *piece, MoveStep *previousStep)
{
	std::list<Move*> *moves = new std::list<Move*>();

	// some steps will specify a different piece to act upon, rather than the piece being moved
	if (!moveSelf)
	{
		piece = baseMove->GetPieceByReference(pieceRef);
		if (piece == 0)
		{
			ReportError("Referenced piece not found for slide move: %s\n", pieceRef);
			return moves;
		}
	}

	Player *player = baseMove->GetPlayer();
	Game *game = player->GetGame();
	direction_t dirs = player->ResolveDirections(direction, previousStep != 0 && previousStep->GetDirection() != 0 ? previousStep->GetDirection() : player->GetForwardDirection());

	FOR_EACH_DIR_IN_SET(dirs, dir)
	{
		int boardMaxDist = piece->GetPosition()->GetMaxDist(dir);
		int minDist = distance->GetValue(previousStep, boardMaxDist);
		int maxDist = distance->GetMaxValue(distanceMax, previousStep, boardMaxDist);
		Cell *cell = piece->GetPosition();
	
        for (int dist = 1; dist <= maxDist; dist++)
		{
			cell = cell->FollowLink(dir);
            if (cell == 0)
                break;

            Piece *target = cell->GetPiece();
            if (dist >= minDist)
			{
				MoveStep *captureStep = 0;
				if (target == 0)
				{
					if (when == Capturing)
						continue; // needs to be a capture for this slide to be valid, and there is no piece here. But there might be pieces beyond this one.
				}
				else
				{
					if (when == Moving)
						break;
					else if (piece->CanCapture(target))
						captureStep = CreateCapture(target, piece);
                    else
                        break; // cannot capture this piece. Slides cannot pass over pieces, so there can be no more valid slides in this direction.
				}

                Move *move = baseMove->Clone();
				MoveStep *lastPerformedStep = move->GetSteps().empty() ? 0 : *move->GetSteps().rbegin();
                if (captureStep != 0)
				{
                    move->AddStep(captureStep);
                    move->AddPieceReference(target, "target");
                }

                move->AddStep(MoveStep::CreateMove(piece, piece->GetPosition(), cell, dir, dist));

				if (conditions == 0 || conditions->IsSatisfied(move, lastPerformedStep))
					moves->push_back(move);
				else
					delete move;
            }

            if (target != 0)
                break; // Slides can't pass intervening pieces. As this cell was occupied, can be no more valid slides in this direction.
        }
    }

	return moves;
}


std::list<Move*> *Leap::DetermineNextSteps(Move *baseMove, Piece *piece, MoveStep *previousStep)
{
	std::list<Move*> *moves = new std::list<Move*>();

	// some steps will specify a different piece to act upon, rather than the piece being moved
	if (!moveSelf)
	{
		piece = baseMove->GetPieceByReference(pieceRef);
		if (piece == 0)
		{
			ReportError("Referenced piece not found for leap move: %s\n", pieceRef);
			return moves;
		}
	}

	Player *player = baseMove->GetPlayer();
	Game *game = player->GetGame();
	direction_t dirs = player->ResolveDirections(direction, previousStep != 0 && previousStep->GetDirection() != 0 ? previousStep->GetDirection() : player->GetForwardDirection());
	
	FOR_EACH_DIR_IN_SET(dirs, firstDir)
	{
		int boardMaxDist = piece->GetPosition()->GetMaxDist(firstDir);
		int minDist = distance->GetValue(previousStep, boardMaxDist);
		int maxDist = distance->GetMaxValue(distanceMax, previousStep, boardMaxDist);

		// may not have a second dir, but we need one to function here
		bool hasSecondDir = secondDir != 0;
		direction_t secondDirs = hasSecondDir ? player->ResolveDirections(secondDir, firstDir) : firstDir;
		
		Cell *straightCell = piece->GetPosition();

        for (int dist = 1; dist <= maxDist; dist++)
		{
			straightCell = straightCell->FollowLink(firstDir);
            if (straightCell == 0)
                break;

            if (dist < minDist)
                continue; // not yet reached minimum straight distance, so don't turn

			FOR_EACH_DIR_IN_SET(secondDirs, secondDir)
			{
				Cell *destCell = straightCell;
				boardMaxDist = destCell->GetMaxDist(secondDir);
				int minDist2 = hasSecondDir ? secondDist->GetValue(previousStep, boardMaxDist) : 0;
				int maxDist2 = hasSecondDir ? secondDist->GetMaxValue(0, previousStep, boardMaxDist) : 0;

				for (int secondDist = hasSecondDir ? 1 : 0; secondDist <= maxDist2; secondDist++)
				{
					if (secondDist > 0)
					{
						destCell = destCell->FollowLink(secondDir);
						if (destCell == 0)
							break;
					}

                    Piece *target = destCell->GetPiece();
                    if (secondDist >= minDist2)
					{
                        MoveStep *captureStep = 0;

						if (target == 0)
						{
							if (when == Capturing)
								continue; // needs to be a capture for this leap to be valid, and there is no piece here. But there might be pieces beyond this one.
						}
						else
						{
							if (when != Moving && piece->CanCapture(target))
								captureStep = CreateCapture(target, piece);
							else
								continue; // cannot capture this piece, but unlike slides, leaps don't stop when a piece is in the way
						}

						Move *move = baseMove->Clone();
						MoveStep *lastPerformedStep = move->GetSteps().empty() ? 0 : *move->GetSteps().rbegin();
						if (captureStep != 0)
						{
							move->AddStep(captureStep);
							move->AddPieceReference(target, "target");
						}

						move->AddStep(MoveStep::CreateMove(piece, piece->GetPosition(), destCell, secondDir, secondDist > 0 ? secondDist : dist));

						if (conditions == 0 || conditions->IsSatisfied(move, lastPerformedStep))
							moves->push_back(move);
						else
							delete move;
                    }
                }
            }
        }
    }

	return moves;
}


std::list<Move*> *Hop::DetermineNextSteps(Move *baseMove, Piece *piece, MoveStep *previousStep)
{
	std::list<Move*> *moves = new std::list<Move*>();

	// some steps will specify a different piece to act upon, rather than the piece being moved
	if (!moveSelf)
	{
		piece = baseMove->GetPieceByReference(pieceRef);
		if (piece == 0)
		{
			ReportError("Referenced piece not found for hop move: %s\n", pieceRef);
			return moves;
		}
	}

	Player *player = baseMove->GetPlayer();
	Game *game = player->GetGame();
	direction_t dirs = player->ResolveDirections(direction, previousStep != 0 && previousStep->GetDirection() != 0 ? previousStep->GetDirection() : player->GetForwardDirection());

	FOR_EACH_DIR_IN_SET(dirs, dir)
	{
		int boardMaxDist = piece->GetPosition()->GetMaxDist(dir);
		int minDistTo = distToHurdle->GetValue(previousStep, boardMaxDist);
		int maxDistTo = distToHurdle->GetMaxValue(distToHurdleMax, previousStep, boardMaxDist);
		int minDistAfter = distAfterHurdle->GetValue(previousStep, boardMaxDist); // doesn't the 'board max' need recalculated based on hurdle position?
		int maxDistAfter = distAfterHurdle->GetMaxValue(distAfterHurdleMax, previousStep, boardMaxDist);
		Cell *hurdleCell = piece->GetPosition();
		
        for (int distTo = 1; distTo <= maxDistTo; distTo++)
		{
			hurdleCell = hurdleCell->FollowLink(dir);
            if (hurdleCell == 0)
                break;

            Piece *hurdle = hurdleCell->GetPiece();
            if (hurdle == 0)
                continue;

            if (distTo < minDistTo)
                break; // this piece is too close to hop over

            if (captureHurdle && !piece->CanCapture(hurdle))
                break; // can't capture this hurdle, so can't hop it

            // now we've found the only possible hurdle. Do what we will with that, then there cannot be another hurdle, so break after
            Cell *destCell = hurdleCell;
            for (int distAfter = 1; distAfter <= maxDistAfter; distAfter++)
			{
                destCell = destCell->FollowLink(dir);
                if (destCell == 0)
                    break;

                Piece *target = destCell->GetPiece();

                if (distAfter >= minDistAfter)
				{
                    MoveStep *captureStep = 0;
					if (target == 0)
					{
						if (when == Capturing)
							continue; // needs to be a capture for this hop to be valid, and there is no piece here. But there might be pieces beyond this one.
					}
					else
					{
						if (when == Moving)
							break;
						else if (piece->CanCapture(target))
							captureStep = CreateCapture(target, piece);
						else
							break; // cannot capture this piece. Cannot hop over a second piece, so there can be no more valid hops in this direction.
					}

                    Move *move = baseMove->Clone();
					MoveStep *lastPerformedStep = move->GetSteps().empty() ? 0 : *move->GetSteps().rbegin();
                    move->AddPieceReference(hurdle, "hurdle");

                    if (captureHurdle)
					{
                        MoveStep *hurdleCaptureStep = CreateCapture(hurdle, piece);
                        move->AddStep(hurdleCaptureStep);
                    }

                    if (captureStep != 0)
					{
                        move->AddPieceReference(target, "target");
                        move->AddStep(captureStep);
                    }

                    move->AddStep(MoveStep::CreateMove(piece, piece->GetPosition(), destCell, dir, distTo + distAfter));

					if (conditions == 0 || conditions->IsSatisfied(move, lastPerformedStep))
						moves->push_back(move);
					else
						delete move;
                }

                if (target != 0)
                    break; // whether we capture it or not, can't pass a second piece
            }

            break;
        }
    }
	
	return moves;
}


std::list<Move*> *Shoot::DetermineNextSteps(Move *baseMove, Piece *piece, MoveStep *previousStep)
{
	std::list<Move*> *moves = new std::list<Move*>();
	
	// some steps will specify a different piece to act upon, rather than the piece being moved
	if (!moveSelf)
	{
		piece = baseMove->GetPieceByReference(pieceRef);
		if (piece == 0)
		{
			ReportError("Referenced piece not found for shoot move: %s\n", pieceRef);
			return moves;
		}
	}

	Player *player = baseMove->GetPlayer();
	Game *game = player->GetGame();
	direction_t dirs = player->ResolveDirections(direction, previousStep != 0 && previousStep->GetDirection() != 0 ? previousStep->GetDirection() : player->GetForwardDirection());

	FOR_EACH_DIR_IN_SET(dirs, firstDir)
	{
		int boardMaxDist = piece->GetPosition()->GetMaxDist(firstDir);
		int minDist = distance->GetValue(previousStep, boardMaxDist);
		int maxDist = distance->GetMaxValue(distanceMax, previousStep, boardMaxDist);

		// may not have a second dir, but we need one to function here
		bool hasSecondDir = secondDir != 0;
		direction_t secondDirs = hasSecondDir ? player->ResolveDirections(secondDir, firstDir) : firstDir;
		
		Cell *straightCell = piece->GetPosition();

		for (int dist = 1; dist <= maxDist; dist++)
		{
			straightCell = straightCell->FollowLink(firstDir);
			if (straightCell == 0)
				break;

			if (dist < minDist)
				continue; // not yet reached minimum straight distance, so don't turn

			FOR_EACH_DIR_IN_SET(secondDirs, secondDir)
			{
				Cell *destCell = straightCell;
				boardMaxDist = destCell->GetMaxDist(secondDir);
				int minDist2 = hasSecondDir ? secondDist->GetValue(previousStep, boardMaxDist) : 0;
				int maxDist2 = hasSecondDir ? secondDist->GetMaxValue(0, previousStep, boardMaxDist) : 0;

				for (int secondDist = hasSecondDir ? 1 : 0; secondDist <= maxDist2; secondDist++)
				{
					if (secondDist > 0)
					{
						destCell = destCell->FollowLink(secondDir);
						if (destCell == 0)
							break;
					}

					Piece *target = destCell->GetPiece();
					if (secondDist >= minDist2)
					{
						MoveStep *captureStep = 0;

						if (target == 0)
							continue; // needs to be a capture for this shoot to be valid, and there is no piece here. But there might be pieces beyond this one.
						else if (piece->CanCapture(target))
							captureStep = CreateCapture(target, piece);
						else
							break; // cannot capture this piece, and unlike leaps, shoots don't continue past other pieces

						Move *move = baseMove->Clone();
						MoveStep *lastPerformedStep = move->GetSteps().empty() ? 0 : *move->GetSteps().rbegin();
						move->AddStep(captureStep);
						move->AddPieceReference(target, "target");

						if (conditions == 0 || conditions->IsSatisfied(move, lastPerformedStep))
							moves->push_back(move);
						else
							delete move;
					}
				}
			}
		}
    }

	return moves;
}


std::list<Move*> *MoveLike::DetermineNextSteps(Move *baseMove, Piece *piece, MoveStep *previousStep)
{
	if (likeTarget)
	{
        // difficult special case - we don't know what piece we're to move like until we've already made the move
        return AppendMoveLikeTarget(baseMove, piece, previousStep);
    }

	std::list<Move*> *moves = new std::list<Move*>();

	// some steps will specify a different piece to act upon, rather than the piece being moved
	Piece *other = baseMove->GetPieceByReference(pieceRef);
	if (other == 0)
	{
		ReportError("Referenced piece not found for move like move: %s\n", pieceRef);
		return moves;
	}
	
    // iterate over all possible moves of the specific piece being referenced, but *for* the piece being moved
	std::list<MoveDefinition*> otherMoves = other->GetType()->GetMoves();
	for (auto it = otherMoves.begin(); it != otherMoves.end(); it++)
	{
		MoveDefinition *moveDef = *it;
		std::list<Move*> *possibilities = moveDef->DetermineNextSteps(baseMove, piece, previousStep);

		for (auto it2 = possibilities->begin(); it2 != possibilities->end(); it2++)
            moves->push_back(*it2);

		delete possibilities;
    }

	return moves;
}


bool MoveLike::AllowMoveLikeTarget = true;
std::list<Move*> *MoveLike::AppendMoveLikeTarget(Move *baseMove, Piece *piece, MoveStep *previousStep)
{
	std::list<Move*> *moves = new std::list<Move*>();
	if (!AllowMoveLikeTarget)
        return moves; // 'move like target' shouldn't try to capture other pieces using 'move like target' - that's nonsense

    AllowMoveLikeTarget = false;

	Player *player = baseMove->GetPlayer();
	Game *game = player->GetGame();

    // loop through all possible piece types, try out every move, and then if the piece(s) captured are of the same type, use it
	std::list<PieceType*> pieceTypes = game->GetAllPieceTypes();
	for (auto it = pieceTypes.begin(); it != pieceTypes.end(); it++)
	{
		PieceType *pieceType = *it;

		std::list<MoveDefinition*> typeMoves = pieceType->GetMoves();
		for (auto it2 = typeMoves.begin(); it2 != typeMoves.end(); it2++)
		{
			MoveDefinition *moveDef = *it2;
			std::list<Move*> *possibilities = moveDef->DetermineNextSteps(baseMove, piece, previousStep);

			for (auto it3 = possibilities->begin(); it3 != possibilities->end(); it3++)
			{
				Move *newMove = *it3;
				bool moveIsOk = false;

				std::list<MoveStep*> steps = newMove->GetSteps();
				int prevNumSteps = baseMove->GetSteps().size();
				for (auto it4 = std::next(steps.begin(), prevNumSteps); it4 != steps.end(); it4++) // for each of the NEW steps added
				{
					MoveStep *step = *it4;
					if (step->GetToState() != Piece::OnBoard)
					{// moving another piece off the board qualifies this as a capture
						Piece *target = step->GetPiece();
						if (target == piece)
							continue; // if capturing like a 'kamakaze' piece, it should be OK to capture yourself, so ignore this

						if (target != 0 && target->GetType() == pieceType && piece->CanCapture(target))
						{
							moveIsOk = true;
							// debatable: do we want an option to allow capturing multiple pieces, as long as ONE is of the target type? If so, you'd break here instead of below
						}
						else
						{
							moveIsOk = false;
							break;
						}
					}
				}

				if (moveIsOk) // must be a capture, and every piece captured must be of pieceType (or be this same piece, for kamikaze purposes)
					moves->push_back(newMove);
				else
					delete newMove;
			}

			delete possibilities;
		}
	}

    AllowMoveLikeTarget = true;
	return moves;
}


std::list<Move*> *ReferencePiece::DetermineNextSteps(Move *baseMove, Piece *piece, MoveStep *previousStep)
{
	std::list<Move*> *moves = new std::list<Move*>();
	
	Piece *other = 0;
	Player *player = baseMove->GetPlayer();
	Game *game = player->GetGame();

	if (direction != 0 && distance != 0)
	{
		direction_t dirs = player->ResolveDirections(direction, previousStep != 0 && previousStep->GetDirection() != 0 ? previousStep->GetDirection() : player->GetForwardDirection());

		FOR_EACH_DIR_IN_SET(dirs, dir)
		{
			int boardMaxDist = piece->GetPosition()->GetMaxDist(dir);
			int minDist = distance->GetValue(previousStep, boardMaxDist);
			int maxDist = distance->GetMaxValue(0, previousStep, boardMaxDist);
			Cell *cell = piece->GetPosition();

			for (int dist = 1; dist <= maxDist; dist++)
			{
				cell = cell->FollowLink(dir);
				if (cell == 0)
					break;

                if (dist < minDist)
                    continue;

                other = cell->GetPiece();
				if (other == 0)
                    continue;

				if (!other->TypeMatches(otherPieceType))
					continue;

				if (relationship != Player::Any && player->GetRelationship(other->GetOwner()) != relationship)
					continue;

				Move *move = baseMove->Clone();
				move->AddPieceReference(other, otherPieceRef);
				moves->push_back(move);
            }
        }
    }
	else
    {
		std::list<Player*> players = game->GetPlayers();
		for (auto it = players.begin(); it != players.end(); it++)
		{
			Player *otherPlayer = *it;

			if (relationship != Player::Any && player->GetRelationship(otherPlayer) != relationship)
				continue;

			std::set<Piece*> pieces = otherPlayer->GetPiecesOnBoard();
			for (auto it2 = pieces.begin(); it2 != pieces.end(); it2++)
			{
				other = *it2;

				if (!other->TypeMatches(otherPieceType))
					continue;

				Move *move = baseMove->Clone();
				move->AddPieceReference(other, otherPieceRef);
				moves->push_back(move);
            }
        }
	}
	
	return moves;
}


std::list<Move*> *Promotion::DetermineNextSteps(Move *baseMove, Piece *piece, MoveStep *previousStep)
{
	std::list<Move*> *moves = new std::list<Move*>();

	// some steps will specify a different piece to act upon, rather than the piece being moved
	if (!moveSelf)
	{
		piece = baseMove->GetPieceByReference(pieceRef);
		if (piece == 0)
		{
			ReportError("Referenced piece not found for promotion: %s\n", pieceRef);
			return moves;
		}
	}

	MoveStep *lastPerformedStep = baseMove->GetSteps().empty() ? 0 : *baseMove->GetSteps().rbegin();
	if (conditions != 0 && !conditions->IsSatisfied(baseMove, lastPerformedStep))
		return moves; // This assumes that no conditions will be dependent on the promoted type. If they are, this needs moved into the loop, and shouldn't look at baseMove

	Player *player = baseMove->GetPlayer();
	Game *game = player->GetGame();
	
	for (auto it = options.begin(); it != options.end(); it++)
	{
		PieceType *type = *it;
		MoveStep *step = MoveStep::CreatePromotion(piece, piece->GetType(), type);
		Move *move = baseMove->Clone();
		move->AddStep(step);
		moves->push_back(move);

		std::string descr = "Promote to ";
		descr += type->GetName();
		move->SetDescription(descr);

		if (appendNotation)
			move->SetAppendNotation(type->GetNotation());
	}

	return moves;
}


std::list<Move*> *MoveGroup::DetermineNextSteps(Move *baseMove, Piece *piece, MoveStep *previousStep)
{
	std::list<Move*> *moves = new std::list<Move*>(), *prevStepMoves = new std::list<Move*>(), *currentStepMoves = new std::list<Move*>();
	Move *clone = baseMove->Clone();
	prevStepMoves->push_back(clone);

	if (previousStep != 0) // because we're checking if steps == previousStep, we need to ensure we're using the right cloned copy
		previousStep = *clone->GetSteps().rbegin();

	for (int rep = 1; rep <= maxOccurs; rep++)
	{
		for (auto itStep = contents.begin(); itStep != contents.end(); itStep++)
		{
			MoveDefinition *stepDef = *itStep;

			for (auto itPrevMove = prevStepMoves->begin(); itPrevMove != prevStepMoves->end(); itPrevMove++)
			{
				Move *prevMove = *itPrevMove;

				bool previousStepReached = previousStep == 0;
				std::list<MoveStep*> steps = prevMove->GetSteps();
				MoveStep *step = 0;
				for (auto itDoStep = steps.begin(); itDoStep != steps.end(); itDoStep++)
				{
					// don't (re)perform moves until we reach the first step of this group
					step = *itDoStep;
					if (previousStepReached)
						step->Perform(false);
					else if (step == previousStep)
						previousStepReached = true;
				}

				std::list<Move*> *nextMovesForStep = stepDef->DetermineNextSteps(prevMove, piece, step == 0 ? previousStep : step);
				currentStepMoves->splice(currentStepMoves->end(), *nextMovesForStep);
				delete nextMovesForStep;

				for (auto itUndoStep = steps.rbegin(); itUndoStep != steps.rend(); itUndoStep++)
				{
					step = *itUndoStep;
					if (step == previousStep)
						break; // don't rewind past this point
					step->Reverse(false);
				}
			}

			std::list<Move*> *tmp = prevStepMoves;
			prevStepMoves = currentStepMoves;
			currentStepMoves = tmp;

			while (!currentStepMoves->empty())
				delete currentStepMoves->front(), currentStepMoves->pop_front();

			if (prevStepMoves->size() == 0)
				break;
		}
        
		if (prevStepMoves->size() == 0) // this FULL iteration through the loop ended with no suitable moves
			break;
           
		if (rep >= minOccurs)
			moves->splice(moves->end(), *prevStepMoves);
	}


	while (!prevStepMoves->empty())
		delete prevStepMoves->front(), prevStepMoves->pop_front();
	delete prevStepMoves;
	delete currentStepMoves;

	// this group failed, but it wasn't essential, so return the previous move, as that's still valid on its own
	if (stepOutIfFail && moves->size() == 0 && baseMove->GetSteps().size() > 0)
		moves->push_back(baseMove->Clone());

	return moves;
}