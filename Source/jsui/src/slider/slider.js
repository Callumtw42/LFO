import { clamp } from "../functions"
import ParameterValueStore from '../ParameterValueStore';
import React, { Component } from 'react';
import Label from '../Label';
import {
    Image,
    Text,
    View,
} from 'juce-blueprint';

const COLOR = "#3bb3eb"

class Slider extends Component {
    constructor(props) {
        super(props);

        // this._onMouseDown = this._onMouseDown.bind(this);
        this._onMouseDrag = this._onMouseDrag.bind(this);
        this.valueAtDragStart = this.props.value
    }



    _onMouseDrag(mouseX, mouseY, mouseDownX, mouseDownY) {
        const { end, min, step, height, width, } = this.props;
        const snapped = this.snap(mouseDownX - mouseX, width, step, min, end);
        const skewed = snapped * skew;
        
        // let dy = mouseDownX - mouseX;
        // const clamped = clamp(this.state.valueAtDragStart + snapped, Math.min(max, min), Math.max(max, min));
        let sensitivity = (1.0 / 200.0);
        let value = Math.max(0.0, Math.min(1.0, this.state.valueAtDragStart + snapped * sensitivity));
        this.props.callBack(value);
    }

    _renderVectorGraphics(value, width, height) {
        const strokeWidth = 2.0;
        const radius = (Math.min(width, height) * 0.5) - (strokeWidth / 2);
        return `
      <svg 
        width="${width}"
        height="${height}"
        viewBox="0 0 ${width} ${height}"
        version="1.1"
        xmlns="http://www.w3.org/2000/svg">

        <path d="M0 ${height / 2} L${width} ${height / 2} Z" stroke="${COLOR}" />
        <circle
          cx="${(value / (this.props.end)) * width}"
          cy="${height / 2}"
          r="${radius}"
          stroke="#626262"
          stroke-width="${strokeWidth}"
          fill="${COLOR}" />
      </svg>
    `;
    }

    render() {
        const { value } = this.props;
        const { width, height } = this.props
        return (<View {...styles.container}>
            <View
                {...this.props}
                onMouseDown={this._onMouseDown}
                onMouseDrag={this._onMouseDrag}
                onMouseUp={this._onMouseUp}
            >
                <Image {...styles.canvas} source={this._renderVectorGraphics(value, width, height)} />
            </View>
        </View>
        );
    }
}

Slider.defaultProps = { skew: 0, step: 0 }

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

export default Slider;
