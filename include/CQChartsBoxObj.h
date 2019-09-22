#ifndef CQChartsBoxObj_H
#define CQChartsBoxObj_H

#include <CQChartsViewPlotObj.h>
#include <CQChartsObjData.h>
#include <CQChartsSides.h>
#include <CQChartsPaintDevice.h>
#include <QRectF>
#include <QPolygonF>

class CQPropertyViewModel;
class CQChartsPaintDevice;

/*!
 * \brief box object
 * \ingroup Charts
 */
class CQChartsBoxObj : public CQChartsViewPlotObj,
 public CQChartsObjBoxData<CQChartsBoxObj> {
  Q_OBJECT

  Q_PROPERTY(bool stateColoring READ isStateColoring WRITE setStateColoring)

  CQCHARTS_BOX_DATA_PROPERTIES

 public:
  using ColorInd = CQChartsUtil::ColorInd;

 public:
  CQChartsBoxObj(CQChartsView *view);
  CQChartsBoxObj(CQChartsPlot *plot);

  virtual ~CQChartsBoxObj() { }

  //---

  // get/set state coloring
  bool isStateColoring() const { return stateColoring_; }
  void setStateColoring(bool b) { stateColoring_ = b; }

  //---

  virtual void boxDataInvalidate(); // TODO: signal

  //---

  virtual void addProperties(CQPropertyViewModel *model, const QString &path,
                             const QString &desc);

  //---

  void draw(CQChartsPaintDevice *device, const QRectF &rect) const;

  void draw(CQChartsPaintDevice *device, const QPolygonF &poly) const;

  void draw(CQChartsPaintDevice *device, const QRectF &rect,
            const QPen &pen, const QBrush &brush) const;

 protected:
  bool stateColoring_ { true }; //!< color depending on inside/selected state
};

#endif
