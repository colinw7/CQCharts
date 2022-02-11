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

  // tick calc
  Q_PROPERTY(CQChartsOptInt tickIncrement   READ tickIncrement     WRITE setTickIncrement  )
  Q_PROPERTY(CQChartsOptInt majorIncrement  READ majorIncrement    WRITE setMajorIncrement )
  Q_PROPERTY(double         start           READ start             WRITE setStart          )
  Q_PROPERTY(double         end             READ end               WRITE setEnd            )
  Q_PROPERTY(bool           includeZero     READ isIncludeZero     WRITE setIncludeZero    )

  Q_PROPERTY(bool annotation      READ isAnnotation      WRITE setAnnotation     )
  Q_PROPERTY(bool allowHtmlLabels READ isAllowHtmlLabels WRITE setAllowHtmlLabels)

  Q_PROPERTY(CQChartsOptReal valueStart READ valueStart WRITE setValueStart)
  Q_PROPERTY(CQChartsOptReal valueEnd   READ valueEnd   WRITE setValueEnd  )

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
  Q_PROPERTY(CQChartsAxisTickLabelPlacement tickLabelPlacement
             READ tickLabelPlacement  WRITE setTickLabelPlacement)

  CQCHARTS_NAMED_TEXT_DATA_PROPERTIES(AxesTickLabel, axesTickLabel)

  Q_PROPERTY(QString tickLabels READ tickLabelsStr WRITE setTickLabelsStr)

  Q_PROPERTY(QString customTickLabels READ customTickLabelsStr WRITE setCustomTickLabelsStr)

  // label
  Q_PROPERTY(CQChartsOptString label            READ label            WRITE setLabel           )
  Q_PROPERTY(QString           labelStr         READ labelStr         WRITE setLabelStr        )
  Q_PROPERTY(QString           defLabel         READ defLabel         WRITE setDefLabel        )
  Q_PROPERTY(QString           userLabel        READ userLabel        WRITE setUserLabel       )
  Q_PROPERTY(bool              scaleLabelFont   READ isScaleLabelFont WRITE setScaleLabelFont  )
  Q_PROPERTY(CQChartsLength    scaleLabelExtent READ scaleLabelExtent WRITE setScaleLabelExtent)

  CQCHARTS_NAMED_TEXT_DATA_PROPERTIES(AxesLabel, axesLabel)

  // grid line/fill
  Q_PROPERTY(GridLinesDisplayed gridLinesDisplayed
             READ gridLinesDisplayed WRITE setGridLinesDisplayed)
  Q_PROPERTY(GridFillDisplayed  gridFillDisplayed
             READ gridFillDisplayed  WRITE setGridFillDisplayed )

  Q_PROPERTY(bool gridMid   READ isGridMid   WRITE setGridMid  )
  Q_PROPERTY(bool gridAbove READ isGridAbove WRITE setGridAbove)

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
  using View                   = CQChartsView;
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
  Plot *plot() { return const_cast<Plot *>(plot_); }
  const Plot *plot() const { return plot_; }
  void setPlot(const Plot *plot) { plot_ = plot; }

  //! get/set view
  View *view();
  const View *view() const;
  void setView(const View *view) { view_ = view; }

  //---

  //! get/set direction (TODO: allow set direction)
  Qt::Orientation direction() const { return direction_; }
  void setDirection(Qt::Orientation dir) { direction_ = dir; updatePlotPosition(); }

  bool isHorizontal() const { return direction() == Qt::Horizontal; }

  //---

  //! get/set visible
  void setVisible(bool b) override;

  //---

  //! get/set updates enabled
  bool isUpdatesEnabled() const { return updatesEnabled_; }
  void setUpdatesEnabled(bool b) { updatesEnabled_ = b; }

  //! get/set update draws all plot data
  bool isDrawAll() const { return drawAll_; }
  void setDrawAll(bool b) { drawAll_ = b; }

  //---

  void setSelected(bool b) override;

  //---

  //! get/set side (horizontal: top/bottom, vertical bottom/top)
  AxisSide side() const { return side_; }
  void setSide(AxisSide side) { side_ = side; updatePlotPosition(); }

  //---

  //! get/set position (x for vertical, y for horzontal)
  const OptReal &position() const { return position_; }
  void setPosition(const OptReal &r);

  //---

  //! get/set start position
  double start() const { return start_; }
  void setStart(double start) { setRange(start, end_); }

  //! get/set end position
  double end() const { return end_; }
  void setEnd(double end) { setRange(start_, end); }

  //! get/set start and end position
  void setRange(double start, double end);

  //---

  //! get/set custom value start
  const OptReal &valueStart() const { return valueStart_; }
  void setValueStart(const OptReal &v);

  //! get/set custom value end
  const OptReal &valueEnd() const { return valueEnd_; }
  void setValueEnd(const OptReal &v);

  //! get/set custom start and end value
  void setValueRange(double start, double end);

  //---

  //! get/set include zero
  bool isIncludeZero() const { return includeZero_; }
  void setIncludeZero(bool b);

  //---

  //! get/set is annotation
  bool isAnnotation() const { return annotation_; }
  void setAnnotation(bool b);

  //! get/set allow html labels
  bool isAllowHtmlLabels() const { return allowHtmlLabels_; }
  void setAllowHtmlLabels(bool b);

  //---

  //! get/set value type
  const AxisValueType &valueType() const { return valueType_; }
  void setValueType(const AxisValueType &v, bool notify=true);

  bool isIntegral() const { return (valueType_.type() == AxisValueType::Type::INTEGER); }
  bool isDate    () const { return (valueType_.type() == AxisValueType::Type::DATE); }
  bool isLog     () const { return (valueType_.type() == AxisValueType::Type::LOG); }

  //---

  //! get/set associated column
  const Column &column() const { return column_; }
  void setColumn(const Column &c);

  //! get/set if labels are from column
  bool isDataLabels() const { return dataLabels_; }
  void setDataLabels(bool b);

  //! get/set value format
  QString format() const;
  bool setFormat(const QString &s);

  //! get/set maximum fit extent (outside axis line length)
  double maxFitExtent() const { return maxFitExtent_; }
  void setMaxFitExtent(double r);

  //---

  //! get/set label
  const OptString &label() const { return label_; }
  void setLabel(const OptString &str);

  //! get/set label string
  QString labelStr() const { return label_.stringOr(); }
  void setLabelStr(const QString &s);

  //! set default label
  const QString &defLabel() const { return label_.defValue(); }
  void setDefLabel(const QString &str, bool notify=true);

  //! get/set user label
  const QString &userLabel() const { return userLabel_; }
  void setUserLabel(const QString &str);

  //! get/set scale label font
  bool isScaleLabelFont() const { return scaleLabelFont_; }
  void setScaleLabelFont(bool b);

  //! get/set scale label extent
  const Length &scaleLabelExtent() const { return scaleLabelExtent_; }
  void setScaleLabelExtent(const Length &l);

  //---

  //! get/set grid lines displayed
  const GridLinesDisplayed &gridLinesDisplayed() const { return gridLinesDisplayed_; }
  void setGridLinesDisplayed(const GridLinesDisplayed &d);

  //! get/set major grid lines displayed
  bool isMajorGridLinesDisplayed() const {
    return gridLinesDisplayed_ == GridLinesDisplayed::MAJOR ||
           gridLinesDisplayed_ == GridLinesDisplayed::MAJOR_AND_MINOR; }
  bool isMinorGridLinesDisplayed() const {
    return gridLinesDisplayed_ == GridLinesDisplayed::MAJOR_AND_MINOR; }

  //! get/set grid fill displayed
  const GridFillDisplayed &gridFillDisplayed() const { return gridFillDisplayed_; }
  void setGridFillDisplayed(const GridFillDisplayed &d);

  bool isMajorGridFilled() const { return gridFillDisplayed_ == GridFillDisplayed::MAJOR; }

  //---

  //! get/set grid mid line
  bool isGridMid() const { return gridMid_; }
  void setGridMid(bool b);

  //! get/set grid above
  bool isGridAbove() const { return gridAbove_; }
  void setGridAbove(bool b);

  //---

  //! get/set ticks displayed
  const TicksDisplayed &ticksDisplayed() const { return ticksDisplayed_; }
  void setTicksDisplayed(const TicksDisplayed &d);

  //! get/set major ticks displayed
  bool isMajorTicksDisplayed() const {
    return ticksDisplayed_ == TicksDisplayed::MAJOR ||
           ticksDisplayed_ == TicksDisplayed::MAJOR_AND_MINOR; }
  bool isMinorTicksDisplayed() const {
    return ticksDisplayed_ == TicksDisplayed::MAJOR_AND_MINOR; }

  //---

  //! get/set major tick (pixel) length
  int majorTickLen() const { return majorTickLen_; }
  void setMajorTickLen(int i);

  //! get/set minor tick (pixel) length
  int minorTickLen() const { return minorTickLen_; }
  void setMinorTickLen(int i);

  //! get/set tick on inside
  bool isTickInside() const { return tickInside_; }
  void setTickInside(bool b);

  //! get/set mirror ticks
  bool isMirrorTicks() const { return mirrorTicks_; }
  void setMirrorTicks(bool b);

  //---

  //! get/set ticks auto hide
  bool isTickLabelAutoHide() const { return tickLabelAutoHide_; }
  void setTickLabelAutoHide(bool b);

  //! get/set ticks label placement
  const AxisTickLabelPlacement &tickLabelPlacement() const { return tickLabelPlacement_; }
  void setTickLabelPlacement(const AxisTickLabelPlacement &p);

  //---

  uint maxMajorTicks() const { return maxMajorTicks_; }

  double minorIncrement() const;

  //---

  //! get/set user specified tick increment
  const OptInt &tickIncrement() const { return tickIncrement_; }
  void setTickIncrement(const OptInt &tickIncrement);

  void setTickIncrement(int i) { setTickIncrement(OptInt(i)); }

  //---

  //! get/set user specified major increment
  const OptInt &majorIncrement() const { return majorIncrement_; }
  void setMajorIncrement(const OptInt &i);

  void setMajorIncrement(int i) { setMajorIncrement(OptInt(i)); }

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
  const double *tickSpaces() const { return &tickSpaces_[0]; }
  uint          numTickSpaces() const { return uint(tickSpaces_.size()); }

  double tickSpace(int i) const { return CUtil::safeIndex(tickSpaces_, i); }
  void setTickSpaces(double *tickSpaces, uint numTickSpaces);

  //---

  QString tickLabelsStr() const;
  void setTickLabelsStr(const QString &str);

  void clearTickLabels();

  void setTickLabel(long i, const QString &label);

  bool hasTickLabel(long i) const;

  const QString &tickLabel(long i) const;

  bool isRequireTickLabel() const { return requireTickLabel_; }
  void setRequireTickLabel(bool b) { requireTickLabel_ = b; }

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

  void redraw(bool wait=true);

  void updatePlotRange();
  void updatePlotRangeAndObjs();

  //---

  // Implement edit interface
  bool editPress (const Point &) override;
  bool editMove  (const Point &) override;
  bool editMotion(const Point &) override; // return true if inside

  void editMoveBy(const Point &) override;

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

  void drawAxisLabel(const Plot *plot, PaintDevice *device, double apos,
                     double amin, double amax, const QString &text, bool allowHtml) const;

  void getTickLabelsPositions(std::set<int> &positions) const;

 signals:
  void ticksChanged();
  void tickPlacementChanged();

  void selectionChanged();

  void appearanceChanged();

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
                          const Angle &angle, Qt::Alignment align) :
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
  const View*     view_      { nullptr };        //!< parent view
  const Plot*     plot_      { nullptr };        //!< parent plot
  Qt::Orientation direction_ { Qt::Horizontal }; //!< direction

  // general
  bool          updatesEnabled_ { true };  //!< axis updates enabled
  bool          drawAll_        { false }; //!< axis updates enabled
  AxisSide      side_;                     //!< axis plot placement side
  OptReal       position_;                 //!< axis custom position
  AxisValueType valueType_;                //!< value type
  bool          dataLabels_     { false }; //!< use data for labels
  Column        column_;                   //!< associated column
  QString       formatStr_;                //!< value format string
  double        maxFitExtent_   { 10 };    //!< max extent percent for fit

  // label
  bool      labelDisplayed_   { true };  //!< show label
  OptString label_;                      //!< label
  QString   userLabel_;                  //!< user label
  bool      scaleLabelFont_   { false }; //!< scale label font fo fit length
  Length    scaleLabelExtent_;           //!< extent to extend length for scale label

  // grid (lines and gap fill)
  GridLinesDisplayed gridLinesDisplayed_ { GridLinesDisplayed::NONE }; //!< grid lines displayed
  GridFillDisplayed  gridFillDisplayed_  { GridFillDisplayed::NONE  }; //!< grid fill displayed

  bool gridMid_   { false }; //!< show grid at mid point
  bool gridAbove_ { false }; //!< is grid above plot

  // ticks
  TicksDisplayed ticksDisplayed_ { TicksDisplayed::MAJOR_AND_MINOR }; //!< ticks displayed

  int  majorTickLen_ { 8 };     //!< major tick length
  int  minorTickLen_ { 4 };     //!< minor tick length
  bool tickInside_   { false }; //!< tick inside plot
  bool mirrorTicks_  { false }; //!< mirror ticks

  bool                   tickLabelAutoHide_   { true }; //!< tick auto hide
  AxisTickLabelPlacement tickLabelPlacement_;           //!< tick placement

  // placement state
  double start_           { 0.0 };   //!< axis start
  double end_             { 1.0 };   //!< axis end
  bool   includeZero_     { false }; //!< include zero in range
  uint   maxMajorTicks_   { 1000 };  //!< max major ticks
  OptInt tickIncrement_;             //!< user specified tick increment
  OptInt majorIncrement_;            //!< user specified major increment
  bool   needsCalc_       { true };  //!< needs tick calc

  OptReal valueStart_; //!< custom value start
  OptReal valueEnd_;   //!< custom value end

  // customization (for annotations)
  bool annotation_      { false }; //!< is annotation
  bool allowHtmlLabels_ { false }; //!< allow html labels

  // internal calculation data
  CInterval interval_;            //!< interval data
  uint      numMajorTicks_ { 1 }; //!< num major ticks
  uint      numMinorTicks_ { 0 }; //!< num minor ticks
  double    calcIncrement_ { 0 }; //!< calculated axis increment
  double    calcStart_     { 0 }; //!< calculated axis start
  double    calcEnd_       { 1 }; //!< calculated axis end

  // tick customization
  TickSpaces  tickSpaces_;                 //!< tick spaces (?)
  ITickLabels tickLabels_;                 //!< tick labels
  bool        requireTickLabel_ { false }; //!< use tick label values even if empty
  RTickLabels customTickLabels_;           //!< custom tick labels

  // edit state
  EditHandlesP editHandles_; //!< edit handles

  // bbox draw state
  mutable BBox       bbox_;              //!< axis box
  mutable BBox       fitBBox_;           //!< fit box
  mutable BBox       fitLBBox_;          //!< label fit box
  mutable BBox       fitTLBBox_;         //!< tick label fit box
  mutable BBox       lbbox_;             //!< label box
//mutable BBox       lastTickLabelRect_; //!< last tick box (for auto hide)
  mutable TextPlacer textPlacer_;        //!< cache axis tick label draw data

  mutable bool usePen_     { false }; //!< use painter pen
  mutable bool forceColor_ { false }; //!< force painter color for all
  mutable QPen savePen_;              //!< override pen to use
};

#endif
