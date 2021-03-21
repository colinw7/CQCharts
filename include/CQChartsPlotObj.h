#ifndef CQChartsPlotObj_H
#define CQChartsPlotObj_H

#include <CQChartsObj.h>
#include <CQChartsGeom.h>
#include <CQChartsArcData.h>
#include <CQChartsDrawUtil.h>
#include <CQChartsModelIndex.h>
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
 * All interactive objects in the plot need to be derived from this base class.
 *
 * The object interface supports:
 *  + drawing including mouse over
 *  + selection
 *  + edit
 *  + properties
 *
 * Maintains three indices (set, group and value) and x, y values for color interpolation
 */
class CQChartsPlotObj : public CQChartsObj,
 public CQChartsSelectableIFace, public CQChartsEditableIFace {
  Q_OBJECT

  Q_PROPERTY(QString    typeName   READ typeName  )
  Q_PROPERTY(DetailHint detailHint READ detailHint)
  Q_PROPERTY(DrawLayer  drawLayer  READ drawLayer )
  Q_PROPERTY(bool       zoomText   READ isZoomText WRITE setZoomText)

  Q_ENUMS(DetailHint)
  Q_ENUMS(DrawLayer)

 public:
  enum class DetailHint {
    MAJOR,
    MINOR
  };

  enum class DrawLayer {
    NONE,
    BACKGROUND,
    MIDDLE,
    FOREGROUND
  };

  using Plot          = CQChartsPlot;
  using PlotObj       = CQChartsPlotObj;
  using PlotObjs      = std::vector<PlotObj *>;
  using QModelIndices = std::vector<QModelIndex>;
  using Indices       = std::set<QModelIndex>;
  using ColorInd      = CQChartsUtil::ColorInd;
  using PenBrush      = CQChartsPenBrush;
  using ResizeSide    = CQChartsResizeSide;
  using ModelIndex    = CQChartsModelIndex;
  using Color         = CQChartsColor;
  using Alpha         = CQChartsAlpha;
  using EditHandles   = CQChartsEditHandles;

  using PaintDevice       = CQChartsPaintDevice;
  using ScriptPaintDevice = CQChartsScriptPaintDevice;
  using BrushData         = CQChartsBrushData;
  using PenData           = CQChartsPenData;

  using SelMod = CQChartsSelMod;

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

  //! get/set detail hint
  const DetailHint &detailHint() const { return detailHint_; }
  void setDetailHint(const DetailHint &h) { detailHint_ = h; }

  //---

  //! get/set draw layer
  const DrawLayer &drawLayer() const { return drawLayer_; }
  void setDrawLayer(const DrawLayer &l);

  //---

  //! get/set zoom text
  bool isZoomText() const { return zoomText_; }
  void setZoomText(bool b) { zoomText_ = b; }

  //---

  // shapes

  // get is polygon and polygon shape
  virtual bool isPolygon() const { return false; }
  virtual Polygon polygon() const { return Polygon(); }

  // get is circle and circle radius
  virtual bool isCircle() const { return false; }
  virtual double radius() const { return 1.0; }

  // get is arc and arc shape
  virtual bool isArc() const { return false; }
  virtual CQChartsArcData arcData() const { return CQChartsArcData(); }

  //! get is solid (not a point/line)
  virtual bool isSolid() const { return true; }

  //! get is point
  virtual bool isPoint() const { return false; }

  //! get is text
  virtual bool isText() const { return false; }

  //! get is image
  virtual bool isImage() const { return false; }

  //---

  virtual CQChartsObjDrawType drawType() const {
    if (  isPoint()) return CQChartsObjDrawType::SYMBOL;
    if (! isSolid()) return CQChartsObjDrawType::LINE;
    if (  isText ()) return CQChartsObjDrawType::TEXT;
    if (  isImage()) return CQChartsObjDrawType::IMAGE;

    return CQChartsObjDrawType::BOX;
  }

  //---

  // get color type
  virtual CQChartsColorType colorType() const { return CQChartsColorType::AUTO; }

  //! get/set set index
  const ColorInd &is() const { return is_; }
  void setIs(const ColorInd &is) { is_ = is; }

  //! get/set group index
  const ColorInd &ig() const { return ig_; }
  void setIg(const ColorInd &ig) { ig_ = ig; }

  //! get/set value index
  const ColorInd &iv() const { return iv_; }
  void setIv(const ColorInd &iv) { iv_ = iv; }

  //! calc color index (from set, group or value index)
  virtual ColorInd calcColorInd() const;

  //! get x/y color value
  virtual double xColorValue(bool relative=true) const;
  virtual double yColorValue(bool relative=true) const;

  //---

  // get/set model index
  QModelIndex modelInd() const;
  void setModelInd(const QModelIndex &ind);

  // get/set model indices
  const QModelIndices &modelInds() const { return modelInds_; }
  void setModelInds(const QModelIndices &inds);

  // add model index
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

  virtual void flip(Qt::Orientation) { }

  //---

  //! get edit handles
  EditHandles *editHandles() const override;

  void drawEditHandles(QPainter *painter) const override;

  void setEditHandlesBBox() const;

  //---

  bool contains(const Point &p) const override;

  //---

  //! get property path
  virtual QString propertyId() const;

  //! add properties
  virtual void addProperties(CQPropertyViewModel *model, const QString &path);

  //---

  // select

 public:
  // are any of the normalized indices in object's select indices
  bool isSelectIndices(const Indices &inds) const;

//bool isSelectIndex(const QModelIndex &ind) const;

  // get object select indices (normalized)
  void getNormalizedSelectIndices(Indices &inds) const;

  // add object select indices (normalized) to plot selection buffer to use
  // in cross select from objects to selection model
  void addSelectIndices(Plot *plot);

  // add plot object's indices to array for specified (new) column
  virtual void addColumnSelectIndex(Indices &inds, const CQChartsColumn &column) const;

  // add model index to array
  void addSelectIndex(Indices &inds, const ModelIndex &ind) const;

  // add model index data to array (ignore if column not right type)
  void addSelectIndex(Indices &inds, int row, const CQChartsColumn &column,
                      const QModelIndex &parent=QModelIndex()) const;

 protected:
  // get object select indices (non-normalized) from object's columns
  void getSelectIndices(Indices &inds) const;

  // get select indices for object
  virtual void getObjSelectIndices(Indices &) const { }

 public:
  // draw background, middle, foreground
  virtual void drawBg(PaintDevice *) const;
  virtual void draw  (PaintDevice *) const;
  virtual void drawFg(PaintDevice *) const;

  virtual void postDraw(PaintDevice *) { }

  void drawDebugRect(PaintDevice *device);

  virtual bool drawMouseOver() const { return true; }

  //---

  virtual void writeScriptData(ScriptPaintDevice *device) const;

  void writeObjScriptData(ScriptPaintDevice *device) const;

  virtual void writeScriptGC(ScriptPaintDevice *device, const PenBrush &penBrush) const;

  virtual void writeScriptInsideColor(ScriptPaintDevice *device, bool isSave) const;

 signals:
  void layerChanged();

 protected:
  using EditHandlesP = std::unique_ptr<EditHandles>;

  Plot*            plot_        { nullptr };           //!< parent plot
  DetailHint       detailHint_  { DetailHint::MINOR }; //!< interaction detail hint
  DrawLayer        drawLayer_   { DrawLayer::NONE };   //!< draw layer
  bool             zoomText_    { false };             //!< zoom object text
  ColorInd         is_;                                //!< set index
  ColorInd         ig_;                                //!< group index
  ColorInd         iv_;                                //!< value index
  QModelIndices    modelInds_;                         //!< associated model indices
  EditHandlesP     editHandles_;                       //!< edit handles
  mutable PenBrush penBrush_;                          //!< current pen/brush
};

//------

/*!
 * \brief Group Plot object
 * \ingroup Charts
 */
class CQChartsGroupObj : public CQChartsPlotObj {
  Q_OBJECT

 public:
  CQChartsGroupObj(Plot *plot, const BBox &bbox=BBox(), const ColorInd &ig=ColorInd());
};

#endif
