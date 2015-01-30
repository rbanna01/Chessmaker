"use strict";

Slide.prototype.appendValidNextSteps = function (baseMove, piece, previousStep) {
    var moves = [];

    if (this.piece != 'self')
    {// some steps will specify a different piece to act upon, rather than the piece being moved
        piece = baseMove.getPieceByRef(this.piece);
        if (piece == null) {
            console.log('piece ref not found: ' + this.piece);
            return moves;
        }
    }
    
    var dirs = baseMove.player.resolveDirection(this.dir, previousStep != null && previousStep.direction != null ? previousStep.direction : baseMove.player.forwardDir);
    
    for (var i = 0; i < dirs.length; i++) {
        var dir = dirs[i];

        var boardMaxDist = baseMove.prevState.game.board.getMaxDistance(piece.position, dir);
        var distances = this.dist.getRange(this.distMax, previousStep, boardMaxDist);
        var minDist = distances[0]; var maxDist = distances[1];
        var cell = piece.position;

        for (var dist = 1; dist <= maxDist; dist++) {
            cell = cell.links[dir];
            if (cell === undefined)
                break;

            var target = cell.piece;
            if (dist >= minDist) {
                var captureStep = null;
                if (this.moveWhen == MoveDefinition.When.Capture) {
                    if (target == null)
                        continue; // needs to be a capture for this slide to be valid, and there is no piece here. But there might be pieces beyond this one.
                    else if (piece.canCapture(target))
                        captureStep = MoveStep.CreateCapture(target, cell, piece.ownerPlayer, baseMove.prevState.game.holdCapturedPieces);
                    else
                        break; // cannot capture this piece. Slides cannot pass over pieces, so there can be no more valid slides in this direction.
                }
                else if (this.moveWhen == MoveDefinition.When.Move && target != null)
                    break;
                else if (target != null) {
                    if (piece.canCapture(target))
                        captureStep = MoveStep.CreateCapture(target, cell, piece.ownerPlayer, baseMove.prevState.game.holdCapturedPieces);
                    else
                        break; // cannot capture this piece (probably own it)
                }

                var move = baseMove.clone();
                if (captureStep != null) {
                    move.addStep(captureStep);
                    move.addPieceReference(target, 'target');
                }

                move.addStep(MoveStep.CreateMove(piece, piece.position, cell, dir));

                if (this.conditions.isSatisfied(move))
                    moves.push(move);
            }

            if (target != null)
                break; // Slides can't pass intervening pieces. As this cell was occupied, can be no more valid slides in this direction.
        }
    }

    return moves;
};


Leap.prototype.appendValidNextSteps = function (baseMove, piece, previousStep) {
    var moves = [];

    if (this.piece != 'self') {// some steps will specify a different piece to act upon, rather than the piece being moved
        piece = baseMove.getPieceByRef(this.piece);
        if (piece == null) {
            console.log('piece ref not found: ' + this.piece);
            return moves;
        }
    }
    
    var dirs = baseMove.player.resolveDirection(this.dir, previousStep != null && previousStep.direction != null ? previousStep.direction : baseMove.player.forwardDir);

    for (var i = 0; i < dirs.length; i++) {
        var firstDir = dirs[i];

        var boardMaxDist = baseMove.prevState.game.board.getMaxDistance(piece.position, firstDir);
        var distances = this.dist.getRange(this.distMax, previousStep, boardMaxDist);
        var minDist = distances[0]; var maxDist = distances[1];

        var secondDirs = baseMove.player.resolveDirection(this.secondDir, firstDir);
        for (var j = 0; j < secondDirs.length; j++) {
            var secondDir = secondDirs[j];

            boardMaxDist = baseMove.prevState.game.board.getMaxDistance(piece.position, secondDir);
            distances = this.secondDist.getRange(null, previousStep, boardMaxDist);
            var minDist2 = distances[0]; var maxDist2 = distances[1];

            var straightCell = piece.position;

            for (var dist = 1; dist <= maxDist; dist++) {
                straightCell = straightCell.links[firstDir];
                if (straightCell === undefined)
                    break;

                if (dist < minDist)
                    continue; // not yet reached minimum straight distance, so don't turn

                var destCell = straightCell;
                for (var secondDist = 1; secondDist <= maxDist2; secondDist++) {
                    destCell = destCell.links[secondDir];
                    if (destCell === undefined)
                        break;

                    var target = destCell.piece;
                    if (secondDist >= minDist2) {
                        var captureStep = null;
                        if (this.moveWhen == MoveDefinition.When.Capture) {
                            if (target == null)
                                continue; // needs to be a capture for this leap to be valid, and there is no piece here. But there might be pieces beyond this one.
                            else if (piece.canCapture(target))
                                captureStep = MoveStep.CreateCapture(target, destCell, piece.ownerPlayer, baseMove.prevState.game.holdCapturedPieces);
                            else
                                continue; // cannot capture this piece
                        }
                        else if (this.moveWhen == MoveDefinition.When.Move && target != null)
                            break;
                        else if (target != null) {
                            if (piece.canCapture(target))
                                captureStep = MoveStep.CreateCapture(target, destCell, piece.ownerPlayer, baseMove.prevState.game.holdCapturedPieces);
                            else
                                continue; // cannot capture this piece
                        }

                        var move = baseMove.clone();
                        if (captureStep != null) {
                            move.addStep(captureStep);
                            move.addPieceReference(target, 'target');
                        }

                        move.addStep(MoveStep.CreateMove(piece, piece.position, destCell, secondDir));

                        if (this.conditions.isSatisfied(move))
                            moves.push(move);
                    }
                }
            }
        }
    }

    return moves;
};

Hop.prototype.appendValidNextSteps = function (baseMove, piece, previousStep) {
    var moves = [];

    if (this.piece != 'self') {// some steps will specify a different piece to act upon, rather than the piece being moved
        piece = baseMove.getPieceByRef(this.piece);
        if (piece == null) {
            console.log('piece ref not found: ' + this.piece);
            return moves;
        }
    }

    var dirs = baseMove.player.resolveDirection(this.dir, previousStep != null && previousStep.direction != null ? previousStep.direction : baseMove.player.forwardDir);

    
    for (var i = 0; i < dirs.length; i++) {
        var dir = dirs[i];

        var boardMaxDist = baseMove.prevState.game.board.getMaxDistance(piece.position, dir);
        var distances = this.distToHurdle.getRange(this.distToHurdleMax, previousStep, boardMaxDist);
        var minDistTo = distances[0]; var maxDistTo = distances[1];
        distances = this.distAfterHurdle.getRange(this.distAfterHurdleMax, previousStep, boardMaxDist); /* doesn't the 'board max' need recalculated based on hurdle position? */
        var minDistAfter = distances[0]; var maxDistAfter = distances[1];

        var hurdleCell = piece.position;
        for (var distTo = 1; distTo <= maxDistTo; distTo++) {
            hurdleCell = hurdleCell.links[dir];
            if (hurdleCell === undefined)
                break;

            var hurdle = hurdleCell.piece;
            if (hurdle == null)
                continue;

            if (distTo < minDistTo)
                break; // this piece is too close to hop over

            if (this.captureHurdle && !piece.canCapture(hurdle))
                break; // can't capture this hurdle, so can't hop it

            // now we've found the only possible hurdle. Do what we will with that, then there cannot be another hurdle, so break after
            var destCell = hurdleCell;
            for (var distAfter = 1; distAfter <= maxDistAfter; distAfter++) {
                destCell = destCell.links[dir];
                if (destCell === undefined)
                    break;

                var target = destCell.piece;

                if (distAfter >= minDistAfter) {
                    var captureStep = null;
                    if (this.moveWhen == MoveDefinition.When.Capture) {
                        if (target == null)
                            continue; // needs to be a capture for this hop to be valid, and there is no piece here. But there might be pieces beyond this one.
                        else if (piece.canCapture(target))
                            captureStep = MoveStep.CreateCapture(target, destCell, piece.ownerPlayer, baseMove.prevState.game.holdCapturedPieces);
                        else
                            break; // cannot capture this piece. Cannot hop over a second piece, so there can be no more valid hops in this direction.
                    }
                    else if (this.moveWhen == MoveDefinition.When.Move && target != null)
                        break;
                    else if (target != null) {
                        if (piece.canCapture(target))
                            captureStep = MoveStep.CreateCapture(target, destCell, piece.ownerPlayer, baseMove.prevState.game.holdCapturedPieces);
                        else
                            break; // cannot capture this piece (probably own it)
                    }

                    var move = baseMove.clone();
                    move.addPieceReference(hurdle, 'hurdle');

                    if (this.captureHurdle) {
                        var hurdleCaptureStep = MoveStep.CreateCapture(hurdle, hurdle.position, piece.ownerPlayer, baseMove.prevState.game.holdCapturedPieces);
                        move.addStep(hurdleCaptureStep);
                    }

                    if (captureStep != null) {
                        move.addPieceReference(target, 'target');
                        move.addStep(captureStep);
                    }

                    move.addStep(MoveStep.CreateMove(piece, piece.position, destCell, dir));

                    if (this.conditions.isSatisfied(move))
                        moves.push(move);
                }

                if (target != null)
                    break; // whether we capture it or not, can't pass a second piece
            }

            break;
        }
    }

    return moves;
};

Shoot.prototype.appendValidNextSteps = function (baseMove, piece, previousStep) {
    var moves = [];

    if (this.piece != 'self') {// some steps will specify a different piece to act upon, rather than the piece being moved
        piece = baseMove.getPieceByRef(this.piece);
        if (piece == null) {
            console.log('piece ref not found: ' + this.piece);
            return moves;
        }
    }

    var dirs = baseMove.player.resolveDirection(this.dir, previousStep != null && previousStep.direction != null ? previousStep.direction : baseMove.player.forwardDir);

    for (var i = 0; i < dirs.length; i++) {
        var firstDir = dirs[i];

        var boardMaxDist = baseMove.prevState.game.board.getMaxDistance(piece.position, firstDir);
        var distances = this.dist.getRange(this.distMax, previousStep, boardMaxDist);
        var minDist = distances[0]; var maxDist = distances[1];

        var secondDirs = baseMove.player.resolveDirection(this.secondDir, firstDir);
        if (secondDirs.length == 0 && this.secondDist == Distance.Zero)
            secondDirs = [firstDir]; // shoot moves may not have a second dir, but we need one to function here

        for (var j = 0; j < secondDirs.length; j++) {
            var secondDir = secondDirs[j];

            boardMaxDist = baseMove.prevState.game.board.getMaxDistance(piece.position, secondDir);
            distances = this.secondDist.getRange(null, previousStep, boardMaxDist);
            var minDist2 = distances[0]; var maxDist2 = distances[1];

            var straightCell = piece.position;

            for (var dist = 1; dist <= maxDist; dist++) {
                straightCell = straightCell.links[firstDir];
                if (straightCell === undefined)
                    break;

                if (dist < minDist)
                    continue; // not yet reached minimum straight distance, so don't turn

                var destCell = straightCell;
                for (var secondDist = 0; secondDist <= maxDist2; secondDist++) {
                    if (secondDist > 0) {
                        destCell = destCell.links[secondDir];
                        if (destCell === undefined)
                            break;
                    }

                    var target = destCell.piece;
                    if (secondDist >= minDist2) {
                        var captureStep = null;
                        if (target == null)
                            continue; // needs to be a capture for this shoot to be valid, and there is no piece here. But there might be pieces beyond this one.
                        else if (piece.canCapture(target))
                            captureStep = MoveStep.CreateCapture(target, destCell, piece.ownerPlayer, baseMove.prevState.game.holdCapturedPieces);
                        else
                            break; // cannot capture this piece

                        var move = baseMove.clone();
                        move.addStep(captureStep);
                        move.addPieceReference(target, 'target');

                        if (this.conditions.isSatisfied(move))
                            moves.push(move);
                    }
                }
            }
        }
    }

    return moves;
};

MoveLike.prototype.appendValidNextSteps = function (baseMove, piece, previousStep) {
    if (this.pieceref == 'target') {
        // difficult special case - we don't know what piece we're to move like until we've already made the move
        return this.appendMoveLikeTarget(baseMove, piece, previousStep);
    }

    var moves = [];

    var other = baseMove.getPieceByRef(this.piece);
    if (other == null) {
        console.log('piece ref not found: ' + this.piece);
        return moves;
    }

    // iterate over all possible moves of the specific piece being referenced, but *for* the piece being moved
    for (var i = 0; i < other.pieceType.moves; i++) {
        var moveDef = other.pieceType.moves[i];
        var possibilities = moveDef.appendValidNextSteps(baseMove, piece, previousStep);
        for (var j = 0; j < possibilities.length; j++)
            moves.push(possibilities[j]);
    }

    return moves;
};

MoveLike.AllowMoveLikeTarget = true;

MoveLike.prototype.appendMoveLikeTarget = function (baseMove, piece, previousStep) {
    var moves = [];

    if (!MoveLike.AllowMoveLikeTarget)
        return moves; // 'move like target' shouldn't try to capture other pieces using 'move like target' - that's nonsense

    MoveLike.AllowMoveLikeTarget = false;


    // loop through all possible piece types, try out every move, and then if the piece(s) captured are of the same type, use it
/*
	for (var i=0; i<state.game.pieceTypes.length; i++)
	{
		var pieceType = state.game.pieceTypes[i];
		for ( var j=0; j<pieceType.allMoves.length; j++ )
		{
			var possibilities = pieceType.allMoves[j].appendValidNextSteps(move, piece, previousStep);
			for ( var k=0; k<possibilities.length; k++ )
			{
				var newMove = possibilities[k];
				var moveIsOk = false;
				for ( var step = move.steps.length; step < newMove.steps.count; step++ ) // for each of the NEW steps added
				{
					if ( newMove.steps[step].toState != Piece.State.OnBoard )
					{// moving piece off board means its a capture (debatable)
						var target = newMove.steps[step].piece;
						if ( target == piece )
							continue; // if capturing like a 'kamakaze' piece, it should be OK to capture yourself
								
						if ( target != null && target.pieceType == pieceType && piece.canCapture(target) )
							moveIsOk = true;
							// debatable: do we want an option to allow capturing multiple pieces, as long as ONE is of the target type? If so, you'd break here instead of below
						else
						{
							moveIsOk = false;
							break;
						}
					}
				}
				if (moveIsOk) // must be a capture, and every piece captured must be of pieceType.Name
					moves.push(newMove);
			}
		}
	}    
*/

    MoveLike.AllowMoveLikeTarget = true;
    return moves;
};

ReferencePiece.prototype.appendValidNextSteps = function (baseMove, piece, previousStep) {
    var moves = [];
    var other = null;

    if (this.direction != null && this.distance != null) {
        var dirs = baseMove.player.resolveDirection(this.direction, previousStep != null && previousStep.direction != null ? previousStep.direction : baseMove.player.forwardDir);
        for (var i = 0; i < dirs.length; i++) {
            var dir = dirs[i];

            var boardMaxDist = baseMove.prevState.game.board.getMaxDistance(piece.position, dir);
            var distances = this.distance.getRange(null, previousStep, boardMaxDist);
            var minDist = distances[0]; var maxDist = distances[1];

            var cell = piece.position;
            for (var dist = 1; dist <= maxDist; dist++) {
                cell = cell.links[dir];
                if (cell === undefined)
                    break;

                if (dist < minDist)
                    continue;

                var target = cell.piece;
                if (target == null)
                    continue;
                
                if (this.otherRelationship != Player.Relationship.Any && piece.ownerPlayer.getRelationship(target.ownerPlayer) != this.otherRelationship)
                    continue;

                if (!target.typeMatches(this.otherType))
                    continue;

                var newMove = baseMove.clone();
                newMove.addPieceReference(target, this.refName);
                moves.push(newMove);
            }
        }
    }
	else
    {
        for (var i = 0; i < baseMove.prevState.game.players.length; i++) {
            var player = this.players[i];

            if (this.otherRelationship != Player.Relationship.Any && piece.ownerPlayer.getRelationship(player) != this.otherRelationship)
                continue;

            for (var j = 0; j < player.piecesOnBoard.length; j++) {
                var target = player.piecesOnBoard[j];

                if (!target.typeMatches(this.otherType))
                    continue;

                var newMove = baseMove.clone();
                newMove.addPieceReference(target, this.refName);
                moves.push(newMove);
            }
        }
	}

    return moves;
};

MoveGroup.prototype.appendValidNextSteps = function (baseMove, piece, previousStep) {
    var results = [];
    var prevStepMoves = [baseMove];
	for (var rep = 1; rep <= this.maxOccurs; rep++) {
	    for (var iStep = 0; iStep < this.contents.length; iStep++)
		{
	        var stepDef = this.contents[iStep];
			var stepMoves = [];
            
			for (var j=0; j<prevStepMoves.length; j++)
			{
			    var prevMove = prevStepMoves[j];
			    
			    for (var doStep = 0; doStep < prevMove.steps.length; doStep++)
			        prevMove.steps[doStep].perform(baseMove.prevState.game);
			    
			    var nextMovesForStep = stepDef.appendValidNextSteps(prevMove, piece, prevMove.steps.length > 0 ? prevMove.steps[prevMove.steps.length - 1] : previousStep);
					
				stepMoves = stepMoves.concat(nextMovesForStep);
				
				for (var undoStep = prevMove.steps.length - 1; undoStep >= 0; undoStep--)
				    prevMove.steps[undoStep].reverse(baseMove.prevState.game);
			}

			prevStepMoves = stepMoves;
			if (prevStepMoves.Count == 0)
				break;
		}
        
		if (prevStepMoves.length == 0) // this FULL iteration through the loop ended with no suitable moves
			break;
           
		if (rep >= this.minOccurs)
			results = results.concat(prevStepMoves);
	}
		
    // this group failed, but it wasn't essential, so return the previous move, as that's still valid on its own
    if (this.stepOutIfFail && results.length == 0 && baseMove.steps.count > 0)
        return [baseMove];

	return results;
};