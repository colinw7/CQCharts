#ifndef CQChartsAxis_H
#define CQChartsAxis_H

#include <QObject>
#include <QColor>
#include <QFont>
#include <CLineDash.h>
#include <CBBox2D.h>
#include <COptVal.h>

#include <sys/types.h>

#include <map>
#include <vector>
#include <string>

class CQChartsPlot;
class CQChartsPlot;
class CQPropertyTree;
class QPainter;

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
  Q_PROPERTY(bool tickLabelDisplayed  READ isTickLabelDisplayed   WRITE setTickLabelDisplayed )

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

 ~CQChartsAxis() { }

  bool getVisible() const { return visible_; }
  void setVisible(bool b) { visible_ = b; }

  Direction getDirection() const { return direction_; }
  void setDirection(Direction dir) { direction_ = dir; }

  Side getSide() const { return side_; }
  void setSide(Side side) { side_ = side; }

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
  void setFormat(const QString &v);

  //---

  // label

  const QString &getLabel() const { return label_; }
  void setLabel(const QString &str) { label_ = str; redraw(); }

  const QFont &getLabelFont() const { return labelFont_; }
  void setLabelFont(const QFont &font) { labelFont_ = font; redraw(); }

  const QColor &getLabelColor() const { return labelColor_; }
  void setLabelColor(const QColor &color) { labelColor_ = color; redraw(); }

  bool isLabelDisplayed() const { return labelDisplayed_; }
  void setLabelDisplayed(bool b) { labelDisplayed_ = b; redraw(); }

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

  bool isTickLabelDisplayed() const { return tickLabelDisplayed_; }
  void setTickLabelDisplayed(bool b) { tickLabelDisplayed_ = b; redraw(); }

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
  void setBBox(const CBBox2D &v) { bbox_ = v; }

  void addProperties(CQPropertyTree *tree, const QString &path);

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
  typedef std::vector<double>   TickSpaces;
  typedef std::map<int,QString> TickLabels;

  CQChartsPlot*   plot_                { nullptr };
  bool            visible_             { true };
  Direction       direction_           { Direction::HORIZONTAL };
  Side            side_                { Side::BOTTOM_LEFT };
  double          start_               { 0.0 };
  double          end_                 { 1.0 };
  double          start1_              { 0 };
  double          end1_                { 1 };
  bool            integral_            { false };
  bool            dataLabels_          { false };
  int             column_              { -1 };

  // label
  QString         label_;
  QFont           labelFont_;
  QColor          labelColor_          { 0, 0, 0 };
  bool            labelDisplayed_      { true };

  // line
  bool            lineDisplayed_       { true };
  QColor          lineColor_           { 128, 128, 128 };

  // grid
  bool            gridDisplayed_       { false };
  QColor          gridColor_           { 0, 0, 0 };
  CLineDash       gridDash_            { 2, 2 };
  double          gridWidth_           { 0 };
  bool            gridAbove_           { false };

  // ticks
  bool            minorTicksDisplayed_ { true };
  bool            majorTicksDisplayed_ { true };
  int             minorTickLen_        { 4 };
  int             majorTickLen_        { 8 };
  bool            tickLabelDisplayed_  { true };
  uint            numTicks1_           { 1 };
  uint            numTicks2_           { 0 };
  uint            tickIncrement_       { 0 };
  double          majorIncrement_      { 0 };
  TickSpaces      tickSpaces_;
  TickLabels      tickLabels_;

  COptReal        pos_;
  mutable CBBox2D bbox_;
};

#endif
