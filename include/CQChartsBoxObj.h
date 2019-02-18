#ifndef CQChartsBoxObj_H
#define CQChartsBoxObj_H

#include <CQChartsObj.h>
#include <CQChartsObjData.h>
#include <CQChartsSides.h>
#include <QRectF>
#include <QPolygonF>

class CQChartsView;
class CQCharts;
class CQPropertyViewModel;
class QPainter;

class CQChartsBoxObj : public CQChartsObj,
 public CQChartsObjShapeData<CQChartsBoxObj> {
  Q_OBJECT

  Q_PROPERTY(bool   visible       READ isVisible       WRITE setVisible      )
  Q_PROPERTY(double margin        READ margin          WRITE setMargin       )
  Q_PROPERTY(double padding       READ padding         WRITE setPadding      )
  Q_PROPERTY(bool   stateColoring READ isStateColoring WRITE setStateColoring)

  CQCHARTS_SHAPE_DATA_PROPERTIES

  Q_PROPERTY(CQChartsSides borderSides READ borderSides WRITE setBorderSides)

 public:
  CQChartsBoxObj(CQChartsView *view);
  CQChartsBoxObj(CQChartsPlot *plot);

  virtual ~CQChartsBoxObj() { }

  //---

  CQCharts *charts() const;

  CQChartsView *view() const;
  CQChartsPlot *plot() const { return plot_; }

  //---

  // get/set visible
  bool isVisible() const { return boxData_.isVisible(); }
  void setVisible(bool b) { boxData_.setVisible(b); redrawBoxObj(); }

  //---

  // inside margin
  double margin() const { return boxData_.margin(); }
  void setMargin(double r) { boxData_.setMargin(r); redrawBoxObj(); }

  // outside padding
  double padding() const { return boxData_.padding(); }
  void setPadding(double r) { boxData_.setPadding(r); redrawBoxObj(); }

  //---

  bool isStateColoring() const { return stateColoring_; }
  void setStateColoring(bool b) { stateColoring_ = b; }

  //---

  // border
  const CQChartsSides &borderSides() const { return boxData_.borderSides(); }
  void setBorderSides(const CQChartsSides &s) { boxData_.setBorderSides(s); redrawBoxObj(); }

  //---

  CQChartsBoxData boxData() const {
    CQChartsBoxData data = boxData_;

    data.setShape(shapeData());

    return data;
  }

  void setBoxData(const CQChartsBoxData &data) {
    boxData_ = data;

    setShapeData(data.shape());
  }

  //---

  virtual void redrawBoxObj(); // TODO: signal

  //---

  virtual void addProperties(CQPropertyViewModel *model, const QString &path);

  //---

  void draw(QPainter *painter, const QRectF &rect) const;
  void draw(QPainter *painter, const QPolygonF &poly) const;

 protected:
  CQChartsView*   view_          { nullptr }; //! parent view
  CQChartsPlot*   plot_          { nullptr }; //! parent plot
  CQChartsBoxData boxData_;                   //! box data
  bool            stateColoring_ { true };    //! color depending on inside/selected state
};

#endif
