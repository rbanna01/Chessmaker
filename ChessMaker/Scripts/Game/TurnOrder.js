function TurnOrder() {
    this.currentStep = null;
}

TurnOrder.parse = function (xmlNode, game) {
    var turnOrder = new TurnOrder();
    var players = {};
    for (var i = 0; i < game.players.length; i++) {
        var p = game.players[i]
        players[p.name] = p;
    }

    var fakeStep = new TurnStep(null);
    fakeStep.next = TurnOrder.parseRepeat(xmlNode, players, true);
    turnOrder.currentStep = fakeStep;
    return turnOrder;
};

TurnOrder.parseRepeat = function (xmlNode, players, topLevel) {
    var count = xmlNode.getAttribute('count');
    if (count != null)
        count = turnRepeat.parseCount(count);

    var children = xmlNode.childNodes;

    var firstChild = null; var lastChild = null;

    for (var i = 0; i < children.length; i++) {
        var childNode = children[i];
        
        var child;
        if (childNode.tagName == 'repeat')
            child = TurnOrder.parseRepeat(childNode, players, false);
        else if (childNode.tagName == 'turn') {
            var playerName = childNode.getAttribute('player');
            var player = players[playerName];
            if (player === undefined)
                console.log('Invalid player in turn order: ' + playerName);
            child = new TurnStep(player);
        }

        if (i == 0)
            firstChild = child;
        else {
            lastChild.next = child;
            child.prev = lastChild;
        }
        lastChild = child;
    }
    
    if (topLevel)
        return firstChild;

    var repeat = new TurnRepeat(count, firstChild, lastChild);
    firstChild.prev = repeat;
    lastChild.next = repeat;

    return repeat;
};

TurnOrder.createDefault = function (game) {
    var turnOrder = new TurnOrder();

    var firstP = null; var lastP = null;
    for (var i = 0; i < game.players.length; i++) {
        var player = game.players[i];
        
        var p = new TurnStep(player);
        if (i == 0)
            firstP = p;
        else {
            lastP.next = p;
            p.prev = lastP;
        }
        lastP = p;
    }

    var repeat = new TurnRepeat(null, firstP, lastP);

    firstP.prev = repeat;
    lastP.next = repeat;

    var fakeStep = new TurnStep(null);
    fakeStep.next = repeat;
    turnOrder.currentStep = fakeStep;

    return turnOrder;
};

TurnOrder.prototype.getNextPlayer = function (xml) {
    if (this.currentStep == null)
        return null;

    do {
        this.currentStep = this.currentStep.getNext();
        if (this.currentStep == null)
            break;
    } while (this.currentStep.isGroup);
    
    return this.currentStep.player;
};

TurnOrder.prototype.stepBackward = function () {
    if (this.currentStep == null)
        return;

    do {
        this.currentStep = this.currentStep.getPrev();
        if (this.currentStep == null)
            break;
    } while (this.currentStep.isGroup);
};

function TurnRepeat(count, firstChild, lastChild) {
    this.currentIteration = 0;
    this.count = count;
    this.firstChild = firstChild;
    this.lastChild = lastChild;
    this.next = null;
    this.prev = null;
    this.isGroup = true;
}

TurnRepeat.parseCount = function (val) {
    // todo: this could reference several different variables, it might not be just an integer
    return parseInt(val);
};

TurnRepeat.prototype.getCount = function () {
    if (this.count === null || typeof this.count == 'number')
        return this.count;

    // otherwise, count is some variable. Get an integer out of that, somehow.
    throw 'Got non-integer count on TurnRepeat. Cannot currently handle...'
};

TurnRepeat.prototype.getNext = function () {
    this.currentIteration++;
    var count = this.getCount();
    if (count == null) // no limit, keep going
        return this.firstChild;

    if (this.currentIteration > count) {
        this.currentIteration = 0;
        return this.next;
    }

    return this.firstChild;
};

TurnRepeat.prototype.getPrev = function () {
    this.currentIteration--;

    if (this.currentIteration == 0)
        return this.prev;
    
    return this.lastChild;
};

function TurnStep(player) {
    this.player = player;
    this.next = null;
    this.prev = null;
    this.isGroup = false;
}

TurnStep.prototype.getNext = function () {
    return this.next;
};

TurnStep.prototype.getPrev = function () {
    return this.prev;
};