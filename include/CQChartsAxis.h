#ifndef CQChartsAxis_H
#define CQChartsAxis_H

#include <QColor>
#include <QFont>
#include <CLineDash.h>
#include <CQChartsGeom.h>

#include <sys/types.h>

#include <map>
#include <vector>
#include <string>

#include <boost/optional.hpp>

class CQChartsAxis;
class CQChartsAxisLabel;
class CQChartsAxisTickLabel;
class CQChartsPlot;
class CQChartsLineObj;
class CQChartsFillObj;
class CQPropertyViewModel;
class QPainter;

// Axis Data
class CQChartsAxis : public QObject {
  Q_OBJECT

  // general
  Q_PROPERTY(bool      visible        READ isVisible      WRITE setVisible       )
  Q_PROPERTY(Direction direction      READ direction      WRITE setDirection     )
  Q_PROPERTY(Side      side           READ side           WRITE setSide          )
  Q_PROPERTY(bool      hasPosition    READ hasPosition    WRITE setHasPosition   )
  Q_PROPERTY(double    position       READ position       WRITE setPosition      )
  Q_PROPERTY(bool      integral       READ isIntegral     WRITE setIntegral      )
  Q_PROPERTY(QString   format         READ format         WRITE setFormat        )
  Q_PROPERTY(double    tickIncrement  READ tickIncrement  WRITE setTickIncrement )
  Q_PROPERTY(double    majorIncrement READ majorIncrement WRITE setMajorIncrement)
  Q_PROPERTY(double    start          READ start          WRITE setStart         )
  Q_PROPERTY(double    end            READ end            WRITE setEnd           )

  // line
  Q_PROPERTY(bool      lineDisplayed READ isLineDisplayed WRITE setLineDisplayed)
  Q_PROPERTY(QColor    lineColor     READ lineColor       WRITE setLineColor    )
  Q_PROPERTY(double    lineWidth     READ lineWidth       WRITE setLineWidth    )
  Q_PROPERTY(CLineDash lineDash      READ lineDash        WRITE setLineDash     )

  // ticks
  Q_PROPERTY(bool minorTicksDisplayed READ isMinorTicksDisplayed WRITE setMinorTicksDisplayed)
  Q_PROPERTY(bool majorTicksDisplayed READ isMajorTicksDisplayed WRITE setMajorTicksDisplayed)
  Q_PROPERTY(int  minorTickLen        READ minorTickLen          WRITE setMinorTickLen       )
  Q_PROPERTY(int  majorTickLen        READ majorTickLen          WRITE setMajorTickLen       )
  Q_PROPERTY(bool tickInside          READ isTickInside          WRITE setTickInside         )
  Q_PROPERTY(bool mirrorTicks         READ isMirrorTicks         WRITE setMirrorTicks        )

  // ticks label
  Q_PROPERTY(bool   tickLabelDisplayed READ isTickLabelDisplayed WRITE setTickLabelDisplayed)
  Q_PROPERTY(QFont  tickLabelFont      READ tickLabelFont        WRITE setTickLabelFont     )
  Q_PROPERTY(QColor tickLabelColor     READ tickLabelColor       WRITE setTickLabelColor    )
  Q_PROPERTY(double tickLabelAngle     READ tickLabelAngle       WRITE setTickLabelAngle    )
  Q_PROPERTY(bool   tickLabelAutoHide  READ isTickLabelAutoHide  WRITE setTickLabelAutoHide )

  // label
  Q_PROPERTY(bool    labelDisplayed READ isLabelDisplayed WRITE setLabelDisplayed)
  Q_PROPERTY(QString label          READ label            WRITE setLabel         )
  Q_PROPERTY(QFont   labelFont      READ labelFont        WRITE setLabelFont     )
  Q_PROPERTY(QColor  labelColor     READ labelColor       WRITE setLabelColor    )

  // grid line/fill
  Q_PROPERTY(bool      gridDisplayed READ isGridDisplayed WRITE setGridDisplayed)
  Q_PROPERTY(QColor    gridColor     READ gridColor       WRITE setGridColor    )
  Q_PROPERTY(double    gridWidth     READ gridWidth       WRITE setGridWidth    )
  Q_PROPERTY(CLineDash gridDash      READ gridDash        WRITE setGridDash     )
  Q_PROPERTY(bool      gridAbove     READ isGridAbove     WRITE setGridAbove    )
  Q_PROPERTY(bool      gridFill      READ isGridFill      WRITE setGridFill     )
  Q_PROPERTY(QColor    gridFillColor READ gridFillColor   WRITE setGridFillColor)
  Q_PROPERTY(double    gridFillAlpha READ gridFillAlpha   WRITE setGridFillAlpha)

  Q_ENUMS(Direction)
  Q_ENUMS(Side)

 public:
  enum class Direction {
    HORIZONTAL,
    VERTICAL
  };

  enum class Side {
    BOTTOM_LEFT,
    TOP_RIGHT
  };

  using OptReal = boost::optional<double>;

 public:
  CQChartsAxis(CQChartsPlot *plot, Direction direction=Direction::HORIZONTAL,
               double start=0.0, double end=1.0);

 ~CQChartsAxis();

  bool isVisible() const { return visible_; }
  void setVisible(bool b) { visible_ = b; }

  Direction direction() const { return direction_; }
  void setDirection(Direction dir) { direction_ = dir; }

  Side side() const { return side_; }
  void setSide(Side side) { side_ = side; updatePlotPosition(); }

  double start() const { return start_; }
  void setStart(double start) { setRange(start, end_); }

  double end() const { return end_; }
  void setEnd(double end) { setRange(start_, end); }

  void setRange(double start, double end);

  bool isIntegral() const { return integral_; }
  void setIntegral(bool b);

  int column() const { return column_; }
  void setColumn(int i) { column_ = i; }

  bool isDataLabels() const { return dataLabels_; }
  void setDataLabels(bool b) { dataLabels_ = b; }

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

  const QColor &labelColor() const;
  void setLabelColor(const QColor &color);

  //---

  // line

  bool isLineDisplayed() const;
  void setLineDisplayed(bool b);

  const QColor &lineColor() const;
  void setLineColor(const QColor &c);

  double lineWidth() const;
  void setLineWidth(double r);

  const CLineDash &lineDash() const;
  void setLineDash(const CLineDash &dash);

  //---

  // grid

  bool isGridDisplayed() const;
  void setGridDisplayed(bool b);

  const QColor &gridColor() const;
  void setGridColor(const QColor &c);

  double gridWidth() const;
  void setGridWidth(double r);

  const CLineDash &gridDash() const;
  void setGridDash(const CLineDash &dash);

  bool isGridAbove() const { return gridAbove_; }
  void setGridAbove(bool b) { gridAbove_ = b; redraw(); }

  bool isGridFill() const;
  void setGridFill(bool b);

  const QColor &gridFillColor() const;
  void setGridFillColor(const QColor &c);

  double gridFillAlpha() const;
  void setGridFillAlpha(double a);

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

  const QColor &tickLabelColor() const;
  void setTickLabelColor(const QColor &color);

  double tickLabelAngle() const;
  void setTickLabelAngle(double r);

  bool isTickLabelAutoHide() const { return tickLabelAutoHide_; }
  void setTickLabelAutoHide(bool b) { tickLabelAutoHide_ = b; redraw(); }

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

  void clearTickLabels() {
    tickLabels_.clear();
  }

  void setTickLabel(long i, const QString &label) { tickLabels_[i] = label; redraw(); }

  bool hasTickLabel(long i) const { return (tickLabels_.find(i) != tickLabels_.end()); }

  const QString &tickLabel(long i) const {
    auto p = tickLabels_.find(i);
    assert(p != tickLabels_.end());

    return (*p).second;
  }

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

  const CQChartsGeom::BBox &bbox() const { return bbox_; }
  void setBBox(const CQChartsGeom::BBox &b) { bbox_ = b; redraw(); }

  void addProperties(CQPropertyViewModel *model, const QString &path);

  void updatePlotPosition();

  //---

  void redraw();

  //---

  void drawGrid(CQChartsPlot *plot, QPainter *p);

  void draw(CQChartsPlot *plot, QPainter *p);

  void drawLine(CQChartsPlot *plot, QPainter *p, double apos, double amin, double amax);

  void drawTickLine(CQChartsPlot *plot, QPainter *p, double apos, double tpos,
                    bool inside, bool major);

  void drawTickLabel(CQChartsPlot *plot, QPainter *p, double apos, double tpos, bool inside);

  void drawAxisLabel(CQChartsPlot *plot, QPainter *p, double apos, double amin, double amax,
                     const QString &text);

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
  Direction                  direction_           { Direction::HORIZONTAL };
  Side                       side_                { Side::BOTTOM_LEFT };
  bool                       integral_            { false };
  bool                       dataLabels_          { false };
  int                        column_              { -1 };

  // label
  bool                       labelDisplayed_      { true };
  CQChartsAxisLabel*         label_               { nullptr };

  // line
  CQChartsLineObj*           lineObj_;

  // grid (lines and gap fill)
  bool                       gridAbove_           { false };
  CQChartsLineObj*           gridLineObj_;
  CQChartsFillObj*           gridFill_;

  // ticks
  bool                       minorTicksDisplayed_ { true };
  bool                       majorTicksDisplayed_ { true };
  int                        minorTickLen_        { 4 };
  int                        majorTickLen_        { 8 };
  bool                       tickInside_          { false };
  bool                       mirrorTicks_         { false };

  // tick label
  bool                       tickLabelDisplayed_ { true };
  CQChartsAxisTickLabel*     tickLabel_          { nullptr };
  bool                       tickLabelAutoHide_  { false };

  // state
  double                     start_               { 0.0 };
  double                     end_                 { 1.0 };
  double                     start1_              { 0 };
  double                     end1_                { 1 };
  uint                       numMajorTicks_       { 1 };
  uint                       numMinorTicks_       { 0 };
  uint                       tickIncrement_       { 0 };
  double                     majorIncrement_      { 0 };
  TickSpaces                 tickSpaces_;
  TickLabels                 tickLabels_;
  OptReal                    pos_;
  mutable CQChartsGeom::BBox bbox_;

  mutable double             lmin_ { INT_MAX };
  mutable double             lmax_ { INT_MIN };
  mutable CQChartsGeom::BBox lastTickLabelRect_;
};

#endif
