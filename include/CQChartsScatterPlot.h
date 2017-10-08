#ifndef CQChartsScatterPlot_H
#define CQChartsScatterPlot_H

#include <CQChartsPlot.h>
#include <CQChartsPlotObj.h>
#include <CQChartsValueSet.h>

class CQChartsScatterPlot;

class CQChartsScatterPointObj : public CQChartsPlotObj {
  Q_OBJECT

 public:
  CQChartsScatterPointObj(CQChartsScatterPlot *plot, const CBBox2D &rect, const QPointF &p,
                          double size, double color, int i, int n);

  bool inside(const CPoint2D &p) const override;

  void draw(QPainter *p, const CQChartsPlot::Layer &) override;

 private:
  CQChartsScatterPlot *plot_  { nullptr };
  QPointF              p_;
  double               size_  { -1 };
  double               color_ { -1 };
  int                  i_     { -1 };
  int                  n_     { -1 };
};

//---

#include <CQChartsKey.h>

class CQChartsScatterKeyColor : public CQChartsKeyColorBox {
  Q_OBJECT

 public:
  CQChartsScatterKeyColor(CQChartsScatterPlot *plot, int i, int n);

  bool mousePress(const CPoint2D &p) override;

  QColor fillColor() const override;
};

//---

class CQChartsScatterPlotType : public CQChartsPlotType {
 public:
  CQChartsScatterPlotType();

  QString name() const override { return "scatter"; }
  QString desc() const override { return "Scatter"; }

  CQChartsPlot *create(CQChartsView *view, const ModelP &model) const override;
};

//---

class CQChartsScatterPlot : public CQChartsPlot {
  Q_OBJECT

  Q_PROPERTY(int    nameColumn        READ nameColumn        WRITE setNameColumn       )
  Q_PROPERTY(int    xColumn           READ xColumn           WRITE setXColumn          )
  Q_PROPERTY(int    yColumn           READ yColumn           WRITE setYColumn          )
  Q_PROPERTY(int    sizeColumn        READ sizeColumn        WRITE setSizeColumn       )
  Q_PROPERTY(int    colorColumn       READ colorColumn       WRITE setColorColumn      )
  Q_PROPERTY(double symbolSize        READ symbolSize        WRITE setSymbolSize       )
  Q_PROPERTY(QColor symbolBorderColor READ symbolBorderColor WRITE setSymbolBorderColor)
  Q_PROPERTY(double symbolSizeMin     READ symbolSizeMin     WRITE setSymbolSizeMin    )
  Q_PROPERTY(double symbolSizeMax     READ symbolSizeMax     WRITE setSymbolSizeMax    )

 public:
  struct Point {
    QPointF p;
    QString sizeStr;
    double  size  { -1 };
    QString colorStr;
    double  color { -1 };

    Point(double x, double y, const QString &sizeStr="", double size=-1,
          const QString &colorStr="", double color=-1) :
     p(x, y), sizeStr(sizeStr), size(size), colorStr(colorStr), color(color) {
    }
  };

  typedef std::vector<Point>       Values;
  typedef std::map<QString,Values> NameValues;

 public:
  CQChartsScatterPlot(CQChartsView *view, const ModelP &model);

  //---

  int nameColumn() const { return nameColumn_; }
  void setNameColumn(int i) { nameColumn_ = i; update(); }

  int xColumn() const { return xColumn_; }
  void setXColumn(int i) { xColumn_ = i; update(); }

  int yColumn() const { return yColumn_; }
  void setYColumn(int i) { yColumn_ = i; update(); }

  int sizeColumn() const { return sizeColumn_; }
  void setSizeColumn(int i) { sizeColumn_ = i; update(); }

  int colorColumn() const { return colorColumn_; }
  void setColorColumn(int i) { colorColumn_ = i; update(); }

  //---

  double symbolSize() const { return symbolSize_; }
  void setSymbolSize(double s) { symbolSize_ = s; initObjs(/*force*/true); update(); }

  const QColor &symbolBorderColor() const { return symbolBorderColor_; }
  void setSymbolBorderColor(const QColor &c) { symbolBorderColor_ = c; update(); }

  double symbolSizeMin() const { return symbolSizeMin_; }
  void setSymbolSizeMin(double r) { symbolSizeMin_ = r; initObjs(/*force*/true); update(); }

  double symbolSizeMax() const { return symbolSizeMax_; }
  void setSymbolSizeMax(double r) { symbolSizeMax_ = r; initObjs(/*force*/true); update(); }

  //---

  const NameValues &nameValues() const { return nameValues_; }

  //---

  void addProperties() override;

  void updateRange() override;

  void initObjs(bool force=false) override;

  //---

  int numRows() const;

  int nameIndex(const QString &name) const;

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

  double mapSymbolSize(double s) const;

 private:
  typedef std::map<int,bool> IdHidden;

  int              nameColumn_        { -1 };
  int              xColumn_           { 0 };
  int              yColumn_           { 1 };
  int              sizeColumn_        { -1 };
  int              colorColumn_       { -1 };
  double           symbolSize_        { 4 };
  QColor           symbolBorderColor_ { Qt::black };
  double           symbolSizeMin_     { 16 };
  double           symbolSizeMax_     { 64 };
  CQChartsValueSet sizeSet_;
  CQChartsValueSet colorSet_;
  NameValues       nameValues_;
  IdHidden         idHidden_;
};

#endif
