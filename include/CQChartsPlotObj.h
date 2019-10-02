#ifndef CQChartsPlotObj_H
#define CQChartsPlotObj_H

#include <CQChartsObj.h>
#include <CQChartsGeom.h>
#include <set>

class CQChartsPlot;
class CQChartsPenBrush;
class CQChartsLength;
class CQChartsPaintDevice;
class CQPropertyViewModel;

/*!
 * \brief Plot Object base class
 * \ingroup Charts
 *
 * Maintains three indices (set, group and value) and x, y values for color interpolation
 */
class CQChartsPlotObj : public CQChartsObj {
  Q_OBJECT

  Q_PROPERTY(QString typeName READ typeName)
  Q_PROPERTY(bool    visible  READ isVisible WRITE setVisible)

 public:
  enum class DetailHint {
    MAJOR,
    MINOR
  };

  using ModelIndices = std::vector<QModelIndex>;
  using Indices      = std::set<QModelIndex>;
  using ColorInd     = CQChartsUtil::ColorInd;

 public:
  CQChartsPlotObj(CQChartsPlot *plot, const CQChartsGeom::BBox &rect=CQChartsGeom::BBox(),
                  const ColorInd &is=ColorInd(), const ColorInd &ig=ColorInd(),
                  const ColorInd &iv=ColorInd());

  virtual ~CQChartsPlotObj() { }

  //---

  //! get parent plot
  CQChartsPlot *plot() const { return plot_; }

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

  //! get/set visible
  bool isVisible() const { return visible_; }
  void setVisible(bool b) { visible_ = b; }

  //---

  virtual bool visible() const { return isVisible(); }

  //---

  virtual bool isPolygon() const { return false; }
  virtual QPolygonF polygon() const { return QPolygonF(); }

  //---

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

  QModelIndex modelInd() const {
    return (! modelInds_.empty() ? modelInds_[0] : QModelIndex()); }
  void setModelInd(const QModelIndex &ind) {
    modelInds_.clear(); addModelInd(ind); }

  const ModelIndices &modelInds() const { return modelInds_; }
  void setModelInds(const ModelIndices &inds) { modelInds_ = inds; }

  void addModelInd(const QModelIndex &ind) { modelInds_.push_back(ind); }

  //---

  // is point inside (override if not simple rect shape)
  virtual bool inside(const CQChartsGeom::Point &p) const {
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
  virtual bool rectIntersect(const CQChartsGeom::BBox &r, bool inside) const {
    if (! isVisible()) return false;

    if (inside)
      return r.inside(rect());
    else
      return r.overlaps(rect());
  }

  //virtual void postResize() { }

  virtual void selectPress() { }

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

  virtual void getSelectIndices(Indices &inds) const = 0;

  virtual void addColumnSelectIndex(Indices &inds, const CQChartsColumn &column) const;

  void addSelectIndex(Indices &inds, const CQChartsModelIndex &ind) const;
  void addSelectIndex(Indices &inds, int row, const CQChartsColumn &column,
                      const QModelIndex &parent=QModelIndex()) const;
  void addSelectIndex(Indices &inds, const QModelIndex &ind) const;

  //---

  // draw
  virtual void drawBg(CQChartsPaintDevice *) const;
  virtual void drawFg(CQChartsPaintDevice *) const;

  virtual void draw(CQChartsPaintDevice *);

  void drawRoundedPolygon(CQChartsPaintDevice *device, const CQChartsPenBrush &penBrush,
                          const CQChartsGeom::BBox &rect, const CQChartsLength &cornerSize) const;

  void drawDebugRect(CQChartsPaintDevice *device);

  //---

  virtual void writeScriptData(std::ostream &os) const;

 protected:
  CQChartsPlot* plot_       { nullptr };           //!< parent plot
  DetailHint    detailHint_ { DetailHint::MINOR }; //!< interaction detail hint
  bool          visible_    { true };              //!< is visible
  ColorInd      is_;                               //!< set index
  ColorInd      ig_;                               //!< group index
  ColorInd      iv_;                               //!< value index
  ModelIndices  modelInds_;                        //!< associated model indices
};

//------

/*!
 * \brief Group Plot object
 * \ingroup Charts
 */
class CQChartsGroupObj : public CQChartsPlotObj {
  Q_OBJECT

 public:
  CQChartsGroupObj(CQChartsPlot *plot, const CQChartsGeom::BBox &bbox=CQChartsGeom::BBox(),
                   const ColorInd &ig=ColorInd());
};

#endif
