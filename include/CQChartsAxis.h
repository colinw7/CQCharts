#ifndef CQChartsAxis_H
#define CQChartsAxis_H

#include <QObject>
#include <QColor>
#include <QFont>
#include <COptVal.h>
#include <CBBox2D.h>

#include <sys/types.h>

#include <string>
#include <vector>

class CQChartsPlot;
class CQChartsPlot;
class CQPropertyTree;
class QPainter;

// Axis Data
class CQChartsAxis : public QObject {
  Q_OBJECT

  Q_PROPERTY(bool      visible             READ getVisible             WRITE setVisible            )
  Q_PROPERTY(Direction direction           READ getDirection           WRITE setDirection          )
  Q_PROPERTY(Side      side                READ getSide                WRITE setSide               )
  Q_PROPERTY(QString   label               READ getLabel               WRITE setLabel              )
  Q_PROPERTY(QFont     labelFont           READ getLabelFont           WRITE setLabelFont          )
  Q_PROPERTY(QColor    labelColor          READ getLabelColor          WRITE setLabelColor         )
  Q_PROPERTY(bool      lineDisplayed       READ getLineDisplayed       WRITE setLineDisplayed      )
  Q_PROPERTY(bool      labelDisplayed      READ getLabelDisplayed      WRITE setLabelDisplayed     )
  Q_PROPERTY(QColor    lineColor           READ getLineColor           WRITE setLineColor          )
  Q_PROPERTY(QColor    gridColor           READ getGridColor           WRITE setGridColor          )
  Q_PROPERTY(bool      gridDisplayed       READ getGridDisplayed       WRITE setGridDisplayed      )
  Q_PROPERTY(bool      minorTicksDisplayed READ getMinorTicksDisplayed WRITE setMinorTicksDisplayed)
  Q_PROPERTY(bool      majorTicksDisplayed READ getMajorTicksDisplayed WRITE setMajorTicksDisplayed)
  Q_PROPERTY(int       minorTickLen        READ getMinorTickLen        WRITE setMinorTickLen       )
  Q_PROPERTY(int       majorTickLen        READ getMajorTickLen        WRITE setMajorTickLen       )
  Q_PROPERTY(bool      integral            READ isIntegral             WRITE setIntegral           )
  Q_PROPERTY(QString   format              READ format                 WRITE setFormat             )

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

  const QString &getLabel() const { return label_; }
  void setLabel(const QString &str) { label_ = str; }

  const QFont &getLabelFont() const { return labelFont_; }
  void setLabelFont(const QFont &font) { labelFont_ = font; }

  const QColor &getLabelColor() const { return labelColor_; }
  void setLabelColor(const QColor &color) { labelColor_ = color; }

  bool getLineDisplayed() const { return lineDisplayed_; }
  void setLineDisplayed(bool b) { lineDisplayed_ = b; }

  bool getLabelDisplayed() const { return labelDisplayed_; }
  void setLabelDisplayed(bool b) { labelDisplayed_ = b; }

  const QColor &getLineColor() const { return lineColor_; }
  void setLineColor(const QColor &c) { lineColor_ = c; }

  bool getGridDisplayed() const { return gridDisplayed_; }
  void setGridDisplayed(bool b) { gridDisplayed_ = b; }

  const QColor &getGridColor() const { return gridColor_; }
  void setGridColor(const QColor &c) { gridColor_ = c; }

  bool getMinorTicksDisplayed() const { return minorTicksDisplayed_; }
  void setMinorTicksDisplayed(bool b) { minorTicksDisplayed_ = b; }

  bool getMajorTicksDisplayed() const { return majorTicksDisplayed_; }
  void setMajorTicksDisplayed(bool b) { majorTicksDisplayed_ = b; }

  int getMinorTickLen() const { return minorTickLen_; }
  void setMinorTickLen(int i) { minorTickLen_ = i; }

  int getMajorTickLen() const { return majorTickLen_; }
  void setMajorTickLen(int i) { majorTickLen_ = i; }

  int column() const { return column_; }
  void setColumn(int i) { column_ = i; }

  uint getNumMajorTicks() const { return numTicks1_; }
  void setNumMajorTicks(uint n) { numTicks1_ = n; }

  uint getNumMinorTicks() const { return numTicks2_; }
  void setNumMinorTicks(uint n) { numTicks2_ = n; }

  uint getTickIncrement() const { return tickIncrement_; }
  void setTickIncrement(uint tickIncrement);

  double getMinorIncrement() const;

  double getMajorIncrement() const;
  void setMajorIncrement(double i);

  const double *getTickSpaces   () const { return &tickSpaces_[0]; }
  uint          getNumTickSpaces() const { return tickSpaces_.size(); }

  void clearTickLabels() {
    tickLabels_.clear();
  }

  void setTickLabel(int i, const QString &label) {
    tickLabels_[i] = label;
  }

  bool hasTickLabel(int i) const {
    return (tickLabels_.find(i) != tickLabels_.end());
  }

  const QString &getTickLabel(int i) const {
    auto p = tickLabels_.find(i);

    return (*p).second;
  }

  bool isIntegral() const { return integral_; }
  void setIntegral(bool b);

  bool isDataLabels() const { return dataLabels_; }
  void setDataLabels(bool b) { dataLabels_ = b; }

  QString format() const;
  void setFormat(const QString &v);

  double getTickSpace(int i) const { return tickSpaces_[i]; }
  void setTickSpaces(double *tickSpaces, uint numTickSpaces);

  void setPos(double r) { pos_ = r; }

  QString getValueStr(double pos) const;

  const CBBox2D &bbox() const { return bbox_; }
  void setBBox(const CBBox2D &v) { bbox_ = v; }

  void addProperties(CQPropertyTree *tree, const QString &path);

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
  QString         label_;
  QFont           labelFont_;
  QColor          labelColor_          { 0, 0, 0 };
  bool            lineDisplayed_       { true };
  QColor          lineColor_           { 128, 128, 128 };
  bool            labelDisplayed_      { true };
  QColor          gridColor_           { 0, 0, 0 };
  bool            gridDisplayed_       { false };
  bool            minorTicksDisplayed_ { true };
  bool            majorTicksDisplayed_ { true };
  int             minorTickLen_        { 4 };
  int             majorTickLen_        { 8 };
  double          start1_              { 0 };
  double          end1_                { 1 };
  uint            numTicks1_           { 1 };
  uint            numTicks2_           { 0 };
  uint            tickIncrement_       { 0 };
  double          majorIncrement_      { 0 };
  TickSpaces      tickSpaces_;
  TickLabels      tickLabels_;
  bool            integral_            { false };
  bool            dataLabels_          { false };
  int             column_              { -1 };
  COptReal        pos_;
  mutable CBBox2D bbox_;
};

#endif
