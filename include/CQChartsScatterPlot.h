#ifndef CQChartsScatterPlot_H
#define CQChartsScatterPlot_H

#include <CQChartsPlot.h>
#include <CQChartsPlotObj.h>
#include <CQUtil.h>
#include <CPen.h>
#include <CBrush.h>
#include <CSymbolType.h>

class CQChartsScatterPlot;

class CQChartsScatterPointObj : public CQChartsPlotObj {
  Q_OBJECT

 public:
  CQChartsScatterPointObj(CQChartsScatterPlot *plot, const CBBox2D &rect, double x,
                     double y, double s, const QString &name, int ind);

  int ind() const { return ind_; }
  void setInd(int i) { ind_ = i; }

  bool inside(const CPoint2D &p) const override;

  void draw(QPainter *p) override;

 private:
  CQChartsScatterPlot *plot_ { nullptr };
  double               x_    { 0.0 };
  double               y_    { 0.0 };
  double               s_    { 0.0 };
  QString              name_;
  int                  ind_  { -1 };
};

//---

class CQChartsScatterPlot : public CQChartsPlot {
  Q_OBJECT

  Q_PROPERTY(int nameColumn READ nameColumn WRITE setNameColumn)
  Q_PROPERTY(int xColumn    READ xColumn    WRITE setXColumn   )
  Q_PROPERTY(int yColumn    READ yColumn    WRITE setYColumn   )

 public:
  typedef std::vector<QPointF>     Values;
  typedef std::map<QString,Values> NameValues;

 public:
  CQChartsScatterPlot(QAbstractItemModel *model);

  int nameColumn() const { return nameColumn_; }
  void setNameColumn(int i) { nameColumn_ = i; update(); }

  int xColumn() const { return xColumn_; }
  void setXColumn(int i) { xColumn_ = i; update(); }

  int yColumn() const { return yColumn_; }
  void setYColumn(int i) { yColumn_ = i; update(); }

  const NameValues &nameValues() const { return nameValues_; }

  void updateRange();

  int numRows() const;

  int nameIndex(const QString &name) const;

  void initObjs();

  void paintEvent(QPaintEvent *) override;

 private:
  int        nameColumn_ { -1 };
  int        xColumn_    { 0 };
  int        yColumn_    { 1 };
  NameValues nameValues_;
};

#endif
