#ifndef CQChartsAxis_H
#define CQChartsAxis_H

#include <CQChartsTextBoxObj.h>
#include <QColor>
#include <QFont>
#include <CLineDash.h>
#include <CBBox2D.h>

#include <sys/types.h>

#include <map>
#include <vector>
#include <string>

#include <boost/optional.hpp>

class CQChartsAxis;
class CQChartsPlot;
class CQPropertyViewTree;
class QPainter;

class CQChartsAxisLabel : public CQChartsTextBoxObj {
  Q_OBJECT

 public:
  CQChartsAxisLabel(CQChartsAxis *axis);

  void redrawBoxObj() override;

 private:
  CQChartsAxis *axis_ { nullptr };
};

//---

class CQChartsAxisTickLabel : public CQChartsTextBoxObj {
  Q_OBJECT

 public:
  CQChartsAxisTickLabel(CQChartsAxis *axis);

  void redrawBoxObj() override;

 private:
  CQChartsAxis *axis_ { nullptr };
};

//---

// Axis Data
class CQChartsAxis : public QObject {
  Q_OBJECT

  // general
  Q_PROPERTY(bool      visible   READ getVisible   WRITE setVisible  )
  Q_PROPERTY(Direction direction READ getDirection WRITE setDirection)
  Q_PROPERTY(Side      side      READ getSide      WRITE setSide     )
  Q_PROPERTY(bool      integral  READ isIntegral   WRITE setIntegral )
  Q_PROPERTY(QString   format    READ format       WRITE setFormat   )

  // line
  Q_PROPERTY(QColor lineColor     READ getLineColor     WRITE setLineColor    )
  Q_PROPERTY(bool   lineDisplayed READ getLineDisplayed WRITE setLineDisplayed)

  // ticks
  Q_PROPERTY(bool minorTicksDisplayed READ getMinorTicksDisplayed WRITE setMinorTicksDisplayed)
  Q_PROPERTY(bool majorTicksDisplayed READ getMajorTicksDisplayed WRITE setMajorTicksDisplayed)
  Q_PROPERTY(int  minorTickLen        READ getMinorTickLen        WRITE setMinorTickLen       )
  Q_PROPERTY(int  majorTickLen        READ getMajorTickLen        WRITE setMajorTickLen       )

  // ticks label
  Q_PROPERTY(bool   tickLabelDisplayed READ isTickLabelDisplayed WRITE setTickLabelDisplayed)
  Q_PROPERTY(QFont  tickLabelFont      READ getTickLabelFont     WRITE setTickLabelFont     )
  Q_PROPERTY(QColor tickLabelColor     READ getTickLabelColor    WRITE setTickLabelColor    )
  Q_PROPERTY(double tickLabelAngle     READ getTickLabelAngle    WRITE setTickLabelAngle    )

  // label
  Q_PROPERTY(bool    labelDisplayed READ isLabelDisplayed WRITE setLabelDisplayed)
  Q_PROPERTY(QString label          READ getLabel         WRITE setLabel         )
  Q_PROPERTY(QFont   labelFont      READ getLabelFont     WRITE setLabelFont     )
  Q_PROPERTY(QColor  labelColor     READ getLabelColor    WRITE setLabelColor    )

  // grid
  Q_PROPERTY(bool      gridDisplayed READ getGridDisplayed WRITE setGridDisplayed)
  Q_PROPERTY(QColor    gridColor     READ getGridColor     WRITE setGridColor    )
  Q_PROPERTY(CLineDash gridDash      READ getGridDash      WRITE setGridDash     )
  Q_PROPERTY(double    gridWidth     READ getGridWidth     WRITE setGridWidth    )
  Q_PROPERTY(bool      gridAbove     READ isGridAbove      WRITE setGridAbove    )
  Q_PROPERTY(bool      gridFill      READ isGridFill       WRITE setGridFill     )
  Q_PROPERTY(QColor    gridFillColor READ gridFillColor    WRITE setGridFillColor)
  Q_PROPERTY(double    gridFillAlpha READ gridFillAlpha    WRITE setGridFillAlpha)

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

 public:
  CQChartsAxis(CQChartsPlot *plot, Direction direction=Direction::HORIZONTAL,
               double start=0.0, double end=1.0);

 ~CQChartsAxis();

  bool getVisible() const { return visible_; }
  void setVisible(bool b) { visible_ = b; }

  Direction getDirection() const { return direction_; }
  void setDirection(Direction dir) { direction_ = dir; }

  Side getSide() const { return side_; }
  void setSide(Side side) { side_ = side; updatePlotPosition(); }

  double getStart() const { return start_; }
  void setStart(double start) { setRange(start, end_); }

  double getEnd() const { return end_; }
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

  const QString &getLabel() const;
  void setLabel(const QString &str);

  const QFont &getLabelFont() const;
  void setLabelFont(const QFont &font);

  const QColor &getLabelColor() const;
  void setLabelColor(const QColor &color);

  //---

  // line

  bool getLineDisplayed() const { return lineDisplayed_; }
  void setLineDisplayed(bool b) { lineDisplayed_ = b; redraw(); }

  const QColor &getLineColor() const { return lineColor_; }
  void setLineColor(const QColor &c) { lineColor_ = c; redraw(); }

  //---

  // grid

  bool getGridDisplayed() const { return gridDisplayed_; }
  void setGridDisplayed(bool b) { gridDisplayed_ = b; redraw(); }

  const QColor &getGridColor() const { return gridColor_; }
  void setGridColor(const QColor &c) { gridColor_ = c; redraw(); }

  const CLineDash &getGridDash() const { return gridDash_; }
  void setGridDash(const CLineDash &dash) { gridDash_ = dash; redraw(); }

  double getGridWidth() const { return gridWidth_; }
  void setGridWidth(double r) { gridWidth_ = r; redraw(); }

  bool isGridAbove() const { return gridAbove_; }
  void setGridAbove(bool b) { gridAbove_ = b; redraw(); }

  bool isGridFill() const { return gridFill_; }
  void setGridFill(bool b) { gridFill_ = b; redraw(); }

  const QColor &gridFillColor() const { return gridFillColor_; }
  void setGridFillColor(const QColor &c) { gridFillColor_ = c; redraw(); }

  double gridFillAlpha() const { return gridFillAlpha_; }
  void setGridFillAlpha(double r) { gridFillAlpha_ = r; redraw(); }

  //---

  // ticks

  bool getMinorTicksDisplayed() const { return minorTicksDisplayed_; }
  void setMinorTicksDisplayed(bool b) { minorTicksDisplayed_ = b; redraw(); }

  bool getMajorTicksDisplayed() const { return majorTicksDisplayed_; }
  void setMajorTicksDisplayed(bool b) { majorTicksDisplayed_ = b; redraw(); }

  int getMinorTickLen() const { return minorTickLen_; }
  void setMinorTickLen(int i) { minorTickLen_ = i; redraw(); }

  int getMajorTickLen() const { return majorTickLen_; }
  void setMajorTickLen(int i) { majorTickLen_ = i; redraw(); }

  //---

  // ticks label

  bool isTickLabelDisplayed() const;
  void setTickLabelDisplayed(bool b);

  const QFont &getTickLabelFont() const;
  void setTickLabelFont(const QFont &font);

  const QColor &getTickLabelColor() const;
  void setTickLabelColor(const QColor &color);

  double getTickLabelAngle() const;
  void setTickLabelAngle(double r);

  //---

  uint getNumMajorTicks() const { return numTicks1_; }
  void setNumMajorTicks(uint n) { numTicks1_ = n; }

  uint getNumMinorTicks() const { return numTicks2_; }
  void setNumMinorTicks(uint n) { numTicks2_ = n; }

  uint getTickIncrement() const { return tickIncrement_; }
  void setTickIncrement(uint tickIncrement);

  double getMinorIncrement() const;

  double getMajorIncrement() const;
  void setMajorIncrement(double i);

  //---

  // used ?
  const double *getTickSpaces   () const { return &tickSpaces_[0]; }
  uint          getNumTickSpaces() const { return tickSpaces_.size(); }

  double getTickSpace(int i) const { return tickSpaces_[i]; }
  void setTickSpaces(double *tickSpaces, uint numTickSpaces);

  //---

  void clearTickLabels() {
    tickLabels_.clear();
  }

  void setTickLabel(long i, const QString &label) {
    tickLabels_[i] = label;
  }

  bool hasTickLabel(long i) const {
    return (tickLabels_.find(i) != tickLabels_.end());
  }

  const QString &getTickLabel(long i) const {
    auto p = tickLabels_.find(i);

    return (*p).second;
  }

  //---

  void setPos(double r) { pos_ = r; }

  QString getValueStr(double pos) const;

  const CBBox2D &bbox() const { return bbox_; }
  void setBBox(const CBBox2D &b) { bbox_ = b; }

  void addProperties(CQPropertyViewTree *tree, const QString &path);

  void updatePlotPosition();

  //---

  void redraw();

  //---

  void drawGrid(CQChartsPlot *plot, QPainter *p);

  void draw(CQChartsPlot *plot, QPainter *p);

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
  typedef std::vector<double>     TickSpaces;
  typedef std::map<int,QString>   TickLabels;
  typedef boost::optional<double> OptReal;

  CQChartsPlot*          plot_                { nullptr };
  bool                   visible_             { true };
  Direction              direction_           { Direction::HORIZONTAL };
  Side                   side_                { Side::BOTTOM_LEFT };
  double                 start_               { 0.0 };
  double                 end_                 { 1.0 };
  double                 start1_              { 0 };
  double                 end1_                { 1 };
  bool                   integral_            { false };
  bool                   dataLabels_          { false };
  int                    column_              { -1 };

  // label
  bool                   labelDisplayed_  { true };
  CQChartsAxisLabel*     label_           { nullptr };

  // line
  bool                   lineDisplayed_       { true };
  QColor                 lineColor_           { 128, 128, 128 };

  // grid
  bool                   gridDisplayed_       { false };
  QColor                 gridColor_           { 0, 0, 0 };
  CLineDash              gridDash_            { 2, 2 };
  double                 gridWidth_           { 0 };
  bool                   gridAbove_           { false };
  bool                   gridFill_            { false };
  QColor                 gridFillColor_       { 128, 128, 128 };
  double                 gridFillAlpha_       { 0.5 };

  // ticks
  bool                   minorTicksDisplayed_ { true };
  bool                   majorTicksDisplayed_ { true };
  int                    minorTickLen_        { 4 };
  int                    majorTickLen_        { 8 };

  // tick label
  bool                   tickLabelDisplayed_ { true };
  CQChartsAxisTickLabel *tickLabel_          { nullptr };

  uint                   numTicks1_           { 1 };
  uint                   numTicks2_           { 0 };
  uint                   tickIncrement_       { 0 };
  double                 majorIncrement_      { 0 };
  TickSpaces             tickSpaces_;
  TickLabels             tickLabels_;

  OptReal                pos_;
  mutable CBBox2D        bbox_;
};

#endif
