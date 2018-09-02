#ifndef CQChartsViewData_H
#define CQChartsViewData_H

#include <CQChartsData.h>
#include <CQChartsUtil.h>

#define CQCHARTS_VIEW_NAMED_SHAPE_DATA_PROPERTIES(UNAME,LNAME) \
Q_PROPERTY(bool             LNAME##Border \
           READ is##UNAME##Border  WRITE set##UNAME##Border     ) \
Q_PROPERTY(CQChartsColor    LNAME##BorderColor \
           READ LNAME##BorderColor WRITE set##UNAME##BorderColor) \
Q_PROPERTY(double           LNAME##BorderAlpha \
           READ LNAME##BorderAlpha WRITE set##UNAME##BorderAlpha) \
Q_PROPERTY(CQChartsLength   LNAME##BorderWidth \
           READ LNAME##BorderWidth WRITE set##UNAME##BorderWidth) \
Q_PROPERTY(CQChartsLineDash LNAME##BorderDash \
           READ LNAME##BorderDash  WRITE set##UNAME##BorderDash ) \
\
Q_PROPERTY(bool                LNAME##Filled \
           READ is##UNAME##Filled  WRITE set##UNAME##Filled     ) \
Q_PROPERTY(CQChartsColor       LNAME##FillColor \
           READ LNAME##FillColor   WRITE set##UNAME##FillColor  ) \
Q_PROPERTY(double              LNAME##FillAlpha \
           READ LNAME##FillAlpha   WRITE set##UNAME##FillAlpha  ) \
Q_PROPERTY(CQChartsFillPattern LNAME##FillPattern \
           READ LNAME##FillPattern WRITE set##UNAME##FillPattern)

class CQChartsView;

#define CQCHARTS_VIEW_NAMED_SHAPE_DATA(UNAME,LNAME) \
class CQChartsView##UNAME##ShapeData { \
 public: \
  CQChartsView##UNAME##ShapeData(CQChartsView *view) : \
   LNAME##ShapeDataView_(view) { \
  } \
\
  bool is##UNAME##Border() const { return LNAME##ShapeData_.border.visible; } \
  void set##UNAME##Border(bool b) { \
    CQChartsUtil::testAndSet(LNAME##ShapeData_.border.visible, b, [&]() { \
      LNAME##ShapeDataInvalidate(); } ); \
  } \
\
  const CQChartsColor &LNAME##BorderColor() const { return LNAME##ShapeData_.border.color; } \
  void set##UNAME##BorderColor(const CQChartsColor &c) { \
    CQChartsUtil::testAndSet(LNAME##ShapeData_.border.color, c, [&]() { \
      LNAME##ShapeDataInvalidate(); } ); \
  } \
\
  double LNAME##BorderAlpha() const { return LNAME##ShapeData_.border.alpha; } \
  void set##UNAME##BorderAlpha(double a) { \
    CQChartsUtil::testAndSet(LNAME##ShapeData_.border.alpha, a, [&]() { \
      LNAME##ShapeDataInvalidate(); } ); \
  } \
\
  const CQChartsLength &LNAME##BorderWidth() const { return LNAME##ShapeData_.border.width; } \
  void set##UNAME##BorderWidth(const CQChartsLength &l) { \
    CQChartsUtil::testAndSet(LNAME##ShapeData_.border.width, l, [&]() { \
      LNAME##ShapeDataInvalidate(); } ); \
  } \
\
  const CQChartsLineDash &LNAME##BorderDash() const { return LNAME##ShapeData_.border.dash; } \
  void set##UNAME##BorderDash(const CQChartsLineDash &d) { \
    CQChartsUtil::testAndSet(LNAME##ShapeData_.border.dash, d, [&]() { \
      LNAME##ShapeDataInvalidate(); } ); \
  } \
\
  QColor interp##UNAME##BorderColor(int i, int n) const { \
    return LNAME##BorderColor().interpColor(LNAME##ShapeDataView_, i, n); \
  } \
\
  bool is##UNAME##Filled() const { return LNAME##ShapeData_.background.visible; } \
  void set##UNAME##Filled(bool b) { \
    CQChartsUtil::testAndSet(LNAME##ShapeData_.background.visible, b, [&]() { \
      LNAME##ShapeDataInvalidate(); } ); \
  } \
\
  const CQChartsColor &LNAME##FillColor() const { return LNAME##ShapeData_.background.color; } \
  void set##UNAME##FillColor(const CQChartsColor &c) { \
    CQChartsUtil::testAndSet(LNAME##ShapeData_.background.color, c, [&]() { \
      LNAME##ShapeDataInvalidate(); } ); \
  } \
\
  double LNAME##FillAlpha() const { return LNAME##ShapeData_.background.alpha; } \
  void set##UNAME##FillAlpha(double a) { \
    CQChartsUtil::testAndSet(LNAME##ShapeData_.background.alpha, a, [&]() { \
      LNAME##ShapeDataInvalidate(); } ); \
  } \
\
  const CQChartsFillPattern &LNAME##FillPattern() const { \
    return LNAME##ShapeData_.background.pattern; } \
  void set##UNAME##FillPattern(const CQChartsFillPattern &p) { \
    CQChartsUtil::testAndSet(LNAME##ShapeData_.background.pattern, p, [&]() { \
      LNAME##ShapeDataInvalidate(); } ); \
  } \
\
  QColor interp##UNAME##FillColor(int i, int n) const { \
    return LNAME##FillColor().interpColor(LNAME##ShapeDataView_, i, n); \
  } \
\
 private: \
  void LNAME##ShapeDataInvalidate() { \
  } \
\
 private: \
  CQChartsView* LNAME##ShapeDataView_ { nullptr }; \
\
 protected: \
  CQChartsShapeData LNAME##ShapeData_; \
};

#endif
