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
  CQChartsScatterPointObj(CQChartsScatterPlot *plot, const CBBox2D &rect, const QPointF &p,
                          int i, int n);

  bool inside(const CPoint2D &p) const override;

  void draw(QPainter *p) override;

 private:
  CQChartsScatterPlot *plot_ { nullptr };
  QPointF              p_;
  int                  i_    { -1 };
  int                  n_    { -1 };
};

//---

#include <CQChartsKey.h>

class CQChartsScatterKeyColor : public CQChartsKeyColorBox {
  Q_OBJECT

 public:
  CQChartsScatterKeyColor(CQChartsScatterPlot *plot, int i, int n);

  void mousePress(const CPoint2D &p) override;
};

//---

class CQChartsScatterPlot : public CQChartsPlot {
  Q_OBJECT

  Q_PROPERTY(int nameColumn READ nameColumn WRITE setNameColumn)
  Q_PROPERTY(int xColumn    READ xColumn    WRITE setXColumn   )
  Q_PROPERTY(int yColumn    READ yColumn    WRITE setYColumn   )
  Q_PROPERTY(int symbolSize READ symbolSize WRITE setSymbolSize)

 public:
  typedef std::vector<QPointF>     Values;
  typedef std::map<QString,Values> NameValues;

 public:
  CQChartsScatterPlot(CQChartsWindow *window, QAbstractItemModel *model);

  int nameColumn() const { return nameColumn_; }
  void setNameColumn(int i) { nameColumn_ = i; update(); }

  int xColumn() const { return xColumn_; }
  void setXColumn(int i) { xColumn_ = i; update(); }

  int yColumn() const { return yColumn_; }
  void setYColumn(int i) { yColumn_ = i; update(); }

  int symbolSize() const { return symbolSize_; }
  void setSymbolSize(int i) { symbolSize_ = i; }

  const NameValues &nameValues() const { return nameValues_; }

  //---

  void addProperties();

  void updateRange();

  int numRows() const;

  int nameIndex(const QString &name) const;

  void initObjs(bool force=false);

  //---

  bool isSetHidden(int i) const {
    auto p = idHidden_.find(i);

    if (p == idHidden_.end())
      return false;

    return (*p).second;
  }

  void setSetHidden(int i, bool hidden) { idHidden_[i] = hidden; }

  //---

  void draw(QPainter *) override;

 private:
  typedef std::map<int,bool> IdHidden;

  int        nameColumn_ { -1 };
  int        xColumn_    { 0 };
  int        yColumn_    { 1 };
  int        symbolSize_ { 4 };
  NameValues nameValues_;
  IdHidden   idHidden_;
};

#endif
