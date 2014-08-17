function extend(child, parent) {
    function ctor() { }
    ctor.prototype = parent.prototype;
    child.prototype = new ctor();

    child.prototype.constructor = child;
}