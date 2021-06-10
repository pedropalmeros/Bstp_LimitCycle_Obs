// %flair:license{
// This file is part of the Flair framework distributed under the
// CECILL-C License, Version 1.0.
// %flair:license}
/*!
 * \file obsAvoid_impl.h
 * \brief Classe permettant le calcul d'un Pid
 * \author Guillaume Sanahuja, Copyright Heudiasyc UMR UTC/CNRS 7253
 * \date 2011/05/01
 * \version 4.0
 */

#ifndef OBSAVOID_IMPL_H
#define OBSAVOID_IMPL_H

#include <Object.h>

namespace flair {
namespace core {
class Matrix;
class io_data;
}
namespace gui {
class LayoutPosition;
class DoubleSpinBox;
}
namespace filter {
class obsAvoid;
}
}

/*! \class obsAvoid_impl
* \brief Class defining a PID
*/

class obsAvoid_impl {
public:
  obsAvoid_impl(flair::filter::obsAvoid *self, const flair::gui::LayoutPosition *position,
           std::string name);
  ~obsAvoid_impl();
  void UseDefaultPlot(const flair::gui::LayoutPosition *position);
  void UpdateFrom(const flair::core::io_data *data);
  float i;
  bool first_update;
  float SatVal(float &signal, float saturation_value);
  float Distance(float x1, float y1, float x2, float y2);



private:
  flair::filter::obsAvoid *self;

  // matrix
  flair::core::Matrix *state;

  flair::gui::DoubleSpinBox *posX, *posY;

  /****     GAINS FOR APF             *******/
  flair::gui::DoubleSpinBox *radioAPFx, *radioAPFy;
  flair::gui::DoubleSpinBox *scaleAPFx, *scaleAPFy;
  flair::gui::DoubleSpinBox *satAPFx, *satAPFy;

  /****     GAINS FOR LC          ************/
  flair::gui::DoubleSpinBox *radioLC;
  flair::gui::DoubleSpinBox *alpha;
  flair::gui::DoubleSpinBox *gamma;
  flair::gui::DoubleSpinBox *scaleLC_x, *scaleLC_y;
  flair::gui::DoubleSpinBox *satLC_x, *satLC_y;


  flair::gui::DoubleSpinBox *sigma;
  flair::gui::DoubleSpinBox *scaleGen;
  flair::gui::DoubleSpinBox *satGen;

  //flair::gui::DoubleSpinBox *goal_x;
  //flair::gui::DoubleSpinBox *goal_y;

};

#endif // obsAvoid_impl_H
