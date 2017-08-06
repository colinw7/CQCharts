#ifndef CQChartsPlot_H
#define CQChartsPlot_H

#include <CQChartsQuadTree.h>
#include <QFrame>
#include <CDisplayRange2D.h>
#include <CRange2D.h>

class CQChartsPlotExpander;
class CQChartsAxis;
class CQChartsPlotObj;

class CQPropertyTree;
class CGradientPalette;
class QAbstractItemModel;

class CQChartsPlot : public QFrame {
  Q_OBJECT

  Q_PROPERTY(QColor background READ background WRITE setBackground)

 public:
  typedef std::vector<int> Columns;

 public:
  CQChartsPlot(QWidget *parent, QAbstractItemModel *model);

  QAbstractItemModel *model() const { return model_; }

  const CRange2D &dataRange() const { return dataRange_; }
  void setDataRange(const CRange2D &r) { dataRange_ = r; }

  CGradientPalette *palette() const { return palette_; }
  void setPalette(CGradientPalette *palette) { palette_ = palette; }

  const QColor &background() const { return background_; }
  void setBackground(const QColor &v) { background_ = v; update(); }

  void windowToPixel(double wx, double wy, double &px, double &py);
  void pixelToWindow(double px, double py, double &wx, double &wy);

  void windowToPixel(const CPoint2D &w, CPoint2D &p);
  void pixelToWindow(const CPoint2D &p, CPoint2D &w);

  void windowToPixel(const CBBox2D &wrect, CBBox2D &prect);
  void pixelToWindow(const CBBox2D &prect, CBBox2D &wrect);

  void addAxes();

  void addProperty(const QString &path, QObject *object,
                   const QString &name, const QString &alias="");

  void addPlotObject(CQChartsPlotObj *obj);

  int numPlotObjects() const { return plotObjs_.size(); }

  void mousePressEvent(QMouseEvent *me) override;
  void mouseMoveEvent (QMouseEvent *me) override;

  void resizeEvent(QResizeEvent *) override;

  void updateGeometry();

  void drawAxes(QPainter *painter);

  QColor objectColor(CQChartsPlotObj *obj, int i, int n, const QColor &def) const;

  QColor paletteColor(int i, int n, const QColor &def) const;

  QSize sizeHint() const;

 protected:
  typedef std::vector<CQChartsPlotObj *>            PlotObjs;
  typedef CQChartsQuadTree<CQChartsPlotObj,CBBox2D> PlotObjTree;

 protected:
  void objsAtPoint(const CPoint2D &p, PlotObjTree::DataList &dataList1) const;

 protected:
  QWidget*              parent_       { nullptr };
  QAbstractItemModel*   model_        { nullptr };
  CDisplayRange2D       displayRange_;
  CRange2D              dataRange_;
  CGradientPalette*     palette_      { nullptr };
  CQChartsPlotExpander* expander_     { nullptr };
  CQPropertyTree*       propertyTree_ { nullptr };
  QColor                background_   { 255, 255, 255 };
  CQChartsAxis*         xAxis_        { nullptr };
  CQChartsAxis*         yAxis_        { nullptr };
  PlotObjs              plotObjs_;
  PlotObjTree           plotObjTree_;
};

//---

class CQChartsPlotExpander : public QFrame {
  Q_OBJECT

  Q_PROPERTY(bool expanded READ isExpanded WRITE setExpanded)

 public:
  CQChartsPlotExpander(CQChartsPlot *plot);

  bool isExpanded() const { return expanded_; }
  void setExpanded(bool b) { expanded_ = b; update(); }

  void mousePressEvent(QMouseEvent *);

  void paintEvent(QPaintEvent *);

 private:
  CQChartsPlot *plot_     { nullptr };
  bool          expanded_ { false };
};

#endif
