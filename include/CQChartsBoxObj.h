#ifndef CQChartsBoxObj_H
#define CQChartsBoxObj_H

#include <CQChartsViewPlotObj.h>
#include <CQChartsObjData.h>
#include <CQChartsSides.h>
#include <CQChartsPaintDevice.h>

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

  enum class ShapeType {
    NONE,
    TRIANGLE,
    DIAMOND,
    BOX,
    POLYGON,
    CIRCLE,
    DOUBLE_CIRCLE,
    DOT_LINE
  };

  using PropertyModel = CQPropertyViewModel;
  using Alpha         = CQChartsAlpha;
  using FillPattern   = CQChartsFillPattern;

 public:
  CQChartsBoxObj(View *view);
  CQChartsBoxObj(Plot *plot);

  virtual ~CQChartsBoxObj() { }

  //---

  // get/set state coloring
  bool isStateColoring() const { return stateColoring_; }
  void setStateColoring(bool b) { stateColoring_ = b; }

  //---

  const BBox &bbox() const { return bbox_; }
  void setBBox(const BBox &b);

  //---

  void dataInvalidate() override { boxDataInvalidate(); }

  virtual void boxDataInvalidate(); // TODO: signal

  //---

  virtual void addProperties(PropertyModel *model, const QString &path, const QString &desc);

  void addBoxProperties(PropertyModel *model, const QString &path, const QString &desc);

  //---

  void draw(PaintDevice *device, const BBox &bbox) const;
  void draw(PaintDevice *device, const Polygon &poly) const;

  void draw(PaintDevice *device, const BBox &bbox, const PenBrush &penBrush) const;

  //---

  bool contains(const Point &p) const override;

 protected:
  bool         stateColoring_ { true }; //!< color depending on inside/selected state
  mutable BBox bbox_;                   //!< last drawn bbox
};

#endif
