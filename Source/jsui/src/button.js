import React, { Component } from 'react'
import {
    Image,
    View,
} from 'juce-blueprint';


export default class Button extends Component {

    constructor(props) {
        super(props)

        this.modes = [
            { label: "Free", color: "#3ade3a" },
            { label: "1Sht", color: "#FFEF08" },
            { label: "Hold", color: "#0866FF" },
        ]
        this.state = {
            mode: 0
        }
    }

    _onMouseDown() {
        const { mode } = this.state;
        const {modes} = this
        this.setState({
            mode: (mode + 1) % this.modes.length
        })
        this.props.callBack(this.state.mode);
    }

    getColor() {
        // if (this.state.on)
        // return this.modes.color
        // else return "#626262"
        return this.modes[this.state.mode].color;
    }

    getLabel() {
        return this.modes[this.state.mode].label;
    }

    svg() {
        const { height, width, label } = this.props;
        return `
        <svg
        width="${width}"
        height="${height}"
        viewBox="0 0 ${width} ${height}"
        version="1.1"
        xmlns="http://www.w3.org/2000/svg">
            <circle cx="${width / 2}" cy="${height / 2}" r="${width / 3}" fill="${this.getColor()}" />
            <text x="${width / 2}" y="${(9 / 16) * height}" text-anchor="middle" fill="#000000"><tspan style="font-size:9.0">${this.getLabel()}</tspan></text>>


        </svg>
        `
    }

    render() {
        return (
            <View {...styles.container}{...this.props} onMouseDown={this._onMouseDown.bind(this)}>
                <Image {...styles.canvas} source={this.svg()} />
            </View>
        )
    }
}

const styles = {
    container: {
        'flex-direction': 'column',
    },
    canvas: {
        'height': '100%',
        'width': '100%',
        'interceptClickEvents': false,

        'position': 'absolute'
    },
}

