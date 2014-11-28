function EndOfGame(game) {
    this.game = game;
    this.checks = [];
}

EndOfGame.parse = function (xmlNode, game) {
    var endOfGame = new EndOfGame(game);
    
    var childNodes = xmlNode.childNodes;
    for (var i = 0; i < childNodes.length; i++) {
        var childNode = childNodes[i];

        var checkType;
        switch (childNode.nodeName) {
            case 'win':
                checkType = EndOfGame.CheckType.Win; break;
            case 'lose':
                checkType = EndOfGame.CheckType.Lose; break;
            case 'draw':
                checkType = EndOfGame.CheckType.Draw; break;
            default:
                throw 'Unexpected endOfGame node type: ' + childNode.name;
        }

        var conditions = Conditions.parse(childNode);
        endOfGame.checks.push(new EndOfGameCheck(checkType, conditions));
    }

    return endOfGame;
};

EndOfGame.createDefault = function (game) {
    var endOfGame = new EndOfGame(game);

    var condition = new Conditions();
    // add a <pieceCount owner="self" check="equals">0</pieceCount> condition here
    var outOfPieces = new EndOfGameCheck(EndOfGame.CheckType.Lose, condition);
    endOfGame.checks.push(outOfPieces);

    condition = new Conditions();
    // add a <cannotMove /> condition here
    var outOfMoves = new EndOfGameCheck(EndOfGame.CheckType.Draw, condition);
    endOfGame.checks.push(outOfMoves);

    return endOfGame;
};

EndOfGame.prototype.check = function () {
    // return the victor, or null if nobody wins. return undefined if the game isn't over yet.

    // if the next player is null, we've reached the end of the turn order
    var nextPlayer = this.game.turnOrder.getNextPlayer();
    if (nextPlayer == null)
        return null; // todo: this should consider the stated conditions, but we ALWAYS want to end the game in this scenario. Save off a special "out of turns" conditions?
    this.game.turnOrder.stepBackward();

    /*
    for (var i = 0; i < this.checks.length; i++) {
        var check = this.checks[i];
        if (check.conditions.isSatisfied(MOVE_ARGH_DONT_HAVE_A_MOVE, this.game))
            switch (check.type) {
                case EndOfGame.CheckType.Win:
                    return this.game.currentPlayer;
                case EndOfGame.CheckType.Lose:
                    ;// argh, somehow remove this player from the runnings, but don't end the game if there's more than one TEAM remaining ... ack
                    break;
                case EndOfGame.CheckType.Draw:
                    return null;
            }
    }
    */

    // for now, just count remaining pieces. Eventually, should look at victory conditions.
    var playerWithPieces = null;
    for (var i = 0; i < this.game.players.length; i++) {
        var player = this.game.players[i];
        if (player.piecesOnBoard.length == 0)
            continue;
        else if (playerWithPieces == null)
            playerWithPieces = player;
        else
            return undefined; // multiple players still have pieces, game is not over.
    }

    // return null if nobody has pieces left: this is stalemate. Otherwise, only one player has pieces left: they win.
    return playerWithPieces;
};

EndOfGame.CheckType = {
    Win: 0,
    Lose: 1,
    Draw: 2
};

function EndOfGameCheck(type, conditions) {
    this.type = type;
    this.conditions = conditions;
}