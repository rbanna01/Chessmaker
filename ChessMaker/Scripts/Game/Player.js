﻿function Player(name, forwardDir) {
    this.name = name;
    this.forwardDir = forwardDir;
    this.nextPlayer = null;
    
    this.piecesOnBoard = [];
    this.piecesHeld = [];
    this.piecesCaptured = [];
}

Player.parseAll = function (xml, game, boardSVG) {
    var setupXml = xml.children('setup');
    setupXml.children().each(function () {

        var playerName = this.getAttribute('name');
        var forwardDir = this.getAttribute('forwardDirection');
        var player = new Player(playerName, forwardDir);
        game.players[player.name] = player;

        $(this).children('piece').each(function () {

            var typeName = this.getAttribute('type');
            var type = PieceType.allTypes[typeName];
            if (type === undefined)
                throw 'Unrecognized piece type: ' + typeName;

            var position = this.getAttribute('location');

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
                    player.piecesOnBoard.push(piece);
                    break;
                case Piece.State.Held:
                    player.piecesHeld.push(piece);
                    break;
            }
        })
    });

    game.setupTurnOrder();
}

Player.prototype.calculatePossibleMoves = function () {
    var moves = [];

    return moves;
}