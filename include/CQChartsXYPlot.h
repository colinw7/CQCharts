#ifndef CQChartsXYPlot_H
#define CQChartsXYPlot_H

#include <CQChartsPointPlot.h>
#include <CQChartsPlotObj.h>
#include <CQChartsFillUnder.h>
#include <CQChartsFitData.h>
#include <CQStatData.h>
#include <CQChartsUtil.h>

class CQChartsXYPlot;
class CQChartsXYLabelObj;
class CQChartsXYPolylineObj;
class CQChartsArrow;
class CQChartsGrahamHull;

//---

/*!
 * \brief XY plot type
 * \ingroup Charts
 */
class CQChartsXYPlotType : public CQChartsPointPlotType {
 public:
  using ColumnType = CQBaseModelType;

 public:
  CQChartsXYPlotType();

  QString name() const override { return "xy"; }
  QString desc() const override { return "XY"; }

  void addParameters() override;

  bool canProbe() const override { return true; }

  bool supportsIdColumn   () const override { return true; }
  bool supportsImageColumn() const override { return true; }

  QString description() const override;

  //---

  void analyzeModel(ModelData *modelData, AnalyzeModelData &analyzeModelData) override;

  //---

  Plot *create(View *view, const ModelP &model) const override;
};

//---

/*!
 * \brief XY Plot Bivariate Line object
 * \ingroup Charts
 */
class CQChartsXYBiLineObj : public CQChartsPlotObj {
  Q_OBJECT

  Q_PROPERTY(double x  READ x  WRITE setX )
  Q_PROPERTY(double y1 READ y1 WRITE setY1)
  Q_PROPERTY(double y2 READ y2 WRITE setY2)

 public:
  using Plot = CQChartsXYPlot;

 public:
  CQChartsXYBiLineObj(const Plot *plot, int groupInd, const BBox &rect,
                      double x, double y1, double y2, const QModelIndex &ind,
                      const ColorInd &is, const ColorInd &ig, const ColorInd &iv);

  //---

  const Plot *plot() const { return plot_; }

  double x() const { return x_; }
  void setX(double r) { x_ = r; }

  double y1() const { return y1_; }
  void setY1(double r) { y1_ = r; }

  double y2() const { return y2_; }
  void setY2(double r) { y2_ = r; }

  //---

  QString typeName() const override { return "biline"; }

  QString calcId() const override;

  QString calcTipId() const override;

  //---

  bool isVisible() const override;

  bool inside(const Point &p) const override;

  void getObjSelectIndices(Indices &inds) const override;

  void draw(PaintDevice *device) const override;

  void calcPenBrush(PenBrush &penBrush, bool updateState) const override;

  void calcPointPenBrush(PenBrush &penBrush, bool updateState) const;

 private:
  void drawLines (PaintDevice *device, const Point &p1, const Point &p2) const;
  void drawPoints(PaintDevice *device, const Point &p1, const Point &p2) const;

 private:
  const Plot* plot_     { nullptr }; //!< parent plot
  int         groupInd_ { -1 };      //!< group ind
  double      x_        { 0.0 };     //!< x
  double      y1_       { 0.0 };     //!< start y
  double      y2_       { 0.0 };     //!< end y
};

//---

/*!
 * \brief XY Plot Impulse Line object
 * \ingroup Charts
 */
class CQChartsXYImpulseLineObj : public CQChartsPlotObj {
  Q_OBJECT

  Q_PROPERTY(bool   line READ isLine WRITE setLine)
  Q_PROPERTY(double x    READ x      WRITE setX   )
  Q_PROPERTY(double y1   READ y1     WRITE setY1  )
  Q_PROPERTY(double y2   READ y2     WRITE setY2  )

 public:
  using Plot = CQChartsXYPlot;

 public:
  CQChartsXYImpulseLineObj(const Plot *plot, int groupInd, const BBox &rect,
                           double x, double y1, double y2, const QModelIndex &ind,
                           const ColorInd &is, const ColorInd &ig, const ColorInd &iv);

  //---

  const Plot *plot() const { return plot_; }

  //---

  bool isLine() const { return line_; }
  void setLine(bool b) { line_ = b; }

  double x() const { return x_; }
  void setX(double r) { x_ = r; }

  double y1() const { return y1_; }
  void setY1(double r) { y1_ = r; }

  double y2() const { return y2_; }
  void setY2(double r) { y2_ = r; }

  //---

  QString typeName() const override { return "impulse"; }

  QString calcId() const override;

  QString calcTipId() const override;

  //---

  bool isVisible() const override;

  bool inside(const Point &p) const override;

  void getObjSelectIndices(Indices &inds) const override;

  void draw(PaintDevice *device) const override;

  void calcPenBrush(PenBrush &penBrush, bool updateState) const override;

 private:
  const Plot* plot_     { nullptr }; //!< parent plot
  int         groupInd_ { -1 };      //!< group ind
  bool        line_     { true };    //!< is line
  double      x_        { 0.0 };     //!< x
  double      y1_       { 0.0 };     //!< start y
  double      y2_       { 0.0 };     //!< end y
};

//---

/*!
 * \brief XY Plot Point object
 * \ingroup Charts
 */
class CQChartsXYPointObj : public CQChartsPlotPointObj {
  Q_OBJECT

  Q_PROPERTY(CQChartsSymbol symbol     READ symbol     WRITE setSymbol    )
  Q_PROPERTY(CQChartsLength symbolSize READ symbolSize WRITE setSymbolSize)
  Q_PROPERTY(CQChartsLength fontSize   READ fontSize   WRITE setFontSize  )
  Q_PROPERTY(CQChartsColor  color      READ color      WRITE setColor     )

 public:
  using Plot        = CQChartsXYPlot;
  using LabelObj    = CQChartsXYLabelObj;
  using PolylineObj = CQChartsXYPolylineObj;
  using Symbol      = CQChartsSymbol;
  using SymbolType  = CQChartsSymbolType;
  using Length      = CQChartsLength;
  using Color       = CQChartsColor;
  using Image       = CQChartsImage;
  using Units       = CQChartsUnits::Type;

 public:
  CQChartsXYPointObj(const Plot *plot, int groupInd, const BBox &rect, const Point &p,
                     const QModelIndex &ind, const ColorInd &is, const ColorInd &ig,
                     const ColorInd &iv);

  //---

  const Plot *plot() const { return plot_; }

  //---

  void setSelected(bool b) override;

  //---

  // get/set associated line object
  const PolylineObj *lineObj() const { return lineObj_; }
  void setLineObj(const PolylineObj *obj) { lineObj_ = obj; }

  //---

  // get/set associated label object
  const LabelObj *labelObj() const { return labelObj_; }
  void setLabelObj(const LabelObj *obj) { labelObj_ = obj; }

  //---

  QString calcId() const override;

  QString calcTipId() const override;

  //---

  // symbol type
  const Symbol &symbol() const { return extraData()->symbol; }
  void setSymbol(const Symbol &s) { extraData()->symbol = s; }

  Symbol calcSymbol() const;

  // symbol size
  const Length &symbolSize() const { return extraData()->symbolSize; }
  void setSymbolSize(const Length &s) { extraData()->symbolSize = s; }

  Length calcSymbolSize() const override;

  // font size
  const Length &fontSize() const { return extraData()->fontSize; }
  void setFontSize(const Length &s) { extraData()->fontSize = s; }

  Length calcFontSize() const;

  // color
  Color color() const { return extraData()->color; }
  void setColor(const Color &c) { extraData()->color = c; }

  Color calcColor() const;

  // Image
  Image image() const { return extraData()->image; }
  void setImage(const Image &i) { extraData()->image = i; }

  Image calcImage() const;

  // vector
  bool isVector() const;
  Point vector() const;
  void setVector(const Point &v) { extraData()->vector = v; }

  //---

  bool isVisible() const override;

  void getObjSelectIndices(Indices &inds) const override;

  //---

  void draw(PaintDevice *device) const override;

  void calcPenBrush(PenBrush &penBrush, bool updateState) const override;

 private:
  using OptPoint = boost::optional<Point>;

  struct ExtraData {
    Symbol   symbol;     //!< symbol
    Length   symbolSize; //!< symbol size
    Color    color;      //!< symbol fill color
    Length   fontSize;   //!< font size
    Image    image;      //!< image data
    OptPoint vector;     //!< optional vector
  };

  using ExtraDataP = std::unique_ptr<ExtraData>;

 private:
  const ExtraData *extraData(bool create=true) const;
  ExtraData *extraData();

 private:
  const Plot*        plot_     { nullptr }; //!< parent plot
  int                groupInd_ { -1 };      //!< group ind
  ExtraDataP         edata_;                //!< extra data
  const LabelObj*    labelObj_ { nullptr }; //!< label obj
  const PolylineObj* lineObj_  { nullptr }; //!< line obj
};

//---

/*!
 * \brief XY Plot Point Label object
 * \ingroup Charts
 */
class CQChartsXYLabelObj : public CQChartsPlotObj {
  Q_OBJECT

  Q_PROPERTY(double  x     READ x     WRITE setX    )
  Q_PROPERTY(double  y     READ y     WRITE setY    )
  Q_PROPERTY(QString label READ label WRITE setLabel)

 public:
  using Plot     = CQChartsXYPlot;
  using Column   = CQChartsColumn;
  using PointObj = CQChartsXYPointObj;

 public:
  CQChartsXYLabelObj(const Plot *plot, int groupInd, const BBox &rect,
                     double x, double y, const QString &label, const QModelIndex &ind,
                     const ColorInd &is, const ColorInd &iv);

 ~CQChartsXYLabelObj();

  //---

  const Plot *plot() const { return plot_; }

  double x() const { return pos_.x; }
  void setX(double r) { pos_.setX(r); }

  double y() const { return pos_.y; }
  void setY(double r) { pos_.setY(r); }

  const QString &label() const { return label_; }
  void setLabel(const QString &s) { label_ = s; }

  const Column &labelColumn() const { return labelColumn_; }
  void setLabelColumn(const Column &v) { labelColumn_ = v; }

  //---

  // get/set associated point object
  const PointObj *pointObj() const { return pointObj_; }
  void setPointObj(const PointObj *obj) { pointObj_ = obj; }

  //---

  QString typeName() const override { return "label"; }

  QString calcId() const override;

  QString calcTipId() const override;

  //---

  bool isVisible() const override;

  bool inside(const Point &p) const override;

  void getObjSelectIndices(Indices &inds) const override;

  void draw(PaintDevice *device) const override;

  void calcPenBrush(PenBrush &penBrush, bool updateState) const override;

 private:
  const Plot*     plot_     { nullptr }; //!< parent plot
  int             groupInd_ { -1 };      //!< group ind
  Point           pos_;                  //!< position
  QString         label_;                //!< label string
  Column          labelColumn_;          //!< label column
  const PointObj* pointObj_ { nullptr }; //!< point obj
};

//---

class CQChartsSmooth;

/*!
 * \brief XY Plot Polyline object (connected line)
 * \ingroup Charts
 */
class CQChartsXYPolylineObj : public CQChartsPlotObj {
  Q_OBJECT

  Q_PROPERTY(QString name READ name WRITE setName)

 public:
  using Plot   = CQChartsXYPlot;
  using Column = CQChartsColumn;

 public:
  CQChartsXYPolylineObj(const Plot *plot, int groupInd, const BBox &rect,
                        const Polygon &poly, const QString &name, const ColorInd &is,
                        const ColorInd &ig);

 ~CQChartsXYPolylineObj();

  //---

  const Plot *plot() const { return plot_; }

  const QString &name() const { return name_; }
  void setName(const QString &s) { name_ = s; }

  //---

  QString typeName() const override { return "polyline"; }

  QString calcId() const override;

  QString calcTipId() const override;

  //---

  bool isPolygon() const override { return true; }
  Polygon polygon() const override { return poly_; }

  bool isSolid() const override { return false; }

  //---

  const PlotObjs &pointObjs() const { return pointObjs_; }
  void setPointObjs(const PlotObjs &pointObjs) { pointObjs_ = pointObjs; }

  //---

//bool isVisible() const override;

  bool inside(const Point &p) const override;

//bool rectIntersect(const BBox &r, bool inside) const override;

  bool isSelectable() const override;

  bool interpY(double x, std::vector<double> &yvals) const;

  //---

  void resetBestFit();

  //---

  void getObjSelectIndices(Indices &inds) const override;

  //---

  void draw(PaintDevice *device) const override;

  void drawHull         (PaintDevice *device) const;
  void drawLines        (PaintDevice *device) const;
  void drawBestFit      (PaintDevice *device) const;
  void drawStatsLines   (PaintDevice *device) const;
  void drawMovingAverage(PaintDevice *device) const;
  void drawLineLabel    (PaintDevice *device) const;

  void calcPenBrush(PenBrush &penBrush, bool updateState) const override;

  //---

  BBox fitBBox() const;

  //---

  void initBestFit();
  void initStats();

  void initSmooth() const;

 private:
  using Smooth   = CQChartsSmooth;
  using SmoothP  = std::unique_ptr<Smooth>;
  using FitData  = CQChartsFitData;
  using StatData = CQStatData;
  using Hull     = CQChartsGrahamHull;
  using HullP    = std::unique_ptr<Hull>;

  const Plot* plot_     { nullptr }; //!< parent plot
  int         groupInd_ { -1 };      //!< group ind
  Polygon     poly_;                 //!< polygon
  QString     name_;                 //!< name
  PlotObjs    pointObjs_;            //!< point objects
  SmoothP     smooth_;               //!< smooth object
  FitData     bestFit_;              //!< best fit data
  StatData    statData_;             //!< statistics data
  HullP       hull_;                 //!< hull
};

//---

/*!
 * \brief XY Plot Polygon object (fill under)
 * \ingroup Charts
 */
class CQChartsXYPolygonObj : public CQChartsPlotObj {
  Q_OBJECT

  Q_PROPERTY(QString name READ name WRITE setName)

 public:
  using Plot = CQChartsXYPlot;

 public:
  CQChartsXYPolygonObj(const Plot *plot, int groupInd, const BBox &rect,
                       const Polygon &poly, const QString &name, const ColorInd &is,
                       const ColorInd &ig, bool under);

 ~CQChartsXYPolygonObj();

  //---

  const Plot *plot() const { return plot_; }

  const QString &name() const { return name_; }
  void setName(const QString &s) { name_ = s; }

  //---

  QString typeName() const override { return "polygon"; }

  QString calcId() const override;

  QString calcTipId() const override;

  //---

  bool isPolygon() const override { return true; }
  Polygon polygon() const override { return poly_; }

  //---

  bool isVisible() const override;

  bool inside(const Point &p) const override;

//bool rectIntersect(const BBox &r, bool inside) const override;

  bool isSelectable() const override;

  //---

  void getObjSelectIndices(Indices &inds) const override;

  //---

  void draw(PaintDevice *device) const override;

  void calcPenBrush(PenBrush &penBrush, bool updateState) const override;

  //---

  void initSmooth() const;

 private:
  using Smooth  = CQChartsSmooth;
  using SmoothP = std::unique_ptr<CQChartsSmooth>;

  const Plot* plot_     { nullptr }; //!< parent plot
  int         groupInd_ { -1 };      //!< group ind
  Polygon     poly_;                 //!< polygon
  QString     name_;                 //!< name
  bool        under_    { false };   //!< has under points
  SmoothP     smooth_;               //!< smooth object
};

//---

#include <CQChartsKey.h>

/*!
 * \brief XY Plot Key Color Box
 * \ingroup Charts
 */
class CQChartsXYColorKeyItem : public CQChartsColorBoxKeyItem {
  Q_OBJECT

 public:
  using Plot   = CQChartsXYPlot;
  using SelMod = CQChartsSelMod;
  using Obj    = CQChartsPlotObj;

 public:
  CQChartsXYColorKeyItem(Plot *plot, const ColorInd &is, const ColorInd &ig);

  Plot *plot() const { return plot_; }

  void doSelect(SelMod selMod) override;

  void draw(PaintDevice *device, const BBox &rect) const override;

  QBrush fillBrush() const override;

  bool calcHidden() const override;

  Obj *plotObj() const;

 protected:
  void drawLine(PaintDevice *device, const BBox &rect) const;

 protected:
  Plot* plot_ { nullptr }; //!< parent plot
};

/*!
 * \brief XY Plot Key Text
 * \ingroup Charts
 */
class CQChartsXYTextKeyItem : public CQChartsTextKeyItem {
  Q_OBJECT

 public:
  using Plot = CQChartsXYPlot;

 public:
  CQChartsXYTextKeyItem(Plot *plot, const QString &text, const ColorInd &is, const ColorInd &ig);

  Plot *plot() const { return plot_; }

  QColor interpTextColor(const ColorInd &ind) const override;

  bool calcHidden() const override;

 protected:
  Plot*    plot_ { nullptr }; //!< parent plot
  ColorInd is_   { 0 };       //!< set color index
  ColorInd ig_   { 0 };       //!< group color index
};

//---

/*!
 * \brief Custom invalidator for XY plot
 * \ingroup Charts
 */
class CQChartsXYInvalidator : public CQChartsInvalidator {
 public:
  CQChartsXYInvalidator(QObject *obj) :
    CQChartsInvalidator(obj) {
  }

  void invalidate(bool reload) override;
};

//---

CQCHARTS_NAMED_SHAPE_DATA(Impulse, impulse)
CQCHARTS_NAMED_LINE_DATA(Bivariate, bivariate)
CQCHARTS_NAMED_FILL_DATA(FillUnder, fillUnder)
CQCHARTS_NAMED_LINE_DATA(MovingAverage, movingAverage)

/*!
 * \brief XY Plot
 * \ingroup Charts
 *
 * Plot Type
 *   + \ref CQChartsXYPlotType
 *
 * Example
 *   + \image html xychart.png
 */
class CQChartsXYPlot : public CQChartsPointPlot,
 public CQChartsObjLineData             <CQChartsXYPlot>,
 public CQChartsObjPointData            <CQChartsXYPlot>,
 public CQChartsObjImpulseShapeData     <CQChartsXYPlot>,
 public CQChartsObjBivariateLineData    <CQChartsXYPlot>,
 public CQChartsObjFillUnderFillData    <CQChartsXYPlot>,
 public CQChartsObjMovingAverageLineData<CQChartsXYPlot> {
  Q_OBJECT

  // columns
  Q_PROPERTY(CQChartsColumn  xColumn       READ xColumn       WRITE setXColumn      )
  Q_PROPERTY(CQChartsColumns yColumns      READ yColumns      WRITE setYColumns     )
  Q_PROPERTY(CQChartsColumn  labelColumn   READ labelColumn   WRITE setLabelColumn  )
  Q_PROPERTY(CQChartsColumn  vectorXColumn READ vectorXColumn WRITE setVectorXColumn)
  Q_PROPERTY(CQChartsColumn  vectorYColumn READ vectorYColumn WRITE setVectorYColumn)

  // x is unique string (map to index)
  Q_PROPERTY(bool mapXColumn READ isMapXColumn WRITE setMapXColumn)

  // bivariate line data
  CQCHARTS_NAMED_LINE_DATA_PROPERTIES(Bivariate, bivariate)

  // stacked, cumulative
  Q_PROPERTY(bool stacked    READ isStacked    WRITE setStacked   )
  Q_PROPERTY(bool cumulative READ isCumulative WRITE setCumulative)

  // column series
  Q_PROPERTY(bool columnSeries READ isColumnSeries WRITE setColumnSeries)

  // vectors
  Q_PROPERTY(bool vectors READ isVectors WRITE setVectors)

  // impulse line data
  Q_PROPERTY(bool           impulseVisible READ isImpulseVisible WRITE setImpulseVisible)
  Q_PROPERTY(bool           impulseLines   READ isImpulseLines   WRITE setImpulseLines  )
  Q_PROPERTY(CQChartsLength impulseWidth   READ impulseWidth     WRITE setImpulseWidth  )

  CQCHARTS_NAMED_SHAPE_DATA_PROPERTIES(Impulse, impulse)

  // horizon
  Q_PROPERTY(int layers READ layers WRITE setLayers)

  // point: (display, symbol)
  CQCHARTS_POINT_DATA_PROPERTIES

  Q_PROPERTY(bool pointLineSelect READ isPointLineSelect WRITE setPointLineSelect)
  Q_PROPERTY(int  pointDelta      READ pointDelta        WRITE setPointDelta)
  Q_PROPERTY(int  pointCount      READ pointCount        WRITE setPointCount)
  Q_PROPERTY(int  pointStart      READ pointStart        WRITE setPointStart)

  // lines (selectable, rounded, display, stroke)
  Q_PROPERTY(bool linesSelectable READ isLinesSelectable WRITE setLinesSelectable)
  Q_PROPERTY(bool roundedLines    READ isRoundedLines    WRITE setRoundedLines   )
  Q_PROPERTY(bool lineLabel       READ isLineLabel       WRITE setLineLabel      )

  CQCHARTS_LINE_DATA_PROPERTIES

  // moving average
  Q_PROPERTY(bool movingAverage    READ isMovingAverage  WRITE setMovingAverage)
  Q_PROPERTY(int  numMovingAverage READ numMovingAverage WRITE setNumMovingAverage)

  CQCHARTS_NAMED_LINE_DATA_PROPERTIES(MovingAverage, movingAverage)

  // key
  Q_PROPERTY(bool keyLine READ isKeyLine WRITE setKeyLine)

  // fill under
  Q_PROPERTY(bool                  fillUnderSelectable
             READ isFillUnderSelectable WRITE setFillUnderSelectable)
  Q_PROPERTY(CQChartsFillUnderPos  fillUnderPos
             READ fillUnderPos          WRITE setFillUnderPos       )
  Q_PROPERTY(CQChartsFillUnderSide fillUnderSide
             READ fillUnderSide         WRITE setFillUnderSide      )

  CQCHARTS_NAMED_FILL_DATA_PROPERTIES(FillUnder, fillUnder)

 public:
  using FillUnderPos  = CQChartsFillUnderPos;
  using FillUnderSide = CQChartsFillUnderSide;

 private:
  struct FillUnderData {
    bool          selectable { false }; //!< is fill under selectable
    FillUnderPos  pos;                  //!< fill under position
    FillUnderSide side;                 //!< fill under side
  };

 public:
  using PolylineObj    = CQChartsXYPolylineObj;
  using PolygonObj     = CQChartsXYPolygonObj;
  using PointObj       = CQChartsXYPointObj;
  using BiLineObj      = CQChartsXYBiLineObj;
  using LabelObj       = CQChartsXYLabelObj;
  using ImpulseLineObj = CQChartsXYImpulseLineObj;

  using Length      = CQChartsLength;
  using Color       = CQChartsColor;
  using Alpha       = CQChartsAlpha;
  using ColorInd    = CQChartsUtil::ColorInd;
  using PenBrush    = CQChartsPenBrush;
  using Symbol      = CQChartsSymbol;
  using SymbolType  = CQChartsSymbolType;
  using TextOptions = CQChartsTextOptions;
  using BrushData   = CQChartsBrushData;
  using PenData     = CQChartsPenData;

 public:
  CQChartsXYPlot(View *view, const ModelP &model);
 ~CQChartsXYPlot();

  //---

  void init() override;
  void term() override;

  //---

  // x, y, label columns
  const Column &xColumn() const { return xColumn_; }
  void setXColumn(const Column &c);

  const Columns &yColumns() const { return yColumns_; }
  void setYColumns(const Columns &c);

  const Column &labelColumn() const { return labelColumn_; }
  void setLabelColumn(const Column &c);

  // vector columns
  const Column &vectorXColumn() const { return vectorXColumn_; }
  void setVectorXColumn(const Column &c);

  const Column &vectorYColumn() const { return vectorYColumn_; }
  void setVectorYColumn(const Column &c);

  //---

  Column getNamedColumn(const QString &name) const override;
  void setNamedColumn(const QString &name, const Column &c) override;

  Columns getNamedColumns(const QString &name) const override;
  void setNamedColumns(const QString &name, const Columns &c) override;

  //---

  QString columnValueToString(const Column &column, const QVariant &var) const override;

  //---

  //! get/set map x column
  bool isMapXColumn() const { return mapXColumn_; }
  void setMapXColumn(bool b);

  bool calcMapXColumn() const;

  //---

  // stacked, cumulative
  bool isStacked   () const { return stacked_; }
  bool isCumulative() const { return cumulative_; }

  //---

  // get/set is time series
  bool isColumnSeries() const { return columnSeries_; }
  void setColumnSeries(bool b) { columnSeries_ = b; }

  //---

  // select lines when point selected
  bool isPointLineSelect() const { return pointLineSelect_; }
  void setPointLineSelect(bool b) { pointLineSelect_ = b; }

  // point delta/count
  int pointDelta() const { return pointDelta_; }
  void setPointDelta(int i);

  int pointCount() const { return pointCount_; }
  void setPointCount(int i);

  int pointStart() const { return pointStart_; }
  void setPointStart(int i);

  //---

  // lines selectable, rounded, line label
  bool isLinesSelectable() const { return linesSelectable_; }
  void setLinesSelectable(bool b);

  bool isRoundedLines() const { return roundedLines_; }
  void setRoundedLines(bool b);

  bool isLineLabel() const { return lineLabel_; }
  void setLineLabel(bool b);

  //---

  // draw moving average
  bool isMovingAverage() const { return movingAverageData_.displayed; }

  int numMovingAverage() const { return movingAverageData_.n; }
  void setNumMovingAverage(int b);

  //---

  // draw line on key
  bool isKeyLine() const { return keyLine_; }
  void setKeyLine(bool b);

  //---

  void setFixedSymbolSize(const Length &s) override { setSymbolSize(s); }
  const Length &fixedSymbolSize() const override { return symbolSize(); }

  void setFixedSymbol(const Symbol &s) override { setSymbol(s); }
  const Symbol &fixedSymbol() const override { return symbol(); }

  //---

  bool canBivariateLines() const;

 private:
  void resetBestFit() override;

  //---

 public:
  // fill under
  bool isFillUnderSelectable() const { return fillUnderData_.selectable; }
  void setFillUnderSelectable(bool b);

  const FillUnderPos &fillUnderPos() const { return fillUnderData_.pos; }
  void setFillUnderPos(const FillUnderPos &p);

  const FillUnderSide &fillUnderSide() const { return fillUnderData_.side; }
  void setFillUnderSide(const FillUnderSide &s);

  //---

  // vectors
  bool isVectors() const;

  //---

  // impulse
  bool calcImpulseVisible() const;

  bool isImpulseVisible() const { return impulseData_.visible; }
  void setImpulseVisible(bool b);

  bool isImpulseLines() const { return impulseData_.lines; }
  void setImpulseLines(bool b);

  const Length &impulseWidth() const { return impulseData_.width; }
  void setImpulseWidth(const Length &l);

  double calcImpulsePixelWidth(double b=2.0) const;

  //---

  // get/set number of horizon layers
  int layers() const { return layers_; }
  void setLayers(int i);

  double layerMin() const { return layerMin_; }
  double layerDelta() const { return layerDelta_; }

  //---

  // custom color interp (for overlay)
  QColor interpColor(const Color &c, const ColorInd &ind) const override;

  //---

  // add properties
  void addProperties() override;

  //---

  Range calcRange() const override;

  void postCalcRange() override;

  bool createObjs(PlotObjs &objs) const override;

  //---

  void updateColumnNames() override;

  //---

  bool calcGroupHidden(int groupInd) const;

  int numVisibleGroups() const override;

  int mapVisibleGroup(int groupInd) const override;
  int unmapVisibleGroup(int groupInd) const override;

  //---

  bool headerSeriesData(std::vector<double> &x) const;

  bool rowData(const ModelVisitor::VisitData &data, double &x, std::vector<double> &yv,
               QModelIndex &ind, bool skipBad) const;

  //---

  PolylineObj *addPolyLine(const Polygon &polyLine, int groupInd, const ColorInd &is,
                           const ColorInd &ig, const QString &name, PlotObjs &pointObjs,
                           PlotObjs &objs) const;

  void addPolygon(const Polygon &poly, int groupInd, const ColorInd &is,
                  const ColorInd &ig, const QString &name, PlotObjs &objs, bool under) const;

  //---

  double drawRangeXMin(int groupInd, bool adjust=false) const;
  double drawRangeXMax(int groupInd, bool adjust=false) const;

  double drawRangeYMin(int groupInd) const;
  double drawRangeYMax(int groupInd) const;

  //---

  virtual PointObj *createPointObj(int groupInd, const BBox &rect, const Point &p,
                                   const QModelIndex &ind, const ColorInd &is, const ColorInd &ig,
                                   const ColorInd &iv) const;

  virtual BiLineObj *createBiLineObj(int groupInd, const BBox &rect,
                                     const Point &p1, const Point &p2,
                                     const QModelIndex &ind, const ColorInd &is,
                                     const ColorInd &ig, const ColorInd &iv) const;

  virtual LabelObj *createLabelObj(int groupInd, const BBox &rect, const Point &p,
                                   const QString &label, const QModelIndex &ind,
                                   const ColorInd &is, const ColorInd &iv) const;

  virtual ImpulseLineObj *createImpulseLineObj(int groupInd, const BBox &rect,
                                               const Point &p1, const Point &p2,
                                               const QModelIndex &ind, const ColorInd &is,
                                               const ColorInd &ig, const ColorInd &iv) const;

  virtual PolylineObj *createPolylineObj(int groupInd, const BBox &rect, const Polygon &poly,
                                         const QString &name, const ColorInd &is,
                                         const ColorInd &ig) const;

  virtual PolygonObj *createPolygonObj(int groupInd, const BBox &rect, const Polygon &poly,
                                       const QString &name, const ColorInd &is,
                                       const ColorInd &ig, bool under) const;

  //---

  QString valueName(int is, int ns, int irow, bool tip=false) const;

  void addKeyItems(PlotKey *key) override;

  //---

  double symbolWidth () const { return symbolWidth_ ; }
  double symbolHeight() const { return symbolHeight_; }

  //---

  bool probe(ProbeData &probeData) const override;

  struct InterpValue {
    double y;
    Column column;

    InterpValue(double y, Column column) :
     y(y), column(column) {
    }
  };

  using InterpValues = std::vector<InterpValue>;

  bool interpY(double x, InterpValues &yvals) const;

  //---

  BBox dataFitBBox() const override;

  //---

  QString posStr(const Point &w) const override;

  //---

  bool addMenuItems(QMenu *menu, const Point &p) override;

  //---

  BBox axesFitBBox() const override;

  bool hasFgAxes() const override;

  void drawFgAxes(PaintDevice *device) const override;

  //---

  void drawBackgroundRect(PaintDevice *device, const DrawRegion &drawRegion,
                          const BBox &rect, const BrushData &brushData,
                          const PenData &penData, const Sides &sides) const override;

  //---

  BBox calcExtraFitBBox() const override;

  double xAxisHeight(const CQChartsAxisSide::Type &side) const override;
  double yAxisWidth (const CQChartsAxisSide::Type &side) const override;

  //---

  bool hasBackground() const override;

  void execDrawBackground(PaintDevice *device) const override;

  //---

  void preDrawObjs (CQChartsPaintDevice *device) const override;
  void postDrawObjs(CQChartsPaintDevice *device) const override;

  //---

  void drawXAxisAt(PaintDevice *device, Plot *plot, double pos) const override;
  void drawYAxisAt(PaintDevice *device, Plot *plot, double pos) const override;

  //---

  void drawArrow(PaintDevice *device, const Point &p1, const Point &p2) const;

  //---

  void drawXRug(PaintDevice *device) const;
  void drawYRug(PaintDevice *device) const;

  void drawXYRug(PaintDevice *device, const RugP &rug, double delta=0.0) const;

  //---

  void drawDataLabel(PaintDevice *device, const BBox &bbox, const QString &str,
                     const PenBrush &penBrush, const Font &font) const;

  //---

  // axis names
  bool xAxisName(QString &name, const QString &def="") const override;
  bool yAxisName(QString &name, const QString &def="") const override;

  bool xColumnName(QString &name, const QString &def, bool tip=false) const;
  bool yColumnName(QString &name, const QString &def, bool tip=false) const;

  //---

  // object for group
  CQChartsXYPolylineObj *getGroupObj(int ig) const;

  //---

  void write(std::ostream &os, const QString &plotVarName, const QString &modelVarName,
             const QString &viewVarName) const override;

  //---

  void addDrawText(PaintDevice *device, const QString &str, const Point &point,
                   const TextOptions &textOptions, const Point &targetPoint,
                   bool centered) const;

 public slots:
  //! set points visible
  void setPointsSlot(bool b);

  //! set lines visible
  void setLinesSlot(bool b);

  //! set bivariate
  void setBivariateLinesSlot(bool b);

  //! set stacked
  void setStacked(bool b);

  //! set cumulative
  void setCumulative(bool b);

  //! set impulse
  void setImpulseVisibleSlot(bool b);

  //! set vectors
  void setVectors(bool b);

  //! set fill under
  void setFillUnderFilledSlot(bool b);

  //! set draw moving average
  void setMovingAverage(bool b);

 private slots:
  void yAxisIncludeZeroSlot();

 private:
  struct IndPoly {
    using Inds = std::vector<QModelIndex>;

    Inds    inds;
    Polygon poly;
  };

  using SetIndPoly      = std::vector<IndPoly>;
  using GroupSetIndPoly = std::map<int, SetIndPoly>;

 private:
  void updateAxes();

  void createGroupSetIndPoly(GroupSetIndPoly &groupSetIndPoly) const;
  bool createGroupSetObjs(const GroupSetIndPoly &groupSetIndPoly, PlotObjs &objs) const;

  bool addBivariateLines(int groupInd, const SetIndPoly &setPoly,
                         const ColorInd &ig, PlotObjs &objs) const;
  bool addLines(int groupInd, const SetIndPoly &setPoly,
                const ColorInd &ig, PlotObjs &objs) const;

  Point calcFillUnderPos(int groupInd, double x, double y) const;

  int numSets() const;

 protected:
  CQChartsPlotCustomControls *createCustomControls() override;

 private:
  using Arrow  = CQChartsArrow;
  using ArrowP = std::unique_ptr<CQChartsArrow>;

  // columns
  Column  xColumn_;       //!< x column
  Columns yColumns_;      //!< y columns
  Column  labelColumn_;   //!< label column
  Column  vectorXColumn_; //!< vector x direction column
  Column  vectorYColumn_; //!< vector y direction column

  bool mapXColumn_ { false }; //!< map x column value to unique id

  ColumnType xColumnType_ { ColumnType::NONE }; //!< x column type

  bool columnSeries_ { false }; //!< are column as series

  // point data
  bool pointLineSelect_ { false }; //!< select line of point
  int  pointDelta_      { -1 };    //!< point delta
  int  pointCount_      { -1 };    //!< point count
  int  pointStart_      { 0 };     //!< point start (0=start, -1=end, -2=middle)

  // plot type
  bool stacked_    { false }; //!< is stacked
  bool cumulative_ { false }; //!< cumulate values

  // line options
  bool linesSelectable_ { false }; //!< are lines selectable
  bool roundedLines_    { false }; //!< draw rounded (smooth) lines
  bool lineLabel_       { false }; //!< draw line label

  // overlays
  struct MovingAverageData {
    bool displayed { false };
    int  n         { 3 };
  };

  MovingAverageData movingAverageData_; //!< moving average data

  // key
  bool keyLine_ { false }; //!< draw line on key

  // fill under data
  FillUnderData fillUnderData_; //!< fill under data

  // vector data
  ArrowP arrowObj_; //!< vectors data

  // impulse
  struct ImpulseData {
    bool   visible { false };  //!< impulse visible
    bool   lines   { true };   //!< impulse lines
    Length width   { "16px" }; //!< impulse width
  };

  ImpulseData impulseData_;

  // horizon layers
  int    layers_     { -1 };  //!< nummber of horizon layers
  double layerMin_   { 0.0 }; //!< horizon min
  double layerMax_   { 0.0 }; //!< horizon max
  double layerDelta_ { 0.0 }; //!< horizon delta

  double symbolWidth_  { 1.0 }; //!< current symbol width
  double symbolHeight_ { 1.0 }; //!< current symbol height

  // axis side data
  using AxisSideSize = std::map<CQChartsAxisSide::Type, double>;

  mutable AxisSideSize xAxisSideHeight_; //!< top or bottom
  mutable AxisSideSize yAxisSideWidth_;  //!< left or right

  mutable int maxNumPoints_ { 0 };

  // invalidator
  CQChartsXYInvalidator xyInvalidator_;
};

//---

#include <CQChartsPointPlotCustomControls.h>

/*!
 * \brief XY Plot plot custom controls
 * \ingroup Charts
 */
class CQChartsXYPlotCustomControls : public CQChartsPointPlotCustomControls {
  Q_OBJECT

 public:
  CQChartsXYPlotCustomControls(CQCharts *charts);

  void init() override;

  void setPlot(CQChartsPlot *plot) override;

 public slots:
  void updateWidgets() override;

 protected:
  void addWidgets() override;

  void addColumnWidgets() override;

  void addOptionsWidgets() override;

  void connectSlots(bool b) override;

 protected slots:
  void pointsSlot(int);
  void linesSlot(int);
  void fillUnderSlot(int);
  void stackedSlot(int);
  void impulseSlot(int);
  void bestFitSlot(int);
  void convexHullSlot(int);
  void statsLinesSlot(int);
  void movingAverageSlot(int);

 protected:
  CQChartsXYPlot* plot_ { nullptr };

  FrameData optionsFrame_;

  CQChartsBoolParameterEdit* pointsCheck_    { nullptr };
  CQChartsBoolParameterEdit* linesCheck_     { nullptr };
  CQChartsBoolParameterEdit* fillUnderCheck_ { nullptr };

  CQChartsBoolParameterEdit* stackedCheck_ { nullptr };
  CQChartsBoolParameterEdit* impulseCheck_ { nullptr };

  CQChartsBoolParameterEdit* bestFitCheck_       { nullptr };
  CQChartsBoolParameterEdit* hullCheck_          { nullptr };
  CQChartsBoolParameterEdit* statsCheck_         { nullptr };
  CQChartsBoolParameterEdit* movingAverageCheck_ { nullptr };
};

#endif
