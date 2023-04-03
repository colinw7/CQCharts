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
  enum class ShapeType {
    NONE          = static_cast<int>(CQChartsShapeType::NONE),
    TRIANGLE      = static_cast<int>(CQChartsShapeType::TRIANGLE),
    DIAMOND       = static_cast<int>(CQChartsShapeType::DIAMOND),
    BOX           = static_cast<int>(CQChartsShapeType::BOX),
    POLYGON       = static_cast<int>(CQChartsShapeType::POLYGON),
    CIRCLE        = static_cast<int>(CQChartsShapeType::CIRCLE),
    DOUBLE_CIRCLE = static_cast<int>(CQChartsShapeType::DOUBLE_CIRCLE),
    DOT_LINE      = static_cast<int>(CQChartsShapeType::DOT_LINE),
    RECORD        = static_cast<int>(CQChartsShapeType::RECORD),
    PLAIN_TEXT    = static_cast<int>(CQChartsShapeType::PLAIN_TEXT),
    RARROW        = static_cast<int>(CQChartsShapeType::RARROW),
    OVAL          = static_cast<int>(CQChartsShapeType::OVAL),
    RPROMOTER     = static_cast<int>(CQChartsShapeType::RPROMOTER)
  };

  using PropertyModel = CQPropertyViewModel;
  using Alpha         = CQChartsAlpha;
  using FillPattern   = CQChartsFillPattern;
  using Length        = CQChartsLength;
  using ColorInd      = CQChartsUtil::ColorInd;

 public:
  CQChartsBoxObj(View *view);
  CQChartsBoxObj(Plot *plot);

  virtual ~CQChartsBoxObj() = default;

  //---

  //! get/set state coloring
  bool isStateColoring() const { return stateColoring_; }
  void setStateColoring(bool b) { stateColoring_ = b; }

  //---

  //! get/set bbox
  const BBox &bbox() const { return bbox_; }
  void setBBox(const BBox &b);

  //---

  CQChartsGeom::Margin parentMargin(const Margin &m) const;

  virtual bool hasCustomMargin() const { return false; }
  virtual bool hasCustomPadding() const { return false; }

  //---

  void dataInvalidate(int) override { boxObjInvalidate(); }

  virtual void boxObjInvalidate(); // TODO: signal

  //---

  virtual void addProperties(PropertyModel *model, const QString &path, const QString &desc);

  void addBoxProperties(PropertyModel *model, const QString &path, const QString &desc);

  virtual void updateProperties(PropertyModel *, const QString &) { }

  //---

  void draw(PaintDevice *device, const BBox &bbox) const;
  void draw(PaintDevice *device, const Polygon &poly) const;

  void draw(PaintDevice *device, const BBox &bbox, const PenBrush &penBrush) const;
  void draw(PaintDevice *device, const Polygon &poly, const PenBrush &penBrush) const;

  static void drawBox(PaintDevice *device, const BBox &bbox, const PenBrush &penBrush,
                      const Length &cornerSize, const Sides &borderSides);
  static void drawPolygon(PaintDevice *device, const Polygon &poly, const PenBrush &penBrush,
                          const Length &cornerSize);

  void calcPenBrush(PenBrush &penBrush) const;

  //---

  bool contains(const Point &p) const override;

 protected:
  bool         stateColoring_ { true }; //!< color depending on inside/selected state
  mutable BBox bbox_;                   //!< last drawn bbox
};

#endif
