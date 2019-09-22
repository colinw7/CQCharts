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

//---

/*!
 * \brief XY plot type
 * \ingroup Charts
 */
class CQChartsXYPlotType : public CQChartsPointPlotType {
 public:
  CQChartsXYPlotType();

  QString name() const override { return "xy"; }
  QString desc() const override { return "XY"; }

  void addParameters() override;

  bool canProbe() const override { return true; }

  QString description() const override;

  CQChartsPlot *create(CQChartsView *view, const ModelP &model) const override;
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
  CQChartsXYBiLineObj(const CQChartsXYPlot *plot, int groupInd, const CQChartsGeom::BBox &rect,
                      double x, double y1, double y2, const QModelIndex &ind,
                      const ColorInd &is, const ColorInd &iv);

  //---

  const CQChartsXYPlot *plot() const { return plot_; }

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

  bool visible() const override;

  bool inside(const CQChartsGeom::Point &p) const override;

  void getSelectIndices(Indices &inds) const override;

  void draw(CQChartsPaintDevice *device) override;

 private:
  const CQChartsXYPlot* plot_     { nullptr }; //!< parent plot
  int                   groupInd_ { -1 };      //!< group ind
  double                x_        { 0.0 };     //!< x
  double                y1_       { 0.0 };     //!< start y
  double                y2_       { 0.0 };     //!< end y
};

//---

/*!
 * \brief XY Plot Impulse Line object
 * \ingroup Charts
 */
class CQChartsXYImpulseLineObj : public CQChartsPlotObj {
  Q_OBJECT

  Q_PROPERTY(double x  READ x  WRITE setX )
  Q_PROPERTY(double y1 READ y1 WRITE setY1)
  Q_PROPERTY(double y2 READ y2 WRITE setY2)

 public:
  CQChartsXYImpulseLineObj(const CQChartsXYPlot *plot, int groupInd, const CQChartsGeom::BBox &rect,
                           double x, double y1, double y2, const QModelIndex &ind,
                           const ColorInd &is, const ColorInd &iv);

  //---

  const CQChartsXYPlot *plot() const { return plot_; }

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

  bool visible() const override;

  bool inside(const CQChartsGeom::Point &p) const override;

  void getSelectIndices(Indices &inds) const override;

  void draw(CQChartsPaintDevice *device) override;

 private:
  const CQChartsXYPlot* plot_     { nullptr }; //!< parent plot
  int                   groupInd_ { -1 };      //!< group ind
  double                x_        { 0.0 };     //!< x
  double                y1_       { 0.0 };     //!< start y
  double                y2_       { 0.0 };     //!< end y
};

//---

/*!
 * \brief XY Plot Point object
 * \ingroup Charts
 */
class CQChartsXYPointObj : public CQChartsPlotObj {
  Q_OBJECT

  Q_PROPERTY(double x READ x WRITE setX)
  Q_PROPERTY(double y READ y WRITE setY)

  Q_PROPERTY(CQChartsSymbol symbolType READ symbolType WRITE setSymbolType)
  Q_PROPERTY(CQChartsLength symbolSize READ symbolSize WRITE setSymbolSize)
  Q_PROPERTY(CQChartsLength fontSize   READ fontSize   WRITE setFontSize  )
  Q_PROPERTY(CQChartsColor  color      READ color      WRITE setColor     )

 public:
  CQChartsXYPointObj(const CQChartsXYPlot *plot, int groupInd,
                     const CQChartsGeom::BBox &rect, const QPointF &p,
                     const ColorInd &is, const ColorInd &ig, const ColorInd &iv);

 ~CQChartsXYPointObj();

  //---

  void setSelected(bool b) override;

  //---

  const CQChartsXYPlot *plot() const { return plot_; }

  //---

  // position
  double x() const { return pos_.x(); }
  void setX(double r) { pos_.setX(r); }

  double y() const { return pos_.y(); }
  void setY(double r) { pos_.setY(r); }

  //---

  // get/set associated line object
  const CQChartsXYPolylineObj *lineObj() const { return lineObj_; }
  void setLineObj(const CQChartsXYPolylineObj *obj) { lineObj_ = obj; }

  //---

  // get/set associated label object
  const CQChartsXYLabelObj *labelObj() const { return labelObj_; }
  void setLabelObj(const CQChartsXYLabelObj *obj) { labelObj_ = obj; }

  //---

  QString typeName() const override { return "point"; }

  QString calcId() const override;

  QString calcTipId() const override;

  //---

  // symbol type
  CQChartsSymbol symbolType() const;
  void setSymbolType(CQChartsSymbol type) { extraData()->symbolType = type; }

  // symbol size
  CQChartsLength symbolSize() const;
  void setSymbolSize(const CQChartsLength &s) { extraData()->symbolSize = s; }

  // font size
  CQChartsLength fontSize() const;
  void setFontSize(const CQChartsLength &s) { extraData()->fontSize = s; }

  // color
  CQChartsColor color() const;
  void setColor(const CQChartsColor &c) { extraData()->color = c; }

  // Image
  QImage image() const;
  void setImage(const QImage &i) { extraData()->image = i; }

  // vector
  bool isVector() const;
  QPointF vector() const;
  void setVector(const QPointF &v) { extraData()->vector = v; }

  //---

  bool visible() const override;

  bool inside(const CQChartsGeom::Point &p) const override;

  void getSelectIndices(Indices &inds) const override;

  void draw(CQChartsPaintDevice *device) override;

 private:
  using OptPoint = boost::optional<QPointF>;

  struct ExtraData {
    CQChartsSymbol symbolType { CQChartsSymbol::Type::NONE }; //!< symbol type
    CQChartsLength symbolSize { CQChartsUnits::NONE, 0.0 };   //!< symbol size
    CQChartsLength fontSize   { CQChartsUnits::NONE, 0.0 };   //!< font size
    CQChartsColor  color;                                     //!< symbol fill color
    OptPoint       vector;                                    //!< optional vector
    QImage         image;                                     //!< image
  };

 private:
  const ExtraData *extraData() const;
  ExtraData *extraData();

 private:
  const CQChartsXYPlot*        plot_     { nullptr }; //!< parent plot
  int                          groupInd_ { -1 };      //!< group ind
  QPointF                      pos_;                  //!< position
  ExtraData*                   edata_    { nullptr }; //!< extra data
  const CQChartsXYLabelObj*    labelObj_ { nullptr }; //!< label obj
  const CQChartsXYPolylineObj* lineObj_  { nullptr }; //!< line obj
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
  CQChartsXYLabelObj(const CQChartsXYPlot *plot, int groupInd, const CQChartsGeom::BBox &rect,
                     double x, double y, const QString &label, const QModelIndex &ind,
                     const ColorInd &is, const ColorInd &iv);

 ~CQChartsXYLabelObj();

  //---

  const CQChartsXYPlot *plot() const { return plot_; }

  double x() const { return pos_.x(); }
  void setX(double r) { pos_.setX(r); }

  double y() const { return pos_.y(); }
  void setY(double r) { pos_.setY(r); }

  const QString &label() const { return label_; }
  void setLabel(const QString &v) { label_ = v; }

  //---

  // get/set associated point object
  const CQChartsXYPointObj *pointObj() const { return pointObj_; }
  void setPointObj(const CQChartsXYPointObj *obj) { pointObj_ = obj; }

  //---

  QString typeName() const override { return "label"; }

  QString calcId() const override;

  QString calcTipId() const override;

  //---

  bool visible() const override;

  bool inside(const CQChartsGeom::Point &p) const override;

  void getSelectIndices(Indices &inds) const override;

  void draw(CQChartsPaintDevice *device) override;

 private:
  using OptPoint = boost::optional<QPointF>;

  const CQChartsXYPlot*     plot_     { nullptr }; //!< parent plot
  int                       groupInd_ { -1 };      //!< group ind
  QPointF                   pos_;                  //!< position
  QString                   label_;                //!< label
  const CQChartsXYPointObj* pointObj_ { nullptr }; //!< point obj
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
  CQChartsXYPolylineObj(const CQChartsXYPlot *plot, int groupInd, const CQChartsGeom::BBox &rect,
                        const QPolygonF &poly, const QString &name,
                        const ColorInd &is, const ColorInd &ig);

 ~CQChartsXYPolylineObj();

  //---

  const CQChartsXYPlot *plot() const { return plot_; }

  const QString &name() const { return name_; }
  void setName(const QString &s) { name_ = s; }

  //---

  QString typeName() const override { return "polyline"; }

  QString calcId() const override;

  QString calcTipId() const override;

  //---

  bool visible() const override;

  bool inside(const CQChartsGeom::Point &p) const override;

  bool rectIntersect(const CQChartsGeom::BBox &r, bool inside) const override;

  bool interpY(double x, std::vector<double> &yvals) const;

  bool isOutlier(double y) const;

  //---

  void resetBestFit();

  //---

  void getSelectIndices(Indices &inds) const override;

  void draw(CQChartsPaintDevice *device) override;

  void initBestFit();
  void initStats();

  void initSmooth() const;

 private:
  const CQChartsXYPlot* plot_     { nullptr }; //!< parent plot
  int                   groupInd_ { -1 };      //!< group ind
  QPolygonF             poly_;                 //!< polygon
  QString               name_;                 //!< name
  CQChartsSmooth*       smooth_   { nullptr }; //!< smooth object
  CQChartsFitData       bestFit_;              //!< best fit data
  CQStatData            statData_;             //!< statistics data
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
  CQChartsXYPolygonObj(const CQChartsXYPlot *plot, int groupInd, const CQChartsGeom::BBox &rect,
                       const QPolygonF &poly, const QString &name,
                       const ColorInd &is, const ColorInd &ig);

 ~CQChartsXYPolygonObj();

  //---

  const CQChartsXYPlot *plot() const { return plot_; }

  const QString &name() const { return name_; }
  void setName(const QString &s) { name_ = s; }

  //---

  QString typeName() const override { return "polygon"; }

  QString calcId() const override;

  QString calcTipId() const override;

  //---

  bool visible() const override;

  bool inside(const CQChartsGeom::Point &p) const override;

  bool rectIntersect(const CQChartsGeom::BBox &r, bool inside) const override;

  void getSelectIndices(Indices &inds) const override;

  void draw(CQChartsPaintDevice *device) override;

  void initSmooth() const;

 private:
  const CQChartsXYPlot* plot_     { nullptr }; //!< parent plot
  int                   groupInd_ { -1 };      //!< group ind
  QPolygonF             poly_;                 //!< polygon
  QString               name_;                 //!< name
  CQChartsSmooth*       smooth_   { nullptr }; //!< smooth object
};

//---

#include <CQChartsKey.h>

/*!
 * \brief XY Plot Key Color Box
 * \ingroup Charts
 */
class CQChartsXYKeyColor : public CQChartsKeyColorBox {
  Q_OBJECT

 public:
  CQChartsXYKeyColor(CQChartsXYPlot *plot, const ColorInd &is, const ColorInd &ig);

  CQChartsXYPlot *plot() const { return plot_; }

  void doSelect(CQChartsSelMod selMod) override;

  QBrush fillBrush() const override;

  CQChartsPlotObj *plotObj() const;

 protected:
  CQChartsXYPlot* plot_ { nullptr }; //!< parent plot
};

/*!
 * \brief XY Plot Key Line
 * \ingroup Charts
 */
class CQChartsXYKeyLine : public CQChartsKeyItem {
  Q_OBJECT

 public:
  CQChartsXYKeyLine(CQChartsXYPlot *plot, const ColorInd &is, const ColorInd &ig);

  CQChartsXYPlot *plot() const { return plot_; }

  void doSelect(CQChartsSelMod selMod) override;

  QSizeF size() const override;

  void draw(CQChartsPaintDevice *device, const CQChartsGeom::BBox &rect) const override;

  CQChartsPlotObj *plotObj() const;

 protected:
  CQChartsXYPlot* plot_ { nullptr }; //!< parent plot
  ColorInd        is_   { 0 };       //!< set color index
  ColorInd        ig_   { 0 };       //!< group color index
};

/*!
 * \brief XY Plot Key Text
 * \ingroup Charts
 */
class CQChartsXYKeyText : public CQChartsKeyText {
  Q_OBJECT

 public:
  CQChartsXYKeyText(CQChartsXYPlot *plot, const QString &text,
                    const ColorInd &is, const ColorInd &ig);

  CQChartsXYPlot *plot() const { return plot_; }

  QColor interpTextColor(const ColorInd &ind) const override;

 protected:
  CQChartsXYPlot* plot_ { nullptr }; //!< parent plot
  ColorInd        is_   { 0 };       //!< set color index
  ColorInd        ig_   { 0 };       //!< group color index
};

//---

CQCHARTS_NAMED_LINE_DATA(Impulse,impulse)
CQCHARTS_NAMED_LINE_DATA(Bivariate,bivariate)
CQCHARTS_NAMED_FILL_DATA(FillUnder,fillUnder)

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
 public CQChartsObjLineData         <CQChartsXYPlot>,
 public CQChartsObjPointData        <CQChartsXYPlot>,
 public CQChartsObjBestFitShapeData <CQChartsXYPlot>,
 public CQChartsObjStatsLineData    <CQChartsXYPlot>,
 public CQChartsObjImpulseLineData  <CQChartsXYPlot>,
 public CQChartsObjBivariateLineData<CQChartsXYPlot>,
 public CQChartsObjFillUnderFillData<CQChartsXYPlot> {
  Q_OBJECT

  // columns
  Q_PROPERTY(CQChartsColumn  xColumn       READ xColumn       WRITE setXColumn      )
  Q_PROPERTY(CQChartsColumns yColumns      READ yColumns      WRITE setYColumns     )
  Q_PROPERTY(CQChartsColumn  labelColumn   READ labelColumn   WRITE setLabelColumn  )
  Q_PROPERTY(CQChartsColumn  vectorXColumn READ vectorXColumn WRITE setVectorXColumn)
  Q_PROPERTY(CQChartsColumn  vectorYColumn READ vectorYColumn WRITE setVectorYColumn)

  // bivariate
  CQCHARTS_NAMED_LINE_DATA_PROPERTIES(Bivariate, bivariate)

  // stacked, cumulative
  Q_PROPERTY(bool stacked    READ isStacked    WRITE setStacked   )
  Q_PROPERTY(bool cumulative READ isCumulative WRITE setCumulative)

  // best fit
  Q_PROPERTY(bool bestFit          READ isBestFit          WRITE setBestFit         )
  Q_PROPERTY(bool bestFitOutliers  READ isBestFitOutliers  WRITE setBestFitOutliers )
  Q_PROPERTY(int  bestFitOrder     READ bestFitOrder       WRITE setBestFitOrder    )
  Q_PROPERTY(bool bestFitDeviation READ isBestFitDeviation WRITE setBestFitDeviation)

  CQCHARTS_NAMED_SHAPE_DATA_PROPERTIES(BestFit, bestFit)

  // stats
  CQCHARTS_NAMED_LINE_DATA_PROPERTIES(Stats, stats)

  // vectors
  Q_PROPERTY(bool vectors READ isVectors WRITE setVectors)

  // impulse
  CQCHARTS_NAMED_LINE_DATA_PROPERTIES(Impulse, impulse)

  // point: (display, symbol)
  CQCHARTS_POINT_DATA_PROPERTIES

  Q_PROPERTY(bool pointLineSelect READ isPointLineSelect WRITE setPointLineSelect)
  Q_PROPERTY(int  pointDelta      READ pointDelta        WRITE setPointDelta)
  Q_PROPERTY(int  pointCount      READ pointCount        WRITE setPointCount)
  Q_PROPERTY(int  pointStart      READ pointStart        WRITE setPointStart)

  // lines (selectable, rounded, display, stroke)
  Q_PROPERTY(bool linesSelectable READ isLinesSelectable WRITE setLinesSelectable)
  Q_PROPERTY(bool roundedLines    READ isRoundedLines    WRITE setRoundedLines   )

  CQCHARTS_LINE_DATA_PROPERTIES

  // fill under
  Q_PROPERTY(bool                  fillUnderSelectable
             READ isFillUnderSelectable WRITE setFillUnderSelectable)
  Q_PROPERTY(CQChartsFillUnderPos  fillUnderPos
             READ fillUnderPos          WRITE setFillUnderPos       )
  Q_PROPERTY(CQChartsFillUnderSide fillUnderSide
             READ fillUnderSide         WRITE setFillUnderSide      )

  CQCHARTS_NAMED_FILL_DATA_PROPERTIES(FillUnder,fillUnder)

 private:
  struct FillUnderData {
    bool                  selectable { false }; //!< is fill under selectable
    CQChartsFillUnderPos  pos;                  //!< fill under position
    CQChartsFillUnderSide side;                 //!< fill under side
  };

 public:
  CQChartsXYPlot(CQChartsView *view, const ModelP &model);

 ~CQChartsXYPlot();

  //---

  // x, y, label columns
  const CQChartsColumn &xColumn() const { return xColumn_; }
  void setXColumn(const CQChartsColumn &c);

  const CQChartsColumns &yColumns() const { return yColumns_; }
  void setYColumns(const CQChartsColumns &c);

  const CQChartsColumn &labelColumn() const { return labelColumn_; }
  void setLabelColumn(const CQChartsColumn &c);

  // vector columns
  const CQChartsColumn &vectorXColumn() const { return vectorXColumn_; }
  void setVectorXColumn(const CQChartsColumn &c);

  const CQChartsColumn &vectorYColumn() const { return vectorYColumn_; }
  void setVectorYColumn(const CQChartsColumn &c);

  //---

  // stacked, cumulative
  bool isStacked() const { return stacked_; }

  bool isCumulative() const { return cumulative_; }

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

  // lines selectable, rounded
  bool isLinesSelectable() const { return linesSelectable_; }
  void setLinesSelectable(bool b);

  bool isRoundedLines() const { return roundedLines_; }
  void setRoundedLines(bool b);

  //---

  // best fit
  bool isBestFit() const { return bestFitData_.visible; }

  bool isBestFitOutliers() const { return bestFitData_.includeOutliers; }
  void setBestFitOutliers(bool b);

  int bestFitOrder() const { return bestFitData_.order; }
  void setBestFitOrder(int o);

  bool isBestFitDeviation() const { return bestFitData_.showDeviation; }
  void setBestFitDeviation(bool b);

 private:
  void resetBestFit();

  //---

 public:
  // fill under
  bool isFillUnderSelectable() const { return fillUnderData_.selectable; }
  void setFillUnderSelectable(bool b);

  const CQChartsFillUnderPos &fillUnderPos() const { return fillUnderData_.pos; }
  void setFillUnderPos(const CQChartsFillUnderPos &p);

  const CQChartsFillUnderSide &fillUnderSide() const { return fillUnderData_.side; }
  void setFillUnderSide(const CQChartsFillUnderSide &s);

  QPointF calcFillUnderPos(double x, double y) const;

  //---

  // vectors
  bool isVectors() const;

  //---

  QColor interpPaletteColor(const ColorInd &ind, bool scale=false) const override;

  //---

  void addProperties() override;

  //---

  CQChartsGeom::Range calcRange() const override;

  bool createObjs(PlotObjs &objs) const override;

  //---

  void updateColumnNames() override;

  //---

  bool rowData(const ModelVisitor::VisitData &data, double &x, std::vector<double> &yv,
               QModelIndex &ind, bool skipBad) const;

  //---

  CQChartsXYPolylineObj *addPolyLine(const QPolygonF &polyLine, int groupInd, const ColorInd &is,
                                     const ColorInd &ig, const QString &name, PlotObjs &pointObjs,
                                     PlotObjs &objs) const;

  void addPolygon(const QPolygonF &poly, int groupInd, const ColorInd &is, const ColorInd &ig,
                  const QString &name, PlotObjs &objs) const;

  QString valueName(int is, int ns, int irow) const;

  void addKeyItems(CQChartsPlotKey *key) override;

  //---

  double symbolWidth () const { return symbolWidth_ ; }
  double symbolHeight() const { return symbolHeight_; }

  //---

  bool probe(ProbeData &probeData) const override;

  bool interpY(double x, std::vector<double> &yvals) const;

  //---

  CQChartsGeom::BBox dataFitBBox() const override;

  //---

  bool addMenuItems(QMenu *menu) override;

  //---

  void drawArrow(CQChartsPaintDevice *device, const QPointF &p1, const QPointF &p2) const;

  //---

  // axis names
  QString xAxisName(const QString &def="") const;
  QString yAxisName(const QString &def="") const;

  // object for group
  CQChartsPlotObj *getGroupObj(int ig) const;

  //---

  void write(std::ostream &os, const QString &varName, const QString &modelName) const override;

 public slots:
  // set points visible
  void setPointsSlot(bool b);

  // set lines visible
  void setLinesSlot(bool b);

  // set bivariate
  void setBivariateLinesSlot(bool b);

  // set stacked
  void setStacked(bool b);

  // set cumulative
  void setCumulative(bool b);

  // set impulse
  void setImpulseLinesSlot(bool b);

  // set vectors
  void setVectors(bool b);

  // set fill under
  void setFillUnderFilledSlot(bool b);

  // set best fit
  void setBestFit(bool b);

 private:
  struct IndPoly {
    using Inds = std::vector<QModelIndex>;

    Inds      inds;
    QPolygonF poly;
  };

  using SetIndPoly      = std::vector<IndPoly>;
  using GroupSetIndPoly = std::map<int,SetIndPoly>;

  struct BestFitData {
    bool visible         { false }; //!< show fit
    bool includeOutliers { true };  //!< include outliers
    bool showDeviation   { false }; //!< show deviation
    int  order           { 3 };     //!< fit order
  };

 private:
  void initAxes();

  void createGroupSetIndPoly(GroupSetIndPoly &groupSetIndPoly) const;
  bool createGroupSetObjs(const GroupSetIndPoly &groupSetIndPoly, PlotObjs &objs) const;

  bool addBivariateLines(int groupInd, const SetIndPoly &setPoly,
                         const ColorInd &ig, PlotObjs &objs) const;
  bool addLines(int groupInd, const SetIndPoly &setPoly,
                const ColorInd &ig, PlotObjs &objs) const;

 private:
  // columns
  CQChartsColumn  xColumn_;       //!< x column
  CQChartsColumns yColumns_;      //!< y columns
  CQChartsColumn  labelColumn_;   //!< label column
  CQChartsColumn  vectorXColumn_; //!< vector x direction column
  CQChartsColumn  vectorYColumn_; //!< vector y direction column

  // point data
  bool pointLineSelect_ { false }; //!< select line of point
  int  pointDelta_      { -1 };    //!< point delta
  int  pointCount_      { -1 };    //!< point count
  int  pointStart_      { 0 };     //!< point start (0=start, -1=end, -2=middle)

  // plot type
  bool stacked_         { false }; //!< is stacked
  bool cumulative_      { false }; //!< cumulate values
  bool roundedLines_    { false }; //!< draw rounded (smooth) lines
  bool linesSelectable_ { false }; //!< are lines selectable

  // fill under data
  FillUnderData fillUnderData_; //!< fill under data

  // plot overlay data
  BestFitData bestFitData_; //!< best fit data

  // vector data
  CQChartsArrow* arrowObj_ { nullptr }; //!< vectors data

  double symbolWidth_  { 1.0 }; //!< current symbol width
  double symbolHeight_ { 1.0 }; //!< current symbol height
};

#endif
