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
