
export default class Mouse {
    constructor(mouseX, mouseY) {
        this.x = mouseX;
        this.y = mouseY;
    };
    isOverlapping(point) {
        if (
            this.x > point.x - point.radius
            && this.x < point.x + point.radius
            && this.y > point.y - point.radius
            && this.y < point.y + point.radius
        )
            return true
        else return false;
    }
}
