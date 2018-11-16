import React from 'react';
import { bindActionCreators } from 'redux';
import { connect } from 'react-redux';
import * as Actions from '../logic/actions';
import LineChart from './LineChart';

export default connect(
  state => ({ ...state.reducer }),
  dispatch => bindActionCreators(Actions, dispatch),
)(({ deviceData }) => (
  deviceData ? deviceData.map(device => <LineChart key={device.attribute} {...device} />) : null));
