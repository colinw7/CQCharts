#ifndef CQChartsAxis_H
#define CQChartsAxis_H

#include <CQChartsEditHandles.h>
#include <CQChartsData.h>
#include <CQChartsColumn.h>
#include <CQChartsGeom.h>

#include <QFont>
#include <sys/types.h>

#include <map>
#include <vector>
#include <string>

#include <boost/optional.hpp>

class CQChartsAxis;
class CQChartsAxisLabel;
class CQChartsAxisTickLabel;
class CQChartsPlot;
class CQPropertyViewModel;
class QPainter;

// Axis Data
class CQChartsAxis : public QObject {
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
  Q_PROPERTY(bool             lineDisplayed READ isLineDisplayed WRITE setLineDisplayed)
  Q_PROPERTY(CQChartsColor    lineColor     READ lineColor       WRITE setLineColor    )
  Q_PROPERTY(CQChartsLength   lineWidth     READ lineWidth       WRITE setLineWidth    )
  Q_PROPERTY(CQChartsLineDash lineDash      READ lineDash        WRITE setLineDash     )

  // ticks
  Q_PROPERTY(bool minorTicksDisplayed READ isMinorTicksDisplayed WRITE setMinorTicksDisplayed)
  Q_PROPERTY(bool majorTicksDisplayed READ isMajorTicksDisplayed WRITE setMajorTicksDisplayed)
  Q_PROPERTY(int  minorTickLen        READ minorTickLen          WRITE setMinorTickLen       )
  Q_PROPERTY(int  majorTickLen        READ majorTickLen          WRITE setMajorTickLen       )
  Q_PROPERTY(bool tickInside          READ isTickInside          WRITE setTickInside         )
  Q_PROPERTY(bool mirrorTicks         READ isMirrorTicks         WRITE setMirrorTicks        )

  // ticks label
  Q_PROPERTY(bool          tickLabelDisplayed READ isTickLabelDisplayed WRITE setTickLabelDisplayed)
  Q_PROPERTY(QFont         tickLabelFont      READ tickLabelFont        WRITE setTickLabelFont     )
  Q_PROPERTY(CQChartsColor tickLabelColor     READ tickLabelColor       WRITE setTickLabelColor    )
  Q_PROPERTY(double        tickLabelAngle     READ tickLabelAngle       WRITE setTickLabelAngle    )
  Q_PROPERTY(bool          tickLabelAutoHide  READ isTickLabelAutoHide  WRITE setTickLabelAutoHide )

  Q_PROPERTY(TickLabelPlacement tickLabelPlacement
             READ tickLabelPlacement WRITE setTickLabelPlacement )

  // label
  Q_PROPERTY(bool          labelDisplayed READ isLabelDisplayed WRITE setLabelDisplayed)
  Q_PROPERTY(QString       label          READ label            WRITE setLabel         )
  Q_PROPERTY(QFont         labelFont      READ labelFont        WRITE setLabelFont     )
  Q_PROPERTY(CQChartsColor labelColor     READ labelColor       WRITE setLabelColor    )

  // grid line/fill
  Q_PROPERTY(bool             gridMajorDisplayed READ  isGridMajorDisplayed
                                                 WRITE setGridMajorDisplayed)
  Q_PROPERTY(CQChartsColor    gridMajorColor     READ  gridMajorColor    WRITE setGridMajorColor)
  Q_PROPERTY(CQChartsLength   gridMajorWidth     READ  gridMajorWidth    WRITE setGridMajorWidth)
  Q_PROPERTY(CQChartsLineDash gridMajorDash      READ  gridMajorDash     WRITE setGridMajorDash )
  Q_PROPERTY(bool             gridMinorDisplayed READ  isGridMinorDisplayed
                                                 WRITE setGridMinorDisplayed)
  Q_PROPERTY(CQChartsColor    gridMinorColor     READ  gridMinorColor    WRITE setGridMinorColor)
  Q_PROPERTY(CQChartsLength   gridMinorWidth     READ  gridMinorWidth    WRITE setGridMinorWidth)
  Q_PROPERTY(CQChartsLineDash gridMinorDash      READ  gridMinorDash     WRITE setGridMinorDash )
  Q_PROPERTY(bool             gridAbove          READ  isGridAbove       WRITE setGridAbove     )
  Q_PROPERTY(bool             gridFill           READ  isGridFill        WRITE setGridFill      )
  Q_PROPERTY(CQChartsColor    gridFillColor      READ  gridFillColor     WRITE setGridFillColor )
  Q_PROPERTY(double           gridFillAlpha      READ  gridFillAlpha     WRITE setGridFillAlpha )

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

  bool isVisible() const { return visible_; }
  void setVisible(bool b) { visible_ = b; redraw(); }

  bool isSelected() const { return selected_; }
  void setSelected(bool b) { selected_ = b; redraw(); }

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
  void setColumn(const CQChartsColumn &c) { column_ = c; redraw(); }

  bool isDataLabels() const { return dataLabels_; }
  void setDataLabels(bool b) { dataLabels_ = b; redraw(); }

  QString format() const;
  bool setFormat(const QString &s);

  //---

  // label
  bool isLabelDisplayed() const;
  void setLabelDisplayed(bool b);

  const QString &label() const;
  void setLabel(const QString &str);

  const QFont &labelFont() const;
  void setLabelFont(const QFont &font);

  const CQChartsColor &labelColor() const;
  void setLabelColor(const CQChartsColor &c);

  QColor interpLabelColor(int i, int n) const;

  //---

  // line
  bool isLineDisplayed() const;
  void setLineDisplayed(bool b);

  const CQChartsLength &lineWidth() const;
  void setLineWidth(const CQChartsLength &l);

  const CQChartsLineDash &lineDash() const;
  void setLineDash(const CQChartsLineDash &dash);

  const CQChartsColor &lineColor() const;
  void setLineColor(const CQChartsColor &c);

  QColor interpLineColor(int i, int n) const;

  //---

  // grid
  bool isGridMajorDisplayed() const;
  void setGridMajorDisplayed(bool b);

  const CQChartsColor &gridMajorColor() const;
  void setGridMajorColor(const CQChartsColor &c);

  QColor interpGridMajorColor(int i, int n) const;

  const CQChartsLength &gridMajorWidth() const;
  void setGridMajorWidth(const CQChartsLength &l);

  const CQChartsLineDash &gridMajorDash() const;
  void setGridMajorDash(const CQChartsLineDash &dash);

  bool isGridMinorDisplayed() const;
  void setGridMinorDisplayed(bool b);

  const CQChartsColor &gridMinorColor() const;
  void setGridMinorColor(const CQChartsColor &c);

  QColor interpGridMinorColor(int i, int n) const;

  const CQChartsLength &gridMinorWidth() const;
  void setGridMinorWidth(const CQChartsLength &l);

  const CQChartsLineDash &gridMinorDash() const;
  void setGridMinorDash(const CQChartsLineDash &dash);

  bool isGridAbove() const { return gridAbove_; }
  void setGridAbove(bool b) { gridAbove_ = b; redraw(); }

  bool isGridFill() const;
  void setGridFill(bool b);

  double gridFillAlpha() const;
  void setGridFillAlpha(double a);

  const CQChartsColor &gridFillColor() const;
  void setGridFillColor(const CQChartsColor &c);

  QColor interpGridFillColor(int i, int n) const;

  //---

  // ticks
  bool isMinorTicksDisplayed() const { return minorTicksDisplayed_; }
  void setMinorTicksDisplayed(bool b) { minorTicksDisplayed_ = b; redraw(); }

  bool isMajorTicksDisplayed() const { return majorTicksDisplayed_; }
  void setMajorTicksDisplayed(bool b) { majorTicksDisplayed_ = b; redraw(); }

  int minorTickLen() const { return minorTickLen_; }
  void setMinorTickLen(int i) { minorTickLen_ = i; redraw(); }

  int majorTickLen() const { return majorTickLen_; }
  void setMajorTickLen(int i) { majorTickLen_ = i; redraw(); }

  bool isTickInside() const { return tickInside_; }
  void setTickInside(bool b) { tickInside_ = b; redraw(); }

  bool isMirrorTicks() const { return mirrorTicks_; }
  void setMirrorTicks(bool b) { mirrorTicks_ = b; redraw(); }

  //---

  // ticks label
  bool isTickLabelDisplayed() const;
  void setTickLabelDisplayed(bool b);

  const QFont &tickLabelFont() const;
  void setTickLabelFont(const QFont &font);

  const CQChartsColor &tickLabelColor() const;
  void setTickLabelColor(const CQChartsColor &c);

  QColor interpTickLabelColor(int i, int n) const;

  double tickLabelAngle() const;
  void setTickLabelAngle(double r);

  bool isTickLabelAutoHide() const { return tickLabelAutoHide_; }
  void setTickLabelAutoHide(bool b) { tickLabelAutoHide_ = b; redraw(); }

  const TickLabelPlacement &tickLabelPlacement() const { return tickLabelPlacement_; }
  void setTickLabelPlacement(const TickLabelPlacement &v) { tickLabelPlacement_ = v; redraw(); }

  //---

  uint numMajorTicks() const { return numMajorTicks_; }
  void setNumMajorTicks(uint n) { numMajorTicks_ = n; redraw(); }

  uint numMinorTicks() const { return numMinorTicks_; }
  void setNumMinorTicks(uint n) { numMinorTicks_ = n; redraw(); }

  uint tickIncrement() const { return tickIncrement_; }
  void setTickIncrement(uint tickIncrement);

  double minorIncrement() const;

  double majorIncrement() const;
  void setMajorIncrement(double i);

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
  void setPos(double r) { pos_ = r; redraw(); }

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

  const CQChartsGeom::BBox &bbox() const { return bbox_; }
  void setBBox(const CQChartsGeom::BBox &b) { bbox_ = b; redraw(); }

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

  void drawGrid(CQChartsPlot *plot, QPainter *painter);

  void draw(CQChartsPlot *plot, QPainter *painter);

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

  bool testAxisGaps(double start, double end, double testIncrement, uint testNumGapTicks,
                    AxisGapData &axisGapData);

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
  CQChartsAxisLabel*         label_               { nullptr };

  // line
  CQChartsLineData           lineData_;

  // grid (lines and gap fill)
  bool                       gridAbove_           { false };
  CQChartsLineData           majorGridLineData_;
  CQChartsLineData           minorGridLineData_;
  CQChartsFillData           gridFill_;

  // ticks
  bool                       minorTicksDisplayed_ { true };
  bool                       majorTicksDisplayed_ { true };
  int                        minorTickLen_        { 4 };
  int                        majorTickLen_        { 8 };
  bool                       tickInside_          { false };
  bool                       mirrorTicks_         { false };

  // tick label (TODO: use CQChartsAxisTickLabel for all tick label data)
  bool                       tickLabelDisplayed_ { true };
  CQChartsAxisTickLabel*     tickLabel_          { nullptr };
  bool                       tickLabelAutoHide_  { true };
  TickLabelPlacement         tickLabelPlacement_ { TickLabelPlacement::MIDDLE };

  // state
  double                     start_               { 0.0 };
  double                     end_                 { 1.0 };
  double                     start1_              { 0 };
  double                     end1_                { 1 };
  bool                       includeZero_         { false };
  uint                       numMajorTicks_       { 1 };
  uint                       numMinorTicks_       { 0 };
  uint                       tickIncrement_       { 0 };
  double                     majorIncrement_      { 0 };
  TickSpaces                 tickSpaces_;
  TickLabels                 tickLabels_;
  bool                       requireTickLabel_    { false };
  OptReal                    pos_;
  mutable CQChartsGeom::BBox bbox_;

  CQChartsEditHandles        editHandles_;
//mutable double             lmin_ { INT_MAX };
//mutable double             lmax_ { INT_MIN };
  mutable CQChartsGeom::BBox lbbox_;
  mutable CQChartsGeom::BBox lastTickLabelRect_;
};

#endif
