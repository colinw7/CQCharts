#ifndef CQChartsAxis_H
#define CQChartsAxis_H

#include <CQChartsObj.h>
#include <CQChartsObjData.h>
#include <CQChartsColumn.h>
#include <CQChartsGeom.h>
#include <CQChartsUtil.h>
#include <CQChartsOptReal.h>
#include <CQChartsAxisTickLabelPlacement.h>
#include <CInterval.h>

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
class QPainter;

CQCHARTS_NAMED_LINE_DATA(Axes,axes)
CQCHARTS_NAMED_TEXT_DATA(AxesTickLabel,axesTickLabel)
CQCHARTS_NAMED_TEXT_DATA(AxesLabel,axesLabel)
CQCHARTS_NAMED_LINE_DATA(AxesMajorGrid,axesMajorGrid)
CQCHARTS_NAMED_LINE_DATA(AxesMinorGrid,axesMinorGrid)
CQCHARTS_NAMED_FILL_DATA(AxesGrid,axesGrid)

//! \brief Axis Data
class CQChartsAxis : public CQChartsObj,
 public CQChartsObjAxesLineData         <CQChartsAxis>,
 public CQChartsObjAxesTickLabelTextData<CQChartsAxis>,
 public CQChartsObjAxesLabelTextData    <CQChartsAxis>,
 public CQChartsObjAxesMajorGridLineData<CQChartsAxis>,
 public CQChartsObjAxesMinorGridLineData<CQChartsAxis>,
 public CQChartsObjAxesGridFillData     <CQChartsAxis> {
  Q_OBJECT

  // general
  Q_PROPERTY(bool             visible          READ isVisible          WRITE setVisible         )
  Q_PROPERTY(Qt::Orientation  direction        READ direction          WRITE setDirection       )
  Q_PROPERTY(CQChartsAxisSide side             READ side               WRITE setSide            )
  Q_PROPERTY(CQChartsOptReal  position         READ position           WRITE setPosition        )
  Q_PROPERTY(bool             requireTickLabel READ isRequireTickLabel WRITE setRequireTickLabel)
  Q_PROPERTY(bool             integral         READ isIntegral         WRITE setIntegral        )
  Q_PROPERTY(bool             date             READ isDate             WRITE setDate            )
  Q_PROPERTY(bool             log              READ isLog              WRITE setLog             )
  Q_PROPERTY(QString          format           READ format             WRITE setFormat          )
  Q_PROPERTY(double           maxFitExtent     READ maxFitExtent       WRITE setMaxFitExtent    )
  Q_PROPERTY(double           tickIncrement    READ tickIncrement      WRITE setTickIncrement   )
  Q_PROPERTY(double           majorIncrement   READ majorIncrement     WRITE setMajorIncrement  )
  Q_PROPERTY(double           start            READ start              WRITE setStart           )
  Q_PROPERTY(double           end              READ end                WRITE setEnd             )
  Q_PROPERTY(bool             includeZero      READ isIncludeZero      WRITE setIncludeZero     )

  // line
  CQCHARTS_NAMED_LINE_DATA_PROPERTIES(Axes,axes)

  // ticks
  Q_PROPERTY(bool minorTicksDisplayed READ isMinorTicksDisplayed WRITE setMinorTicksDisplayed)
  Q_PROPERTY(bool majorTicksDisplayed READ isMajorTicksDisplayed WRITE setMajorTicksDisplayed)
  Q_PROPERTY(int  minorTickLen        READ minorTickLen          WRITE setMinorTickLen       )
  Q_PROPERTY(int  majorTickLen        READ majorTickLen          WRITE setMajorTickLen       )
  Q_PROPERTY(bool tickInside          READ isTickInside          WRITE setTickInside         )
  Q_PROPERTY(bool mirrorTicks         READ isMirrorTicks         WRITE setMirrorTicks        )

  // ticks label
  Q_PROPERTY(bool                           tickLabelAutoHide
             READ isTickLabelAutoHide WRITE setTickLabelAutoHide )
  Q_PROPERTY(CQChartsAxisTickLabelPlacement tickLabelPlacement
             READ tickLabelPlacement  WRITE setTickLabelPlacement)

  CQCHARTS_NAMED_TEXT_DATA_PROPERTIES(AxesTickLabel,axesTickLabel)

  // label
  Q_PROPERTY(QString label READ label WRITE setLabel)

  CQCHARTS_NAMED_TEXT_DATA_PROPERTIES(AxesLabel,axesLabel)

  // grid line/fill
  Q_PROPERTY(bool gridMid   READ isGridMid   WRITE setGridMid  )
  Q_PROPERTY(bool gridAbove READ isGridAbove WRITE setGridAbove)

  CQCHARTS_NAMED_LINE_DATA_PROPERTIES(AxesMajorGrid,axesMajorGrid)
  CQCHARTS_NAMED_LINE_DATA_PROPERTIES(AxesMinorGrid,axesMinorGrid)
  CQCHARTS_NAMED_FILL_DATA_PROPERTIES(AxesGrid,axesGrid)

 public:
  CQChartsAxis(const CQChartsPlot *plot, Qt::Orientation direction=Qt::Horizontal,
               double start=0.0, double end=1.0);

  virtual ~CQChartsAxis();

  CQCharts *charts() const;

  QString calcId() const override;

  CQChartsPlot *plot() { return const_cast<CQChartsPlot *>(plot_); }
  const CQChartsPlot *plot() const { return plot_; }
  void setPlot(const CQChartsPlot *plot) { plot_ = plot; }

  CQChartsView *view();
  const CQChartsView *view() const;

  bool isVisible() const { return visible_; }
  void setVisible(bool b);

  void setSelected(bool b) override;

  Qt::Orientation direction() const { return direction_; }
  void setDirection(Qt::Orientation dir) { direction_ = dir; updatePlotPosition(); }

  bool isHorizontal() const { return direction() == Qt::Horizontal; }

  CQChartsAxisSide side() const { return side_; }
  void setSide(CQChartsAxisSide side) { side_ = side; updatePlotPosition(); }

  double start() const { return start_; }
  void setStart(double start) { setRange(start, end_); }

  double end() const { return end_; }
  void setEnd(double end) { setRange(start_, end); }

  bool isIncludeZero() const { return includeZero_; }
  void setIncludeZero(bool b) { includeZero_ = b; updatePlotRange(); }

  void setRange(double start, double end);

  bool isIntegral() const { return integral_; }
  void setIntegral(bool b);

  bool isDate() const { return date_; }
  void setDate(bool b);

  bool isLog() const { return log_; }
  void setLog(bool b);

  const CQChartsColumn &column() const { return column_; }
  void setColumn(const CQChartsColumn &c);

  bool isDataLabels() const { return dataLabels_; }
  void setDataLabels(bool b);

  QString format() const;
  bool setFormat(const QString &s);

  double maxFitExtent() const { return maxFitExtent_; }
  void setMaxFitExtent(double r) {
    CQChartsUtil::testAndSet(maxFitExtent_, r, [&]() { redraw(); } ); }

  //---

  // label
  const QString &label() const;
  void setLabel(const QString &str);

  //---

  // grid
  bool isGridMid() const { return gridMid_; }
  void setGridMid(bool b) { CQChartsUtil::testAndSet(gridMid_, b, [&]() { redraw(); } ); }

  bool isGridAbove() const { return gridAbove_; }
  void setGridAbove(bool b) { CQChartsUtil::testAndSet(gridAbove_, b, [&]() { redraw(); } ); }

  //----

  // ticks
  bool isMinorTicksDisplayed() const { return minorTicksDisplayed_; }
  void setMinorTicksDisplayed(bool b) {
    CQChartsUtil::testAndSet(minorTicksDisplayed_, b, [&]() { redraw(); } ); }

  bool isMajorTicksDisplayed() const { return majorTicksDisplayed_; }
  void setMajorTicksDisplayed(bool b) {
    CQChartsUtil::testAndSet(majorTicksDisplayed_, b, [&]() { redraw(); } ); }

  int minorTickLen() const { return minorTickLen_; }
  void setMinorTickLen(int i) { CQChartsUtil::testAndSet(minorTickLen_, i, [&]() { redraw(); } ); }

  int majorTickLen() const { return majorTickLen_; }
  void setMajorTickLen(int i) { CQChartsUtil::testAndSet(majorTickLen_, i, [&]() { redraw(); } ); }

  bool isTickInside() const { return tickInside_; }
  void setTickInside(bool b) { CQChartsUtil::testAndSet(tickInside_, b, [&]() { redraw(); } ); }

  bool isMirrorTicks() const { return mirrorTicks_; }
  void setMirrorTicks(bool b) { CQChartsUtil::testAndSet(mirrorTicks_, b, [&]() { redraw(); } ); }

  //---

  // ticks label
  bool isTickLabelAutoHide() const { return tickLabelAutoHide_; }
  void setTickLabelAutoHide(bool b) {
    CQChartsUtil::testAndSet(tickLabelAutoHide_, b, [&]() { redraw(); } ); }

  const CQChartsAxisTickLabelPlacement &tickLabelPlacement() const { return tickLabelPlacement_; }
  void setTickLabelPlacement(const CQChartsAxisTickLabelPlacement &v) {
    CQChartsUtil::testAndSet(tickLabelPlacement_, v, [&]() { redraw(); } ); }

  //---

  uint numMajorTicks() const { return numMajorTicks_; }
  void setNumMajorTicks(uint n) {
    CQChartsUtil::testAndSet(numMajorTicks_, n, [&]() { redraw(); } ); }

  uint maxMajorTicks() const { return maxMajorTicks_; }

  uint numMinorTicks() const { return numMinorTicks_; }
  void setNumMinorTicks(uint n) {
    CQChartsUtil::testAndSet(numMinorTicks_, n, [&]() { redraw(); } ); }

  uint tickIncrement() const { return tickIncrement_; }
  void setTickIncrement(uint tickIncrement);

  double minorIncrement() const;

  double majorIncrement() const { return majorIncrement_; }
  void setMajorIncrement(double i);

  //---

  double calcStart    () const { return calcStart_    ; }
  double calcEnd      () const { return calcEnd_      ; }
  double calcIncrement() const { return calcIncrement_; }

  //---

  double majorTickIncrement() const;
  double minorTickIncrement() const;

  //---

  // used ?
  const double *tickSpaces() const { return &tickSpaces_[0]; }
  uint          numTickSpaces() const { return tickSpaces_.size(); }

  double tickSpace(int i) const { return tickSpaces_[i]; }
  void setTickSpaces(double *tickSpaces, uint numTickSpaces);

  //---

  void clearTickLabels();

  void setTickLabel(long i, const QString &label);

  bool hasTickLabel(long i) const;

  const QString &tickLabel(long i) const;

  bool isRequireTickLabel() const { return requireTickLabel_; }
  void setRequireTickLabel(bool b) { requireTickLabel_ = b; }

  //---

  const CQChartsOptReal &position() const { return position_; }
  void setPosition(const CQChartsOptReal &r);

  //---

  QString valueStr(double pos) const;

  QString valueStr(const CQChartsPlot *plot, double pos) const;

  //--

  const CQChartsGeom::BBox &bbox() const { return bbox_; }
  void setBBox(const CQChartsGeom::BBox &b) {
    CQChartsUtil::testAndSet(bbox_, b, [&]() { redraw(); } ); }

  CQChartsGeom::BBox fitBBox() const;

  //--

  void addProperties(CQPropertyViewModel *model, const QString &path);

  void updatePlotPosition();

  bool contains(const CQChartsGeom::Point &p) const;

  //---

  void redraw(bool wait=true);

  void updatePlotRange();

  CQChartsEditHandles *editHandles() const;

  //---

  virtual bool editPress  (const CQChartsGeom::Point &);
  virtual bool editMove   (const CQChartsGeom::Point &);
  virtual bool editMotion (const CQChartsGeom::Point &);
  virtual bool editRelease(const CQChartsGeom::Point &);

  virtual void editMoveBy(const QPointF &d);

  //---

  bool isDrawGrid() const;

  void drawGrid(const CQChartsPlot *plot, QPainter *painter);

  //---

  void draw(const CQChartsPlot *plot, QPainter *painter);

  void drawEditHandles(QPainter *painter) const;

  void calcPos(const CQChartsPlot *plot, double &apos1, double &apos2) const;

  void drawLine(const CQChartsPlot *plot, QPainter *painter,
                double apos, double amin, double amax);

  void drawMajorGridLine(const CQChartsPlot *plot, QPainter *painter,
                         double apos, double amin, double amax);
  void drawMinorGridLine(const CQChartsPlot *plot, QPainter *painter,
                         double apos, double amin, double amax);

  void drawMajorTickLine(const CQChartsPlot *plot, QPainter *painter,
                         double apos, double tpos, bool inside);
  void drawMinorTickLine(const CQChartsPlot *plot, QPainter *painter,
                         double apos, double tpos, bool inside);

  void drawTickLine(const CQChartsPlot *plot, QPainter *painter,
                    double apos, double tpos, bool inside, bool major);

  void drawTickLabel(const CQChartsPlot *plot, QPainter *painter,
                     double apos, double tpos, bool inside);

  void drawAxisTickDrawDatas(const CQChartsPlot *plot, QPainter *painter);

  void drawAxisLabel(const CQChartsPlot *plot, QPainter *painter,
                     double apos, double amin, double amax, const QString &text);

  void getTickLabelsPositions(std::set<int> &positions) const;

 signals:
  void ticksChanged();

  void selectionChanged();

 private:
  struct AxisGapData {
    double start       { 0.0 };
    double end         { 0.0 };
    double increment   { 0.0 };
    uint   numGaps     { 0 };
    uint   numGapTicks { 0 };
  };

  struct AxisTickDrawData {
    AxisTickDrawData(const QPointF &p, const CQChartsGeom::BBox &bbox, const QString &text) :
     p(p), bbox(bbox), text(text) {
    }

    AxisTickDrawData(const QPointF &p, const CQChartsGeom::BBox &bbox, const QString &text,
                     double angle, Qt::Alignment align) :
     p(p), bbox(bbox), text(text), angle(angle), align(align) {
    }

    QPointF            p;
    CQChartsGeom::BBox bbox;
    QString            text;
    double             angle   { 0.0 };
    Qt::Alignment      align   { Qt::AlignHCenter };
    bool               visible { true };
  };

  using AxisTickDrawDatas = std::vector<AxisTickDrawData>;

 private:
  void calcAndRedraw();

  void calc();

  void emitSelectionChanged();

  CQChartsGeom::Point windowToPixel(const CQChartsPlot *plot, double x, double y) const;

 private:
  using TickSpaces = std::vector<double>;
  using TickLabels = std::map<int,QString>;

  using CQChartsEditHandlesP = std::unique_ptr<CQChartsEditHandles>;

  const CQChartsPlot*  plot_                { nullptr }; //!< parent plot

  // general
  bool                 visible_             { true };           //!< is visible
  Qt::Orientation      direction_           { Qt::Horizontal }; //!< direction
  CQChartsAxisSide     side_;                                   //!< axis plot side
  bool                 integral_            { false };          //!< are values integral
  bool                 date_                { false };          //!< are values dates
  bool                 log_                 { false };          //!< is log values
  bool                 dataLabels_          { false };          //!< use data for labels
  CQChartsColumn       column_;                                 //!< associated column
  QString              formatStr_;                              //!< value format string
  double               maxFitExtent_        { 10 };             //!< max extent percent for fit

  // label
  bool                 labelDisplayed_      { true }; //!< show label
  QString              label_;                        //!< label

  // grid (lines and gap fill)
  bool                 gridMid_             { false }; //!< show grid at mid point
  bool                 gridAbove_           { false }; //!< is grid above plot

  // ticks
  bool                 minorTicksDisplayed_ { true };  //!< minor ticks displayed
  bool                 majorTicksDisplayed_ { true };  //!< major ticks displayed
  int                  minorTickLen_        { 4 };     //!< minor tick length
  int                  majorTickLen_        { 8 };     //!< major tick length
  bool                 tickInside_          { false }; //!< tick inside plot
  bool                 mirrorTicks_         { false }; //!< mirror ticks

  bool                           tickLabelAutoHide_  { true }; //!< tick auto hide
  CQChartsAxisTickLabelPlacement tickLabelPlacement_;          //!< tick placement

  // state
  double               start_               { 0.0 };     //!< axis start
  double               end_                 { 1.0 };     //!< axis end
  bool                 includeZero_         { false };   //!< include zero in range
  uint                 numMajorTicks_       { 1 };       //!< num major ticks
  uint                 maxMajorTicks_       { 1000 };    //!< max major ticks
  uint                 numMinorTicks_       { 0 };       //!< num minor ticks
  uint                 tickIncrement_       { 0 };       //!< tick increment
  double               majorIncrement_      { 0 };       //!< major increment
  double               calcStart_           { 0 };       //!< calculated axis start
  double               calcEnd_             { 1 };       //!< calculated axis end
  double               calcIncrement_       { 0 };       //!< calculated axis increment
  CInterval            interval_;                        //!< interval data
  TickSpaces           tickSpaces_;                      //!< tick spaces (?)
  TickLabels           tickLabels_;                      //!< tick labels
  bool                 requireTickLabel_    { false };   //!< use tick label values even if empty
  CQChartsOptReal      position_;                        //!< axis position
  CQChartsEditHandlesP editHandles_;                     //!< edit handles

  CQChartsGeom::BBox   bbox_;              //!< axis box
  CQChartsGeom::BBox   fitBBox_;           //!< fit box
  CQChartsGeom::BBox   fitLBBox_;          //!< label fit box
  CQChartsGeom::BBox   fitTLBBox_;         //!< tick label fit box
  CQChartsGeom::BBox   lbbox_;             //!< label box
//CQChartsGeom::BBox   lastTickLabelRect_; //!< last tick box (for auto hide)
  AxisTickDrawDatas    axisTickDrawDatas_; //!< cache axis tick label draw data
};

#endif
