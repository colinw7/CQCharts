#ifndef CQChartsPlotBarChart_H
#define CQChartsPlotBarChart_H

#include <CQChartsPlot.h>

class CQChartsAxis;

class CQChartsPlotBarChart : public CQChartsPlot {
  Q_OBJECT

  // properties
  //  bar color

  Q_PROPERTY(int xColumn READ xColumn WRITE setXColumn)
  Q_PROPERTY(int yColumn READ yColumn WRITE setYColumn)

 public:
  CQChartsPlotBarChart(QAbstractItemModel *model);

  int xColumn() const { return xColumn_; }
  void setXColumn(int i) { xColumn_ = i; }

  int yColumn() const { return yColumn_; }
  void setYColumn(int i) { yColumn_ = i; }

  void updateRange();

  void paintEvent(QPaintEvent *) override;

 private:
  double modelReal(int row, int col) const;

 private:
  typedef std::vector<double> Values;

  struct ValueSet {
    QString name;
    Values  values;

    ValueSet(const QString &name) :
     name(name) {
    }
  };

  typedef std::vector<ValueSet> ValueSets;

  int           xColumn_ { 0 };
  int           yColumn_ { 1 };
  CQChartsAxis* xAxis_ { nullptr };
  CQChartsAxis* yAxis_ { nullptr };
  ValueSets     valueSets_;
};

#endif
