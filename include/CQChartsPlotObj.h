#ifndef CQChartsPlotObj_H
#define CQChartsPlotObj_H

#include <CQChartsObj.h>
#include <CQChartsGeom.h>
#include <CQChartsArcData.h>
#include <CQChartsDrawUtil.h>
#include <set>

class  CQChartsPlot;
struct CQChartsPenBrush;
class  CQChartsLength;
class  CQChartsPaintDevice;
class  CQChartsScriptPaintDevice;
class  CQPropertyViewModel;
class  CQChartsEditHandles;

/*!
 * \brief Plot Object base class
 * \ingroup Charts
 *
 * Maintains three indices (set, group and value) and x, y values for color interpolation
 */
class CQChartsPlotObj : public CQChartsObj {
  Q_OBJECT

  Q_PROPERTY(QString typeName READ typeName)

 public:
  enum class DetailHint {
    MAJOR,
    MINOR
  };

  using Plot         = CQChartsPlot;
  using PlotObj      = CQChartsPlotObj;
  using PlotObjs     = std::vector<PlotObj *>;
  using ModelIndices = std::vector<QModelIndex>;
  using Indices      = std::set<QModelIndex>;
  using ColorInd     = CQChartsUtil::ColorInd;
  using PenBrush     = CQChartsPenBrush;
  using ResizeSide   = CQChartsResizeSide;

  using PaintDevice       = CQChartsPaintDevice;
  using ScriptPaintDevice = CQChartsScriptPaintDevice;

  using Point    = CQChartsGeom::Point;
  using Size     = CQChartsGeom::Size;
  using BBox     = CQChartsGeom::BBox;
  using Polygon  = CQChartsGeom::Polygon;
  using Polygons = CQChartsGeom::Polygons;

 public:
  CQChartsPlotObj(Plot *plot, const BBox &rect=BBox(),
                  const ColorInd &is=ColorInd(), const ColorInd &ig=ColorInd(),
                  const ColorInd &iv=ColorInd());

  virtual ~CQChartsPlotObj();

  //---

  //! get parent plot
  Plot *plot() const { return plot_; }

  //---

  //! get type name (for id)
  virtual QString typeName() const = 0;

  //---

  //! get id from idColumn for index (if defined)
  bool calcColumnId(const QModelIndex &ind, QString &str) const;

  //---

  const DetailHint &detailHint() const { return detailHint_; }
  void setDetailHint(const DetailHint &h) { detailHint_ = h; }

  //---

  // shapes
  virtual bool isPolygon() const { return false; }
  virtual Polygon polygon() const { return Polygon(); }

  virtual bool isCircle() const { return false; }
  virtual double radius() const { return 1.0; }

  virtual bool isArc() const { return false; }
  virtual CQChartsArcData arcData() const { return CQChartsArcData(); }

  virtual bool isSolid() const { return true; }

  //---

  // colors
  virtual CQChartsColorType colorType() const { return CQChartsColorType::AUTO; }

  const ColorInd &is() const { return is_; }
  void setIs(const ColorInd &is) { is_ = is; }

  const ColorInd &ig() const { return ig_; }
  void setIg(const ColorInd &ig) { ig_ = ig; }

  const ColorInd &iv() const { return iv_; }
  void setIv(const ColorInd &iv) { iv_ = iv; }

  virtual ColorInd calcColorInd() const;

  virtual double xColorValue(bool relative=true) const;
  virtual double yColorValue(bool relative=true) const;

  //---

  // model data
  QModelIndex modelInd() const;
  void setModelInd(const QModelIndex &ind);

  const ModelIndices &modelInds() const { return modelInds_; }
  void setModelInds(const ModelIndices &inds);

  void addModelInd(const QModelIndex &ind);

  //---

  // connected
  virtual PlotObjs getConnected() const { return PlotObjs(); }

  //---

  // is point inside (override if not simple rect shape)
  virtual bool inside(const Point &p) const {
    if (! isVisible()) return false;
    return rect().inside(p);
  }

  // is x inside (override if not simple rect shape)
  virtual bool insideX(double x) const {
    if (! isVisible()) return false;
    return rect().insideX(x);
  }

  // is y inside (override if not simple rect shape)
  virtual bool insideY(double y) const {
    if (! isVisible()) return false;
    return rect().insideY(y);
  }

  // is rect inside/touching (override if not simple rect shape)
  virtual bool rectIntersect(const BBox &r, bool inside) const {
    if (! isVisible()) return false;

    if (inside)
      return r.inside(rect());
    else
      return r.overlaps(rect());
  }

  //---

  //virtual void postResize() { }

  //---

  // Select Interface

  //! handle select press, move, release
  virtual bool selectPress  (const Point &, CQChartsSelMod) { return false; }
  virtual bool selectMove   (const Point &) { return false; }
  virtual bool selectRelease(const Point &) { return false; }

  //---

  // Edit Interface

  //! handle edit press, move, motion, release
  virtual bool editPress  (const Point &) { return false; }
  virtual bool editMove   (const Point &) { return false; }
  virtual bool editMotion (const Point &) { return false; }
  virtual bool editRelease(const Point &) { return true; }

  //! handle edit move by
  virtual void editMoveBy(const Point &) { }

  //! set new bounding box
  virtual void setEditBBox(const BBox &, const ResizeSide &) { }

  virtual bool isEditResize() const { return false; }

  //---

  virtual void flip(Qt::Orientation) { }

  //---

  //! get edit handles
  CQChartsEditHandles *editHandles() const;

  virtual void drawEditHandles(QPainter *painter) const;

  //---

  bool contains(const Point &p) const override;

  //---

  //! get property path
  virtual QString propertyId() const;

  //! add properties
  virtual void addProperties(CQPropertyViewModel *model, const QString &path);

  //---

  // select
  bool isSelectIndex(const QModelIndex &ind) const;

  void addSelectIndices();

  void getHierSelectIndices(Indices &inds) const;

  void getSelectIndices(Indices &inds) const;

  void getDefSelectIndices(Indices &inds) const;

  virtual void getObjSelectIndices(Indices &) const { }

  virtual void addColumnSelectIndex(Indices &inds, const CQChartsColumn &column) const;

  void addSelectIndex(Indices &inds, const CQChartsModelIndex &ind) const;
  void addSelectIndex(Indices &inds, int row, const CQChartsColumn &column,
                      const QModelIndex &parent=QModelIndex()) const;
  void addSelectIndex(Indices &inds, const QModelIndex &ind) const;

  //---

  // draw
  virtual void drawBg(PaintDevice *) const;
  virtual void drawFg(PaintDevice *) const;

  virtual void draw(PaintDevice *);

  virtual void postDraw(PaintDevice *) { }

  void drawRoundedPolygon(PaintDevice *device, const PenBrush &penBrush,
                          const BBox &rect, const CQChartsLength &cornerSize) const;

  void drawDebugRect(PaintDevice *device);

  //---

  virtual void writeScriptData(ScriptPaintDevice *device) const;

  void writeObjScriptData(ScriptPaintDevice *device) const;

  virtual void writeScriptGC(ScriptPaintDevice *device, const PenBrush &penBrush) const;

  virtual void writeScriptInsideColor(ScriptPaintDevice *device, bool isSave) const;

 protected:
  Plot*                plot_        { nullptr };           //!< parent plot
  DetailHint           detailHint_  { DetailHint::MINOR }; //!< interaction detail hint
  ColorInd             is_;                                //!< set index
  ColorInd             ig_;                                //!< group index
  ColorInd             iv_;                                //!< value index
  ModelIndices         modelInds_;                         //!< associated model indices
  CQChartsEditHandles* editHandles_ { nullptr };           //!< edit handles
  mutable PenBrush     penBrush_;                          //!< current pen/brush
};

//------

/*!
 * \brief Group Plot object
 * \ingroup Charts
 */
class CQChartsGroupObj : public CQChartsPlotObj {
  Q_OBJECT

 public:
  CQChartsGroupObj(CQChartsPlot *plot, const BBox &bbox=BBox(), const ColorInd &ig=ColorInd());
};

#endif
