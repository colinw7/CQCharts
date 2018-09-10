#ifndef CQChartsAxis_H
#define CQChartsAxis_H

#include <CQChartsEditHandles.h>
#include <CQChartsPlotData.h>
#include <CQChartsColumn.h>
#include <CQChartsGeom.h>
#include <CQChartsUtil.h>
#include <CInterval.h>

#include <map>
#include <vector>
#include <string>
#include <sys/types.h>

#include <boost/optional.hpp>

class CQChartsAxis;
class CQChartsPlot;
class CQPropertyViewModel;
class QPainter;

CQCHARTS_NAMED_LINE_DATA(Axes,axes)
CQCHARTS_NAMED_TEXT_DATA(AxesTickLabel,axesTickLabel)
CQCHARTS_NAMED_TEXT_DATA(AxesLabel,axesLabel)
CQCHARTS_NAMED_LINE_DATA(AxesMajorGrid,axesMajorGrid)
CQCHARTS_NAMED_LINE_DATA(AxesMinorGrid,axesMinorGrid)
CQCHARTS_NAMED_FILL_DATA(AxesGrid,axesGrid)

// Axis Data
class CQChartsAxis : public QObject,
 public CQChartsPlotAxesLineData         <CQChartsAxis>,
 public CQChartsPlotAxesTickLabelTextData<CQChartsAxis>,
 public CQChartsPlotAxesLabelTextData    <CQChartsAxis>,
 public CQChartsPlotAxesMajorGridLineData<CQChartsAxis>,
 public CQChartsPlotAxesMinorGridLineData<CQChartsAxis>,
 public CQChartsPlotAxesGridFillData     <CQChartsAxis> {
  Q_OBJECT

  // general
  Q_PROPERTY(bool      visible          READ isVisible          WRITE setVisible         )
  Q_PROPERTY(bool      selected         READ isSelected         WRITE setSelected        )
  Q_PROPERTY(Direction direction        READ direction          WRITE setDirection       )
  Q_PROPERTY(Side      side             READ side               WRITE setSide            )
  Q_PROPERTY(bool      hasPosition      READ hasPosition        WRITE setHasPosition     )
  Q_PROPERTY(double    position         READ position           WRITE setPosition        )
  Q_PROPERTY(bool      requireTickLabel READ isRequireTickLabel WRITE setRequireTickLabel)
  Q_PROPERTY(bool      integral         READ isIntegral         WRITE setIntegral        )
  Q_PROPERTY(bool      log              READ isLog              WRITE setLog             )
  Q_PROPERTY(QString   format           READ format             WRITE setFormat          )
  Q_PROPERTY(double    tickIncrement    READ tickIncrement      WRITE setTickIncrement   )
  Q_PROPERTY(double    majorIncrement   READ majorIncrement     WRITE setMajorIncrement  )
  Q_PROPERTY(double    start            READ start              WRITE setStart           )
  Q_PROPERTY(double    end              READ end                WRITE setEnd             )
  Q_PROPERTY(bool      includeZero      READ isIncludeZero      WRITE setIncludeZero     )

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
  Q_PROPERTY(bool               tickLabelAutoHide
             READ isTickLabelAutoHide WRITE setTickLabelAutoHide )
  Q_PROPERTY(TickLabelPlacement tickLabelPlacement
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

  Q_ENUMS(Direction)
  Q_ENUMS(Side)
  Q_ENUMS(TickLabelPlacement)

 public:
  enum class Direction {
    HORIZONTAL,
    VERTICAL
  };

  enum class Side {
    BOTTOM_LEFT,
    TOP_RIGHT
  };

  enum TickLabelPlacement {
    MIDDLE,
    BOTTOM_LEFT,
    TOP_RIGHT,
    BETWEEN
  };

  using OptReal = boost::optional<double>;

 public:
  CQChartsAxis(CQChartsPlot *plot, Direction direction=Direction::HORIZONTAL,
               double start=0.0, double end=1.0);

  virtual ~CQChartsAxis();

  CQChartsPlot *plot() const { return plot_; }

  CQChartsView *view() const;

  bool isVisible() const { return visible_; }
  void setVisible(bool b) { CQChartsUtil::testAndSet(visible_, b, [&]() { redraw(); } ); }

  bool isSelected() const { return selected_; }
  void setSelected(bool b) { CQChartsUtil::testAndSet(selected_, b, [&]() { redraw(); } ); }

  Direction direction() const { return direction_; }
  void setDirection(Direction dir) { direction_ = dir; updatePlotPosition(); }

  Side side() const { return side_; }
  void setSide(Side side) { side_ = side; updatePlotPosition(); }

  double start() const { return start_; }
  void setStart(double start) { setRange(start, end_); }

  double end() const { return end_; }
  void setEnd(double end) { setRange(start_, end); }

  bool isIncludeZero() const { return includeZero_; }
  void setIncludeZero(bool b) { includeZero_ = b; updatePlotRange(); }

  void setRange(double start, double end);

  bool isIntegral() const { return integral_; }
  void setIntegral(bool b);

  bool isLog() const { return log_; }
  void setLog(bool b);

  const CQChartsColumn &column() const { return column_; }
  void setColumn(const CQChartsColumn &c) {
    CQChartsUtil::testAndSet(column_, c, [&]() { redraw(); } ); }

  bool isDataLabels() const { return dataLabels_; }
  void setDataLabels(bool b) { CQChartsUtil::testAndSet(dataLabels_, b, [&]() { redraw(); } ); }

  QString format() const;
  bool setFormat(const QString &s);

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

  const TickLabelPlacement &tickLabelPlacement() const { return tickLabelPlacement_; }
  void setTickLabelPlacement(const TickLabelPlacement &v) {
    CQChartsUtil::testAndSet(tickLabelPlacement_, v, [&]() { redraw(); } ); }

  //---

  uint numMajorTicks() const { return numMajorTicks_; }
  void setNumMajorTicks(uint n) {
    CQChartsUtil::testAndSet(numMajorTicks_, n, [&]() { redraw(); } ); }

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

  const OptReal &pos() const { return pos_; }
  void setPos(double r) { if (! pos_ || *pos_ != r) { pos_ = r; redraw(); } }

  void unsetPos() { pos_.reset(); redraw(); }

  bool hasPosition() const { return !!pos_; }

  void setHasPosition(bool b) {
    if (! b) { if (  hasPosition()) unsetPos(); }
    else     { if (! hasPosition()) setPos(0.0); }
  }

  double position() const { return pos_.value_or(0.0); }
  void setPosition(double r) { setPos(r); }

  //---

  QString valueStr(double pos) const;

  QString valueStr(CQChartsPlot *plot, double pos) const;

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

  void redraw();

  void updatePlotRange();

  CQChartsEditHandles &editHandles() { return editHandles_; }

  //---

  virtual bool editPress  (const CQChartsGeom::Point &);
  virtual bool editMove   (const CQChartsGeom::Point &);
  virtual bool editMotion (const CQChartsGeom::Point &);
  virtual bool editRelease(const CQChartsGeom::Point &);

  virtual void editMoveBy(const QPointF &d);

  //---

  bool isDrawGrid() const;

  void drawGrid(CQChartsPlot *plot, QPainter *painter);

  //---

  void draw(CQChartsPlot *plot, QPainter *painter);

  void drawEditHandles(QPainter *painter);

  void calcPos(CQChartsPlot *plot, double &apos1, double &apos2) const;

  void drawLine(CQChartsPlot *plot, QPainter *painter,
                double apos, double amin, double amax);

  void drawMajorGridLine(CQChartsPlot *plot, QPainter *painter,
                         double apos, double amin, double amax);
  void drawMinorGridLine(CQChartsPlot *plot, QPainter *painter,
                         double apos, double amin, double amax);

  void drawMajorTickLine(CQChartsPlot *plot, QPainter *painter,
                         double apos, double tpos, bool inside);
  void drawMinorTickLine(CQChartsPlot *plot, QPainter *painter,
                         double apos, double tpos, bool inside);

  void drawTickLine(CQChartsPlot *plot, QPainter *painter,
                    double apos, double tpos, bool inside, bool major);

  void drawTickLabel(CQChartsPlot *plot, QPainter *painter,
                     double apos, double tpos, bool inside);

  void drawAxisLabel(CQChartsPlot *plot, QPainter *painter,
                     double apos, double amin, double amax, const QString &text);

 signals:
  void ticksChanged();

 private:
  struct AxisGapData {
    double start       { 0.0 };
    double end         { 0.0 };
    double increment   { 0.0 };
    uint   numGaps     { 0 };
    uint   numGapTicks { 0 };
  };

  void calc();

 private:
  using TickSpaces = std::vector<double>;
  using TickLabels = std::map<int,QString>;

  CQChartsPlot*              plot_                { nullptr };

  // general
  bool                       visible_             { true };
  bool                       selected_            { false };
  Direction                  direction_           { Direction::HORIZONTAL };
  Side                       side_                { Side::BOTTOM_LEFT };
  bool                       integral_            { false };
  bool                       log_                 { false };
  bool                       dataLabels_          { false };  // labels from model row
  CQChartsColumn             column_;
  QString                    formatStr_;

  // label
  bool                       labelDisplayed_      { true };
  QString                    label_;

  // grid (lines and gap fill)
  bool                       gridMid_             { false };
  bool                       gridAbove_           { false };

  // ticks
  bool                       minorTicksDisplayed_ { true };
  bool                       majorTicksDisplayed_ { true };
  int                        minorTickLen_        { 4 };
  int                        majorTickLen_        { 8 };
  bool                       tickInside_          { false };
  bool                       mirrorTicks_         { false };

  bool                       tickLabelAutoHide_   { true };
  TickLabelPlacement         tickLabelPlacement_  { TickLabelPlacement::MIDDLE };

  // state
  double                     start_               { 0.0 };
  double                     end_                 { 1.0 };
  bool                       includeZero_         { false };
  uint                       numMajorTicks_       { 1 };
  uint                       numMinorTicks_       { 0 };
  uint                       tickIncrement_       { 0 };
  double                     majorIncrement_      { 0 };
  double                     calcStart_           { 0 };
  double                     calcEnd_             { 1 };
  double                     calcIncrement_       { 0 };
  CInterval                  interval_;
  TickSpaces                 tickSpaces_;
  TickLabels                 tickLabels_;
  bool                       requireTickLabel_    { false };
  OptReal                    pos_;
  mutable CQChartsGeom::BBox bbox_;
  mutable CQChartsGeom::BBox fitBBox_;

  CQChartsEditHandles        editHandles_;
//mutable double             lmin_ { INT_MAX };
//mutable double             lmax_ { INT_MIN };
  mutable CQChartsGeom::BBox lbbox_;
  mutable CQChartsGeom::BBox lastTickLabelRect_;
};

#endif
