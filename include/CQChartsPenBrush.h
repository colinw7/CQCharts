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
  double   altAlpha  { 1.0 };
  double   fillAngle { 45 };
  FillType fillType  { FillType::NONE };

  CQChartsPenBrush() = default;

  CQChartsPenBrush(const QPen &pen, const QBrush &brush) :
   pen(pen), brush(brush) {
  }
};

#endif
