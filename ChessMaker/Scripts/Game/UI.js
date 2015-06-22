"use strict";

var worker;
var numPlayers;
var capturedPieces, heldPieces;

$(function () {
    $('#moveDisambiguation, #helpPopup').dialog({
        modal: true,
        autoOpen: false
    });

    $('#moveDisambiguationOptions').on('click', 'li', selectMoveFromPopup);

    $('#helpLink').click(function () { $('#helpPopup').dialog('open'); });
});

function initializeGame(workerUrl, defUrl, players) {
    numPlayers = players.length;

    capturedPieces = {}; heldPieces = {};
    for (var i = 1; i <= numPlayers; i++)
        capturedPieces[i] = heldPieces[i] = 0;

    console.time('load all');

    worker = new Worker(workerUrl);
    worker.onmessage = function (event) {
        switch (event.data[0]) {
            case 'init':
                initializeUI(event.data[1], event.data[2], event.data[3]);
                break;
            case 'error':
                console.error(event.data[1]);
                break;
            case 'poss':
                addPossibleMove(event.data[1], event.data[2], event.data[3], event.data[4]);
                break;
            case 'player':
                startTurn(event.data[1], event.data[2]);
                break;
            case 'end':
                showGameEnd(event.data[1]);
                break;
            case 'log':
                logMove(event.data[1], event.data[2], event.data[3], event.data[4], event.data[5]);
                break;
            case 'move':
                movePiece(event.data[1], event.data[2], event.data[3], event.data[4], event.data[5], event.data[6]);
                break;
            case 'msg':
                showMessage(event.data[1]);
                break;
            default:
                console.log("Didn't understand message from worker: " + event.data[0]);
                break;
        }
    };

    worker.postMessage(['load', defUrl, players]);
}

function initializeUI(boardSVG, showCaptured, showHeld) {
    var pos = boardSVG.indexOf('</defs>');

    var gradients =
'<radialgradient id="lastFromLight" cx="50%" cy="50%" r="50%" fx="50%" fy="50%"><stop class="stopLightHighlight" offset="20%" /><stop class="stopLight" offset="50%" /></radialgradient>' +
'<radialgradient id="lastFromMid" cx="50%" cy="50%" r="50%" fx="50%" fy="50%"><stop class="stopMidHiglight" offset="20%" /><stop class="stopMid" offset="50%" /></radialgradient>' +
'<radialgradient id="lastFromDark" cx="50%" cy="50%" r="50%" fx="50%" fy="50%"><stop class="stopDarkHighlight" offset="20%" /><stop class="stopDark" offset="50%" /></radialgradient>' +
'<radialgradient id="lastToLight" cx="50%" cy="50%" r="50%" fx="50%" fy="50%"><stop class="stopLight" offset="80%" /><stop class="stopLightHighlight" offset="100%" /></radialgradient>' +
'<radialgradient id="lastToMid" cx="50%" cy="50%" r="50%" fx="50%" fy="50%"><stop class="stopMid" offset="80%" /><stop class="stopMidHighlight" offset="100%" /></radialgradient>' +
'<radialgradient id="lastToDark" cx="50%" cy="50%" r="50%" fx="50%" fy="50%"><stop class="stopDark" offset="80%" /><stop class="stopDarkHighlight" offset="100%" /></radialgradient>';
    boardSVG = boardSVG.substr(0, pos) + gradients + boardSVG.substr(pos);

    document.getElementById('main').innerHTML = boardSVG;
    console.timeEnd('load all');

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
    
    if (showCaptured)
        $('#captureSection').slideDown();
    else
        $('#captureSection').remove();

    if (showHeld)
        $('#heldSection').slideDown();
    else
        $('#heldSection').remove();
}

var possibleMovesByCell = {};
function addPossibleMove(notation, fromCell, toCell, disambiguation) {
    if (disambiguation == '')
        disambiguation = notation;

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

    toCellMoves.push([notation, disambiguation]);
}

function clearPossibleMoves() {
    possibleMovesByCell = {};
}

function startTurn(msg, isLocal) {
    clearPossibleMoves();

    $('#nextMove').text(msg);

    if (isLocal)
        $('#wait').hide();
    else
        $('#wait').show();

    $('#messageSection').slideUp(400, function () { $('#message').empty(); });
}

function showGameEnd(message) {
    clearPossibleMoves();

    $('#nextMove').text(message);
    $('#wait').hide();
}

function logMove(player, number, notation, fromRef, toRef) {
    var historyDiv = $('#moveHistory');

    $('<div/>', {
        class: 'move ' + player,
        number: number,
        html: notation
    }).appendTo(historyDiv);

    historyDiv.get(0).scrollTop = historyDiv.get(0).scrollHeight;

    remClass($('#render .cell.lastMoveFrom'), 'lastMoveFrom');
    remClass($('#render .cell.lastMoveTo'), 'lastMoveTo');

    if (fromRef != '') {
        var fromCell = document.getElementById(fromRef);
        if (fromCell != null)
            addClassSingle(fromCell, 'lastMoveFrom');
    }

    if (toRef != '') {
        var toCell = document.getElementById(toRef);
        if (toCell != null)
            addClassSingle(toCell, 'lastMoveTo');
    }
}

function movePiece(pieceID, state, posX, posY, owner, appearance) {
    var element = document.getElementById('p' + pieceID);
    var board = document.getElementById('render');

    element.setAttribute('class', 'piece player' + owner);

    var changeApp = function (x) {
        element.setAttributeNS('http://www.w3.org/1999/xlink', 'href', appearance);
    };
    
    if (state == 'board') {
        if (element.parentElement == board) {// already on board - remove & add so its on top for animation purposes
            $(element).velocity({ x: posX, y: posY }, { duration: 600, easing: "ease-in-out", begin: function (elements) {
                    board.removeChild(element);
                    board.appendChild(element);
                },
                complete: changeApp
            });
        }
        else {
            $(element) // moving onto board, so fade out, move instantly, then fade in
                .velocity({ opacity: 0 }, {
                    complete: function (elements) {
                        changeApp();

                        var container = element.parent;
                        container.removeChild(element);
                        board.appendChild(element);
                        // todo: determine the counter for this player's captured/held pieces, and reduce it by one. Slide everything else in the container up to account for the empty space. And the squeezing.
                    }
                })
                .velocity({ x: posX, y: posY }, { duration: 0 })
                .velocity({ opacity: 1 });
        }
    }
    else {
        var counter = state == 'captured' ? capturedPieces : heldPieces;
        var container = document.getElementById(state);  // 'captured' or 'held'
        counter[owner]++;

        posX = owner * 196 / (numPlayers + 1);
        posY = counter[owner] * 40 - 20; // todo: this should squeeze to fit things in. 5 can fit without squeezing.
        $(element)
            .velocity({ opacity: 0 }, { complete: function (elements) {
                    changeApp();

                    element.parentElement.removeChild(element);
                    if (container != null)
                        container.appendChild(element);
                }
            })
            .velocity({ x: posX, y: posY }, { duration: 0 })
            .velocity({ opacity: 1 });
    }
}

var selectedCell;
function cellClicked(e) {
    if (hasClass(this, 'selected'))
        clearSelection();
    else if (hasClass(this, 'option') && selectedCell != null) {
        selectMoveFromCell(this);
        clearSelection();
    }
    else
        showMoveOptions(this);

    $('#pieceName').text("piece name");

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

function selectMoveFromCell(clicked) {
    var fromCell = selectedCell.getAttribute('id');
    var toCell = clicked.getAttribute('id');

    var moves = possibleMovesByCell[fromCell][toCell];
    if (moves === undefined) {
        console.log('error: invalid move selection');
        return;
    }

    if (moves.length == 1) {
        var move = moves[0];
        worker.postMessage(['move', move[0]]);
    }
    else {
        var output = '';
        for (var i = 0; i < moves.length; i++) {
            var move = moves[i];
            output += '<li notation="' + move[0] + '">' + move[1] + '</li>';
        }
        $('#moveDisambiguationOptions').html(output);
        $('#moveDisambiguation').dialog('open');
    }
}

function selectMoveFromPopup() {
    var move = this.getAttribute('notation');
    worker.postMessage(['move', move]);
    $('#moveDisambiguation').dialog('close');
}

function showMessage(msg) {
    $('#message').append(msg);
    $('#messageSection').slideDown(400, function () { $("#messageSection").effect('highlight', [], 1000); });
}