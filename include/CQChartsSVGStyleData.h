#ifndef CQChartsSVGStyleData_H
#define CQChartsSVGStyleData_H

#include <CQChartsArrowData.h>

struct CQChartsSVGStyleData {
  using ArrowData  = CQChartsArrowData;
  using Angle      = CQChartsAngle;
  using NameValues = std::map<QString, QString>;

  QPen   pen;
  QBrush brush;

  QString       textFont;
  Qt::Alignment textAlign  { Qt::AlignLeft | Qt::AlignVCenter };
  QString       textAngle;
  bool          textScaled { false };

  ArrowData arrowData;

  Angle arcStart    { 0.0 };
  Angle arcDelta    { 90.0 };
  Angle arcEnd      { 180.0 };
  Angle arcEndDelta { 90.0 };

  double innerRadius { 0.0 };
  double outerRadius { 1.0 };

  QString drawType;
  QString direction;

  QString symbolType;
  double  symbolSize { -1.0 };

  QString tip;

  NameValues nameValues;
};

#endif
