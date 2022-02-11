#ifndef CQChartsPenBrush_H
#define CQChartsPenBrush_H

#include <CQChartsFillPattern.h>

/*!
 * \brief Pen/Brush Data
 * \ingroup Charts
 */
struct CQChartsPenBrush {
  using FillType = CQChartsFillPattern::Type;

  QPen     pen;
  QBrush   brush;
  QColor   altColor;
  double   altAlpha   { 1.0 };
  double   fillAngle  { 0.0 };
  FillType fillType   { FillType::NONE };
  double   fillRadius { 0.0 };
  double   fillDelta  { 0.0 };

  CQChartsPenBrush() = default;

  CQChartsPenBrush(const QPen &pen, const QBrush &brush) :
   pen(pen), brush(brush) {
  }
};

#endif
