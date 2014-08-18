function PieceType() {
    this.name = null;
    this.value = 1;
    this.capturedAs = null;
    this.moves = [];
    //this.promotionOpportunities = [];
}

PieceType.allTypes = {};

// read definitions from xml
PieceType.parseAll = function (piecesNode) {
    $(piecesNode).children().each(function () {
        var type = PieceType.parse(this);

        if (definitions.hasOwnProperty(type.name)) // check for dupes
            throw "Duplicate piece types detected: two piece types exist with the name '" + type.name + "'. This is not allowed.";
        PieceType.allTypes[type.name] = type;
    });

    // resolve references to other types
    for (key in PieceType.allTypes) {
        var type = PieceType.allTypes[key];

        if (type.capturedAs != null)
            type.capturedAs = PieceType.allTypes[type.capturedAs];
        else
            type.capturedAs = type;

        //for (var i = 0; i < type.promotionOpportunities.length; i++)
            //type.promotionOpportunities[i].resolveOptions(definitions);
    }
};

PieceType.parse = function (xmlNode) {
    var type = new PieceType();

    type.name = $(xmlNode).children("name").text();

    var value = $(xmlNode).attr("value");
    if (value !== undefined)
        type.value = value;

    var capturedAs = $(xmlNode).children("captured_as");
    if (capturedAs !== undefined)
        type.capturedAs = $(capturedAs).text();

    var movesNode = $(xmlNode).children("moves");
    if (movesNode !== undefined)
        $(movesNode).children().each(function () {
            type.allMoves.push(MoveDefinition.parse(this, true));
        });
    /*
    var specialNode = $(xmlNode).children("special");
    if (specialNode != undefined)
        $(specialNode).children().each(function () {
            if (this.tagName == "royal") // consider: while these properties should remain on pieces IN CODE (for game logic's sake) - shouldn't royalty in the DEFINITION be handled via victory conditions? lose when any/all pieces of given type are checkmated/captured/are in check/aren't in check? loading code could then apply royal / antiroyal values
                type.royalty = PieceType.RoyalState.Royal;
            else if (this.tagName == "anti_royal")
                type.royalty = PieceType.RoyalState.AntiRoyal;
            else if (this.tagName == "immobilize")
                type.immobilizations.push(Immobilization.parse(this));
            else // consider: other special types: blocks (as per immobilize, but instead prevents pieces entering a square), kills (kills pieces in target squares without expending a move)
                throw "Unexpected node name in piece's \"special\" tag: " + this.tagName;
        });
    */
    /*
    $(xmlNode).children("promotion").each(function () {
        type.promotionOpportunities.push(PromotionOpportunity.parse(this));
    });
    */
    return type;
};

PieceType.prototype.someFunction = function () {

};