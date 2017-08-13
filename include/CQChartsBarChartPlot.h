#ifndef CQChartsBarChartPlot_H
#define CQChartsBarChartPlot_H

#include <CQChartsPlot.h>
#include <CQChartsPlotObj.h>

class CQChartsBarChartPlot;

class CQChartsBarChartObj : public CQChartsPlotObj {
  Q_OBJECT

 public:
  CQChartsBarChartObj(CQChartsBarChartPlot *plot, const CBBox2D &rect, int iset, int ival);

  void draw(QPainter *p) override;

 private:
  CQChartsBarChartPlot *plot_ { nullptr };
  int                   iset_ { -1 };
  int                   ival_ { -1 };
};

//---

#include <CQChartsKey.h>

class CQChartsBarKeyColor : public CQChartsKeyItem {
  Q_OBJECT

 public:
  CQChartsBarKeyColor(CQChartsBarChartPlot *plot, bool valueColor, int ind);

  QSizeF size() const override;

  void draw(QPainter *p, const CBBox2D &rect) override;

 private:
  CQChartsBarChartPlot *plot_       { nullptr };
  int                   valueColor_ { false };
  int                   ind_        { 0 };
};

//---

class CQChartsBarChartPlot : public CQChartsPlot {
  Q_OBJECT

  // properties
  //  bar color

  Q_PROPERTY(int    xColumn  READ xColumn   WRITE setXColumn )
  Q_PROPERTY(int    yColumn  READ yColumn   WRITE setYColumn )
  Q_PROPERTY(QColor barColor READ barColor  WRITE setBarColor)
  Q_PROPERTY(bool   stacked  READ isStacked WRITE setStacked )

 public:
  CQChartsBarChartPlot(QAbstractItemModel *model);

  int xColumn() const { return xColumn_; }
  void setXColumn(int i) { xColumn_ = i; }

  int yColumn() const { return yColumn_; }
  void setYColumn(int i) { yColumn_ = i; }

  const Columns &yColumns() const { return yColumns_; }
  void setYColumns(const Columns &yColumns) { yColumns_ = yColumns; }

  const QColor &barColor() const { return barColor_; }
  void setBarColor(const QColor &c) { barColor_ = c; update(); }

  bool isStacked() const { return stacked_; }
  void setStacked(bool b) { stacked_ = b; initObjs(/*force*/true); update(); }

  void updateRange();

  void initObjs(bool force=false);

  int numSets() const;

  int getSetColumn(int i) const;

  int numValueSets() const;

  int numSetValues() const;

  void paintEvent(QPaintEvent *) override;

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
  typedef std::vector<QString>  ValueNames;

 private:
  ValueSet *getValueSet(const QString &name);

 private:
  int        xColumn_  { 0 };
  int        yColumn_  { 1 };
  Columns    yColumns_;
  QColor     barColor_ { 100, 100, 200 };
  bool       stacked_  { false };
  ValueSets  valueSets_;
  ValueNames valueNames_;
};

#endif
