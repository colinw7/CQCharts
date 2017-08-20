#ifndef CQChartsBoxPlot_H
#define CQChartsBoxPlot_H

#include <CQChartsPlot.h>
#include <CBoxWhisker.h>

#include <CQChartsPlotObj.h>

class CQChartsBoxPlot;

class CQChartsBoxObj : public CQChartsPlotObj {
  Q_OBJECT

 public:
  CQChartsBoxObj(CQChartsBoxPlot *plot, const CBBox2D &rect,
                 double pos, const CBoxWhisker &whisker, int ind);

  void draw(QPainter *p) override;

 private:
  CQChartsBoxPlot *plot_ { nullptr };
  double           pos_  { 0.0 };
  CBoxWhisker      whisker_;
  int              ind_  { -1 };
};

//---

class CQChartsBoxPlot : public CQChartsPlot {
  Q_OBJECT

  Q_PROPERTY(int    xColumn  READ xColumn  WRITE setXColumn )
  Q_PROPERTY(int    yColumn  READ yColumn  WRITE setYColumn )
  Q_PROPERTY(QColor boxColor READ boxColor WRITE setBoxColor)

 public:
  CQChartsBoxPlot(CQChartsWindow *window, QAbstractItemModel *model);

  int xColumn() const { return xColumn_; }
  void setXColumn(int i) { xColumn_ = i; update(); }

  int yColumn() const { return yColumn_; }
  void setYColumn(int i) { yColumn_ = i; update(); }

  const QColor &boxColor() const { return boxColor_; }
  void setBoxColor(const QColor &v) { boxColor_ = v; }

  void addProperties();

  void init();

  void initObjs();

  int numObjs() const;

  void draw(QPainter *) override;

 private:
  typedef std::map<int,CBoxWhisker> Whiskers;

  int      xColumn_  { 0 };
  int      yColumn_  { 1 };
  QColor   boxColor_ { "#46A2B4" };
  Whiskers whiskers_;
};

#endif
