#ifndef CQChartsBoxPlot_H
#define CQChartsBoxPlot_H

#include <CQChartsPlot.h>
#include <CQChartsPlotObj.h>

#include <CBoxWhisker.h>
#include <map>

class CQChartsBoxPlot;

class CQChartsBoxPlotObj : public CQChartsPlotObj {
  Q_OBJECT

 public:
  CQChartsBoxPlotObj(CQChartsBoxPlot *plot, const CBBox2D &rect, double pos,
                     const CBoxWhisker &whisker, int i, int n);

  void draw(QPainter *p) override;

 private:
  CQChartsBoxPlot *plot_ { nullptr };
  double           pos_  { 0.0 };
  CBoxWhisker      whisker_;
  int              i_    { -1 };
  int              n_    { 0 };
};

//---

#include <CQChartsKey.h>

class CQChartsBoxKeyColor : public CQChartsKeyColorBox {
  Q_OBJECT

 public:
  CQChartsBoxKeyColor(CQChartsBoxPlot *plot, int i, int n);

  bool mousePress(const CPoint2D &p) override;

  QColor fillColor() const override;
};

class CQChartsBoxKeyText : public CQChartsKeyText {
  Q_OBJECT

 public:
  CQChartsBoxKeyText(CQChartsBoxPlot *plot, int i, const QString &text);

  QColor textColor() const override;

 private:
  int i_ { 0 };
};

//---

class CQChartsBoxPlot : public CQChartsPlot {
  Q_OBJECT

  Q_PROPERTY(int    xColumn  READ xColumn  WRITE setXColumn )
  Q_PROPERTY(int    yColumn  READ yColumn  WRITE setYColumn )
  Q_PROPERTY(QColor boxColor READ boxColor WRITE setBoxColor)

 public:
  CQChartsBoxPlot(CQChartsView *view, QAbstractItemModel *model);

  const char *typeName() const override { return "BoxPlot"; }

  int xColumn() const { return xColumn_; }
  void setXColumn(int i) { xColumn_ = i; update(); }

  int yColumn() const { return yColumn_; }
  void setYColumn(int i) { yColumn_ = i; update(); }

  const QColor &boxColor() const { return boxColor_; }
  void setBoxColor(const QColor &c) { boxColor_ = c; }

  void addProperties();

  void updateRange();

  void initObjs(bool force=false);

  void addKeyItems(CQChartsKey *key) override;

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
  typedef std::map<int,CBoxWhisker> Whiskers;
  typedef std::map<int,bool>        IdHidden;

  int      xColumn_  { 0 };
  int      yColumn_  { 1 };
  QColor   boxColor_ { "#46A2B4" };
  Whiskers whiskers_;
  IdHidden idHidden_;
};

#endif
