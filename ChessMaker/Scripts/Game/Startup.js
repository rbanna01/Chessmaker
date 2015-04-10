"use strict";

function loadDefinition(xml) {
    console.time('load');
    if (!Module.ccall('Initialize', 'bool', ['string'], [xml])) {
        console.log('An error occurred initializing the game');
        return;
    }
    console.timeEnd('load');

    initializeUI();
}

function initializeUI() {
    var boardSVG = Module.ccall('GetBoardSVG', 'string', [], []);
    document.getElementById('main').innerHTML = boardSVG;
    
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
    /*
    // calculate column positions within captured piece controls
    var listX = 25, listXStep = 50; // stepListX should be calculated based on the pixel width of ... either of the two containers. Probably. And whether or not we have multiple columns.
    for (var i = 0; i < game.players.length; i++) {
        var player = game.players[i];
        player.pieceListX = listX;
        listX += listXStep;
    }
    */
    $('#moveHistory').slideDown();
    /*
    if (game.showCaptured)
        $('#captureSection').slideDown();
    else
        $('#captureSection').remove();

    if (game.showHeld)
        $('#heldSection').slideDown();
    else
        $('#heldSection').remove();
    */
}

function setCurrentPlayer(name, isLocal) {
    name = Pointer_stringify(name);
    $('#nextMove').text(name.substr(0, 1).toUpperCase() + name.substr(1) + ' to move');

    if (isLocal)
        $('#wait').hide();
    else
        $('#wait').show();
}

function logMove(player, number, notation) {
    var historyDiv = $('#moveHistory');

    $('<div/>', {
        class: 'move ' + Pointer_stringify(player),
        number: number,
        html: notation
    }).appendTo(historyDiv);

    historyDiv.get(0).scrollTop = historyDiv.get(0).scrollHeight;
}

function movePiece(pieceID, state, stateOwner, posX, posY, owner, appearance) {
    var element = document.getElementById('p' + pieceID);

    owner = Pointer_stringify(owner);
    appearance = Pointer_stringify(appearance);

    // at some point, use state and stateOwner to determine where to place piece elements outwith the board

    element.setAttribute('class', 'piece ' + owner);
    element.setAttribute('x', posX);
    element.setAttribute('y', posY);
    element.setAttributeNS('http://www.w3.org/1999/xlink', 'href', appearance);
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

        /*
        var cell = game.board.cells[this.getAttribute('id')];
        if (game.state.currentPlayer.type == Player.Type.Local && cell != null && cell.piece != null && cell.piece.ownerPlayer == game.state.currentPlayer) {
            console.log('clicked ' + cell.piece.ownerPlayer.name + ' ' + cell.piece.pieceType.name + ' in cell ' + cell.name);
            game.showMoveOptions(cell.piece);
        }
        else
            console.log('clicked cell ' + cell.name);
        */
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
    /*
    var pieceCell = game.board.getCellBySelector('.selected');
    var destCell = game.board.getCellByElement(clicked);

    if (pieceCell == null || destCell == null)
        return;

    var piece = pieceCell.piece;
    if (piece == null)
        return;

    game.selectMoveByCell(piece, destCell);
    */
}