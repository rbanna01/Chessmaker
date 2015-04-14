"use strict";

var performMove;

function loadDefinition(xml) {
    console.time('load');
    if (!Module.ccall('Initialize', 'bool', ['string'], [xml])) {
        console.log('An error occurred initializing the game');
        return;
    }
    console.timeEnd('load');
    
    // set players to the correct types
    var setPlayerLocal = Module.cwrap('SetPlayerLocal', 'bool', ['number']);
    var setPlayerRemote = Module.cwrap('SetPlayerRemote', 'bool', ['number']);
    var setPlayerAI = Module.cwrap('SetPlayerAI', 'bool', ['number', 'string']);
    for (var i = 1; i <= players.length; i++) {
        var player = players[i - 1];
        if (player === true) {
            if (!setPlayerLocal(i))
                console.log('Unable to set player #' + i + ' to play locally');
        }
        else if (player === false) {
            if (!setPlayerRemote(i))
                console.log('Unable to set player #' + i + ' to play remotely');
        }
        else {
            if (!setPlayerAI(i, player))
                console.log('Unable to set player #' + i + ' to use "' + player + '" AI');
        }
    }
    
    // these are just for testing
    performMove = Module.cwrap('PerformMove', 'number', ['string']);

    initializeUI();
}

function initializeUI() {
    var boardSVG = Module.ccall('GetBoardSVG', 'string');
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

var possibleMovesByCell = {};
function addPossibleMove(notation, fromCell, toCell) {
    notation = Pointer_stringify(notation);
    fromCell = Pointer_stringify(fromCell);
    toCell = Pointer_stringify(toCell);

    var fromCellMoves = possibleMovesByCell[fromCell];
    if (fromCellMoves === undefined) {
        fromCellMoves = {};
        possibleMovesByCell[fromCell] = fromCellMoves;
    }

    var toCellMoves = fromCellMoves[toCell];
    if (toCellMoves === undefined) {
        toCellMoves = [];
        fromCellMoves[toCell] = toCellMoves;
    }
    toCellMoves.push(notation);
}

function clearPossibleMoves() {
    possibleMovesByCell = {};
}

function setCurrentPlayer(name, isLocal) {
    clearPossibleMoves();

    name = Pointer_stringify(name);
    $('#nextMove').text(name.substr(0, 1).toUpperCase() + name.substr(1) + ' to move');

    if (isLocal)
        $('#wait').hide();
    else
        $('#wait').show();
}

function showGameEnd(message) {
    clearPossibleMoves();

    message = Pointer_stringify(message);
    $('#nextMove').text(message);
    $('#wait').hide();
}

function logMove(player, number, notation) {
    var historyDiv = $('#moveHistory');

    $('<div/>', {
        class: 'move ' + Pointer_stringify(player),
        number: number,
        html: Pointer_stringify(notation)
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

var selectedCell;
function cellClicked(e) {
    if (hasClass(this, 'selected'))
        clearSelection();
    else if (hasClass(this, 'option') && selectedCell != null) {
        selectMove(this);
        clearSelection();
    }
    else
        showMoveOptions(this);

    return false;
}

function clearSelection() {
    selectedCell = null;

    var paths = $('#render .selected');
    if (paths.length > 0)
        remClass(paths, 'selected');

    var paths = $('#render .option');
    if (paths.length > 0)
        remClass(paths, 'option');
}

function showMoveOptions(clicked) {
    if (possibleMovesByCell.length == 0)
        return;

    var movesFromThisCell = possibleMovesByCell[clicked.getAttribute('id')];
    if (movesFromThisCell === undefined) {
        // if no cells currently highlighted, highlight cells containing pieces that CAN move. Otherwise, clear the selection.
        if ($('#render .cell.option').length > 0) {
            clearSelection();
            return;
        }

        for (var fromCell in possibleMovesByCell) {
            var cell = document.getElementById(fromCell);
            addClass($(cell), 'option');
        }
        selectedCell = null;
        return;
    }

    clearSelection();

    addClass($(clicked), 'selected');
    selectedCell = clicked;

    for (var toRef in movesFromThisCell) {
        var toCell = document.getElementById(toRef);
        addClass($(toCell), 'option');
    }
}

function selectMove(clicked) {
    var fromCell = selectedCell.getAttribute('id');
    var toCell = clicked.getAttribute('id');

    var moves = possibleMovesByCell[fromCell][toCell];
    if (moves === undefined) {
        console.log('error: invalid move selection');
        return;
    }

    if (moves.length == 1)
        performMove(moves[0]);
    else {
        console.log('todo: add a popup showing the move options for these cells');
    }
}