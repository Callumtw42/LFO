import React, { Component } from 'react'

import {
    Image,
    View,
    EventBridge,
} from 'juce-blueprint';

import { plotHeight, plotWidth, plotResolution, boxWidth } from "./lfo";

export default class PlayHead extends Component {
    constructor(props) {
        super(props)
        this._onIndexUpdated = this._onIndexUpdated.bind(this);

        this.state = { position: 0 }
    }

    // <path d="M${playX} ${0} L${playX} ${height} Z" stroke="${"#3ade3a"}" stroke-width="1"/>
    playhead() {
        const { playhead } = this.state;
        const { height, width } = this.props;
        const playX = (playhead / plotResolution) * width;
        return (
            ` < svg width="${width}" height="${height}" viewBox="0 0 0 0" xmlns="http://www.w3.org/2000/svg" >
            <rect x="${0}" y="${0}" stroke="green" stroke-width="1" fill="#2a302a" width="${width - 10}" height="${height - 10}" />
            </svg>
        `
        )
    }

    componentDidMount() {
        EventBridge.addListener('updateCurrentIndex', this._onIndexUpdated);
    }

    _onIndexUpdated(index) {
        // global.log(index);
        this.setState({ position: index })
    }

    render() {
        return (
            <View {...styles.container}>
                <Image {...styles.canvas} source={this.playhead()} />
            </View >
        )
    }
}

const styles = {
    playhead: {
        'height': plotHeight,
        'width': plotWidth,
        'position': 'absolute'
    },
    canvas: {
        'height': '100%',
        'width': '100%',
        'interceptClickEvents': false,

        'position': 'absolute'
    },
    container: {
        'height': plotHeight,
        'width': plotWidth,
        'flex-direction': 'column',
        'position': 'absolute',
        'left': 0,
        'top': 0
    },
}