#ifndef CQChartsWheelPlot_H
#define CQChartsWheelPlot_H

#include <CQChartsPlot.h>
#include <CQChartsPlotType.h>
#include <CQChartsPlotObj.h>

//---

struct CQChartsWheelPointData {
  using Point   = CQChartsGeom::Point;
  using OptReal = CQChartsOptReal;

  Point    point;
  OptReal  min;
  OptReal  max;
  QVariant innerBar;
  OptReal  outerBar;
  OptReal  outerBubble;
};

struct CQChartsPolar {
  double a { 0.0 }; // angle in degrees
  double r { 0.0 }; // radius

  CQChartsPolar() = default;

  CQChartsPolar(double a, double r) :
   a(a), r(r) {
  }
};

struct CQChartsPolarPoint {
  using Polar = CQChartsPolar;
  using Point = CQChartsGeom::Point;

  Polar polar;
  Point point;
};

//---

/*!
 * \brief Wheel plot type
 * \ingroup Charts
 */
class CQChartsWheelPlotType : public CQChartsPlotType {
 public:
  using ColumnType = CQBaseModelType;

 public:
  CQChartsWheelPlotType();

  QString name() const override { return "wheel"; }
  QString desc() const override { return "Wheel"; }

  Dimension dimension() const override { return Dimension::TWO_D; }

  void addParameters() override;

  bool hasAxes() const override { return false; }

  bool hasKey() const override { return false; }

  bool allowXLog() const override { return false; }
  bool allowYLog() const override { return false; }

  bool canProbe() const override { return false; }

  bool isPrioritySort() const override { return true; }

  QString description() const override;

  //---

  void analyzeModel(ModelData *modelData, AnalyzeModelData &analyzeModelData) override;

  //---

  Plot *create(View *view, const ModelP &model) const override;
};

//---

class CQChartsWheelPlot;

//---

/*!
 * \brief Wheel Plot point object
 * \ingroup Charts
 */
class CQChartsPointObj : public CQChartsPlotObj {
  Q_OBJECT

  Q_PROPERTY(CQChartsGeom::Point point READ point)

 public:
  using WheelPlot = CQChartsWheelPlot;
  using PointData = CQChartsWheelPointData;

 public:
  CQChartsPointObj(const WheelPlot *plot, const BBox &rect, const PointData &pointData,
                   const QModelIndex &ind, const ColorInd &iv);

  QString typeName() const override { return "point"; }

  const Point &point() const { return pointData_.point; }

  QString calcId() const override;

  QString calcTipId() const override;

  //---

  const PointData &pointData() const { return pointData_; }

  //---

  void addProperties(CQPropertyViewModel *model, const QString &path) override;

  void getObjSelectIndices(Indices &inds) const override;

  //---

  void draw(PaintDevice *device) const override;

  //---

  void writeScriptData(ScriptPaintDevice *device) const override;

 private:
  const WheelPlot* plot_ { nullptr }; //!< parent plot
  PointData        pointData_;        //!< point data
};

//---

/*!
 * \brief Wheel Plot line object
 * \ingroup Charts
 */
class CQChartsLineObj : public CQChartsPlotObj {
  Q_OBJECT

  Q_PROPERTY(CQChartsGeom::Polygon poly READ poly)

 public:
  using WheelPlot = CQChartsWheelPlot;

 public:
  CQChartsLineObj(const WheelPlot *plot, const BBox &rect, const Polygon &poly,
                  const QModelIndex &ind, const ColorInd &iv);

  QString typeName() const override { return "line"; }

  const Polygon &poly() const { return poly_; }

  QString calcId() const override;

  QString calcTipId() const override;

  //---

  void addProperties(CQPropertyViewModel *model, const QString &path) override;

  void getObjSelectIndices(Indices &inds) const override;

  //---

  void draw(PaintDevice *device) const override;

  void calcPenBrush(PenBrush &penBrush, bool updateState) const;

  //---

  void writeScriptData(ScriptPaintDevice *device) const override;

 private:
  const WheelPlot* plot_ { nullptr }; //!< parent plot
  Polygon          poly_;             //!< polygon
};

//---

/*!
 * \brief Wheel Plot bubble object
 * \ingroup Charts
 */
class CQChartsInnerBarObj : public CQChartsPlotObj {
  Q_OBJECT

  Q_PROPERTY(CQChartsGeom::Point point READ point)

 public:
  using WheelPlot = CQChartsWheelPlot;
  using PointData = CQChartsWheelPointData;

 public:
  CQChartsInnerBarObj(const WheelPlot *plot, const BBox &rect, const PointData &pointData,
                      const QModelIndex &ind, const ColorInd &iv);

  QString typeName() const override { return "bubble"; }

  const Point &point() const { return pointData_.point; }

  QString calcId() const override;

  QString calcTipId() const override;

  //---

  void addProperties(CQPropertyViewModel *model, const QString &path) override;

  void getObjSelectIndices(Indices &inds) const override;

  //---

  void draw(PaintDevice *device) const override;

  void calcPenBrush(PenBrush &penBrush, bool updateState) const;

  //---

  void writeScriptData(ScriptPaintDevice *device) const override;

 private:
  const WheelPlot* plot_ { nullptr }; //!< parent plot
  PointData        pointData_;        //!< point data
};

//---

/*!
 * \brief Wheel Plot bubble object
 * \ingroup Charts
 */
class CQChartsOuterBarObj : public CQChartsPlotObj {
  Q_OBJECT

  Q_PROPERTY(CQChartsGeom::Point point READ point)

 public:
  using WheelPlot = CQChartsWheelPlot;
  using PointData = CQChartsWheelPointData;

 public:
  CQChartsOuterBarObj(const WheelPlot *plot, const BBox &rect, const PointData &pointData,
                      const QModelIndex &ind, const ColorInd &iv);

  QString typeName() const override { return "bubble"; }

  const Point &point() const { return pointData_.point; }

  QString calcId() const override;

  QString calcTipId() const override;

  //---

  void addProperties(CQPropertyViewModel *model, const QString &path) override;

  void getObjSelectIndices(Indices &inds) const override;

  //---

  void draw(PaintDevice *device) const override;

  void calcPenBrush(PenBrush &penBrush, bool updateState) const;

  //---

  void writeScriptData(ScriptPaintDevice *device) const override;

 private:
  const WheelPlot* plot_ { nullptr }; //!< parent plot
  PointData        pointData_;        //!< point data
};

//---

/*!
 * \brief Wheel Plot bubble object
 * \ingroup Charts
 */
class CQChartsOuterBubbleObj : public CQChartsPlotObj {
  Q_OBJECT

  Q_PROPERTY(CQChartsGeom::Point point READ point)

 public:
  using WheelPlot = CQChartsWheelPlot;
  using PointData = CQChartsWheelPointData;

 public:
  CQChartsOuterBubbleObj(const WheelPlot *plot, const BBox &rect, const PointData &pointData,
                         const QModelIndex &ind, const ColorInd &iv);

  QString typeName() const override { return "bubble"; }

  const Point &point() const { return pointData_.point; }

  QString calcId() const override;

  QString calcTipId() const override;

  //---

  void addProperties(CQPropertyViewModel *model, const QString &path) override;

  void getObjSelectIndices(Indices &inds) const override;

  //---

  void draw(PaintDevice *device) const override;

  void calcPenBrush(PenBrush &penBrush, bool updateState) const;

  //---

  void writeScriptData(ScriptPaintDevice *device) const override;

 private:
  const WheelPlot* plot_ { nullptr }; //!< parent plot
  PointData        pointData_;        //!< point data
};

//---

/*!
 * \brief Wheel Plot
 * \ingroup Charts
 */
class CQChartsWheelPlot : public CQChartsPlot {
  Q_OBJECT

  // columns
  Q_PROPERTY(CQChartsColumn xColumn   READ xColumn   WRITE setXColumn  )
  Q_PROPERTY(CQChartsColumn yColumn   READ yColumn   WRITE setYColumn  )
  Q_PROPERTY(CQChartsColumn minColumn READ minColumn WRITE setMinColumn)
  Q_PROPERTY(CQChartsColumn maxColumn READ maxColumn WRITE setMaxColumn)

  Q_PROPERTY(CQChartsColumn innerBarColumn    READ innerBarColumn    WRITE setInnerBarColumn)
  Q_PROPERTY(CQChartsColumn outerBarColumn    READ outerBarColumn    WRITE setOuterBarColumn)
  Q_PROPERTY(CQChartsColumn outerBubbleColumn READ outerBubbleColumn WRITE setOuterBubbleColumn)

  // options
  Q_PROPERTY(double innerRadius READ innerRadius WRITE setInnerRadius)
  Q_PROPERTY(double outerRadius READ outerRadius WRITE setOuterRadius)

  Q_PROPERTY(double              hotTemp     READ hotTemp     WRITE setHotTemp    )
  Q_PROPERTY(double              coldTemp    READ coldTemp    WRITE setColdTemp   )
  Q_PROPERTY(CQChartsPaletteName tempPalette READ tempPalette WRITE setTempPalette)

  Q_PROPERTY(CQChartsColor  lineColor READ lineColor WRITE setLineColor)
  Q_PROPERTY(CQChartsAlpha  lineAlpha READ lineAlpha WRITE setLineAlpha)
  Q_PROPERTY(CQChartsLength lineWidth READ lineWidth WRITE setLineWidth)

  Q_PROPERTY(CQChartsColor gridColor READ gridColor WRITE setGridColor)
  Q_PROPERTY(CQChartsAlpha gridAlpha READ gridAlpha WRITE setGridAlpha)
  Q_PROPERTY(CQChartsFont  gridFont  READ gridFont  WRITE setGridFont )

  Q_PROPERTY(CQChartsColor textColor READ textColor WRITE setTextColor)
  Q_PROPERTY(CQChartsFont  textFont  READ textFont  WRITE setTextFont )

  Q_PROPERTY(CQChartsColor innerBarColor    READ innerBarColor    WRITE setInnerBarColor)
  Q_PROPERTY(CQChartsColor outerBarColor    READ outerBarColor    WRITE setOuterBarColor)
  Q_PROPERTY(CQChartsColor outerBubbleColor READ outerBubbleColor WRITE setOuterBubbleColor)

 public:
  using Color       = CQChartsColor;
  using Alpha       = CQChartsAlpha;
  using ColorInd    = CQChartsUtil::ColorInd;
  using PenBrush    = CQChartsPenBrush;
  using PaletteName = CQChartsPaletteName;

  using PointData  = CQChartsWheelPointData;
  using PointDatas = std::vector<PointData>;
  using PolarPoint = CQChartsPolarPoint;
  using Polar      = CQChartsPolar;

 public:
  CQChartsWheelPlot(View *view, const ModelP &model);
 ~CQChartsWheelPlot();

  //---

  void init() override;
  void term() override;

  //---

  const Column &xColumn() const { return xColumn_; }
  void setXColumn(const Column &c);

  const Column &yColumn() const { return yColumn_; }
  void setYColumn(const Column &c);

  const Column &minColumn() const { return minColumn_; }
  void setMinColumn(const Column &c);

  const Column &maxColumn() const { return maxColumn_; }
  void setMaxColumn(const Column &c);

  const Column &innerBarColumn() const { return innerBarColumn_; }
  void setInnerBarColumn(const Column &c);

  const Column &outerBarColumn() const { return outerBarColumn_; }
  void setOuterBarColumn(const Column &c);

  const Column &outerBubbleColumn() const { return outerBubbleColumn_; }
  void setOuterBubbleColumn(const Column &c);

  //---

  double innerRadius() const { return innerRadius_; }
  void setInnerRadius(double r);

  double outerRadius() const { return outerRadius_; }
  void setOuterRadius(double r);

  //---

  double hotTemp() const { return hotTemp_; }
  void setHotTemp(double t);

  double coldTemp() const { return coldTemp_; }
  void setColdTemp(double t);

  const PaletteName &tempPalette() const { return tempPalette_; }
  void setTempPalette(const PaletteName &s);

  //---

  const Color &lineColor() const { return lineColor_; }
  void setLineColor(const Color &c);

  const Alpha &lineAlpha() const { return lineAlpha_; }
  void setLineAlpha(const Alpha &a);

  const Length &lineWidth() const { return lineWidth_; }
  void setLineWidth(const Length &l);

  //---

  const Color &gridColor() const { return gridColor_; }
  void setGridColor(const Color &c);

  const Alpha &gridAlpha() const { return gridAlpha_; }
  void setGridAlpha(const Alpha &a);

  const Font &gridFont() const { return gridFont_; }
  void setGridFont(const Font &f);

  //---

  const Color &textColor() const { return textColor_; }
  void setTextColor(const Color &c);

  const Font &textFont() const { return textFont_; }
  void setTextFont(const Font &f);

  //---

  const Color &innerBarColor() const { return innerBarColor_; }
  void setInnerBarColor(const Color &c);

  const Color &outerBarColor() const { return outerBarColor_; }
  void setOuterBarColor(const Color &c);

  const Color &outerBubbleColor() const { return outerBubbleColor_; }
  void setOuterBubbleColor(const Color &c);

  //---

  void addProperties() override;

  Range calcRange() const override;

  bool createObjs(PlotObjs &objs) const override;

  //---

  bool hasBackground() const override;
  void execDrawBackground(PaintDevice *device) const override;

  bool hasForeground() const override;
  void execDrawForeground(PaintDevice *device) const override;

  bool hasOverlay() const override;
  void execDrawOverlay(PaintDevice *device) const override;

  //---

  using PointObj       = CQChartsPointObj;
  using LineObj        = CQChartsLineObj;
  using InnerBarObj    = CQChartsInnerBarObj;
  using OuterBarObj    = CQChartsOuterBarObj;
  using OuterBubbleObj = CQChartsOuterBubbleObj;

  virtual PointObj *createPointObj(const BBox &rect, const PointData &pointData,
                                   const QModelIndex &ind, const ColorInd &iv);

  virtual LineObj *createLineObj(const BBox &rect, const Polygon &poly, const QModelIndex &ind,
                                 const ColorInd &iv);

  virtual InnerBarObj *createInnerBarObj(const BBox &rect, const PointData &pointData,
                                         const QModelIndex &ind, const ColorInd &iv);

  virtual OuterBarObj *createOuterBarObj(const BBox &rect, const PointData &pointData,
                                         const QModelIndex &ind, const ColorInd &iv);

  virtual OuterBubbleObj *createOuterBubbleObj(const BBox &rect, const PointData &pointData,
                                               const QModelIndex &ind, const ColorInd &iv);

  //---

  bool selectMove(const Point &p, Constraints constraints, bool first) override;

  //---

  PolarPoint pointToPolarPoint(const Point &point) const;
  Point      polarToPoint(const Polar &polar) const;

  Point pointFromPolar(const Point &point) const;
  Polar polarFromPoint(const Point &point) const;

  //---

  double outerBarRadius   (double value) const;
  double outerBubbleRadius(double value) const;

  double ymax() const { return ymax_; }

  double dx() const { return dx_; }

 private:
  Column xColumn_;   //!< x column
  Column yColumn_;   //!< y column
  Column minColumn_; //!< min column
  Column maxColumn_; //!< max column

  Column innerBarColumn_;   //!< inner fill column
  Column outerBarColumn_;    //!< outer bar column
  Column outerBubbleColumn_; //!< outer bubble column

  //---

  double innerRadius_ { 0.4 }; //!< inner radius
  double outerRadius_ { 1.0 }; //!< outer radius

  double      hotTemp_     { 80 };
  double      coldTemp_    { 32 };
  PaletteName tempPalette_ { "moreland" };

  Color  lineColor_ { QColor(0, 0, 0) };
  Alpha  lineAlpha_ { 0.5 };
  Length lineWidth_ { 4, Units::PIXEL };

  Color gridColor_ { QColor(0, 0, 0) };
  Alpha gridAlpha_ { 0.5 };
  Font  gridFont_;

  Color textColor_ { QColor(0, 0, 0) };
  Font  textFont_;

  Color innerBarColor_    { QColor(200, 200, 200) };
  Color outerBarColor_    { QColor(171, 171, 171) };
  Color outerBubbleColor_ { QColor( 74, 152, 200) };

  //---

  mutable double xmin_ { 0.0 };
  mutable double xmax_ { 1.0 };
  mutable double ymin_ { 0.0 };
  mutable double ymax_ { 1.0 };

  mutable double outerBarMin_ { 0.0 };
  mutable double outerBarMax_ { 1.0 };

  mutable double outerBubbleMin_ { 0.0 };
  mutable double outerBubbleMax_ { 1.0 };

  mutable double da_ { 0.0 };
  mutable double dx_ { 0.0 };

  mutable PolarPoint movePoint_;
  mutable PointObj*  moveObj_ { nullptr };
  mutable QString    moveTip_;
};

#endif
