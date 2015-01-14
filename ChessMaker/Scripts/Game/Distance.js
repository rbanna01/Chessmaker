"use strict";

function Distance(reference, number) {
    this.number = number;
    this.reference = reference;
}

Distance.prototype.equals = function (other) {
    return this.number == other.number && this.reference == other.reference;
};

Distance.RelativeTo = {
    None: 0,
    Max: 1,
    Prev: 2
};

Distance.Any = new Distance(Distance.RelativeTo.None, -1);
Distance.Zero = new Distance(Distance.RelativeTo.None, 0);

Distance.parse = function (val) {
    if (val == null)
        return null;

    if (val == "any")
        return Distance.Any;
    else if (val.substr(0, 3) == "max") // e.g. max-3
        return new Distance(Distance.RelativeTo.Max, parseInt(val.substr(3)));
    else if (val.substr(0, 4) == "prev") // e.g. prev+1
        return new Distance(Distance.RelativeTo.Prev, parseInt(val.substr(4)));
    else
        return new Distance(Distance.RelativeTo.None, parseInt(val));
};

Distance.prototype.getValue = function (previousStep, maxDist) {
    switch (this.reference) {
        case Distance.RelativeTo.None:
            return this.equals(Distance.Any) ? 1 : this.number; // the minimum for "any" is 1, and the max will be maxDist (from getRange)
        case Distance.RelativeTo.Max:
            return maxDist + this.number;
        case Distance.RelativeTo.Prev:
            if (previousStep != null)
                return previousStep.distance + this.number;
            return this.number;
        default:
            throw "Unexpected Distance.RelativeTo value in Distance.parseValue: " + this.reference;
    }
}

Distance.prototype.getRange = function (dist2, previousStep, maxDist) {
    var outDist1 = this.getValue(previousStep, maxDist);
    var outDist2;

    if (this.equals(Distance.Any))
        outDist2 = maxDist;
    else if (dist2 == null)
        outDist2 = outDist1;
    else
        outDist2 = dist2.getValue(previousStep, maxDist);

    if (outDist2 > maxDist)
        outDist2 = maxDist;
    return [outDist1, outDist2];
}