import React from 'react';
import { bindActionCreators } from 'redux';
import { Link } from 'react-router-dom';
import { connect } from 'react-redux';
import Sliders from 'react-feather/dist/icons/sliders';
import Heart from 'react-feather/dist/icons/heart';
import * as Actions from '../logic/actions';

export default connect(
  state => ({ ...state.reducer }),
  dispatch => bindActionCreators(Actions, dispatch),
)(({ devices }) => (
  <nav className="sidebar sidebar-sticky" key="headernav">
    <div className="sidebar-content">
      <Link className="sidebar-brand" to="/">
        <i className="align-middle" data-feather="box" />
        <span className="align-middle">
JDMT
        </span>
      </Link>
      <ul className="sidebar-nav">
        <li className="sidebar-header">
Menu
        </li>
        <li className="sidebar-item active">
          <Link className="sidebar-link" to="/">
            <Sliders />
            <span className="align-middle">
Dashboard
            </span>
          </Link>
        </li>
        {devices.map(device => (
          <li key={device.id} className="sidebar-item active">
            <Link className="sidebar-link" to={`/${device.id}`}>
              <Heart />
              <span className="align-middle">
                {device.id}
              </span>
            </Link>
          </li>
        ))}
      </ul>
    </div>
  </nav>
));
