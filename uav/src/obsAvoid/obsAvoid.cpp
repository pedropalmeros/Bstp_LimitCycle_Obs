// %flair:license{
// This file is part of the Flair framework distributed under the
// CECILL-C License, Version 1.0.
// %flair:license}
//  created:    2011/05/01
//  filename:   Pid.cpp
//
//  author:     Guillaume Sanahuja
//              Copyright Heudiasyc UMR UTC/CNRS 7253
//
//  version:    $Id: $
//
//  purpose:    Class defining a PID
//
//
/*********************************************************************/

#include "obsAvoid.h"
#include "obsAvoid_impl.h"
#include <Matrix.h>
#include <Layout.h>
#include <LayoutPosition.h>

using std::string;
using namespace flair::core;
using namespace flair::gui;

namespace flair {
namespace filter {

obsAvoid::obsAvoid(const LayoutPosition *position, string name)
    : ControlLaw(position->getLayout(), name,3) {
  pimpl_ = new obsAvoid_impl(this, position, name);
  
  SetIsReady(true);
}

obsAvoid::~obsAvoid(void) { delete pimpl_; }

void obsAvoid::UseDefaultPlot(const gui::LayoutPosition *position) {
  pimpl_->UseDefaultPlot(position);
}

void obsAvoid::Reset(void) {
  pimpl_->i = 0;
  pimpl_->first_update = true;
}

float obsAvoid::GetIntegral(void) const { return pimpl_->i; }

void obsAvoid::UpdateFrom(const io_data *data) {
  pimpl_->UpdateFrom(data);
  ProcessUpdate(output);
}

void obsAvoid::SetValues(float uav_pos_x, float uav_pos_y, float uav_vel_x, float uav_vel_y, float goal_x, float goal_y) {
  input->SetValue(0, 0, uav_pos_x);
  input->SetValue(1, 0, uav_pos_y);
  input->SetValue(2, 0, uav_vel_x);
  input->SetValue(3, 0, uav_vel_y);
  input->SetValue(4, 0, goal_x);
  input->SetValue(5, 0, goal_y);
}

} // end namespace filter
} // end namespace flair
