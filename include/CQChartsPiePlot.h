#ifndef CQChartsPiePlot_H
#define CQChartsPiePlot_H

#include <CQChartsGroupPlot.h>
#include <CQChartsPlotObj.h>
#include <CQChartsRotatedTextBoxObj.h>
#include <CQChartsValueSet.h>
#include <CQChartsGeom.h>

#include <boost/optional.hpp>
#include <string>

//---

/*!
 * \brief Pie Plot type
 * \ingroup Charts
 */
class CQChartsPiePlotType : public CQChartsGroupPlotType {
 public:
  CQChartsPiePlotType();

  QString name() const override { return "pie"; }
  QString desc() const override { return "Pie"; }

  Category category() const override { return Category::ONE_D; }

  void addParameters() override;

  bool hasAxes() const override { return false; }

  bool allowXLog() const override { return false; }
  bool allowYLog() const override { return false; }

  bool canProbe() const override { return false; }

  bool canEqualScale() const override { return true; }

  QString description() const override;

  Plot *create(View *view, const ModelP &model) const override;
};

//---

class CQChartsPiePlot;

//---

/*!
 * \brief Pie Plot Text object
 * \ingroup Charts
 */
class CQChartsPieTextObj : public CQChartsRotatedTextBoxObj {
 public:
  using PiePlot = CQChartsPiePlot;

 public:
  CQChartsPieTextObj(const PiePlot *plot);

  const PiePlot *plot() const { return plot_; }

  CQChartsTextOptions textOptions() const;

 private:
  const PiePlot* plot_ { nullptr };
};

//---

class CQChartsPieGroupObj;

/*!
 * \brief Pie Plot object
 * \ingroup Charts
 */
class CQChartsPieObj : public CQChartsPlotObj {
  Q_OBJECT

  Q_PROPERTY(CQChartsAngle angle1      READ angle1      WRITE setAngle1     )
  Q_PROPERTY(CQChartsAngle angle2      READ angle2      WRITE setAngle2     )
  Q_PROPERTY(double        innerRadius READ innerRadius WRITE setInnerRadius)
  Q_PROPERTY(double        outerRadius READ outerRadius WRITE setOuterRadius)
  Q_PROPERTY(QString       label       READ label       WRITE setLabel      )
  Q_PROPERTY(double        value       READ value       WRITE setValue      )
  Q_PROPERTY(bool          missing     READ isMissing   WRITE setMissing    )
  Q_PROPERTY(QString       keyLabel    READ keyLabel    WRITE setKeyLabel   )
  Q_PROPERTY(bool          exploded    READ isExploded  WRITE setExploded   )

 public:
  using PiePlot  = CQChartsPiePlot;
  using GroupObj = CQChartsPieGroupObj;
  using ArcData  = CQChartsArcData;
  using Angle    = CQChartsAngle;
  using Color    = CQChartsColor;
  using Column   = CQChartsColumn;
  using OptReal  = boost::optional<double>;

 public:
  CQChartsPieObj(const PiePlot *plot, const BBox &rect, const QModelIndex &ind,
                 const ColorInd &ig);

  //---

  QString typeName() const override { return "pie"; }

  //---

  bool isArc() const override { return true; }
  ArcData arcData() const override;

  //---

  QString calcId() const override;

  QString calcTipId() const override;

  void calcTipData(QString &groupName, QString &label, QString &valueStr) const;

  QString valueStr() const;

  //---

  //! get/set color index
  const ColorInd &colorIndex() const { return colorIndex_; }
  void setColorIndex(const ColorInd &i) { colorIndex_ = i; }

  //---

  const Angle &angle1() const { return angle1_; }
  void setAngle1(const Angle &a) { angle1_ = a; }

  const Angle &angle2() const { return angle2_; }
  void setAngle2(const Angle &a) { angle2_ = a; }

  double innerRadius() const { return ri_; }
  void setInnerRadius(double r) { ri_ = r; }

  double outerRadius() const { return ro_; }
  void setOuterRadius(double r) { ro_ = r; }

  double valueRadius() const { return rv_; }
  void setValueRadius(double r) { rv_ = r; }

  const QString &label() const { return label_; }
  void setLabel(const QString &s) { label_ = s; }

  double value() const { return value_; }
  void setValue(double r) { value_ = r; }

  bool isMissing() const { return missing_; }
  void setMissing(bool b) { missing_ = b; }

  const OptReal &optRadius() const { return radius_; }
  void setOptRadius(const OptReal &r) { radius_ = r; }

  double radiusScale() const { return radiusScale_; }
  void setRadiusScale(double r) { radiusScale_ = r; }

  const QString &keyLabel() const { return keyLabel_; }
  void setKeyLabel(const QString &s) { keyLabel_ = s; }

  const Color &color() const { return color_; }
  void setColor(const Color &c) { color_ = c; }

  GroupObj *groupObj() const { return groupObj_; }
  void setGroupObj(GroupObj *p) { groupObj_ = p; }

  //---

  bool isExploded() const { return exploded_; }
  void setExploded(bool b) { exploded_ = b; }

  bool calcExploded() const;

  //---

  const BBox &treeMapBBox() const { return treeMapBBox_; }
  void setTreeMapBBox(const BBox &b) { treeMapBBox_ = b; }

  //---

  int waffleStart() const { return waffleStart_; }
  void setWaffleStart(int i) { waffleStart_ = i; }

  int waffleCount() const { return waffleCount_; }
  void setWaffleCount(int n) { waffleCount_ = n; }

  //---

  void addProperties(CQPropertyViewModel *model, const QString &path) override;

  //---

  bool inside(const Point &p) const override;

  void getObjSelectIndices(Indices &inds) const override;

  BBox extraFitBBox() const;

  //---

  void draw(PaintDevice *device) const override;

  void drawSegment(PaintDevice *device) const;
  void drawTreeMap(PaintDevice *device) const;
  void drawWaffle (PaintDevice *device) const;

  void drawFg(PaintDevice *device) const override;

  void drawSegmentLabel(PaintDevice *device) const;
  void drawTreeMapLabel(PaintDevice *device) const;
  void drawWaffleLabel (PaintDevice *device) const;

  //---

  void calcPenBrush(PenBrush &penBrush, bool updateState, bool inside) const;

  void getDrawLabels(QStringList &labels) const;

  void getRadii(double &ri, double &ro, double &rv, bool scaled=true) const;

  BBox getBBox() const;

  BBox calcTreeMapBBox() const;

  //---

  void writeScriptData(ScriptPaintDevice *device) const override;

  QColor fillColor() const;

  //---

  Point getAdjustedCenter() const;

  Point calcCenter() const;

  //---

  double xColorValue(bool relative) const override;
  double yColorValue(bool relative) const override;

 protected:
  const PiePlot* plot_       { nullptr };  //!< parent plot
  ColorInd       colorIndex_;              //!< color index
  Angle          angle1_     { 0.0 };      //!< wedge start angle
  Angle          angle2_     { 360.0 };    //!< wedge end angle
  double         ri_         { 0.0 };      //!< inner radius
  double         ro_         { 0.0 };      //!< outer radius
  double         rv_         { 0.0 };      //!< value radius
  QString        label_      { "" };       //!< label
  double         value_      { 0 };        //!< value
  bool           missing_    { false };    //!< value missing
  OptReal        radius_;                  //!< optional radius value
  double         radiusScale_ { 1.0 };     //!< radius scale
  QString        keyLabel_    { "" };      //!< key label
  Color          color_;                   //!< calculated color
  GroupObj*      groupObj_    { nullptr }; //!< parent group object
  bool           exploded_    { false };   //!< exploded
  BBox           treeMapBBox_;             //!< tree map bbox
  int            waffleStart_ { 0 };       //!< waffle start box
  int            waffleCount_ { 0 };       //!< waffle number of boxes
  mutable BBox   waffleBBox_;              //!< waffle bbox
};

//---

/*!
 * \brief Pie Plot Group object
 * \ingroup Charts
 */
class CQChartsPieGroupObj : public CQChartsGroupObj {
 public:
  using PiePlot = CQChartsPiePlot;
  using PieObj  = CQChartsPieObj;
  using PieObjs = std::vector<PieObj *>;
  using Angle   = CQChartsAngle;

 public:
  CQChartsPieGroupObj(const PiePlot *plot, const BBox &bbox, const ColorInd &groupInd,
                      const QString &name, const ColorInd &ig);

  const PiePlot *plot() const { return plot_; }

  //---

  const ColorInd &groupInd() const { return groupInd_; }
  void setGroupInd(const ColorInd &i) { groupInd_ = i; }

  const QString &name() const { return name_; }
  void setName(const QString &s) { name_ = s; }

  //---

  //! get/set color index
  const ColorInd &colorIndex() const { return colorIndex_; }
  void setColorIndex(const ColorInd &i) { colorIndex_ = i; }

  //---

  double dataTotal() const { return dataTotal_; }
  void setDataTotal(double r) { dataTotal_ = r; }

  int numValues() const { return numValues_; }
  void setNumValues(int n) { numValues_ = n; }

  double radiusMax() const { return radiusMax_; }
  void setRadiusMax(double r) { radiusMax_ = r; }

  bool isRadiusScaled() const { return radiusScaled_; }
  void setRadiusScaled(bool b) { radiusScaled_ = b; }

  double innerRadius() const { return innerRadius_; }
  void setInnerRadius(double r) { innerRadius_ = r; }

  double outerRadius() const { return outerRadius_; }
  void setOuterRadius(double r) { outerRadius_ = r; }

  //---

  const Point &center() const { return center_; }
  void setCenter(const Point &c) { center_ = c; }

  Point calcCenter() const;

  //---

  void addObject(PieObj *obj);

  PieObj *lookupObj(const QString &name) const;

  int numObjs() const { return objs_.size(); }

  const PieObjs &objs() const { return objs_; }

  PieObj *obj(int i) const { assert(i >= 0 && i < numObjs()); return objs_[i]; }

  //---

  const Angle &startAngle() const { return startAngle_; }
  const Angle &endAngle  () const { return endAngle_; }

  void setAngles(const Angle &a1, const Angle &a2) {
    startAngle_ = a1; endAngle_ = a2;
  }

  //---

  QString typeName() const override { return "group"; }

  QString calcId() const override;

  QString calcTipId() const override;

  //---

  bool inside(const Point &p) const override;

  //---

  void draw(PaintDevice *device) const override;

  void drawDonut     (PaintDevice *device) const;
  void drawEmptyGroup(PaintDevice *device) const;
  void drawDumbbell  (PaintDevice *device) const;

  void drawFg(PaintDevice *device) const override;

  void drawDonutText(PaintDevice *device) const;

  void drawTreeMapHeader(PaintDevice *device) const;
  void drawPieBorder    (PaintDevice *device) const;

  void setPenBrush(PaintDevice *device) const;

  void calcPenBrush(PenBrush &penBrush, bool updateState) const;

  //---

  QColor bgColor() const;

  void getRadii(double &ri, double &ro) const;

  BBox getBBox() const;

 private:
  const PiePlot* plot_         { nullptr };  //!< parent plot
  ColorInd       groupInd_;                  //!< group index
  QString        name_;                      //!< group name
  ColorInd       colorIndex_;                //!< color index
  double         dataTotal_    { 0.0 };      //!< value data total
  int            numValues_    { 0 };        //!< num values
  double         radiusMax_    { 0.0 };      //!< radius data max
  bool           radiusScaled_ { false };    //!< radius scaled
  double         innerRadius_  { 0.0 };      //!< inner radius
  double         outerRadius_  { 0.0 };      //!< outer radius
  Point          center_       { 0.0, 0.0 }; //!< center
  PieObjs        objs_;                      //!< objects
  Angle          startAngle_   { 0.0 };      //!< start angle
  Angle          endAngle_     { 0.0 };      //!< end angle
};

//---

#include <CQChartsKey.h>

/*!
 * \brief Pie Plot Key Color Box
 * \ingroup Charts
 */
class CQChartsPieKeyColor : public CQChartsKeyColorBox {
  Q_OBJECT

 public:
  using PiePlot = CQChartsPiePlot;
  using PlotObj = CQChartsPlotObj;
  using SelMod  = CQChartsSelMod;

 public:
  CQChartsPieKeyColor(PiePlot *plot, PlotObj *obj);

  Plot *plot() const { return plot_; }

  void doSelect(SelMod selMod) override;

#if 0
  // handle select press
  bool selectPress(const Point &p, SelMod selMod) override;
#endif

  QBrush fillBrush() const override;

  ColorInd setIndex() const override;

 private:
  PlotObj* obj_ { nullptr };
};

/*!
 * \brief Pie Plot Key Text
 * \ingroup Charts
 */
class CQChartsPieKeyText : public CQChartsKeyText {
  Q_OBJECT

 public:
  using PiePlot = CQChartsPiePlot;
  using PlotObj = CQChartsPlotObj;

 public:
  CQChartsPieKeyText(PiePlot *plot, PlotObj *obj);

  QColor interpTextColor(const ColorInd &ind) const override;

  ColorInd setIndex() const override;

 private:
  PlotObj* obj_ { nullptr };
};

//---

CQCHARTS_NAMED_SHAPE_DATA(Group, group)

/*!
 * \brief Pie Chart Plot
 * \ingroup Charts
 */
class CQChartsPiePlot : public CQChartsGroupPlot,
 public CQChartsObjShapeData     <CQChartsPiePlot>,
 public CQChartsObjGridLineData  <CQChartsPiePlot>,
 public CQChartsObjGroupShapeData<CQChartsPiePlot> {
  Q_OBJECT

  // columns
  Q_PROPERTY(CQChartsColumn  labelColumn    READ labelColumn    WRITE setLabelColumn   )
  Q_PROPERTY(CQChartsColumns valueColumns   READ valueColumns   WRITE setValueColumns  )
  Q_PROPERTY(CQChartsColumn  radiusColumn   READ radiusColumn   WRITE setRadiusColumn  )
  Q_PROPERTY(CQChartsColumn  keyLabelColumn READ keyLabelColumn WRITE setKeyLabelColumn)

  // options
  // . separated, donut, treemap, summary, count
  Q_PROPERTY(DrawType drawType  READ drawType    WRITE setDrawType )
  Q_PROPERTY(bool     separated READ isSeparated WRITE setSeparated)
  Q_PROPERTY(bool     donut     READ isDonut     WRITE setDonut    )
  Q_PROPERTY(bool     summary   READ isSummary   WRITE setSummary  )
  Q_PROPERTY(bool     dumbbell  READ isDumbbell  WRITE setDumbbell )
  Q_PROPERTY(bool     count     READ isCount     WRITE setCount    )
  // . min/max value
  Q_PROPERTY(double minValue READ minValue WRITE setMinValue)
  Q_PROPERTY(double maxValue READ maxValue WRITE setMaxValue)
  // . inner radius, outer radius, label radius, start angle, end angle
  Q_PROPERTY(double        innerRadius READ innerRadius WRITE setInnerRadius)
  Q_PROPERTY(double        outerRadius READ outerRadius WRITE setOuterRadius)
  Q_PROPERTY(double        labelRadius READ labelRadius WRITE setLabelRadius)
  Q_PROPERTY(CQChartsAngle startAngle  READ startAngle  WRITE setStartAngle )
  Q_PROPERTY(CQChartsAngle angleExtent READ angleExtent WRITE setAngleExtent)
  Q_PROPERTY(CQChartsAngle gapAngle    READ gapAngle    WRITE setGapAngle   )
  // . text placement
  Q_PROPERTY(bool adjustText READ isAdjustText WRITE setAdjustText)

  // text
  Q_PROPERTY(bool textVisible READ isTextVisible WRITE setTextVisible)
  Q_PROPERTY(bool rotatedText READ isRotatedText WRITE setRotatedText)

  // explode
  Q_PROPERTY(ExplodeStyle explodeStyle    READ explodeStyle      WRITE setExplodeStyle   )
  Q_PROPERTY(bool         explodeSelected READ isExplodeSelected WRITE setExplodeSelected)
  Q_PROPERTY(double       explodeRadius   READ explodeRadius     WRITE setExplodeRadius  )

  // shape
  CQCHARTS_SHAPE_DATA_PROPERTIES

  // group shape
  CQCHARTS_NAMED_SHAPE_DATA_PROPERTIES(Group, group)

  // grid
  CQCHARTS_NAMED_LINE_DATA_PROPERTIES(Grid, grid)

  Q_ENUMS(DrawType)
  Q_ENUMS(ExplodeStyle)

 public:
  enum class DrawType {
    NONE,
    PIE,
    TREEMAP,
    WAFFLE
  };

  enum class ExplodeStyle {
    OUTSET,
    EDGE
  };

  using PieTextObj  = CQChartsPieTextObj;
  using PieGroupObj = CQChartsPieGroupObj;
  using PieObj      = CQChartsPieObj;
  using Color       = CQChartsColor;
  using ColorInd    = CQChartsUtil::ColorInd;

 public:
  CQChartsPiePlot(View *view, const ModelP &model);
 ~CQChartsPiePlot();

  //---

  void init() override;
  void term() override;

  //---

  const Column &labelColumn() const { return labelColumn_; }
  void setLabelColumn(const Column &c);

  const Columns &valueColumns() const { return valueColumns_; }
  void setValueColumns(const Columns &c);

  //---

  const Column &radiusColumn() const { return radiusColumn_; }
  void setRadiusColumn(const Column &c);

  const Column &keyLabelColumn() const { return keyLabelColumn_; }
  void setKeyLabelColumn(const Column &c);

  //---

  Column getNamedColumn(const QString &name) const override;
  void setNamedColumn(const QString &name, const Column &c) override;

  Columns getNamedColumns(const QString &name) const override;
  void setNamedColumns(const QString &name, const Columns &c) override;

  //---

  const DrawType &drawType() const { return drawType_; }
  void setDrawType(const DrawType &t);

  bool isTreeMap() const { return (drawType() == DrawType::TREEMAP); }
  bool calcTreeMap() const;

  bool isWaffle() const { return (drawType() == DrawType::WAFFLE); }
  bool calcWaffle() const;

  bool calcPie() const;

  //---

  bool isDonut() const { return donut_; }
  bool calcDonut() const;

  bool isSummary() const { return summary_; }

  bool isDumbbell() const { return dumbbell_; }
  bool calcDumbbell() const;

  bool isCount() const { return count_; }

  //---

  double minValue() const { return minValue_; }
  void setMinValue(double r);

  double maxValue() const { return maxValue_; }
  void setMaxValue(double r);

  //---

  double innerRadius() const { return innerRadius_; }
  void setInnerRadius(double r);

  double outerRadius() const { return outerRadius_; }
  void setOuterRadius(double r);

  double labelRadius() const { return labelRadius_; }
  void setLabelRadius(double r);

  const Angle &startAngle() const { return startAngle_; }
  void setStartAngle(const Angle &a);

  const Angle &angleExtent() const { return angleExtent_; }
  void setAngleExtent(const Angle &a);

  const Angle &gapAngle() const { return gapAngle_; }
  void setGapAngle(const Angle &a);

  //---

  //! get/set adjust text
  bool isAdjustText() const { return adjustText_; }
  void setAdjustText(bool b);

  //---

  bool isSeparated() const { return separated_; }
  void setSeparated(bool b);

  bool calcSeparated() const;

  //---

  bool isTextVisible() const;
  void setTextVisible(bool b);

  //---

  bool isRotatedText() const { return rotatedText_; }
  void setRotatedText(bool b);

  //---

  const ExplodeStyle &explodeStyle() const { return explodeData_.style; }
  void setExplodeStyle(const ExplodeStyle &v) { explodeData_.style = v; }

  bool isExplodeSelected() const { return explodeData_.selected; }
  void setExplodeSelected(bool b);

  double explodeRadius() const { return explodeData_.radius; }
  void setExplodeRadius(double r);

  //---

  double insideOffset() const { return insideData_.offset; }
  double insideRadius() const { return insideData_.radius; }

  //---

  PieTextObj *textBox() const { return textBox_; }

  //---

  void addProperties() override;

  Range calcRange() const override;

  BBox calcExtraFitBBox() const override;

  bool createObjs(PlotObjs &objs) const override;

  //---

  void adjustObjAngles() const;

  //---

  bool isIndexHidden(const ModelIndex &ind) const;

  //---

  void addKeyItems(PlotKey *key) override;

  bool addMenuItems(QMenu *menu) override;

  //---

  int numGroupObjs() const { return groupObjs_.size(); }

  //---

  QString calcIndLabel(const QModelIndex &ind) const;

  //---

  double calcMinValue() const;
  double calcMaxValue() const;

  //---

  void postResize() override;

  //---

  bool hasForeground() const override;

  void execDrawForeground(PaintDevice *) const override;

  //---

  void getGroupObjs(int ig, PlotObjs &objs) const;

  bool selectInvalidateObjs() const override { return true; }

  //---

  void write(std::ostream &os, const QString &plotVarName, const QString &modelVarName,
             const QString &viewVarName) const override;

  //---

  virtual PieTextObj *createTextObj() const;

  virtual PieGroupObj *createGroupObj(const BBox &bbox, const ColorInd &groupInd,
                                      const QString &name, const ColorInd &ig) const;

  virtual PieObj *createPieObj(const BBox &rect, const QModelIndex &ind,
                               const ColorInd &ig) const;

 public slots:
  void setDonut   (bool b);
  void setTreeMap (bool b);
  void setWaffle  (bool b);
  void setSummary (bool b);
  void setDumbbell(bool b);
  void setCount   (bool b);

 private:
  void addRow(const ModelVisitor::VisitData &data, PlotObjs &objs) const;

  void addRowColumn(const ModelIndex &ind, PlotObjs &objs) const;

  void calcDataTotal() const;

  void addRowDataTotal(const ModelVisitor::VisitData &data) const;

  void addRowColumnDataTotal(const ModelIndex &ind) const;

  bool getColumnSizeValue(const ModelIndex &ind, double &value, bool &missing) const;

 protected:
  CQChartsPlotCustomControls *createCustomControls() override;

 private:
  struct GroupData {
    GroupData(const QString &name) :
     name(name) {
    }

    QString      name;                     //!< name
    double       dataTotal    { 0.0 };     //!< data column value total
    int          numValues    { 0 };       //!< data column num values
    double       radiusMax    { 0.0 };     //!< radius column value max
    bool         radiusScaled { false };   //!< has radius column value max
    PieGroupObj* groupObj     { nullptr }; //!< associated group obj
  };

  using GroupInd   = std::map<QString, int>;
  using GroupDatas = std::map<int, GroupData>;
  using GroupObjs  = std::vector<PieGroupObj *>;

  struct ExplodeData {
    ExplodeStyle style    { ExplodeStyle::OUTSET }; //!< explode style
    bool         selected { true };                 //!< explode selected pie
    double       radius   { 0.05 };                 //!< expose radius
  };

  struct InsideData {
    double offset { 0.02 };
    double radius { 0.03 };
  };

  // columns
  Column  labelColumn_;    //!< label column
  Columns valueColumns_;   //!< value columns
  Column  radiusColumn_;   //!< radius value column
  Column  keyLabelColumn_; //!< key label column

  DrawType drawType_    { DrawType::PIE }; //!< draw type
  bool     donut_       { false };         //!< show donut
  bool     summary_     { false };         //!< show summary
  bool     dumbbell_    { false };         //!< show dumbbell
  bool     count_       { false };         //!< show value counts
  double   minValue_    { -1.0 };          //!< min value
  double   maxValue_    { -1.0 };          //!< max value
  double   innerRadius_ { 0.3 };           //!< relative inner donut radius
  double   outerRadius_ { 0.9 };           //!< relative outer donut radius
  double   labelRadius_ { 1.1 };           //!< label radius
  Angle    startAngle_  { 90 };            //!< first pie start angle
  Angle    angleExtent_ { 360.0 };         //!< pie angle extent
  Angle    gapAngle_    { 0.0 };           //!< angle gap between segments
  bool     adjustText_  { false };         //!< adjust text position
  bool     separated_   { true };          //!< are grouped pie objects drawn separately
  bool     rotatedText_ { false };         //!< is label rotated

  ExplodeData     explodeData_;                 //!< explode data
  InsideData      insideData_;                  //!< inside data
  PieTextObj*     textBox_         { nullptr }; //!< text box
  Point           center_;                      //!< center point
  GroupDatas      groupDatas_;                  //!< data per group
  GroupObjs       groupObjs_;                   //!< group objects
  CQChartsRValues values_;                      //!< all values
};

//---

#include <CQChartsGroupPlotCustomControls.h>

class CQEnumCombo;

class CQChartsPiePlotCustomControls : public CQChartsGroupPlotCustomControls {
  Q_OBJECT

 public:
  using PiePlot = CQChartsPiePlot;

 public:
  CQChartsPiePlotCustomControls(CQCharts *charts);

  void setPlot(CQChartsPlot *plot) override;

 private:
  void connectSlots(bool b);

 public slots:
  void updateWidgets() override;

 private slots:
  void drawTypeSlot ();
  void separatedSlot();
  void donutSlot    ();
  void summarySlot  ();
  void dumbbellSlot ();
  void countSlot    ();

 private:
  CQChartsColor getColorValue() override;
  void setColorValue(const CQChartsColor &c) override;

 private:
  PiePlot*     plot_           { nullptr };
  CQEnumCombo* drawTypeCombo_  { nullptr };
  QCheckBox*   separatedCheck_ { nullptr };
  QCheckBox*   donutCheck_     { nullptr };
  QCheckBox*   summaryCheck_   { nullptr };
  QCheckBox*   dumbbellCheck_  { nullptr };
  QCheckBox*   countCheck_     { nullptr };
};

#endif
