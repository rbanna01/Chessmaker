function TurnOrder() {
    this.currentStep = null;
}

TurnOrder.parse = function (xmlNode) {
    var turnOrder = new TurnOrder();

    var fakeStep = new TurnStep(null);
    fakeStep.next = TurnOrder.parseRepeat(xmlNode, true);
    turnOrder.currentStep = fakeStep;
    return turnOrder;
};

TurnOrder.parseRepeat = function (xmlNode, topLevel) {
    var count = xmlNode.getAttribute('count');
    if (count != null)
        count = parseInt(count); // todo: this could reference several different variables, it might not be just a number

    var children = xmlNode.childNodes;

    var first = null; var prev = null;

    for (var i = 0; i < children.length; i++) {
        var childNode = children[i];
        
        var child;
        if (childNode.tagName == 'repeat')
            child = TurnOrder.parseRepeat(childNode, false);
        else if (childNode.tagName == 'turn') {
            var player = childNode.getAttribute('player');
            child = new TurnStep(player);
        }

        if (i == 0)
            first = child;
        else
            prev.next = child;
        prev = child;
    }
    
    if (topLevel)
        return first;

    var repeat = new TurnRepeat(count, first);
    prev.next = repeat;

    return repeat;
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

function TurnRepeat(count, firstChild) {
    this.currentIteration = 0;
    this.count = count;
    this.firstChild = firstChild;
    this.next = null;
    this.isGroup = true;
}

TurnRepeat.prototype.getNext = function () {
    if (this.count == null) // no limit, keep going
        return this.firstChild;

    if (this.currentIteration == this.count) {
        this.currentIteration = 0;
        return this.next;
    }

    this.currentIteration++;
    return this.firstChild;
};

function TurnStep(player) {
    this.player = player;
    this.next = null;
    this.isGroup = false;
}

TurnStep.prototype.getNext = function () {
    return this.next;
};