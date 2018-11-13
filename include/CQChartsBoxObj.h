#ifndef CQChartsBoxObj_H
#define CQChartsBoxObj_H

#include <CQChartsObj.h>
#include <CQChartsObjData.h>
#include <CQChartsSides.h>
#include <QRectF>
#include <QPolygonF>

class CQChartsView;
class CQPropertyViewModel;
class QPainter;

class CQChartsBoxObj : public CQChartsObj,
 public CQChartsObjShapeData<CQChartsBoxObj> {
  Q_OBJECT

  Q_PROPERTY(bool   visible READ isVisible WRITE setVisible)
  Q_PROPERTY(double margin  READ margin    WRITE setMargin )
  Q_PROPERTY(double padding READ padding   WRITE setPadding)

  CQCHARTS_SHAPE_DATA_PROPERTIES

  Q_PROPERTY(CQChartsSides borderSides READ borderSides WRITE setBorderSides)

 public:
  CQChartsBoxObj(CQChartsView *view);
  CQChartsBoxObj(CQChartsPlot *plot);

  virtual ~CQChartsBoxObj() { }

  //---

  CQChartsView *view() const;
  CQChartsPlot *plot() const { return plot_; }

  //---

  // get/set visible
  bool isVisible() const { return boxData_.visible; }
  void setVisible(bool b) { boxData_.visible = b; redrawBoxObj(); }

  //---

  // inside margin
  double margin() const { return boxData_.margin; }
  void setMargin(double r) { boxData_.margin = r; redrawBoxObj(); }

  // outside padding
  double padding() const { return boxData_.padding; }
  void setPadding(double r) { boxData_.padding = r; redrawBoxObj(); }

  //---

  // border
  const CQChartsSides &borderSides() const { return boxData_.borderSides; }
  void setBorderSides(const CQChartsSides &s) { boxData_.borderSides = s; redrawBoxObj(); }

  //---

  CQChartsBoxData boxData() const {
    CQChartsBoxData data = boxData_;

    data.shape = shapeData();

    return data;
  }

  void setBoxData(const CQChartsBoxData &data) {
    boxData_ = data;

    setShapeData(data.shape);
  }

  //---

  virtual void redrawBoxObj(); // TODO: signal

  //---

  virtual void addProperties(CQPropertyViewModel *model, const QString &path);

  //---

  void draw(QPainter *painter, const QRectF &rect) const;
  void draw(QPainter *painter, const QPolygonF &poly) const;

 protected:
  CQChartsView*   view_     { nullptr }; // parent view
  CQChartsPlot*   plot_     { nullptr }; // parent plot
  CQChartsBoxData boxData_;              // box data
};

#endif
