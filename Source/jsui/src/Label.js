import {
  EventBridge,
} from 'juce-blueprint';

class Label {
  constructor(paramId) {
    this.paramId = paramId
    this._onParameterValueChange = this._onParameterValueChange.bind(this);
    this.label = "";
    EventBridge.addListener('parameterValueChange', this._onParameterValueChange);
  }

  _onParameterValueChange(index, paramId, defaultValue, currentValue, stringValue) {
    if (paramId === this.paramId) {
      this.label = stringValue
    }
  }
}

export default Label;
