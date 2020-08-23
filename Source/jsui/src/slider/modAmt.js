import Dial from "./dial"

export default class ModAmt extends Dial {

  constructor(props) {
    super(props)
  }

  _renderVectorGraphics(value, width, height) {
    const { end, start, step, label } = this.props;
    const strokeWidth = 2.0;
    const radius = (Math.min(width - 5, height - 5) * 0.5) - (strokeWidth / 2);

    const arcCircumference = 1.5 * Math.PI * radius;
    // const dashArray = [(value - start) / (end - start) * arcCircumference, 2.0 * Math.PI * radius];
    const dashArray = [value * arcCircumference, 2.0 * Math.PI * radius];
    return `
        <svg
        width="${width}"
        height="${height}"
        viewBox="0 0 ${width} ${height}"
        version="1.1"
        xmlns="http://www.w3.org/2000/svg">
        <circle
        transform="rotate(-135,${width / 2},${height / 2})"
          cx="${width / 2}"
          cy="${height / 2}"
          r="${radius}"
          stroke="#626262"
          stroke-width="${strokeWidth}"
          stroke-dasharray="${[arcCircumference, arcCircumference].join(',')}"
          fill="none" />
        <circle
        transform="rotate(-135,${width / 2},${height / 2})"
          cx="${width / 2}"
          cy="${height / 2}"
          r="${radius}"
          stroke="#047cf4"
          stroke-width="${strokeWidth}"
          stroke-dasharray="${dashArray.join(',')}"
          fill="none" />

      </svg>>
    `;

  }
}
