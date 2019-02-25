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
 public CQChartsObjBoxData<CQChartsBoxObj> {
  Q_OBJECT

  Q_PROPERTY(bool stateColoring READ isStateColoring WRITE setStateColoring)

  CQCHARTS_BOX_DATA_PROPERTIES

 public:
  CQChartsBoxObj(CQChartsView *view);
  CQChartsBoxObj(CQChartsPlot *plot);

  virtual ~CQChartsBoxObj() { }

  //---

  CQCharts *charts() const;

  CQChartsView *view() const;
  CQChartsPlot *plot() const { return plot_; }

  //---

  bool isStateColoring() const { return stateColoring_; }
  void setStateColoring(bool b) { stateColoring_ = b; }

  //---

  virtual void boxDataInvalidate(); // TODO: signal

  //---

  virtual void addProperties(CQPropertyViewModel *model, const QString &path);

  //---

  void draw(QPainter *painter, const QRectF &rect) const;
  void draw(QPainter *painter, const QPolygonF &poly) const;

 protected:
  CQChartsView* view_          { nullptr }; //! parent view
  CQChartsPlot* plot_          { nullptr }; //! parent plot
  bool          stateColoring_ { true };    //! color depending on inside/selected state
};

#endif
