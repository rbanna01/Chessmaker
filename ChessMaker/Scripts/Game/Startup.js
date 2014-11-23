var game;
function loadDefinition(xml) {
    xml = xml.firstChild;
    
    game = new Game();
    game.board = new Board(game);

    var defs = SVG('defs');
    var boardXml = xml.firstChild;
    var boardSVG = game.board.loadSVG(boardXml, defs);

    var dirsXml = boardXml.nextSibling;
    game.board.parseDirections(dirsXml);

    var piecesXml = dirsXml.nextSibling;
    PieceType.parseAll(piecesXml, defs);

    var setupXml = piecesXml.nextSibling;
    Player.parseAll(setupXml, game, boardSVG);

    // this needs enhanced to also allow for remote players
    if (typeof AIs != 'undefined') {
        var i = -1;
        for (var name in game.players) {
            i++;
            var AI = AIs[i];
            if (AI == null)
                continue;

            var player = game.players[name];
            player.type = Player.Type.AI;
            player.AI = AIs[i];

            if (i == AIs.length - 1)
                break;
        }
    }

    document.getElementById('main').appendChild(boardSVG);

    $('#render path.cell').click(cellClicked);
    /*
    $('#main').click(function (e) {
        if (!e.shiftKey && !e.ctrlKey)
            clearSelection();
    });
    */
    calculateRatio();
    resizeBoard();
    $(window).resize(function () { resizeBoard(); });

    $('#moveHistory').slideDown();

    if (game.showCaptured)
        $('#captureSection').slideDown();
    else
        $('#captureSection').remove();

    if (game.showHeld)
        $('#heldSection').slideDown();
    else
        $('#heldSection').remove();

    game.startNextTurn();
}

function cellClicked(e) {
    if (hasClass(this, 'selected'))
        clearSelection();
    else if (hasClass(this, 'option')) {
        selectMove(this);
        clearSelection();
    }
    else {
        clearSelection();
        addClass($(this), 'selected');

        var cell = game.board.cells[this.getAttribute('id')];
        if (game.currentPlayer.type == Player.Type.Local && cell != null && cell.piece != null && cell.piece.ownerPlayer == game.currentPlayer) {
            console.log('clicked ' + cell.piece.ownerPlayer.name + ' ' + cell.piece.pieceType.name + ' in cell ' + cell.name);
            game.showMoveOptions(cell.piece);
        }
        else
            console.log('clicked cell ' + cell.name);
    }

    return false;
}

function clearSelection() {
    var paths = $('#render .selected');
    if (paths.length == 0)
        return;

    remClass(paths, 'selected');

    var paths = $('#render .option');
    if (paths.length == 0)
        return;

    remClass(paths, 'option');
}

function selectMove(clicked) {
    var pieceCell = game.board.getCellBySelector('.selected');
    var destCell = game.board.getCellByElement(clicked);

    if (pieceCell == null || destCell == null)
        return;

    var piece = pieceCell.piece;
    if (piece == null)
        return;

    game.selectMoveByCell(piece, destCell);
}