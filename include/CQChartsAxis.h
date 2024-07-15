#ifndef CQChartsAxis_H
#define CQChartsAxis_H

#include <CQChartsObj.h>
#include <CQChartsObjData.h>
#include <CQChartsColumn.h>
#include <CQChartsGeom.h>
#include <CQChartsUtil.h>
#include <CQChartsOptInt.h>
#include <CQChartsOptReal.h>
#include <CQChartsOptString.h>
#include <CQChartsAxisTickLabelPlacement.h>
#include <CQChartsTextPlacer.h>
#include <CInterval.h>
#include <CSafeIndex.h>

#include <QPointer>

#include <map>
#include <set>
#include <vector>
#include <string>
#include <sys/types.h>

class CQChartsAxis;
class CQChartsPlot;
class CQChartsView;
class CQChartsEditHandles;
class CQCharts;

class CQPropertyViewModel;
class CQChartsPaintDevice;

CQCHARTS_NAMED_LINE_DATA(Axes, axes)
CQCHARTS_NAMED_TEXT_DATA(AxesTickLabel, axesTickLabel)
CQCHARTS_NAMED_TEXT_DATA(AxesLabel, axesLabel)
CQCHARTS_NAMED_LINE_DATA(AxesMajorGrid, axesMajorGrid)
CQCHARTS_NAMED_LINE_DATA(AxesMinorGrid, axesMinorGrid)
CQCHARTS_NAMED_FILL_DATA(AxesGrid, axesGrid)

/*!
 * \brief Axis Data
 * \ingroup Charts
 */
class CQChartsAxis : public CQChartsObj, public CQChartsEditableIFace,
 public CQChartsObjAxesLineData         <CQChartsAxis>,
 public CQChartsObjAxesTickLabelTextData<CQChartsAxis>,
 public CQChartsObjAxesLabelTextData    <CQChartsAxis>,
 public CQChartsObjAxesMajorGridLineData<CQChartsAxis>,
 public CQChartsObjAxesMinorGridLineData<CQChartsAxis>,
 public CQChartsObjAxesGridFillData     <CQChartsAxis> {
  Q_OBJECT

  // general
  Q_PROPERTY(Qt::Orientation direction READ direction WRITE setDirection)

  // options
  Q_PROPERTY(CQChartsAxisSide side     READ side     WRITE setSide    )
  Q_PROPERTY(CQChartsOptReal  position READ position WRITE setPosition)

  Q_PROPERTY(bool requireTickLabel READ isRequireTickLabel WRITE setRequireTickLabel)

  Q_PROPERTY(CQChartsAxisValueType valueType READ valueType WRITE setValueType)

  Q_PROPERTY(QString format       READ format       WRITE setFormat      )
  Q_PROPERTY(double  maxFitExtent READ maxFitExtent WRITE setMaxFitExtent)

  // range
  Q_PROPERTY(double       start       READ start         WRITE setStart      )
  Q_PROPERTY(double       end         READ end           WRITE setEnd        )
  Q_PROPERTY(bool         includeZero READ isIncludeZero WRITE setIncludeZero)
  Q_PROPERTY(CQChartsPath path        READ path          WRITE setPath       )

  // tick calc
  Q_PROPERTY(CQChartsOptReal majorIncrement READ majorIncrement WRITE setMajorIncrement)
  Q_PROPERTY(CQChartsOptInt  tickIncrement  READ tickIncrement  WRITE setTickIncrement )

  Q_PROPERTY(bool annotation      READ isAnnotation      WRITE setAnnotation     )
  Q_PROPERTY(bool allowHtmlLabels READ isAllowHtmlLabels WRITE setAllowHtmlLabels)

  Q_PROPERTY(CQChartsOptReal valueStart READ valueStart WRITE setValueStart)
  Q_PROPERTY(CQChartsOptReal valueEnd   READ valueEnd   WRITE setValueEnd  )

  // grid
  Q_PROPERTY(CQChartsOptReal gridStart READ gridStart WRITE setGridStart)
  Q_PROPERTY(CQChartsOptReal gridEnd   READ gridEnd   WRITE setGridEnd  )

  // line
  CQCHARTS_NAMED_LINE_DATA_PROPERTIES(Axes, axes)

  // ticks
  Q_PROPERTY(TicksDisplayed ticksDisplayed READ ticksDisplayed WRITE setTicksDisplayed)
  Q_PROPERTY(int            majorTickLen   READ majorTickLen   WRITE setMajorTickLen  )
  Q_PROPERTY(int            minorTickLen   READ minorTickLen   WRITE setMinorTickLen  )
  Q_PROPERTY(bool           tickInside     READ isTickInside   WRITE setTickInside    )
  Q_PROPERTY(bool           mirrorTicks    READ isMirrorTicks  WRITE setMirrorTicks   )

  // ticks label
  Q_PROPERTY(bool                           tickLabelAutoHide
             READ isTickLabelAutoHide WRITE setTickLabelAutoHide )
  Q_PROPERTY(bool                           tickLabelPerpPath
             READ isTickLabelPerpPath WRITE setTickLabelPerpPath )
  Q_PROPERTY(CQChartsAxisTickLabelPlacement tickLabelPlacement
             READ tickLabelPlacement  WRITE setTickLabelPlacement)

  CQCHARTS_NAMED_TEXT_DATA_PROPERTIES(AxesTickLabel, axesTickLabel)

  Q_PROPERTY(QString tickLabels READ tickLabelsStr WRITE setTickLabelsStr)

  Q_PROPERTY(QString customTickLabels READ customTickLabelsStr WRITE setCustomTickLabelsStr)

  // label
  Q_PROPERTY(CQChartsOptString label             READ label             WRITE setLabel            )
  Q_PROPERTY(QString           labelStr          READ labelStr          WRITE setLabelStr         )
  Q_PROPERTY(QString           defLabel          READ defLabel          WRITE setDefLabel         )
  Q_PROPERTY(QString           userLabel         READ userLabel         WRITE setUserLabel        )
  Q_PROPERTY(bool              scaleLabelFont    READ isScaleLabelFont  WRITE setScaleLabelFont   )
  Q_PROPERTY(CQChartsLength    scaleLabelExtent  READ scaleLabelExtent  WRITE setScaleLabelExtent )
  Q_PROPERTY(double            labelPosition     READ labelPosition     WRITE setLabelPosition    )
  Q_PROPERTY(double            labelPerpPosition READ labelPerpPosition WRITE setLabelPerpPosition)
  Q_PROPERTY(Qt::Alignment     labelAlign        READ labelAlign        WRITE setLabelAlign       )

  CQCHARTS_NAMED_TEXT_DATA_PROPERTIES(AxesLabel, axesLabel)

  // grid line/fill
  Q_PROPERTY(GridLinesDisplayed gridLinesDisplayed
             READ gridLinesDisplayed WRITE setGridLinesDisplayed)
  Q_PROPERTY(GridFillDisplayed  gridFillDisplayed
             READ gridFillDisplayed  WRITE setGridFillDisplayed )

  Q_PROPERTY(bool gridMid   READ isGridMid   WRITE setGridMid  )
  Q_PROPERTY(bool gridAbove READ isGridAbove WRITE setGridAbove)

  Q_PROPERTY(CQChartsColumn column READ column WRITE setColumn)

  //---

  CQCHARTS_NAMED_LINE_DATA_PROPERTIES(AxesMajorGrid, axesMajorGrid)
  CQCHARTS_NAMED_LINE_DATA_PROPERTIES(AxesMinorGrid, axesMinorGrid)
  CQCHARTS_NAMED_FILL_DATA_PROPERTIES(AxesGrid, axesGrid)

  Q_ENUMS(TicksDisplayed)
  Q_ENUMS(GridLinesDisplayed)
  Q_ENUMS(GridFillDisplayed)

 public:
  enum class TicksDisplayed {
    NONE,
    MAJOR,
    MAJOR_AND_MINOR
  };

  enum class GridLinesDisplayed {
    NONE,
    MAJOR,
    MAJOR_AND_MINOR
  };

  enum class GridFillDisplayed {
    NONE,
    MAJOR
  };

  enum class PropertyType {
    STATE      = (1<<0),
    STROKE     = (1<<1),
    ANNOTATION = (1<<2),

    NONE  = 0,
    BASIC = 0,
    ALL   = STATE | STROKE
  };

  using Plot                   = CQChartsPlot;
  using PlotP                  = QPointer<Plot>;
  using View                   = CQChartsView;
  using ViewP                  = QPointer<View>;
  using Axis                   = CQChartsAxis;
  using AxisValueType          = CQChartsAxisValueType;
  using AxisSide               = CQChartsAxisSide;
  using AxisTickLabelPlacement = CQChartsAxisTickLabelPlacement;
  using ColorInd               = CQChartsUtil::ColorInd;
  using EditHandles            = CQChartsEditHandles;
  using OptReal                = CQChartsOptReal;
  using OptInt                 = CQChartsOptInt;
  using OptString              = CQChartsOptString;
  using PaintDevice            = CQChartsPaintDevice;
  using TextOptions            = CQChartsTextOptions;
  using Column                 = CQChartsColumn;
  using Angle                  = CQChartsAngle;
  using Alpha                  = CQChartsAlpha;
  using PenBrush               = CQChartsPenBrush;
  using PenData                = CQChartsPenData;
  using BrushData              = CQChartsBrushData;
  using Length                 = CQChartsLength;
  using LineDash               = CQChartsLineDash;
  using Font                   = CQChartsFont;
  using Color                  = CQChartsColor;
  using Path                   = CQChartsPath;
  using BBox                   = CQChartsGeom::BBox;
  using Point                  = CQChartsGeom::Point;

 public:
  CQChartsAxis(const View *view, Qt::Orientation direction=Qt::Horizontal,
               double start=0.0, double end=1.0);
  CQChartsAxis(const Plot *plot, Qt::Orientation direction=Qt::Horizontal,
               double start=0.0, double end=1.0);

  virtual ~CQChartsAxis();

  //---

  static void swap(Axis *lhs, Axis *rhs);

  //---

  CQCharts *charts() const;

  QString calcId() const override;

  //---

  //! get/set plot
  Plot *plot() const;
  void setPlot(const Plot *plot);

  //! get/set view
  View *view() const;
  void setView(const View *view);

  //---

  //! get/set direction (TODO: allow set direction)
  Qt::Orientation direction() const { return direction_; }
  void setDirection(Qt::Orientation dir);

  bool isHorizontal() const { return direction() == Qt::Horizontal; }

  //---

  //! get/set visible
  void setVisible(bool b, bool notify=true) override;

  //---

  //! get/set updates enabled
  bool isUpdatesEnabled() const { return updatesEnabled_; }
  void setUpdatesEnabled(bool b) { updatesEnabled_ = b; }

  //---

  void setSelected(bool b) override;

  //---

  //! get/set side (horizontal: top/bottom, vertical bottom/top)
  AxisSide side() const { return data_.side; }
  void setSide(AxisSide side);

  //---

  //! get/set position (x for vertical, y for horzontal)
  const OptReal &position() const { return data_.position; }
  void setPosition(const OptReal &r);

  //---

  //! get/set start position
  double start() const { return data_.start; }
  void setStart(double start) { setRange(start, end()); }

  //! get/set end position
  double end() const { return data_.end; }
  void setEnd(double end) { setRange(start(), end); }

  //! get/set start and end position
  void setRange(double start, double end);

  //---

  const Path &path() const { return data_.path; }
  void setPath(const Path &p) { data_.path = p; }

  //! get/set include zero
  bool isIncludeZero() const { return data_.includeZero; }
  void setIncludeZero(bool b);

  //---

  //! get/set custom value start
  const OptReal &valueStart() const { return valueStart_; }
  void setValueStart(const OptReal &r);

  //! get/set custom value end
  const OptReal &valueEnd() const { return valueEnd_; }
  void setValueEnd(const OptReal &r);

  //! set custom start and end value
  void setValueRange(double start, double end);

  //---

  //! get/set custom value start
  const OptReal &gridStart() const { return gridStart_; }
  void setGridStart(const OptReal &r);

  //! get/set custom value end
  const OptReal &gridEnd() const { return gridEnd_; }
  void setGridEnd(const OptReal &r);

  //---

  //! get/set is annotation
  bool isAnnotation() const { return annotation_; }
  void setAnnotation(bool b);

  //! get/set allow html labels
  bool isAllowHtmlLabels() const { return data_.allowHtmlLabels; }
  void setAllowHtmlLabels(bool b);

  //---

  //! get/set value type
  const AxisValueType &valueType() const { return data_.valueType; }
  void setValueType(const AxisValueType &t, bool notify=true);

  bool isIntegral() const { return (valueType().type() == AxisValueType::Type::INTEGER); }
  bool isDate    () const { return (valueType().type() == AxisValueType::Type::DATE); }
  bool isLog     () const { return (valueType().type() == AxisValueType::Type::LOG); }

  //---

  //! get/set associated column
  const Column &column() const { return data_.column; }
  void setColumn(const Column &c);

  //! get/set if labels are from column
  bool isDataLabels() const { return data_.dataLabels; }
  void setDataLabels(bool b);

  //! get/set value format
  QString format() const;
  bool setFormat(const QString &s);

  //! get/set maximum fit extent (outside axis line length)
  double maxFitExtent() const { return data_.maxFitExtent; }
  void setMaxFitExtent(double r);

  //---

  //! get/set label
  const OptString &label() const { return data_.label; }
  void setLabel(const OptString &str);

  //! get/set label string
  QString labelStr() const { return label().stringOr(); }
  void setLabelStr(const QString &s);

  //! set default label
  const QString &defLabel() const { return label().defValue(); }
  void setDefLabel(const QString &str, bool notify=true);

  //! get/set user label
  const QString &userLabel() const { return data_.userLabel; }
  void setUserLabel(const QString &str);

  //! get/set scale label font
  bool isScaleLabelFont() const { return scaleLabelFont_; }
  void setScaleLabelFont(bool b);

  //! get/set scale label extent
  const Length &scaleLabelExtent() const { return scaleLabelExtent_; }
  void setScaleLabelExtent(const Length &l);

  double labelPosition() const { return labelPosition_; }
  void setLabelPosition(double r);

  double labelPerpPosition() const { return labelPerpPosition_; }
  void setLabelPerpPosition(double r);

  const Qt::Alignment &labelAlign() const { return labelAlign_; }
  void setLabelAlign(const Qt::Alignment &a);

  //---

  //! get/set grid lines displayed
  const GridLinesDisplayed &gridLinesDisplayed() const { return data_.gridLinesDisplayed; }
  void setGridLinesDisplayed(const GridLinesDisplayed &d);

  //! get/set major grid lines displayed
  bool isMajorGridLinesDisplayed() const {
    return gridLinesDisplayed() == GridLinesDisplayed::MAJOR ||
           gridLinesDisplayed() == GridLinesDisplayed::MAJOR_AND_MINOR; }
  bool isMinorGridLinesDisplayed() const {
    return gridLinesDisplayed() == GridLinesDisplayed::MAJOR_AND_MINOR; }

  //! get/set grid fill displayed
  const GridFillDisplayed &gridFillDisplayed() const { return data_.gridFillDisplayed; }
  void setGridFillDisplayed(const GridFillDisplayed &d);

  bool isMajorGridFilled() const { return gridFillDisplayed() == GridFillDisplayed::MAJOR; }

  //---

  //! get/set grid mid line
  bool isGridMid() const { return data_.gridMid; }
  void setGridMid(bool b);

  //! get/set grid above
  bool isGridAbove() const { return data_.gridAbove; }
  void setGridAbove(bool b);

  //---

  //! get/set ticks displayed
  const TicksDisplayed &ticksDisplayed() const { return data_.ticksDisplayed; }
  void setTicksDisplayed(const TicksDisplayed &d);

  //! get/set major ticks displayed
  bool isMajorTicksDisplayed() const {
    return ticksDisplayed() == TicksDisplayed::MAJOR ||
           ticksDisplayed() == TicksDisplayed::MAJOR_AND_MINOR; }
  bool isMinorTicksDisplayed() const {
    return ticksDisplayed() == TicksDisplayed::MAJOR_AND_MINOR; }

  //---

  //! get/set major tick (pixel) length
  int majorTickLen() const { return data_.majorTickLen; }
  void setMajorTickLen(int i);

  //! get/set minor tick (pixel) length
  int minorTickLen() const { return data_.minorTickLen; }
  void setMinorTickLen(int i);

  //! get/set tick on inside
  bool isTickInside() const { return data_.tickInside; }
  void setTickInside(bool b);

  //! get/set mirror ticks
  bool isMirrorTicks() const { return data_.mirrorTicks; }
  void setMirrorTicks(bool b);

  //---

  //! get/set ticks auto hide
  bool isTickLabelAutoHide() const { return data_.tickLabelAutoHide; }
  void setTickLabelAutoHide(bool b);

  //! get/set tick label angle is perp to path
  bool isTickLabelPerpPath() const { return data_.tickLabelPerpPath; }
  void setTickLabelPerpPath(bool b);

  //! get/set ticks label placement
  const AxisTickLabelPlacement &tickLabelPlacement() const { return data_.tickLabelPlacement; }
  void setTickLabelPlacement(const AxisTickLabelPlacement &p);

  //---

  uint maxMajorTicks() const { return data_.maxMajorTicks; }

  double minorIncrement() const;

  //---

  //! get/set user specified tick increment
  const OptInt &tickIncrement() const { return data_.tickIncrement; }
  void setTickIncrement(const OptInt &tickIncrement);

  void setTickIncrement(int i) { setTickIncrement(OptInt(i)); }

  //---

  //! get/set user specified major increment
  const OptReal &majorIncrement() const { return data_.majorIncrement; }
  void setMajorIncrement(const OptReal &r);

  void setMajorIncrement(double r) { setMajorIncrement(OptReal(r)); }

  //---

  // internal calculation data
  uint numMajorTicks() const { updateCalc(); return numMajorTicks_; }
  uint numMinorTicks() const { updateCalc(); return numMinorTicks_; }

  double calcStart    () const { updateCalc(); return calcStart_    ; }
  double calcEnd      () const { updateCalc(); return calcEnd_      ; }
  double calcIncrement() const { updateCalc(); return calcIncrement_; }

  void updateCalc() const;

  //---

  double majorTickIncrement() const;
  double minorTickIncrement() const;

  //---

  // used ?
  const double *tickSpaces() const { return &data_.tickSpaces[0]; }
  uint          numTickSpaces() const { return uint(data_.tickSpaces.size()); }

  double tickSpace(int i) const { return CUtil::safeIndex(data_.tickSpaces, i); }
  void setTickSpaces(double *tickSpaces, uint numTickSpaces);

  //---

  QString tickLabelsStr() const;
  void setTickLabelsStr(const QString &str);

  void clearTickLabels();

  void setTickLabel(long i, const QString &label);

  bool hasTickLabel(long i) const;

  const QString &tickLabel(long i) const;

  bool isRequireTickLabel() const { return data_.requireTickLabel; }
  void setRequireTickLabel(bool b) { data_.requireTickLabel = b; }

  //--

  QString customTickLabelsStr() const;
  void setCustomTickLabelsStr(const QString &str);

  //---

  QString valueStr(double pos) const;

  QString valueStr(const Plot *plot, double pos) const;

  //--

  const BBox &bbox() const { return bbox_; }
//void setBBox(const BBox &b);

  BBox fitBBox() const;

  //--

  void addProperties(CQPropertyViewModel *model, const QString &path,
                     const PropertyType &propertyTypes=PropertyType::ALL);

  void updatePlotPosition();

  bool contains(const Point &p) const override;

  //---

  void optRedraw();

  void redraw(bool wait=true);

  void updatePlotRange();
  void updatePlotRangeAndObjs();

  //---

  // Implement edit interface
  bool editPress (const Point &) override;
  bool editMove  (const Point &) override;
  bool editMotion(const Point &) override; // return true if inside

  bool editMoveBy(const Point &) override;

  //---

  void setPen(PenBrush &penBrush, const CQChartsPenData &penData) const;

  //---

 public:
  bool isDrawGrid() const;

  void drawGrid(const Plot *plot, PaintDevice *device) const;

  //---

  void drawAt(double pos, const Plot *plot, PaintDevice *device) const;

  void draw(const View *view, PaintDevice *device, bool usePen=false, bool forceColor=false) const;
  void draw(const Plot *plot, PaintDevice *device, bool usePen=false, bool forceColor=false) const;

 private:
  void drawI(const View *view, const Plot *plot, PaintDevice *device,
             bool usePen=false, bool forceColor=false) const;

  void drawCustomTicks(const Plot *plot, PaintDevice *device, double amin, double amax,
                       double apos1, double apos2) const;
  void drawTickLabels(const Plot *plot, PaintDevice *device, double amin, double amax,
                      double apos1, double apos2) const;
  void drawPathTicks(const Plot *plot, PaintDevice *device) const;
  void drawTicks(const Plot *plot, PaintDevice *device, double amin, double amax,
                 double apos1, double apos2) const;

  //---

 public:
  //! get edit handles
  EditHandles *editHandles() const override;

  void drawEditHandles(PaintDevice *device) const override;

  void setEditHandlesBBox() const;

  //---

  void calcPos(const Plot *plot, double &apos1, double &apos2) const;

  void drawLine(const Plot *plot, PaintDevice *device, double apos, double amin, double amax) const;

  void drawMajorGridLine(const Plot *plot, PaintDevice *device, double apos,
                         double amin, double amax) const;
  void drawMinorGridLine(const Plot *plot, PaintDevice *device, double apos,
                         double amin, double amax) const;

  void drawMajorTickLine(const Plot *plot, PaintDevice *device, double apos,
                         double tpos, bool inside) const;
  void drawMinorTickLine(const Plot *plot, PaintDevice *device, double apos,
                         double tpos, bool inside) const;

  void drawTickLine(const Plot *plot, PaintDevice *device, double apos,
                    double tpos, bool inside, bool major) const;

  void drawTickLabel(const Plot *plot, PaintDevice *device, double apos, double tpos,
                     double value, bool inside) const;

  void drawAxisTickLabelDatas(const Plot *plot, PaintDevice *device) const;

  void drawAxisLabel(const Plot *plot, PaintDevice *device,
                     double apos, double amin, double amax) const;
  void drawAxisLabelI(const Plot *plot, PaintDevice *device, double apos,
                      double amin, double amax, const QString &text, bool allowHtml) const;

  void getTickLabelsPositions(std::set<int> &positions) const;

  //---

  void write(const CQPropertyViewModel *propertyModel, const QString &plotName, std::ostream &os);

 Q_SIGNALS:
  void ticksChanged();
  void tickPlacementChanged();

  void selectionChanged();

  void appearanceChanged();

  void includeZeroChanged();

 private:
  void init();

  void setConstBBox(const BBox &b) const;

 private:
  struct AxisGapData {
    double start       { 0.0 };
    double end         { 0.0 };
    double increment   { 0.0 };
    uint   numGaps     { 0 };
    uint   numGapTicks { 0 };
  };

  struct AxisTickLabelDrawData {
    AxisTickLabelDrawData(const Point &p, const BBox &bbox, const QString &text) :
     p(p), bbox(bbox), text(text) {
    }

    AxisTickLabelDrawData(const Point &p, const BBox &bbox, const QString &text,
                          const Angle &angle, const Qt::Alignment &align) :
     p(p), bbox(bbox), text(text), angle(angle), align(align) {
    }

    Point         p;
    BBox          bbox;
    QString       text;
    Angle         angle;
    Qt::Alignment align   { Qt::AlignHCenter };
    bool          visible { true };
  };

  using AxisTickLabelDrawDatas = std::vector<AxisTickLabelDrawData>;

 private:
  void calcAndRedraw();

  void calc();

  void emitSelectionChanged();

  //---

  Point windowToPixel(const Plot *plot, PaintDevice *device, double x, double y) const;

  BBox windowToPixel(const Plot *plot, PaintDevice *device, const BBox &p) const;
  Point windowToPixel(const Plot *plot, PaintDevice *device, const Point &p) const;

  double windowToPixelWidth(const Plot *plot, PaintDevice *device, double p) const;
  double windowToPixelHeight(const Plot *plot, PaintDevice *device, double p) const;

  BBox pixelToWindow(const Plot *plot, PaintDevice *device, const BBox &p) const;
  Point pixelToWindow(const Plot *plot, PaintDevice *device, const Point &p) const;

  double pixelToWindowWidth(const Plot *plot, PaintDevice *device, double p) const;
  double pixelToWindowHeight(const Plot *plot, PaintDevice *device, double p) const;

  double lengthPixelWidth(const Plot *plot, PaintDevice *device, const Length &len) const;
  double lengthPixelHeight(const Plot *plot, PaintDevice *device, const Length &len) const;

 private:
  using TickSpaces  = std::vector<double>;
  using ITickLabels = std::map<long, QString>;
  using RTickLabels = std::map<double, QString>;

  using EditHandlesP = std::unique_ptr<EditHandles>;

  using TextPlacer = CQChartsAxisTextPlacer;

  // basic state
  ViewP           view_;                         //!< parent view
  PlotP           plot_;                         //!< parent plot
  Qt::Orientation direction_ { Qt::Horizontal }; //!< direction

  //---

  struct Data {
    // general
    AxisSide      side;                     //!< axis plot placement side
    OptReal       position;                 //!< axis custom position
    AxisValueType valueType;                //!< value type
    bool          dataLabels     { false }; //!< use data for labels
    Column        column;                   //!< associated column
    QString       formatStr;                //!< value format string
    double        maxFitExtent   { 10 };    //!< max extent percent for fit

    // label
    bool      labelDisplayed   { true };  //!< show label
    OptString label;                      //!< label
    QString   userLabel;                  //!< user label

    // grid (lines and gap fill)
    GridLinesDisplayed gridLinesDisplayed { GridLinesDisplayed::NONE }; //!< grid lines displayed
    GridFillDisplayed  gridFillDisplayed  { GridFillDisplayed::NONE  }; //!< grid fill displayed

    bool gridMid   { false }; //!< show grid at mid point
    bool gridAbove { false }; //!< is grid above plot

    // ticks
    TicksDisplayed ticksDisplayed { TicksDisplayed::MAJOR_AND_MINOR }; //!< ticks displayed

    int  majorTickLen { 8 };     //!< major tick length
    int  minorTickLen { 4 };     //!< minor tick length
    bool tickInside   { false }; //!< tick inside plot
    bool mirrorTicks  { false }; //!< mirror ticks

    bool                   tickLabelAutoHide   { true }; //!< tick auto hide
    bool                   tickLabelPerpPath   { true }; //!< tick perp path
    AxisTickLabelPlacement tickLabelPlacement;           //!< tick placement

    // range
    double start       { 0.0 };   //!< axis start
    double end         { 1.0 };   //!< axis end
    Path   path;                  //!< path
    bool   includeZero { false }; //!< include zero in range

    // ticks
    uint    maxMajorTicks  { 1000 }; //!< max major ticks
    OptInt  tickIncrement;           //!< user specified tick increment
    OptReal majorIncrement;          //!< user specified major increment

    // customization (for annotations)
    bool allowHtmlLabels { false }; //!< allow html labels

    // tick customization
    TickSpaces  tickSpaces;                 //!< tick spaces (?)
    ITickLabels tickLabels;                 //!< tick labels
    bool        requireTickLabel { false }; //!< use tick label values even if empty
  };

  Data data_; //! state data

  //---

  // general
  bool updatesEnabled_ { true }; //!< axis updates enabled

  // label
  bool          scaleLabelFont_    { false };           //!< scale label font fo fit length
  Length        scaleLabelExtent_;                      //!< extent to extend length for scale label
  double        labelPosition_     { 0.5 };             //!< label position in axis range (0-1)
  double        labelPerpPosition_ { 0.0 };             //!< label perp position (TODO: range)
  Qt::Alignment labelAlign_        { Qt::AlignCenter }; //!< label align

  bool needsCalc_ { true }; //!< needs tick calc

  OptReal valueStart_; //!< custom value start
  OptReal valueEnd_;   //!< custom value end

  OptReal gridStart_; //!< custom grid start
  OptReal gridEnd_;   //!< custom grid end

  // customization (for annotations)
  bool annotation_ { false }; //!< is annotation

  // internal calculation data
  CInterval interval_;            //!< interval data
  uint      numMajorTicks_ { 1 }; //!< num major ticks
  uint      numMinorTicks_ { 0 }; //!< num minor ticks
  double    calcIncrement_ { 0 }; //!< calculated axis increment
  double    calcStart_     { 0 }; //!< calculated axis start
  double    calcEnd_       { 1 }; //!< calculated axis end

  RTickLabels customTickLabels_; //!< custom tick labels

  // edit state
  EditHandlesP editHandles_; //!< edit handles

  // bbox draw state
  mutable BBox       bbox_;              //!< axis box
  mutable BBox       fitBBox_;           //!< fit box
  mutable BBox       fitLBBox_;          //!< label fit box
  mutable BBox       fitTLBBox_;         //!< tick label fit box
  mutable BBox       plbbox_;            //!< label pixel box
  mutable BBox       tlbbox_;            //!< title label box
//mutable BBox       lastTickLabelRect_; //!< last tick box (for auto hide)
  mutable TextPlacer textPlacer_;        //!< cache axis tick label draw data

  mutable bool usePen_     { false }; //!< use painter pen
  mutable bool forceColor_ { false }; //!< force painter color for all
  mutable QPen savePen_;              //!< override pen to use
};

#endif
