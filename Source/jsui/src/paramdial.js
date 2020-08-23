import ParameterValueStore from './ParameterValueStore';
import React, { Component } from 'react';
import Label from './Label';
import Dial from "./slider/dial"
import ModAmt from "./slider/modAmt"
import { clamp } from "./functions"
import {
  Image,
  Text,
  View,
  EventBridge
} from 'juce-blueprint';


class ParamDial extends Component {
  constructor(props) {
    super(props);


    this._onMeasure = this._onMeasure.bind(this);
    this._onMouseDown = this._onMouseDown.bind(this);
    this._onMouseUp = this._onMouseUp.bind(this);
    this._onMouseDrag = this._onMouseDrag.bind(this);
    this._onMouseDoubleClick = this._onMouseDoubleClick.bind(this);
    this._onParameterValueChange = this._onParameterValueChange.bind(this);

    // During a drag, we hold the value at which the drag started here to
    // ensure smooth behavior while the component state is being updated.
    this._valueAtDragStart = 0.0;
    this.label = 0;


    this.state = {
      width: 0,
      height: 0,
      value: 0,
      modAmt: 0,
      initialised: false,
    };
  }

  componentDidMount() {
    EventBridge.addListener('parameterValueChange', this._onParameterValueChange);
  }

  _onMouseDoubleClick(mouseX, mouseY) {
    const x = global.getMouseX();
    const y = global.getMouseY();
    this.props.spawnLFO(this.props.paramId, x, y)
  }

  _onMeasure(width, height) {
    this.setState({
      width: width,
      height: height,
    });
  }

  _onParameterValueChange(index, paramId, defaultValue, currentValue, stringValue) {
    if (paramId === this.props.paramId) {
      this.label = stringValue
      if (!this.state.initialised) {
        this.setState({ value: defaultValue });
      }
    }
  }

  _onMouseDown(mouseX, mouseY) {
    this.setState({
      initialised: true
    })
    this._valueAtDragStart = this.state.value;
    global.beginParameterChangeGesture(this.props.paramId);
  }

  _onMouseUp(mouseX, mouseY) {
    global.endParameterChangeGesture(this.props.paramId);
  }

  _onMouseDrag(mouseX, mouseY, mouseDownX, mouseDownY) {
    let dy = mouseDownY - mouseY;
    let sensitivity = (1.0 / 200.0);
    let value = Math.max(0.0, Math.min(1.0, this._valueAtDragStart + dy * sensitivity));
    this.setState({
      value: value
    })
    this.setValue();
  }

  setValue() {
    if (typeof this.props.paramId === 'string' && this.props.paramId.length > 0 && this.state.initialised) {
      global.setParameterValueNotifyingHost(this.props.paramId, this.state.value);
    }
  }

  setModAmt(v) {
    this.setState({
      modAmt: v
    })
    global.setModAmt(this.props.paramId, v);
  }


  _renderVectorGraphics(value, label, width, height) {

    const cx = width * 0.5;
    const cy = height * 0.4;
    const strokeWidth = 2.0;
    const { modAmt } = this.state;
    const { paramId } = this.props

    // Note that we nudge the radius by half the stroke width; this is because
    // the stroke will extend outwards in both directions from the given coordinates,
    // which gets clipped if we try to draw the circle perfectly on the edge of the
    // image. We nudge it in so that no part of the path gets clipped.
    const paramRadius = (Math.min(width, height) * 0.5) - (strokeWidth / 2) - 10;
    const modRadius = paramRadius + 5;

    // Animate the arc by stroke-dasharray, where the length of the dash is
    // related to the value property and the length of the space takes up the
    // rest of the circle.
    const paramArcCircumference = 1.5 * Math.PI * paramRadius;
    const modArcCircumference = + 1.5 * Math.PI * modRadius;

    const paramDashArray = [value * paramArcCircumference, 2.0 * Math.PI * paramRadius];
    const modDashArray = [clamp(modAmt, 0, 1 - value) * modArcCircumference, 2.0 * Math.PI * modRadius];

    return (
      `
    <svg
      width="${width}"
      height="${height}"
      viewBox="0 0 ${width} ${height}"
      version="1.1"
      xmlns="http://www.w3.org/2000/svg">
    <text font-size="9"  fill="#D4D4D4" x="${cx}" y="${cy + 3}" text-anchor="middle">
       ${label} 
    </text>
    <text font-size="9"  fill="#D4D4D4" x="${cx}" y="${cy + 19}" text-anchor="middle">
       ${paramId} 
    </text>
      <circle
        cx="${cx}"
        cy="${cy}"
        r="${paramRadius}"
        stroke="#626262"
        stroke-width="${strokeWidth}"
        stroke-dasharray="${[paramArcCircumference].join(',')}"
        transform = "rotate(${5 / 8 * 360} ${cx} ${cy})"
        fill="none" />
      <circle
        cx="${cx}"
        cy="${cy}"
        r="${paramRadius}"
        stroke="#66FDCF"
        stroke-width="${strokeWidth}"
        stroke-dasharray="${paramDashArray.join(',')}"
        transform = "rotate(${5 / 8 * 360} ${cx} ${cy})"
        fill="none" />
      <circle
        cx="${cx}"
        cy="${cy}"
        r="${modRadius}"
        stroke="#047cf4"
        stroke-width="${strokeWidth}"
        stroke-dasharray="${[modDashArray].join(",")}"
        fill="none"
        transform = "rotate(${(value * (3 / 4) + (5 / 8)) * 360} ${cx} ${cy})"
        >
        </circle>
      </svg >
    `
    )
  }

  render() {
    const { value, width, height } = this.state;

    return (<>
      <View {...styles.container}>
        <View
          {...this.props}
          onMeasure={this._onMeasure}
          onMouseDown={this._onMouseDown}
          onMouseUp={this._onMouseUp}
          onMouseDrag={this._onMouseDrag}
          onMouseDoubleClick={this._onMouseDoubleClick}>
          <Image {...styles.canvas} source={this._renderVectorGraphics(value, this.label, width, height)} />
        </View>
        <View {...styles.wrap}>
          <ModAmt {...styles.modAmt} value={this.state.modAmt} start={0.0} end={1.0} step={0.01} label={""} callBack={this.setModAmt.bind(this)}  ></ModAmt>
        </View>
      </View>
    </>
    );
  }

}

const styles = {
  container: {
    'flex-direction': 'column',
    'background-color': '0062ffff',
  },
  canvas: {
    'width': "100%",
    'height': "100%",
    'top': 10.0,
    'interceptClickEvents': false,
    'position': 'absolute'
  },
  nameText: {
    'color': 'ff626262',
    'font-size': 12.0,
    'line-spacing': 1.6,
    'left': 7
  },
  label: {
    'flex': 1.0,
    'justify-content': 'center',
    'align-items': 'center',
    'interceptClickEvents': false,
    'top': 10
  },
  modAmt: {
    'width': 15,
    'height': 15,
  },
  wrap: {
    'position': 'absolute'
  }
};



export default ParamDial;
