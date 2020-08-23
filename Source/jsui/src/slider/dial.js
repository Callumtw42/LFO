import Slider from "./slider"
import { clamp } from "../functions"
import React, { Component } from "react";
import {
  Image,
  Text,
  View,
} from 'juce-blueprint';
const PLOT_RES = 128
const MAX_MOUSE_DISP = 200
export default class Dial extends Component {
  constructor(props) {
    super(props)
    this._onMouseDrag = this._onMouseDrag.bind(this);
    this._onMouseDown = this._onMouseDown.bind(this);
    this.valueAtDragStart = this.props.value
    this.plot = new Array(128)
    this.initialise();
  }

  initialise(){
    const { end, start, step, skew, value } = this.props
    let currentVal = start;
    let currentStep = step;
    let values = [];

    while (true) {
      values.push(currentVal);
      currentVal += currentStep;
      currentStep *= skew;
      if (clamp(currentVal, Math.min(start, end), Math.max(start, end)) == end) {
        values.push(currentVal);
        currentVal += currentStep;
        currentStep *= skew;
        break;
      }
    }
    const stepRange = PLOT_RES / values.length;

    let currentBlock = 0;
    for (let i = 0; i < this.plot.length; i++) {
      this.plot[i] = values[currentBlock];
      if (i >= (currentBlock + 1) * stepRange)
        currentBlock++;
    }
  }

  _onMouseDown(mouseX, mouseY) {
    const { end, start, step, height, width, value } = this.props;
    this.valueAtDragStart = this.plot.indexOf(value)
  }

  _onMouseDrag(mouseX, mouseY, mouseDownX, mouseDownY) {
    const { end, start, step, height, width, skew } = this.props;
    const startPos = this.valueAtDragStart;

    const dy = mouseDownY - mouseY;
    const rdy = dy / MAX_MOUSE_DISP;
    const relPosition = rdy * PLOT_RES;
    const index = clamp(Math.round(relPosition + startPos), 0, PLOT_RES - 1)
    this.props.callBack(this.plot[index]);
  }

  _renderVectorGraphics(value, width, height) {
    const { end, start, step, label } = this.props;
    const relValue = this.plot.indexOf(this.props.value) / PLOT_RES;
    const strokeWidth = 2.0;
    const radius = (Math.min(width - 5, height - 5) * 0.5) - (strokeWidth / 2);
    const arcCircumference = 1.5 * Math.PI * radius;
    const dashArray = [relValue * arcCircumference, 2.0 * Math.PI * radius];
    return `
        <svg
        width="${width}"
        height="${height}"
        viewBox="0 0 ${width} ${height}"
        version="1.1"
        xmlns="http://www.w3.org/2000/svg">
        <text x="${width / 2}" y="${height}" text-anchor="middle" fill="#D5D5D5"><tspan style="font-size:9.0">${label}</tspan></text>>
        <text x="${width / 2}" y="${(9 / 16) * height}" text-anchor="middle" fill="#D5D5D5"><tspan style="font-size:9.0">${value.toFixed(2)}</tspan></text>>
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
          stroke="#66FDCF"
          stroke-width="${strokeWidth}"
          stroke-dasharray="${dashArray.join(',')}"
          fill="none" />
      </svg>>
    `;

  }

  render() {
    const { value, width, height } = this.props
    return (<View {...styles.container}>
      <View
        height={height}
        width={width}
        onMouseDown={this._onMouseDown}
        onMouseDrag={this._onMouseDrag}
      >
        <Image {...styles.canvas} source={this._renderVectorGraphics(value, width, height)} />
      </View>
    </View>
    );
  }
}


Dial.defaultProps = { skew: 1 }

const styles = {
  container: {
    'flex-direction': 'column',
    'background-color': '#00333333',
  },
  text: {
    'color': 'ff626262',
    'font-size': 8.0,
  },
  canvas: {
    'height': '100%',
    'width': '100%',
    'interceptClickEvents': false,

    'position': 'absolute'
  },
};