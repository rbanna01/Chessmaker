"use strict";

Distance.prototype.equals = function (other) {
    return this.number == other.number && this.reference == other.reference;
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