// %flair:license{
// This file is part of the Flair framework distributed under the
// CECILL-C License, Version 1.0.
// %flair:license}
//  created:    2011/05/01
//  filename:   obsAvoid_impl.cpp
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
#include "obsAvoid_impl.h"
#include "obsAvoid.h"
#include <Matrix.h>
#include <LayoutPosition.h>
#include <Layout.h>
#include <GroupBox.h>
#include <DoubleSpinBox.h>
#include <DataPlot1D.h>
#include <DataPlot2D.h>
#include <iostream>
#include <AhrsData.h>
#include <DataPlot1D.h>
#include <math.h>
#include <IODevice.h>
#include <string.h>



using namespace std;

using std::string;
using namespace flair::core;
using namespace flair::gui;
using namespace flair::filter;

obsAvoid_impl::obsAvoid_impl(obsAvoid *self, const LayoutPosition *position, string name) {
  i = 0;
  first_update = true;
  this->self = self;

  // init matrix
  self->input = new Matrix(self, 6, 1, floatType, name);
  MatrixDescriptor *desc = new MatrixDescriptor(9, 1);
  desc->SetElementName(0, 0, "APF->X");
  desc->SetElementName(1, 0, "APF->Y");
  desc->SetElementName(2, 0, "LC->X");
  desc->SetElementName(3, 0, "LC->Y");
  desc->SetElementName(4, 0, "Activation");
  desc->SetElementName(5, 0, "ObsTotal->X");
  desc->SetElementName(6, 0, "ObsTotal->Y");
  desc->SetElementName(7, 0, "UAV X");
  desc->SetElementName(8, 0, "UAV Y");


  state = new Matrix(self, desc, floatType, name);
  
  self->AddDataToLog(state);

  delete desc;

  std::string gboxPositionName = name +"Position";
  std::string gboxPositionGraph = name +"Pos_Graph";

  std::string gboxAPFName = name +" APF Conf";
  std::string gboxLCName = name +" Limit Cycle Conf";
  std::string gboxActivationName = name + " Activation Params";
  std::string gboxGenConfName = name + " General Sat & Scale";
  std::string gboxGoalConf = name + " Goal Conf";

  GroupBox *gboxGen = new GroupBox(position, name + " CONF");

  GroupBox *gboxPosition = new GroupBox(gboxGen->NewRow(), gboxPositionName);
  //GroupBox *gboxGraph_pal = new GroupBox(gboxGen->LastRowLastCol(),gboxPositionGraph);

  /*** DEFINE THE OBSTACLE POSITION ***/
  posX =  new DoubleSpinBox(gboxPosition->NewRow(), "Pos X:", " m",-10, 10, 0.01);
  posY =  new DoubleSpinBox(gboxPosition->LastRowLastCol(), "Pos Y:", " m",-10, 10, 0.01);

  /*** DEFINE THE PARAMETERS FOR APF  *****/
  GroupBox *gboxAPF = new GroupBox(gboxGen->NewRow(),gboxAPFName);
  radioAPFx = new DoubleSpinBox(gboxAPF->NewRow(), "Radio_x", " m",0, 10, 0.01);
  radioAPFy = new DoubleSpinBox(gboxAPF->LastRowLastCol(), "Radio_y", " m",0, 10, 0.01);

  scaleAPFx = new DoubleSpinBox(gboxAPF->NewRow(), "Scale_x","",-10,10,0.01);
  scaleAPFy = new DoubleSpinBox(gboxAPF->LastRowLastCol(), "Scale_y","",-10,10,0.01);

  satAPFx   = new DoubleSpinBox(gboxAPF->NewRow(), "Sat_x", "", 0,100,0.01);
  satAPFy   = new DoubleSpinBox(gboxAPF->LastRowLastCol(), "Sat_y", "", 0,100,0.01);

  /***    DEFINE THE LC PARAMS      ****/
  GroupBox *gboxLC  = new GroupBox(gboxGen->NewRow(),gboxLCName);
  radioLC = new DoubleSpinBox(gboxLC->NewRow(), "Rad:"," m",-10,10,0.01);

  alpha = new DoubleSpinBox(gboxLC->NewRow(), "alpha:","",-100,100,0.01);

  gamma = new DoubleSpinBox(gboxLC->LastRowLastCol(), "gamma:","",-10,10,0.01);

  scaleLC_x = new DoubleSpinBox(gboxLC->NewRow(), "Scale_x", "",-100,100,0.01);
  scaleLC_y = new DoubleSpinBox(gboxLC->LastRowLastCol(), "Scale_y", "",-100,100,0.01);

  satLC_x = new DoubleSpinBox(gboxLC->NewRow(), "Sat_x", "", 0,100,0.01);
  satLC_y = new DoubleSpinBox(gboxLC->LastRowLastCol(), "Sat_y", "", 0,100,0.01);


  /***    DEFINE THE ACTIVATION FUNCTION      ******/
  GroupBox *gboxActivation = new GroupBox(gboxGen->NewRow(),gboxActivationName);
  sigma = new DoubleSpinBox(gboxActivation->NewRow(),"Sigma","",-10,10,0.01);



  GroupBox *gboxGenConf  = new GroupBox(gboxGen->NewRow(),gboxGenConfName);
  scaleGen = new DoubleSpinBox(gboxGenConf->NewRow(), "Tot Scale:","",-100,100,0.01);
  satGen = new DoubleSpinBox(gboxGenConf->NewRow(), "Tot Sat:","",0,50,0.01);

}

obsAvoid_impl::~obsAvoid_impl(void) {}

void obsAvoid_impl::UseDefaultPlot(const LayoutPosition *position) {
  GroupBox *ObsGraphics = new GroupBox(position,  "PLOTTING DATA");

  GroupBox *ObsGraphicsX = new GroupBox(ObsGraphics->NewRow(),"X-Data");
  DataPlot1D *plotX = new DataPlot1D(ObsGraphicsX->NewRow(), "Rejection Forces along X axis", -1, 1);
  plotX->AddCurve(state->Element(0), DataPlot::Black);
  plotX->AddCurve(state->Element(2), DataPlot::Blue);
  plotX->AddCurve(state->Element(5), DataPlot::Red);
  plotX->AddCurve(state->Element(4), DataPlot::Yellow);
  plotX->AddCurve(state->Element(7), DataPlot::Red);

  GroupBox *ObsGraphicsY = new GroupBox(ObsGraphics->LastRowLastCol(),"Y-Data");
  DataPlot1D *plotY = new DataPlot1D(ObsGraphicsY->NewRow(), "Rejection Forces along Y axis", -1, 1);
  plotY->AddCurve(state->Element(1), DataPlot::Black);
  plotY->AddCurve(state->Element(3), DataPlot::Blue);
  plotY->AddCurve(state->Element(6), DataPlot::Red);
  plotY->AddCurve(state->Element(4), DataPlot::Yellow);
  plotY->AddCurve(state->Element(8), DataPlot::Red);

  GroupBox *ObsGraphicsXY = new GroupBox(ObsGraphics->NewRow(),"Position on plane XY");
  DataPlot2D *plotXY = new DataPlot2D(ObsGraphicsXY->NewRow(),"Position XY plane","x [m]",-6,6,"y [m]",-6,6);
  plotXY->AddCurve(state->Element(7),state->Element(8),DataPlot::Black,"uav pos");
}

void obsAvoid_impl::UpdateFrom(const io_data *data) {

  const Matrix* input = dynamic_cast<const Matrix*>(data);
  
  if (!input) {
      self->Warn("casting %s to Matrix failed\n",data->ObjectName().c_str());
      return;
  }

  input->GetMutex();

  float uav_x = input->ValueNoMutex(0,0);
  float uav_y = input->ValueNoMutex(1,0);
  float dot_x = input->ValueNoMutex(2,0);
  float dot_y = input->ValueNoMutex(3,0);
  float goal_x = input->ValueNoMutex(4,0);
  float goal_y = input->ValueNoMutex(5,0);

  input->ReleaseMutex();

  float delta_x = uav_x - posX->Value();
  float delta_y = uav_y - posY->Value();

  float error_x = uav_x - goal_x;
  float error_y = uav_y - goal_y;

  float distance = Distance(uav_x,uav_y,posX->Value(),posY->Value());

  /*****************************************************************************
  *********   COMPUTING THE Artificial Potential Field COMPONENT
  ******************************************************************************/

  float Fc1 = 1/distance;
  float Fc2x = 1/radioAPFx->Value();
  float Fc2y = 1/radioAPFy->Value();
  float Fc3 = pow(1/distance,2);

  float APFX = -(Fc1 - Fc2x)*Fc3*delta_x;
  float APFY = -(Fc1 - Fc2y)*Fc3*delta_y;

  float APFactivation = 0;

  if(distance > radioAPFx->Value() || distance > radioAPFy->Value()){
    APFactivation = 0.0;
  }
  else{
    APFactivation = 1.0; 
  }

  APFX *= scaleAPFx->Value();
  SatVal(APFX,satAPFx->Value());

  APFY *= scaleAPFy->Value();
  SatVal(APFY,satAPFy->Value());

  APFX *= APFactivation;
  APFY *= APFactivation;

  float MagnitudAPF = sqrt(pow(APFX,2)+pow(APFY,2));

  /********************************************************************************
  ************          COMPUTING THE Limit Cycle  COMPONENT
  *********************************************************************************
  *** alpha can take values of -1 or 1 this is done to compute the sense of rotation
  *********************************************************************************/
  int LCActivation = 0.0;

  if(distance > radioLC->Value()){
    LCActivation = 0.0;
  }
  else{
    LCActivation = 1.0; 
  }

  float commonLC = pow(radioLC->Value(),2) - pow(delta_x,2) - pow(delta_y,2);

  float LCX = -alpha->Value()*delta_y + gamma->Value()*delta_x*commonLC;
  float LCY =  alpha->Value()*delta_x + gamma->Value()*delta_y*commonLC;

  LCX *= scaleLC_x->Value();
  LCY *= scaleLC_y->Value();
  SatVal(LCX,satLC_x->Value());
  SatVal(LCY,satLC_y->Value());

  LCX *= LCActivation;
  LCY *= LCActivation;
  

  float ObsTotalX = LCX;
  float ObsTotalY = LCY;

  //cout << "e_x: " << error_x << "   e_y: " << error_y << "   ";
  //cout << "Fx: " << LCX << "   Fy: " << LCY<<"   ";




  /*****************************************************************************
  *********   COMPUTING THE Total Rejection Force COMPONENT
  ******************************************************************************/


  state->GetMutex();
  state->SetValueNoMutex(0, 0, APFX);
  state->SetValueNoMutex(1, 0, APFY);
  state->SetValueNoMutex(2, 0, LCX);
  state->SetValueNoMutex(3, 0, LCY);
  state->SetValueNoMutex(4, 0, 0.0);
  state->SetValueNoMutex(5, 0, 0.0);
  state->SetValueNoMutex(6, 0, 0.0);
  state->SetValueNoMutex(7, 0, uav_x);
  state->SetValueNoMutex(8, 0, uav_y);

  state->ReleaseMutex();

  self->output->SetValue(0, 0, ObsTotalX);
  self->output->SetValue(1, 0, ObsTotalY);
  self->output->SetDataTime(data->DataTime());
}


float obsAvoid_impl::SatVal(float &signal, float saturation_value){
  if (signal > saturation_value){
    signal = saturation_value;
  }
  else if(signal < -saturation_value){
    signal = -saturation_value;
  }
  else
    signal = signal;
}


float obsAvoid_impl::Distance(float x1, float y1, float x2, float y2){
  return sqrt(pow(x1-x2,2)+pow(y1-y2,2));
}
