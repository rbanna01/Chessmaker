function Player(name, forwardDir) {
    this.name = name;
    this.forwardDir = forwardDir;
    this.nextPlayer = null;
    this.type = Player.Type.Local;
    this.pieceListX = 0;
    
    this.piecesOnBoard = [];
    this.piecesHeld = [];
    this.piecesCaptured = [];
}

Player.parseAll = function (xml, game, boardSVG) {
    var players = xml.childNodes;
    for (var i=0; i<players.length; i++) {
        var playerNode = players[i];

        var playerName = playerNode.getAttribute('name');
        var forwardDir = playerNode.getAttribute('forwardDirection');
        var player = new Player(playerName, forwardDir);
        game.players[player.name] = player;

        var pieces = playerNode.childNodes;
        for (var j = 0; j < pieces.length; j++) {
            var pieceNode = pieces[j];

            var typeName = pieceNode.getAttribute('type');
            var type = PieceType.allTypes[typeName];
            if (type === undefined)
                throw 'Unrecognized piece type: ' + typeName;

            var position = pieceNode.getAttribute('location');

            var state = Piece.State.OnBoard;
            var stateOwner;

            var cell = undefined;
            if (position == 'held') {
                state = Piece.State.Held;
                stateOwner = player;
            }
            else if (position == 'captured') {
                state = Piece.State.Captured;
                stateOwner = player;
            }
            else {
                cell = game.board.cells[position];
                if (cell === undefined) {
                    console.log('Unrecognised piece location: ' + position);
                    return;
                }
                stateOwner = null;
            }

            var piece = new Piece(player, type, cell, state, stateOwner);

            switch (state) {
                case Piece.State.OnBoard:
                    boardSVG.appendChild(piece.createImage());
                    player.piecesOnBoard.push(piece);

                    if (cell.piece == null)
                        cell.piece = piece;
                    else
                        throw 'Cannot add ' + piece.ownerPlayer.name + ' ' + piece.pieceType.name + ' to cell ' + cell.name + ', as it already has a ' + cell.piece.ownerPlayer.name + ' ' + cell.piece.pieceType.name + ' in it';
                    break;
                case Piece.State.Captured:
                    player.piecesCaptured.push(piece);
                    break;
                case Piece.State.Held:
                    player.piecesHeld.push(piece);
                    break;
            }
        }
    }

    game.setupTurnOrder();
}

// whether its a global, relative or group name, this should output an array of resultant global directions
Player.prototype.resolveDirection = function (name, prevDir) {
    if (name == 'forward')
        return [this.forwardDir];

    return game.board.resolveDirection(name, prevDir);
};

Player.Type = {
    Local: 0,
    AI: 1,
    Remote: 2
};

Player.Relationship = {
    Any: 0,
    Self: 1,
    Enemy: 2,
    Ally: 3
};

Player.Relationship.parse = function (val) {
    if (val == null)
        return "any";
    switch (val) {
        case "any":
            return Player.Relationship.Any;
        case "self":
            return Player.Relationship.Self;
        case "enemy":
            return Player.Relationship.Enemy;
        case "ally":
            return Player.Relationship.Ally;
        default:
            throw "Unexpected relationship value: " + val;
    }
};

Player.prototype.getRelationship = function (otherPlayer) {
    if (this == otherPlayer)
        return Player.Relationship.Self;
    else
        return Player.Relationship.Enemy;
};