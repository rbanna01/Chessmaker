$(function () {
    
});

var board;
function loadDefinition(xml) {
    xml = $(xml.firstChild);
    
    board = new Board();

    var defs = SVG('defs');
    var boardSVG = board.loadSVG(xml, defs);

    PieceType.parseAll(xml.children('pieces'), defs);

    loadInitialLayout(xml, board, boardSVG);

    $('#main').append(boardSVG);

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
}

function loadInitialLayout(xml, board, boardSVG) {
    var setupXml = xml.children('setup');
    setupXml.children().each(function () {

        var player = new Player(this.getAttribute('name'));
        board.players[player.name] = player;

        $(this).children('piece').each(function () {

            var typeName = this.getAttribute('type');
            var type = PieceType.allTypes[typeName];
            if (type === undefined)
                throw 'Unrecognized piece type: ' + typeName;

            var location = this.getAttribute('location');

            var state = Piece.State.OnBoard;

            var cell = undefined;
            if (location == 'held') {
                state = Piece.State.Held;
            }
            else if (location == 'captured') {
                state = Piece.State.Captured;
            }
            else {
                cell = board.cells[location];
                if (cell === undefined) {
                    console.log('Unrecognised piece location: ' + location);
                    return;
                }
            }

            var piece = new Piece(player, type, cell, state);

            if (cell != null) {
                if (cell.piece == null)
                    cell.piece = piece;
                else
                    throw 'Cannot add ' + piece.ownerPlayer.name + ' ' + piece.pieceType.name + ' to cell ' + cell.name + ', as it already has a ' + cell.piece.ownerPlayer.name + ' ' + cell.piece.pieceType.name + ' in it';

                board.pieces.push(piece);
            }
            player.pieces.push(piece);

            if (state == Piece.State.OnBoard)
                boardSVG.appendChild(piece.createImage());
        })
    });
}

function cellClicked(e) {
    if (hasClass(this, 'selected'))
        remClass($(this), 'selected');
    else {
        clearSelection();
        addClass($(this), 'selected');
        logCellInfo(this);
    }

    return false;
}

function clearSelection() {
    var paths = $('#render .selected');
    if (paths.length == 0)
        return;

    remClass(paths, 'selected');
}

function logCellInfo(cellNode) {
    var id = cellNode.getAttribute('id');

    var cell = board.cells[id];

    if (cell.piece == null)
        console.log('Clicked ' + cell.name + ', which is empty');
    else
        console.log('Clicked ' + cell.piece.ownerPlayer.name + ' ' + cell.piece.pieceType.name + ' at ' + cell.name);
}