function movePath(dx, dy) {
    $('#render path.selected').each(function () {
        // all our cell paths are designed such that the first segment is the center of the cell, all others are RELATIVELY positioned.
        // so moving the first should be sufficient!
        var seg = this.pathSegList.getItem(0);
        seg.x += dx; seg.y += dy;

        // begin transform rotation code
        // however, as we now use transforms for rotation, we need to update the center of the transform also
        var transform = this.getAttribute('transform');
        if (transform != null) {
            var rotStart = transform.indexOf('rotate(');
            if (rotStart != -1) {
                rotStart += 7;
                var firstSpace = transform.indexOf(' ', rotStart);
                if (firstSpace != -1) {
                    var newTrans = transform.substr(0, firstSpace + 1) + seg.x + ' ' + seg.y + ')';
                    this.setAttribute('transform', newTrans);
                }
            }
        }
        // end transform rotation code
    });

    var lineMode = $('#lineEnds').attr('mode'); // first, last or both
    var doFirst = lineMode != 'second';
    var doSecond = lineMode != 'first';
    $('#render line.selected').each(function () {
        if (doFirst) {
            this.x1.baseVal.value += dx;
            this.y1.baseVal.value += dy;
        }
        if (doSecond) {
            this.x2.baseVal.value += dx;
            this.y2.baseVal.value += dy;
        }
    });
    updateBounds();
}

function rotatePath(deg) {
    var paths = $('#render path.selected');
    if (paths.length == 0)
        return;

    var b = getBounds(paths);
    var cx = (b.maxX + b.minX) / 2; var cy = (b.maxY + b.minY) / 2;
    //var rad = deg * Math.PI / 180;

    paths.each(function () {
        var seg = this.pathSegList.getItem(0);

        // move this element if it should
        if (seg.x != cx || seg.y != cy) {
            var rad = deg * Math.PI / 180;
            rotateAbout(seg, cx, cy, rad);
        }

        var newAngle = deg;
        var transform = this.getAttribute('transform');
        if (transform != null) {
            var rotStart = transform.indexOf('rotate(');
            if (rotStart != -1) {
                rotStart += 7;
                var firstSpace = transform.indexOf(' ', rotStart);
                if (firstSpace != -1) {
                    newAngle = parseInt(transform.substring(rotStart, firstSpace));
                    if (isNaN(newAngle))
                        newAngle = deg;
                    else {
                        newAngle += deg;
                        if (newAngle > 360)
                            newAngle -= 360;
                        else if (newAngle < 0)
                            newAngle += 360;
                    }
                }
            }
        }

        this.setAttribute('transform', 'rotate(' + newAngle + ' ' + seg.x + ' ' + seg.y + ')');
    });
}

function applyOwnTransform(node, transform) {
    var seg = node.pathSegList.getItem(0);
    var transform = $(node).attr('transform');

    var rotStart = transform.indexOf('rotate(');
    if (rotStart == -1)
        return;

    rotStart += 7;
    var rotEnd = transform.indexOf(')', rotStart);
    if (rotEnd == -1)
        return;

    var rotParts = transform.substring(rotStart, rotEnd).split(' ');

    var deg = parseInt(rotParts[0]);
    var rx = parseFloat(rotParts[1]);
    var ry = parseFloat(rotParts[2]);

    if (isNaN(deg) || isNaN(rx) || isNaN(ry))
        return;

    while (deg > 360)
        deg -= 360;
    while (deg < 0)
        deg += 360;

    var rad = deg * Math.PI / 180;

    // move this element if it should
    if (seg.x != rx || seg.y != ry)
        rotateAbout(seg, rx, ry, rad);

    var numSegments = node.pathSegList.numberOfItems;

    // move each node on the element's path (except the first), assume they are all relative.
    for (var i = 1; i < numSegments; i++)
        rotateAbout(node.pathSegList.getItem(i), 0, 0, rad);
}

function rotateAbout(seg, cx, cy, angle) {
    // all path segment types have x and y properties except horizontal/vertical lines, which only have x or y. These are IDs 12 - 15.
    if (seg.pathSegType >= 12 && seg.pathSegType <= 15)
        return;

    var x2 = Math.cos(angle) * (seg.x - cx) - Math.sin(angle) * (seg.y - cy) + cx;
    var y2 = Math.sin(angle) * (seg.x - cx) + Math.cos(angle) * (seg.y - cy) + cy;
    seg.x = Math.round(x2 * 100) / 100;
    seg.y = Math.round(y2 * 100) / 100;
}

function getBounds(paths) {
    var bounds = { minX: 99999, maxX: -99999, minY: 99999, maxY: -99999 };

    paths.each(function () {
        var seg = this.pathSegList.getItem(0);
        if (seg.x < bounds.minX)
            bounds.minX = seg.x;
        if (seg.x > bounds.maxX)
            bounds.maxX = seg.x;
        if (seg.y < bounds.minY)
            bounds.minY = seg.y;
        if (seg.y > bounds.maxY)
            bounds.maxY = seg.y;
    });
    return bounds;
}

function updateBounds() {
    var paths = $('#render path');

    if (paths.length == 0)
        return;

    var b = getBounds(paths);

    // min & max currently based on object CENTERS. So add half width to get full size
    b.minX -= 40; b.minY -= 40;
    b.maxX += 40; b.maxY += 40;

    // now account for lines
    $('#render line').each(function () {
        b.minX = Math.min(b.minX, this.x1.baseVal.value, this.x2.baseVal.value);
        b.maxX = Math.max(b.maxX, this.x1.baseVal.value, this.x2.baseVal.value);
        b.minY = Math.min(b.minY, this.y1.baseVal.value, this.y2.baseVal.value);
        b.maxY = Math.max(b.maxY, this.y1.baseVal.value, this.y2.baseVal.value);
    });

    var width = b.maxX - b.minX, height = b.maxY - b.minY;

    if (width < 120) {
        b.minX = (b.maxX + b.minX) / 2 - 60;
        width = 120;
    }
    if (height < 120) {
        b.minY = (b.maxY + b.minY) / 2 - 60;
        height = 120;
    }

    // jquery's .attr sets everything lowercase. Won't work for viewBox.
    var svg = $('#render')[0];
    svg.setAttribute('viewBox', b.minX + ' ' + b.minY + ' ' + width + ' ' + height);
    svg.setAttribute('ratio', width / height);
    resizeBoard();
}

function elementClicked(e) {
    if (!e.shiftKey && !e.ctrlKey) {
        clearSelection();
        addClass($(this), 'selected');
    }
    else if (hasClass(this, 'selected'))
        remClass($(this), 'selected');
    else
        addClass($(this), 'selected');

    selectedChanged();
    return false;
}

function clearSelection() {
    var paths = $('#render .selected');
    if (paths.length == 0)
        return;

    remClass(paths, 'selected');
    selectedChanged();
}

function selectedChanged() {
    var paths = $('#render path.selected');
    var lines = $('#render line.selected');

    if (lines.length > 0) {
        if (paths.length > 0) { // both
            $('#lineEnds, #rotateCW, #rotateCCW').hide();

            $('#itemProperties .stroke.color').hide();
        }
        else { // only lines
            $('#lineEnds').show();
            $('#rotateCW, #rotateCCW').hide();

            $('#itemProperties .stroke.color').hide();
        }
    }
    else { // only paths, or nothing
        $('#lineEnds').hide();
        $('#rotateCW, #rotateCCW').show();

        $('#itemProperties .stroke.color').show();
    }

    var noneSelected = paths.length == 0 && lines.length == 0;

    $('#itemProperties .button').button('option', 'disabled', noneSelected);

    var colorOptions = $('#itemProperties .ui-buttonset');
    colorOptions.buttonset('option', 'disabled', noneSelected);

    if (noneSelected)
        return;

    // if selected cells have multiple colors, clear the options
    var last = null;
    paths.each(function () {
        var color;
        if (hasClass(this, 'light'))
            color = 'light';
        else if (hasClass(this, 'mid'))
            color = 'mid';
        else if (hasClass(this, 'dark'))
            color = 'dark';
        else
            return;
        if (color != last && last != null) { // color has changed
            $('#itemProperties .fill input').prop('checked', false);
            last = null;
            return false;
        }
        last = color;
    });

    lines.each(function () {
        var color;
        if (hasClass(this, 'lightStroke'))
            color = 'light';
        else if (hasClass(this, 'midStroke'))
            color = 'mid';
        else if (hasClass(this, 'darkStroke'))
            color = 'dark';
        else
            return;
        if (color != last && last != null) { // color has changed
            $('#itemProperties .fill input').prop('checked', false);
            last = null;
            return false;
        }
        last = color;
    });

    if (last != null) {
        // otherwise, all selected items have same color. Update the color selector to match.
        $('#itemProperties .fill input[value="' + last + '"]').prop('checked', true);
    }

    last = null;
    paths.each(function () {
        var color;
        if (hasClass(this, 'lightStroke'))
            color = 'light';
        else if (hasClass(this, 'midStroke'))
            color = 'mid';
        else if (hasClass(this, 'darkStroke'))
            color = 'dark';
        else
            color = '';
        if (color != last && last != null) { // color has changed
            $('#itemProperties .stroke input').prop('checked', false);
            last = null;
            return false;
        }
        last = color;
    });

    if (last != null) {
        // otherwise, all selected items have same color. Update the color selector to match.
        $('#itemProperties .stroke input[value="' + last + '"]').prop('checked', true);
    }

    colorOptions.buttonset('refresh');
}

function addSingleCell(pathData) {
    clearSelection();
    var fill = $('#itemProperties .fill :radio:checked').attr('value');
    var stroke = $('#itemProperties .stroke :radio:checked').attr('value') + 'Stroke';
    if (stroke != '')
        stroke = ' ' + stroke;

    addCell(pathData, fill, stroke);

    updateBounds();
    selectedChanged();
}

var nextElem = 1;
function addCell(pathData, fillColor, strokeColor) {
    var cellNum = nextElem;
    var elem = $(SVG('path'))
                .attr('d', pathData)
			    .attr('class', 'cell ' + fillColor + strokeColor + ' selected')
                .attr('id', 'cell' + cellNum)
                .appendTo($('#render'))
                .click(elementClicked);

    nextElem++;
    return cellNum;
}

function addLine() {
    clearSelection();
    var stroke = $('#itemProperties .fill :radio:checked').attr('value') + 'Stroke'; // doesn't come from the "border" selection for cells
    if (stroke == '')
        stroke = 'midStroke';

    var elem = $(SVG('line'))
                .attr('x1', '40')
                .attr('x2', '80')
                .attr('y1', '40')
                .attr('y2', '40')
			    .attr('class', 'detail ' + stroke + ' selected')
                .appendTo($('#render'))
                .click(elementClicked);

    updateBounds();
    selectedChanged();
}

function submitPopup(popup) {
    var id = popup.attr('id');
    switch (id) {
        case 'addSquares':
            var width = parseInt($('#sqWidth').val()), height = parseInt($('#sqHeight').val());
            if (isNaN(width) || isNaN(height) || width < 1 || height < 1 || width > 64 || height > 64)
                return false;
            addSquares(width, height, $('#sqPattern').val(), $('#sqStroke').val());
            return true;
        case 'addTriangles':
            var length = parseInt($('#triLength').val());
            if (isNaN(length) || length < 1 || length > 64)
                return false;
            addTriangles(length, $('#triPattern').val(), $('#triStroke').val());
            return true;
        case 'addHexes':
            var length = parseInt($('#hexLength').val());
            if (isNaN(length) || length < 2 || length > 64)
                return false;
            addHexes(length, $('#hexPattern').val(), $('#hexStroke').val());
            return true;
        case 'addCircle':
            var radiusOuter = parseInt($('#cirRadius').val()), radiusInner = parseInt($('#cirRadiusInner').val());
            var slicesTot = parseInt($('#cirSlicesTot').val()), slicesAct = parseInt($('#cirSlicesAct').val());
            if (isNaN(radiusOuter) || isNaN(radiusInner) || isNaN(slicesTot) || isNaN(slicesAct) || radiusOuter < 1 || radiusOuter > 64 || radiusInner > radiusOuter || radiusInner < 0 || slicesTot < 1 || slicesAct < 1 || slicesTot > 64 || slicesAct > slicesTot)
                return false;
            addCircle(radiusOuter, radiusInner, slicesTot, slicesAct, $('#cirPattern').val(), $('#cirStroke').val());
            return true;
        default:
            return false;
    }
}

function resolvePattern(step, pattern) {
    switch (pattern) {
        case '1':
            return 'light';
        case '2':
            return 'mid';
        case '3':
            return 'dark';
        case '4':
            return step % 2 == 0 ? 'light' : 'dark';
        case '5':
            return step % 2 == 0 ? 'light' : 'mid';
        case '6':
            return step % 2 == 0 ? 'mid' : 'dark';
        case '7':
            switch (step % 3) {
                case 0:
                    return 'light';
                case 1:
                    return 'mid';
                case 2:
                    return 'dark';
            }
        default:
            console.log('pattern = ' + pattern);
            return 'light';
    }
}

var nextGroupDir = 1;
function addSquares(width, height, pattern, stroke) {
    var bottomRight = ' m-20 -20 l40 0 l0 40 l-40 0 Z';
    var right = ' m-20 -20 l40 0 l0 41 l-40 0 Z';
    var bottom = ' m-20 -20 l41 0 l0 40 l-41 0 Z';
    var elsewhere = ' m-20 -20 l41 0 l0 41 l-41 0 Z';

    clearSelection();

    if (stroke != '')
        stroke = ' ' + stroke;

    var linkData = $('#linkData')[0];

    for (var iy = 0; iy < height; iy++) {
        var ystep = width * iy;
        if (width % 2 == 0 && iy % 2 == 1)
            ystep++;
        var isTop = iy == 0;
        var isBottom = iy == height - 1;

        for (var ix = 0; ix < width; ix++) {
            var isLeft = ix == 0;
            var isRight = ix == width - 1;
            var path = isRight ? (isBottom ? bottomRight : right) : (isBottom ? bottom : elsewhere);
            var cellNum = addCell('M' + (60 + ix * 40) + ' ' + (60 + iy * 40) + path, resolvePattern(ix + ystep, pattern), stroke);

            if (!isLeft)
                linkData.value += ';cell' + cellNum + ':temp' + nextGroupDir + ':cell' + (cellNum - 1);
            if (!isRight)
                linkData.value += ';cell' + cellNum + ':temp' + (nextGroupDir + 1) + ':cell' + (cellNum + 1);
            if (!isTop)
                linkData.value += ';cell' + cellNum + ':temp' + (nextGroupDir + 2) + ':cell' + (cellNum - width);
            if (!isBottom)
                linkData.value += ';cell' + cellNum + ':temp' + (nextGroupDir + 3) + ':cell' + (cellNum + width);

            if (!isLeft && !isTop)
                linkData.value += ';cell' + cellNum + ':temp' + (nextGroupDir + 4) + ':cell' + (cellNum - width - 1);
            if (!isRight && !isTop)
                linkData.value += ';cell' + cellNum + ':temp' + (nextGroupDir + 5) + ':cell' + (cellNum - width + 1);
            if (!isLeft && !isBottom)
                linkData.value += ';cell' + cellNum + ':temp' + (nextGroupDir + 6) + ':cell' + (cellNum + width - 1);
            if (!isRight && !isBottom)
                linkData.value += ';cell' + cellNum + ':temp' + (nextGroupDir + 7) + ':cell' + (cellNum + width + 1);
        }
    }

    nextGroupDir += 8;
    updateBounds();
    selectedChanged();
}

function addTriangles(size, pattern, stroke) {
    var triPath = ' m0 -24 l20 35 l-40 0 Z', triPathInverted = ' m0 24 l20 -35 l-40 0 Z';

    clearSelection();

    if (stroke != '')
        stroke = ' ' + stroke;

    var offset = nextElem - 1;

    for (var iy = 0; iy < size; iy++) {
        var isTop = iy == 0;
        var isBottom = iy == size - 1;
        var ystep = (iy + 1) * iy;

        for (var ix = 0; ix <= iy; ix++) {
            var isLeft = ix == 0;
            var isRight = ix == iy;
            var cellNum;

            var yAbove = offset + (iy - 1) * (iy - 1) + (ix - 1) * 2 + 1; // for inverted
            var yBelow = offset + (iy + 1) * (iy + 1) + ix * 2 + 2; // for un-inverted

            if (!isLeft) {
                // inverted cell
                cellNum = addCell('M' + (40 + ix * 40 - iy * 20) + ' ' + (47 + iy * 35) + triPathInverted, resolvePattern(ystep + 1, pattern), stroke);

                linkData.value += ';cell' + cellNum + ':temp' + nextGroupDir + ':cell' + (cellNum - 1); // adjacent left
                linkData.value += ';cell' + cellNum + ':temp' + (nextGroupDir + 1) + ':cell' + (cellNum - 1); // adjacent down-left
                linkData.value += ';cell' + cellNum + ':temp' + (nextGroupDir + 2) + ':cell' + (cellNum + 1); // adjacent right
                linkData.value += ';cell' + cellNum + ':temp' + (nextGroupDir + 3) + ':cell' + (cellNum + 1); // adjacent down-right

                if (!isTop) {
                    linkData.value += ';cell' + cellNum + ':temp' + (nextGroupDir + 4) + ':cell' + yAbove; // adjacent up-left
                    linkData.value += ';cell' + cellNum + ':temp' + (nextGroupDir + 5) + ':cell' + yAbove; // adjacent up-right

                    if (ix > 1)
                        linkData.value += ';cell' + cellNum + ':temp' + (nextGroupDir + 6) + ':cell' + (yAbove - 2); // diagonal up-left

                    if (!isRight)
                        linkData.value += ';cell' + cellNum + ':temp' + (nextGroupDir + 7) + ':cell' + (yAbove + 2); // diagonal up-right
                }

                if (!isBottom)
                    linkData.value += ';cell' + cellNum + ':temp' + (nextGroupDir + 8) + ':cell' + (yBelow - 1); // diagonal down
            }

            // right-way-up cell
            cellNum = addCell('M' + (60 + ix * 40 - iy * 20) + ' ' + (60 + iy * 35) + triPath, resolvePattern(ystep, pattern), stroke);

            if (!isLeft) {
                linkData.value += ';cell' + cellNum + ':temp' + nextGroupDir + ':cell' + (cellNum - 1); // adjacent left
                linkData.value += ';cell' + cellNum + ':temp' + (nextGroupDir + 4) + ':cell' + (cellNum - 1); // adjacent up-left
            }
            if (!isRight) {
                linkData.value += ';cell' + cellNum + ':temp' + (nextGroupDir + 2) + ':cell' + (cellNum + 1); // adjacent right
                linkData.value += ';cell' + cellNum + ':temp' + (nextGroupDir + 5) + ':cell' + (cellNum + 1); // adjacent up-right
            }
            if (!isBottom) {
                linkData.value += ';cell' + cellNum + ':temp' + (nextGroupDir + 1) + ':cell' + yBelow; // adjacent down-left
                linkData.value += ';cell' + cellNum + ':temp' + (nextGroupDir + 3) + ':cell' + yBelow; // adjacent down-right

                if (!isLeft)
                    linkData.value += ';cell' + cellNum + ':temp' + (nextGroupDir + 9) + ':cell' + (yBelow - 2); // diagonal down-left
                if (!isRight)
                    linkData.value += ';cell' + cellNum + ':temp' + (nextGroupDir + 10) + ':cell' + (yBelow + 2); // diagonal down-right
            }

            if (!isTop && !isLeft && !isRight)
                linkData.value += ';cell' + cellNum + ':temp' + (nextGroupDir + 11) + ':cell' + (yAbove + 1); // diagonal down
        }
    }

    nextGroupDir += 12;
    updateBounds();
    selectedChanged();
}

function addHexes(size, pattern, stroke) {
    var hexPath = ' m0 -40 l35 20 l0 40 l-35 20 l-35 -20 l0 -40 Z';

    if (stroke != '')
        stroke = ' ' + stroke;

    var maxRowSize = size * 2 - 1, iy = 0;
    for (var rowSize = size; rowSize <= maxRowSize; rowSize++) {
        var rowOffset = (maxRowSize - rowSize) * 2;

        var isTop = iy == 0; // never bottom
        var isMid = rowSize == maxRowSize;

        for (var ix = 0; ix < rowSize; ix++) {
            var isLeft = ix == 0;
            var isRight = ix == (rowSize - 1);

            var cellNum = addCell('M' + (60 + ix * 70 - iy * 35) + ' ' + (60 + iy * 60) + hexPath, resolvePattern(ix + rowOffset, pattern), stroke);

            if (!isLeft)
                linkData.value += ';cell' + cellNum + ':temp' + nextGroupDir + ':cell' + (cellNum - 1); // left
            if (!isRight)
                linkData.value += ';cell' + cellNum + ':temp' + (nextGroupDir + 1) + ':cell' + (cellNum + 1); // right

            if (!isTop) {
                if (!isLeft)
                    linkData.value += ';cell' + cellNum + ':temp' + (nextGroupDir + 2) + ':cell' + (cellNum - rowSize); // up-left
                if (!isRight)
                    linkData.value += ';cell' + cellNum + ':temp' + (nextGroupDir + 3) + ':cell' + (cellNum - rowSize + 1); // up-right

                if (iy > 1 && !isLeft && !isRight)
                    linkData.value += ';cell' + cellNum + ':temp' + (nextGroupDir + 6) + ':cell' + (cellNum - rowSize - rowSize + 2); // jump up

                if (ix > 1)
                    linkData.value += ';cell' + cellNum + ':temp' + (nextGroupDir + 7) + ':cell' + (cellNum - rowSize - 1); // jump up left

                if (ix < rowSize - 2)
                    linkData.value += ';cell' + cellNum + ':temp' + (nextGroupDir + 8) + ':cell' + (cellNum - rowSize + 2); // jump up right
            }

            if (isMid) {
                if (!isLeft)
                    linkData.value += ';cell' + cellNum + ':temp' + (nextGroupDir + 4) + ':cell' + (cellNum + rowSize - 1); // down-left
                if (!isRight)
                    linkData.value += ';cell' + cellNum + ':temp' + (nextGroupDir + 5) + ':cell' + (cellNum + rowSize); // down-right

                if (!isLeft && !isRight)
                    linkData.value += ';cell' + cellNum + ':temp' + (nextGroupDir + 9) + ':cell' + (cellNum + rowSize + rowSize - 2); // jump down

                if (ix > 1)
                    linkData.value += ';cell' + cellNum + ':temp' + (nextGroupDir + 10) + ':cell' + (cellNum + rowSize - 2); // jump down left

                if (ix < rowSize - 2)
                    linkData.value += ';cell' + cellNum + ':temp' + (nextGroupDir + 11) + ':cell' + (cellNum + rowSize + 1); // jump down right
            }
            else {
                linkData.value += ';cell' + cellNum + ':temp' + (nextGroupDir + 4) + ':cell' + (cellNum + rowSize); // down-left
                linkData.value += ';cell' + cellNum + ':temp' + (nextGroupDir + 5) + ':cell' + (cellNum + rowSize + 1); // down-right

                if (rowSize == maxRowSize - 1) {// just above mid
                    linkData.value += ';cell' + cellNum + ':temp' + (nextGroupDir + 9) + ':cell' + (cellNum + rowSize + rowSize + 1); // jump down
                }
                else {
                    linkData.value += ';cell' + cellNum + ':temp' + (nextGroupDir + 9) + ':cell' + (cellNum + rowSize + rowSize + 2); // jump down
                }

                if (!isLeft)
                    linkData.value += ';cell' + cellNum + ':temp' + (nextGroupDir + 10) + ':cell' + (cellNum + rowSize - 1); // jump down left

                if (!isRight)
                    linkData.value += ';cell' + cellNum + ':temp' + (nextGroupDir + 11) + ':cell' + (cellNum + rowSize + 2); // jump down right
            }
        }

        iy++;
    }
    for (var rowSize = maxRowSize - 1; rowSize >= size; rowSize--) {
        var rowOffset = (maxRowSize - rowSize) * 2;

        var isBottom = rowSize == size; // never top

        for (var ix = 0; ix < rowSize; ix++) {
            var isLeft = ix == 0;
            var isRight = ix == (rowSize - 1);

            var cellNum = addCell('M' + (60 + ix * 70 - (maxRowSize - iy - 1) * 35) + ' ' + (60 + iy * 60) + hexPath, resolvePattern(ix + rowOffset, pattern), stroke);

            if (!isLeft)
                linkData.value += ';cell' + cellNum + ':temp' + nextGroupDir + ':cell' + (cellNum - 1); // left
            if (!isRight)
                linkData.value += ';cell' + cellNum + ':temp' + (nextGroupDir + 1) + ':cell' + (cellNum + 1); // right

            linkData.value += ';cell' + cellNum + ':temp' + (nextGroupDir + 2) + ':cell' + (cellNum - rowSize - 1); // up-left
            linkData.value += ';cell' + cellNum + ':temp' + (nextGroupDir + 3) + ':cell' + (cellNum - rowSize); // up-right

            if (!isBottom) {
                if (!isLeft)
                    linkData.value += ';cell' + cellNum + ':temp' + (nextGroupDir + 4) + ':cell' + (cellNum + rowSize - 1); // down-left
                if (!isRight)
                    linkData.value += ';cell' + cellNum + ':temp' + (nextGroupDir + 5) + ':cell' + (cellNum + rowSize); // down-right

                if (rowSize > size) {

                    if (!isLeft && !isRight && rowSize > size + 1)
                        linkData.value += ';cell' + cellNum + ':temp' + (nextGroupDir + 9) + ':cell' + (cellNum + rowSize + rowSize - 2); // jump down

                    if (ix > 1)
                        linkData.value += ';cell' + cellNum + ':temp' + (nextGroupDir + 10) + ':cell' + (cellNum + rowSize - 2); // jump down left

                    if (ix < rowSize - 2)
                        linkData.value += ';cell' + cellNum + ':temp' + (nextGroupDir + 11) + ':cell' + (cellNum + rowSize + 1); // jump down right
                }
            }

            var upOffs = rowSize == maxRowSize - 1 ? 1 : 2
            linkData.value += ';cell' + cellNum + ':temp' + (nextGroupDir + 6) + ':cell' + (cellNum - rowSize - rowSize - upOffs); // jump up

            if (!isLeft)
                linkData.value += ';cell' + cellNum + ':temp' + (nextGroupDir + 7) + ':cell' + (cellNum - rowSize - 2); // jump up left

            if (!isRight)
                linkData.value += ';cell' + cellNum + ':temp' + (nextGroupDir + 8) + ':cell' + (cellNum - rowSize + 1); // jump up right
        }

        iy++;
    }

    nextGroupDir += 12;
    updateBounds();
    selectedChanged();
}

function addCircle(radiusOuter, radiusInner, slicesTot, slicesAct, pattern, stroke) {
    var start = { x: 60, y: 60 };
    var sliceAngle = Math.PI * 2 / slicesTot;

    if (stroke != '')
        stroke = ' ' + stroke;

    for (var ring = radiusInner + 1; ring <= radiusOuter; ring++) {
        var startAngle = 0;

        var innerMost = ring == radiusInner + 1;
        var outerMost = ring == radiusOuter;

        var wrap = slicesAct == slicesTot;

        for (var slice = 0; slice < slicesAct; slice++) {
            var endAngle = startAngle + sliceAngle;
            var outerStart = projectAngle(start, startAngle, ring * 40);
            var outerEnd = projectAngle(start, endAngle, ring * 40);

            var firstSlice = slice == 0;
            var lastSlice = slice == slicesAct - 1;

            var centerX; var centerY;
            var cellNum;
            if (ring == 1) {
                centerX = (outerStart.x + outerEnd.x + start.x) / 3;
                centerY = (outerStart.y + outerEnd.y + start.y) / 3;

                outerEnd.x -= outerStart.x; outerEnd.y -= outerStart.y;
                outerStart.x -= start.x; outerStart.y -= start.y;
                var midPoint = { x: start.x - centerX, y: start.y - centerY };

                cellNum = addCell('M' + centerX + ' ' + centerY + ' m' + midPoint.x + ' ' + midPoint.y +
                            ' l' + outerStart.x + ' ' + outerStart.y +
                            ' a' + (ring * 40) + ',' + (ring * 40) + ' 0 0,1 ' + outerEnd.x + ',' + outerEnd.y + ' z',
                            resolvePattern(slice + ring, pattern), stroke);
            }
            else {
                var innerEnd = projectAngle(start, startAngle, (ring - 1) * 40);
                var innerStart = projectAngle(start, endAngle, (ring - 1) * 40);

                centerX = (outerStart.x + outerEnd.x + innerStart.x + innerEnd.x) / 4;
                centerY = (outerStart.y + outerEnd.y + innerStart.y + innerEnd.y) / 4;

                outerEnd.x -= outerStart.x; outerEnd.y -= outerStart.y;
                outerStart.x -= innerEnd.x; outerStart.y -= innerEnd.y;
                innerEnd.x -= innerStart.x; innerEnd.y -= innerStart.y;
                innerStart.x -= centerX; innerStart.y -= centerY;

                cellNum = addCell('M' + centerX + ' ' + centerY + ' m' + innerStart.x + ' ' + innerStart.y +
                            ' a' + ((ring - 1) * 40) + ',' + ((ring - 1) * 40) + ' 0 0,0 ' + innerEnd.x + ',' + innerEnd.y +
                            ' l' + outerStart.x + ' ' + outerStart.y +
                            ' a' + (ring * 40) + ',' + (ring * 40) + ' 0 0,1 ' + outerEnd.x + ',' + outerEnd.y + ' z',
                            resolvePattern(slice + ring, pattern), stroke);
            }

            startAngle = endAngle;

            // ccw
            if (!firstSlice)
                linkData.value += ';cell' + cellNum + ':temp' + nextGroupDir + ':cell' + (cellNum - 1);
            else if (wrap)
                linkData.value += ';cell' + cellNum + ':temp' + nextGroupDir + ':cell' + (cellNum + slicesAct - 1);

            // cw
            if (!lastSlice)
                linkData.value += ';cell' + cellNum + ':temp' + (nextGroupDir + 1) + ':cell' + (cellNum + 1);
            else if (wrap)
                linkData.value += ';cell' + cellNum + ':temp' + (nextGroupDir + 1) + ':cell' + (cellNum - slicesAct + 1);


            if (!innerMost) {
                linkData.value += ';cell' + cellNum + ':temp' + (nextGroupDir + 2) + ':cell' + (cellNum - slicesAct); // inward

                // inward-ccw
                if (!firstSlice)
                    linkData.value += ';cell' + cellNum + ':temp' + (nextGroupDir + 4) + ':cell' + (cellNum - slicesAct - 1);
                else if (wrap)
                    linkData.value += ';cell' + cellNum + ':temp' + (nextGroupDir + 4) + ':cell' + (cellNum - 1);

                // inward-cw
                if (!lastSlice)
                    linkData.value += ';cell' + cellNum + ':temp' + (nextGroupDir + 5) + ':cell' + (cellNum - slicesAct + 1);
                else if (wrap)
                    linkData.value += ';cell' + cellNum + ':temp' + (nextGroupDir + 5) + ':cell' + (cellNum - slicesAct - slicesAct + 1);
            }

            if (!outerMost) {
                linkData.value += ';cell' + cellNum + ':temp' + (nextGroupDir + 3) + ':cell' + (cellNum + slicesAct); // outward

                // outward-ccw
                if (!firstSlice)
                    linkData.value += ';cell' + cellNum + ':temp' + (nextGroupDir + 6) + ':cell' + (cellNum + slicesAct - 1);
                else if (wrap)
                    linkData.value += ';cell' + cellNum + ':temp' + (nextGroupDir + 6) + ':cell' + (cellNum + slicesAct + slicesAct - 1);

                // outward-cw
                if (!lastSlice)
                    linkData.value += ';cell' + cellNum + ':temp' + (nextGroupDir + 7) + ':cell' + (cellNum + slicesAct + 1);
                else if (wrap)
                    linkData.value += ';cell' + cellNum + ':temp' + (nextGroupDir + 7) + ':cell' + (cellNum + 1);
            }
        }

    }

    nextGroupDir += 8;

    // The "a" segment's parameters are: rx,ry / ? / largeArc,sweep / dx,dy);
    updateBounds();
    selectedChanged();
}

function projectAngle(start, angle, dist) {
    return { x: start.x + dist * Math.sin(angle), y: start.y - dist * Math.cos(angle) };
}

function initializeCounters() {
    // find the biggest numbered cell named like cell#. Set nextElem equal to its number plus 1, or 1 if none are found
    var biggestNum = 0;
    $('#render path.cell[id^="cell"]').each(function () {
        var num = parseInt($(this).attr('id').substr(4));
        if (!isNaN(num) && num > biggestNum)
            biggestNum = num;
    });
    nextElem = biggestNum + 1;

    // find the biggest numbered dir in linkData named like temp#. Set nextGroupDir equal to its number plus 1, or 1 if none are found
    biggestNum = 0;
    var linkData = $('#linkData').val().split(';');
    for (var i = 0; i < linkData.length; i++) {
        var parts = linkData[i].split(':');
        if (parts.length < 3)
            continue;
        var dir = parts[1];
        if (dir.length <= 4 || dir.substr(0, 4) != 'temp')
            continue;

        var num = parseInt(dir.substr(4));
        if (!isNaN(num) && num > biggestNum)
            biggestNum = num;
    }
    nextGroupDir = biggestNum + 1;
}

$(function () {
    $('#shapeForm').submit(function () {
        remClass($('#render path.selected'), 'selected');

        $('#render path.cell[transform]').each(function () {
            applyOwnTransform(this);
            $(this).removeAttr('transform');
        });

        $('#shapeData').val($('#render').prop('outerHTML'));
    });

    var data = $('#shapeData').val();
    if (data.length > 0) {
        $('#render')
            .prop('outerHTML', data);

        $('#render path.cell, #render line')
            .click(elementClicked);
    }

    $('.popup').dialog({
        modal: true,
        autoOpen: false,
        buttons: {
            "Add": function () {
                if (submitPopup($(this)))
                    $(this).dialog("close");
            },
            Cancel: function () {
                $(this).dialog("close");
            }
        }
    }).keyup(function (e) {
        if (e.keyCode != 13)
            return;
        $(this).parent().find('button:nth-child(1)').click();
    });

    $('#toolbox .button').button();
    $('#toolbox .button.square').click(function () {
        addSingleCell('M60 60 m-20 -20 l40 0 l0 40 l-40 0 Z');
    });
    $('#toolbox .button.triangle').click(function () {
        addSingleCell('M60 60 m0 -20 l20 35 l-40 0 Z');
    });
    $('#toolbox .button.hex').click(function () {
        addSingleCell('M60 60 m0 -40 l35 20 l0 40 l-35 20 l-35 -20 l0 -40 Z');
    });
    $('#toolbox .button.line').click(function () {
        addLine();
    });
    $('#toolbox .button.squares').click(function () {
        $("#addSquares").dialog("open");
    });
    $('#toolbox .button.triangles').click(function () {
        $("#addTriangles").dialog("open");
    });
    $('#toolbox .button.hexes').click(function () {
        $("#addHexes").dialog("open");
    });
    $('#toolbox .button.circle').click(function () {
        $("#addCircle").dialog("open");
        // addSingleCell('M60 60 m-20 -0 a20,20 0 0,0 40,0 Z'); // a 75,75 0 0,0 75,75 means rx,ry / ? / largeArc,sweep / dx,dy);
    });

    /*
    $('#toolbox .button[quantity="set"]').click(function () {
    //showAddSetPopup($(this).attr('addtype'));
    });
    */
    $('#moveUp').button({
        icons: {
            primary: "ui-icon-arrowthick-1-n"
        },
        text: false
    }).click(function () {
        movePath(0, -2.5);
    });

    $('#moveDown').button({
        icons: {
            primary: "ui-icon-arrowthick-1-s"
        },
        text: false
    }).click(function () {
        movePath(0, 2.5);
    });

    $('#moveLeft').button({
        icons: {
            primary: "ui-icon-arrowthick-1-w"
        },
        text: false
    }).click(function () {
        movePath(-2.5, 0);
    });

    $('#moveRight').button({
        icons: {
            primary: "ui-icon-arrowthick-1-e"
        },
        text: false
    }).click(function () {
        movePath(2.5, 0);
    });

    $('#moveUpFast').button({
        icons: {
            primary: "ui-icon-arrowthickstop-1-n"
        },
        text: false
    }).click(function () {
        movePath(0, -40);
    });

    $('#moveDownFast').button({
        icons: {
            primary: "ui-icon-arrowthickstop-1-s"
        },
        text: false
    }).click(function () {
        movePath(0, 40);
    });

    $('#moveLeftFast').button({
        icons: {
            primary: "ui-icon-arrowthickstop-1-w"
        },
        text: false
    }).click(function () {
        movePath(-40, 0);
    });

    $('#moveRightFast').button({
        icons: {
            primary: "ui-icon-arrowthickstop-1-e"
        },
        text: false
    }).click(function () {
        movePath(40, 0);
    });

    $('#rotateCW').button({
        icons: {
            primary: "ui-icon-arrowrefresh-1-w"
        },
        text: false
    }).click(function () {
        rotatePath(15);
    });

    $('#rotateCCW').button({
        icons: {
            primary: "ui-icon ui-icon-arrowreturn-1-s"
        },
        text: false
    }).click(function () {
        rotatePath(-15);
    });

    $('#lineEnds').button({
        icons: {
            primary: "ui-icon ui-icon-carat-2-e-w"
        },
        text: false
    }).click(function () {
        var btn = $(this);
        var mode = btn.attr('mode');
        if (mode == 'first') {
            btn.button("option", {
                icons: { primary: "ui-icon ui-icon-carat-1-e" }
            })
            .attr('mode', 'second');
        }
        else if (mode == 'second') {
            btn.button("option", {
                icons: { primary: "ui-icon ui-icon-carat-2-e-w" }
            })
            .attr('mode', 'both');
        }
        else {
            btn.button("option", {
                icons: { primary: "ui-icon ui-icon-carat-1-w" }
            })
            .attr('mode', 'first');
        }
    });

    $('#deleteCell').button({
        icons: {
            primary: "ui-icon-trash"
        },
        text: false
    }).click(function () {
        $('#render .selected').remove();
        selectedChanged();
        updateBounds();
    });

    $('#copy').button({
        icons: {
            primary: "ui-icon-copy"
        },
        text: false
    }).click(function () {
        var source = $('#render .selected');

        source.clone(true).each(function () {
            if (this.nodeName == 'path') {
                // allocate new IDs!
                $(this).attr('id', 'cell' + nextElem);
                nextElem++;
            }
        }).appendTo('#render');

        remClass(source, 'selected');

        selectedChanged();
    });

    $('#itemProperties .color.fill').buttonset().click(function () {
        var fill = $('#itemProperties .color.fill :radio:checked').attr('value');
        var selectedPaths = $('#render path.selected');
        var selectedLines = $('#render line.selected');

        if (fill != 'light') {
            remClass(selectedPaths, 'light');
            remClass(selectedLines, 'lightStroke');
        }
        if (fill != 'mid') {
            remClass(selectedPaths, 'mid');
            remClass(selectedLines, 'midStroke');
        }
        if (fill != 'dark') {
            remClass(selectedPaths, 'dark');
            remClass(selectedLines, 'darkStroke');
        }

        addClass(selectedPaths, fill);
        addClass(selectedLines, fill + 'Stroke');
    });

    $('#itemProperties .color.stroke').buttonset().click(function () {
        var stroke = $('#itemProperties .color.stroke :radio:checked').attr('value');
        var selected = $('#render path.selected');

        if (stroke != 'light')
            remClass(selected, 'lightStroke');
        if (stroke != 'mid')
            remClass(selected, 'midStroke');
        if (stroke != 'dark')
            remClass(selected, 'darkStroke');

        if (stroke != '')
            addClass(selected, stroke + 'Stroke');
    });

    $('#board').click(function (e) {
        if (!e.shiftKey && !e.ctrlKey)
            clearSelection();
    });

    $(document).keydown(function (e) {
        if ($(".ui-dialog").is(":visible"))
            return; // do nothing if a popup is open

        switch (e.which) {
            case 37:
                if (e.shiftKey)
                    $('#moveLeftFast').click();
                else if (e.ctrlKey)
                    $('#rotateCCW').click();
                else
                    $('#moveLeft').click();
                break;
            case 38:
                if (e.shiftKey)
                    $('#moveUpFast').click();
                else
                    $('#moveUp').click();
                break;
            case 39:
                if (e.shiftKey)
                    $('#moveRightFast').click();
                else if (e.ctrlKey)
                    $('#rotateCW').click();
                else
                    $('#moveRight').click();
                break;
            case 40:
                if (e.shiftKey)
                    $('#moveDownFast').click();
                else
                    $('#moveDown').click();
                break;
            case 46:
                $('#deleteCell').click();
                break;
            default:
                return;
        }
        e.preventDefault();
    });

    selectedChanged();
    initializeCounters();
});
