import React, { Component } from 'react';
import { clamp } from "../functions"
import {
  Image,
  View,
  EventBridge
} from 'juce-blueprint';
import Mouse from "./mouse"
import { Node, NodeList } from "./nodelist"
import Slider from "../slider/slider"
import Dial from "../slider/dial"
import Button from "../button"

const boxHeight = 200;
const boxWidth = 300;
export const plotHeight = (3 / 4) * boxHeight;
export const plotWidth = boxWidth;
export const pointRadius = 4;
export const plotResolution = 1024;
const MAX_GRID_RES = 16;
const CURVE_RES = 32;

const GRID_COLOR = "rgba(122, 129, 132, 0.25)"

class LFO extends Component {
  constructor(props) {
    super(props);

    this._onMouseDown = this._onMouseDown.bind(this);
    this._onMouseDrag = this._onMouseDrag.bind(this);
    this._onMouseUp = this._onMouseUp.bind(this);
    this._onMouseDoubleClick = this._onMouseDoubleClick.bind(this);
    this._onIndexUpdated = this._onIndexUpdated.bind(this);
    this.initialised = true;
    EventBridge.addListener('updateCurrentIndex', this._onIndexUpdated);

    this.state = {
      points: new NodeList(
        new Node(0, 0, pointRadius, true),
        new Node(plotWidth, 0, pointRadius, true)
      ),
      plot: new Array(plotResolution),
      gridRes: 16,
      buttonColor: "#626262",
      mode: 0,
      speed: 1.0,
      playhead: 0
    }
  }

  _onIndexUpdated(index) {
    // global.log(index);
    this.setState({ playhead: index })
  }

  componentDidMount() {
    this.state.points.insertAfter(0,
      new Node(plotWidth / 2, plotHeight, pointRadius, false)
    )
    this.renderPlotComponents();
    this.setState(this.state);
  }

  _onMouseDoubleClick(mouseX, mouseY) {
    const { points } = this.state;
    let mouse = new Mouse(mouseX, mouseY);
    let removing = false;

    for (const point of points.get()) {
      if (mouse.isOverlapping(point) && !point.isBound) {
        const index = points.indexOf(point);
        points.remove(index);
        removing = true;
        break;
      }
    }

    if (!removing) {
      let leftNeighbour = points.get()[0];
      let shortestLeftPath = mouse.x - leftNeighbour.x;

      for (const point of points.get()) {
        let leftPath = mouse.x - point.x;
        if (leftPath < 0)
          break;
        else if (leftPath <= shortestLeftPath) {
          leftNeighbour = point;
        }
      }
      const index = points.indexOf(leftNeighbour);

      points.insertAfter(
        index,
        new Node(mouseX, mouseY, pointRadius, false)
      )
    }
    this.renderPlotComponents();
    this.setState(this.state);
  }

  _onMouseDrag(mouseX, mouseY, mouseDownX, mouseDownY) {
    const { gridRes, points } = this.state
    const dx = plotWidth / gridRes
    const dy = plotHeight / gridRes
    const snapX = Math.round((mouseX / plotWidth) * gridRes) * dx
    const snapY = Math.round((mouseY / plotHeight) * gridRes) * dy

    for (const point of points.get()) {
      if (point.isSelected) {
        point.move(snapX, snapY)
        break;
      }
      const path = point.path;
      if (path && path.ctrlParam.isSelected) {
        path.moveControlParam(mouseY);
        break;
      }
    }
    this.renderPlotComponents();
    this.setState(this.state);
  }

  _onMouseDown(mouseX, mouseY) {
    this.initialised = true;
    const points = this.state.points;
    let mouse = new Mouse(mouseX, mouseY);

    for (const point of points.get()) {
      if (mouse.isOverlapping(point)) {
        point.isSelected = true;
        break;
      }
      if (point.path && mouse.isOverlapping(point.path.ctrlParam)) {
        point.path.ctrlParam.isSelected = true;
        break;
      }
    }
  }

  _onMouseUp(mouseX, mouseY) {
    this.state.points.forEach((point) => {
      if (point.isSelected) {
        point.isSelected = false;
      }
      if (point.path)
        point.path.ctrlParam.isSelected = false;
    })
  }

  normalisePlot() {
    return this.state.plot.map((y, index) => {
      return 1 - y / plotHeight;
    })
  }

  generatePlot() {
    const { points, plot } = this.state;
    let newPlot = [];
    points.forEach((point, index) => {
      if (point.path) {
        newPlot = newPlot.concat(point.path.plot);
      }
    })
    this.state.plot = newPlot;
    // global.log(this.state.plot.length)
    if (this.initialised) global.sendPlot(this.props.paramId, this.normalisePlot());
  }

  grid() {
    const { gridRes } = this.state
    const verticalPaths = new Array(gridRes)
    const horizontalPaths = new Array(gridRes)
    let dx = plotWidth / gridRes
    let dy = plotHeight / gridRes
    for (let i = 0; i < gridRes; i++) {
      verticalPaths[i] = `<path d="M${dx * i} ${0} L${dx * i} ${plotHeight} Z" stroke="${GRID_COLOR}" stroke-width="1"/>`
      horizontalPaths[i] = `<path d="M${0} ${dy * i} L${plotWidth} ${dy * i} Z" stroke="${GRID_COLOR}" stroke-width="1"/>`
    }
    return {
      gridX: verticalPaths,
      gridY: horizontalPaths
    }
  }

  playhead() {
    const { playhead } = this.state;
    const playX = (playhead / plotResolution) * plotWidth;
    return (
      ` < svg width="${boxWidth}" height="${boxWidth}" viewBox="0 0 0 0" xmlns="http://www.w3.org/2000/svg" >
          <path d="M${playX} ${0} L${playX} ${plotHeight} Z" stroke="${"#3ade3a"}" stroke-width="1"/>
        </svg>
    `
    )
  }

  renderPlotComponents() {
    this.generatePlot();
    const { points, plot, buttonColor } = this.state;
    const { gridX, gridY } = this.grid();
    // const playhead = this.playhead();

    const values = [];
    for (let i = 0; i < plotResolution - 1; i++) {
      const start = {
        x: (i / plotResolution) * plotWidth,
        y: plot[i]
      }
      values.push(`
        <circle cx="${start.x}" cy="${start.y}" r="${0.5}" fill="${"#3ade3a"}" />
      `)
    }

    const circles = points.map((point, index) => {
      if (point.rightNeighbour) {
        const start = point;
        const end = point.rightNeighbour;
        const { ctrl1, ctrl2 } = point.path.getControlPoints(start, end);
        const curveMod = point.path.ctrlParam;
        const curvePoints = [];
        for (let p of point.path.curvePoints) {
          curvePoints.push(
            `<circle cx="${p.x}" cy="${p.y}" r="${1}" fill="${"#ff2008"}" />`
          )
        }
        // global.log(curveMod.x);

        return `
        <circle cx="${point.x}" cy="${point.y}" r="${point.radius}" fill="${"#0866FF"}" />
        <circle cx="${curveMod.x}" cy="${curveMod.y}" r="${3}" fill="${"#FFEF08"}" />
        <circle cx="${ctrl1.x}" cy="${ctrl1.y}" r="${2}" fill="${"#D519DA"}" />
        <circle cx="${ctrl2.x}" cy="${ctrl2.y}" r="${1.5}" fill="${"#FFFFFF"}" />
        ${curvePoints}
        `
      }
      else return `< circle cx = "${point.x}" cy = "${point.y}" r = "${point.radius}" fill = "${buttonColor}" />
          `
    })

    this.plotComponents = {
      gridX: gridX,
      gridY: gridY,
      circles: circles,
      values: values
    }
  }

  _svg() {
    const { gridX, gridY, circles, values } = this.plotComponents;
    const img =
      `
          < svg width = "${boxWidth}" height = "${boxWidth}" viewBox = "0 0 0 0" xmlns = "http://www.w3.org/2000/svg" >
            <rect x="${0}" y="${0}" stroke="green" stroke-width="1" fill="#2a302a" width="${boxWidth}" height="${plotHeight}" />
        ${gridX}
        ${gridY}
        ${circles}
        ${values}
      </svg >
      `
    return img;
  }

  setSpeed(value) {
    this.setState({
      speed: value
    })
    global.sendLFOSpeed(this.props.paramId, value);
  }

  setGridRes(value) {
    this.state.gridRes = value;
    this.renderPlotComponents();
    this.setState(this.state)
  }

  setMode(mode) {
    this.setState({
      // buttonColor: buttonOn ? "#3ade3a" : "#626262",
      mode: mode
    })
    global.setMode(this.props.paramId, mode)
    // global.sendPlot(this.props.paramId, this.state.plot)
  }

  render() {
    const { speed, gridRes } = this.state;
    styles.container.left = this.props.x;
    styles.container.top = this.props.y;
    if (this.props.paramId == this.props.activeLFO) {
      return (
        <View {...styles.container}>
          {/* <Image
            onMouseDown={this._onMouseDown}
            onMouseUp={this._onMouseUp}
            onMouseDrag={this._onMouseDrag}
            onMouseDoubleClick={this._onMouseDoubleClick}
            {...styles.plot} source={this._svg()} /> */}
          <Image {...styles.playhead} source={this.playhead()} />
          <View {...styles.dials}>
            <Dial {...styles.dial} value={gridRes} start={1} end={16} step={1} label={"Grid"} callBack={this.setGridRes.bind(this)} ></Dial>
            <Dial {...styles.dial} value={speed} start={4} end={1 / 64} step={-2} skew={0.5} label={"Speed"} callBack={this.setSpeed.bind(this)} ></Dial>
            <Button {...styles.button}{...this.props} label={"Trig"} callBack={this.setMode.bind(this)} mode={this.state.mode} />
          </View>
        </View >
      );
    }
    else return <></>
  }
}

let styles = {
  plot: {
    'height': plotHeight,
    'width': plotWidth,
    'interceptClickEvents': true,
  },
  playhead: {
    'height': plotHeight,
    'width': plotWidth,
  },
  // slider: {
  //   'width': boxWidth,
  //   'height': boxHeight - plotHeight,
  // },
  dial: {
    'width': boxHeight / 5,
    'height': boxHeight / 5,
  },
  dials: {
    'flex-direction': 'row'
  },
  container: {
    'flex-direction': 'column',
    'position': 'absolute',
    'left': 0,
    'top': 0
  },
  button: {
    'width': boxHeight / 5,
    'height': boxHeight / 5,
    'color': '#3ade3a',
  }
};

export { LFO, boxHeight as lfoHeight, boxWidth as lfoWidth, plotHeight as lfoViewHeight };
