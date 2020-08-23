import React, { Component } from 'react';
import ParamDial from './paramdial';
import { LFO, lfoHeight, lfoWidth } from "./lfo/lfo";
import {
  View,
  Text
} from 'juce-blueprint';

class App extends Component {
  constructor() {
    super()
    this.state = {
      lfoProps: {
        x: 0,
        y: 0,
        activeLFO: "volume",
      },
    }
  }

  render() {
    const { lfoProps, labelProps } = this.state
    return (
      <View {...styles.container}>
        < LFO paramId={"volume"} {...lfoProps} ></LFO >
      </View >
    );
  }
}

const styles = {
  container: {
    'width': '100%',
    'height': '100%',
    'background-color': 'ff17191f',
    'justify-content': 'center',
    'flex-direction': 'column'
  },
};

export default App;
