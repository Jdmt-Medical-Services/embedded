import React from 'react';
import { BrowserRouter, Route, Switch } from 'react-router-dom';

import Map from './Map';
import Overview from './Overview';
import Header from './Header';
import Content from './Content';
import DeviceControll from './DeviceControll';

export default () => (
  <BrowserRouter>
    <div className="wrapper">
      <div className="d-flex">
        <Header />
        <Switch>
          <Route
            exact
            path="/"
            component={() => (
              <Content header="Dashboard">
                <div className="row">
                  <div className="col-12 col-lg-12 d-flex">
                    <div className="card flex-fill w-100">
                      <div className="card-body">
                        <Map />
                      </div>
                    </div>
                  </div>
                </div>
                <div className="row">
                  <Overview />
                </div>
              </Content>
            )}
          />
          <Route
            exact
            path="/:deviceId"
            component={() => (
              <Content header="test">
                <div className="form-row">
                  <div className="col-6 col-lg-6 d-flex">
                    <div className="card flex-fill w-100">
                      <div className="card-body">
                        <DeviceControll />
                      </div>
                    </div>
                  </div>
                  <div className="col-6 col-lg-6 d-flex">
                    <div className="card flex-fill w-100">
                      <div className="card-body">
                        <Map />
                      </div>
                    </div>
                  </div>
                </div>
                <div className="row">
                  <div className="col-12 col-lg-12 d-flex">
                    <div className="card flex-fill w-100">
                      <div className="card-body">
                        LineCharts
                      </div>
                    </div>
                  </div>
                </div>
              </Content>
            )}
          />
        </Switch>
      </div>
    </div>
  </BrowserRouter>
);