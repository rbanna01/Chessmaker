function Player(name, forwardDir) {
    this.name = name;
    this.forwardDir = forwardDir;
    this.nextPlayer = null;
    
    this.piecesOnBoard = [];
    this.piecesHeld = [];
    this.piecesCaptured = [];
}

Player.parseAll = function (xml, board, boardSVG) {
    var setupXml = xml.children('setup');
    setupXml.children().each(function () {

        var playerName = this.getAttribute('name');
        var forwardDir = this.getAttribute('forwardDirection');
        var player = new Player(playerName, forwardDir);
        board.players[player.name] = player;

        $(this).children('piece').each(function () {

            var typeName = this.getAttribute('type');
            var type = PieceType.allTypes[typeName];
            if (type === undefined)
                throw 'Unrecognized piece type: ' + typeName;

            var position = this.getAttribute('location');

            var state = Piece.State.OnBoard;

            var cell = undefined;
            if (position == 'held') {
                state = Piece.State.Held;
            }
            else if (position == 'captured') {
                state = Piece.State.Captured;
            }
            else {
                cell = board.cells[position];
                if (cell === undefined) {
                    console.log('Unrecognised piece location: ' + position);
                    return;
                }
            }

            var piece = new Piece(player, type, cell, state, player);

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
                    player.piecesOnBoard.push(piece);
                    break;
                case Piece.State.Held:
                    player.piecesHeld.push(piece);
                    break;
            }
        })
    });

    // set up a cycle between the players. At some point, this is presumably gonna need to be more complex.
    var prevPlayer = null, firstPlayer = null;
    for (var name in board.players) {
        var player = board.players[name];
        if (prevPlayer == null)
            firstPlayer = player;
        else
            prevPlayer.nextPlayer = player;

        prevPlayer = player;
    }
    prevPlayer.nextPlayer = firstPlayer;
    board.currentPlayer = firstPlayer;
}

Player.prototype.calculatePossibleMoves = function () {
    var moves = [];

    return moves;
}