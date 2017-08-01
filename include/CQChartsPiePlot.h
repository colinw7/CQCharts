#ifndef CQChartsPiePlot_H
#define CQChartsPiePlot_H

#include <CQChartsPlot.h>

class CQChartsPiePlot : public CQChartsPlot {
  Q_OBJECT

  Q_PROPERTY(bool donut   READ isDonut WRITE setDonut  )
  Q_PROPERTY(int  xColumn READ xColumn WRITE setXColumn)
  Q_PROPERTY(int  yColumn READ yColumn WRITE setYColumn)

  // propeties
  //   donut

 public:
  CQChartsPiePlot(QAbstractItemModel *model);

  bool isDonut() const { return donut_; }
  void setDonut(bool b) { donut_ = b; }

  int xColumn() const { return xColumn_; }
  void setXColumn(int i) { xColumn_ = i; }

  int yColumn() const { return yColumn_; }
  void setYColumn(int i) { yColumn_ = i; }

  void paintEvent(QPaintEvent *) override;

 private:
  bool donut_   { false };
  int  xColumn_ { 0 };
  int  yColumn_ { 1 };
};

#endif
