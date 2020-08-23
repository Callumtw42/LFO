import { pointRadius, plotHeight, plotWidth, plotResolution } from "./lfo"
import { clamp } from "../functions"
const VALUES_PER_CURVE_POINT = 4;

class Path {
    constructor(start, end) {
        this.start = start;
        this.end = end;
        this.plot = new Array(this.widthToArrayLength(this.end.x - this.start.x))
        // this.curveRes = (end.x - start.x / 4)
        this.curvePoints = [];
        this.ctrlParam = {
            relY: 0.5,
            x: start.x + (end.x - start.x) / 2,
            y: start.y + (start.y - end.y) / 2,
            radius: pointRadius,
            isSelected: false
        }
        // this.generatePlot();
    }

    widthToArrayLength(Dx) {
        const relDx = Dx / plotWidth;
        const length = relDx * plotResolution;
        return Math.round(length);
    }

    getControlPoints() {
        const { start, end, ctrlParam } = this;
        const dy = end.y - start.y;
        const dx = end.x - start.x;
        const relY = 1 - ((ctrlParam.relY + 1) / 2);
        const ctrl1 = {
            x: end.x - dx * relY,
            y: start.y + dy * relY
        }

        const ctrl2 = {
            x: end.x - dx * relY,
            y: start.y + dy * relY
        }

        return { ctrl1, ctrl2 }
    }

    generatePlot() {
        this.plot = new Array(this.widthToArrayLength(this.end.x - this.start.x))
        const { start, end, plot, cubicBezier } = this;
        const { ctrl1, ctrl2 } = this.getControlPoints();
        const curveRes = Math.round(plot.length / VALUES_PER_CURVE_POINT);
        this.curvePoints = new Array(curveRes);
        for (let i = 0; i < curveRes; i++) {
            const t = i / curveRes;
            const p = cubicBezier(start, ctrl1, ctrl2, end, t);
            p.targetIndex = Math.ceil(((p.x - start.x) / (end.x - start.x)) * (plot.length));
            this.curvePoints[i] = p;
        }
        let current = 0;
        let next = 1;
        let i = 0;
        while (next < this.curvePoints.length) {
            const startIndex = this.curvePoints[current].targetIndex;
            const targetIndex = this.curvePoints[next].targetIndex
            const targetY = this.curvePoints[next].y;
            const Dx = targetIndex - startIndex;
            const Dy = (targetY - this.curvePoints[current].y)
            const dy = Dy / (Dx | 1);
            for (let j = 0; j < Dx; j++) {
                this.plot[i] = this.curvePoints[current].y + j * dy;
                i++;
            }
            current++;
            next++;
        }
        if (this.curvePoints.length !== 0) {
            const targetY = end.y
            const Dx = plot.length - i;
            const Dy = (targetY - this.curvePoints[current].y)
            const dy = Dy / (Dx | 1);
            for (let j = 0; j < Dx; j++) {
                this.plot[i] = this.curvePoints[current].y + j * dy;
                i++;
            }
        }
        this.ctrlParam.y = this.plot[Math.round(plot.length / 2)]
    }

    updateControlParam() {
        const { start, end, ctrlParam } = this;
        const dx = end.x - start.x;
        const dy = end.y - start.y;
        const relY = this.ctrlParam.relY;

        ctrlParam.x = start.x + dx * 0.5;
        ctrlParam.y = start.y + dy * Math.abs(relY);
        this.generatePlot();
    }


    // quadraticBezier(p0, p1, p2, t, pFinal) {
    //     pFinal = pFinal || {};
    //     pFinal.x = Math.pow(1 - t, 2) * p0.x +
    //         (1 - t) * 2 * t * p1.x +
    //         t * t * p2.x;
    //     pFinal.y = Math.pow(1 - t, 2) * p0.y +
    //         (1 - t) * 2 * t * p1.y +
    //         t * t * p2.y;
    //     return pFinal;
    // }

    cubicBezier(p0, p1, p2, p3, t, pFinal) {
        pFinal = pFinal || {};
        pFinal.x = Math.pow(1 - t, 3) * p0.x +
            Math.pow(1 - t, 2) * 3 * t * p1.x +
            (1 - t) * 3 * t * t * p2.x +
            t * t * t * p3.x;
        pFinal.y = Math.pow(1 - t, 3) * p0.y +
            Math.pow(1 - t, 2) * 3 * t * p1.y +
            (1 - t) * 3 * t * t * p2.y +
            t * t * t * p3.y;
        return pFinal;
    }

    moveControlParam(y) {
        const { start, end } = this;
        const maxY = Math.max(start.y, end.y);
        const minY = Math.min(start.y, end.y);
        const clampY = clamp(y, minY, maxY);
        const dy = end.y - start.y;
        const yInBounds = clampY - end.y;
        this.ctrlParam.relY = (-yInBounds * 2 / dy) - 1;
        this.generatePlot();
        // const { ctrl1, ctrl2 } = this.getControlPoints();
        // const midPoint = this.cubicBezier(start, ctrl1, ctrl2, end, 0.5);
        // this.ctrlParam.y = midPoint.y;
    }
}

export class Node {
    constructor(x, y, radius, isBound) {

        this.x = x;
        this.y = y;
        this.radius = radius;
        this.isSelected = false;
        this.isBound = isBound;
        this.leftNeighbour = null;
        this.rightNeighbour = null;
        this.path = null;
    }

    move(x, y) {
        const clampY = clamp(y, 0, plotHeight);
        const clampX = (this.leftNeighbour && this.rightNeighbour)
            ? clamp(x, this.leftNeighbour.x, this.rightNeighbour.x)
            : this.x;

        this.y = clampY;
        if (!this.isBound)
            this.x = clampX;
        if (this.path) this.path.updateControlParam();
        if (this.leftNeighbour) this.leftNeighbour.path.updateControlParam();
    }
}

export class NodeList {
    constructor(head, tail) {
        head.path = new Path(head, tail)
        this.nodes = [head, tail];
        head.rightNeighbour = tail;
        head.leftNeighbour = null;
        tail.rightNeighbour = null;
        tail.leftNeighbour = head;
    }

    insertAfter(index, node) {
        const leftNeighbour = this.nodes[index];
        const rightNeighbour = leftNeighbour.rightNeighbour;

        this.nodes.splice(
            index + 1,
            0,
            node
        )

        node.leftNeighbour = leftNeighbour;
        leftNeighbour.rightNeighbour = node;

        node.rightNeighbour = rightNeighbour;
        rightNeighbour.leftNeighbour = node;

        leftNeighbour.path = new Path(leftNeighbour, node);
        leftNeighbour.path.updateControlParam();

        node.path = new Path(node, rightNeighbour)
        node.path.updateControlParam();
    }

    remove(index) {
        const point = this.nodes[index];
        const leftNeighbour = point.leftNeighbour;
        const rightNeighbour = point.rightNeighbour;
        leftNeighbour.path = new Path(leftNeighbour, rightNeighbour);

        this.nodes.splice(index, 1);
        this.nodes[index].leftNeighbour = leftNeighbour;
        leftNeighbour.path.updateControlParam();
        this.nodes[index - 1].rightNeighbour = rightNeighbour;

    }

    forEach(...args) {
        return this.nodes.forEach(...args);
    }

    indexOf(...args) {
        return this.nodes.indexOf(...args);
    }

    splice(...args) {
        return this.nodes.splice(...args);
    }

    map(...args) {
        return this.nodes.map(...args);
    }

    get() {
        return this.nodes;
    }
}
