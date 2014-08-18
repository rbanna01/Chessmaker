function Distance(reference, number) {
    this.number = number;
    this.reference = reference;
}

Distance.prototype.equals = function (other) {
    return this.number === other.number && this.reference === other.reference;
};

Distance.RelativeTo = {
    None: 0,
    Max: 1,
    Prev: 2
};

Distance.Any = new Distance(Distance.RelativeTo.None, -1);
Distance.Zero = new Distance(Distance.RelativeTo.None, 0);

Distance.parse = function (val) {
    if (val === undefined)
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