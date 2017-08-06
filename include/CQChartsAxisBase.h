#ifndef CQChartsAxisBase_H
#define CQChartsAxisBase_H

#include <QObject>
#include <QColor>
#include <QFont>

class QPainter;
class CQChartsPlot;
class CQPropertyTree;

class CQChartsAxisBase : public QObject {
  Q_OBJECT

  Q_PROPERTY(bool    visible        READ getVisible        WRITE setVisible       )
  Q_PROPERTY(QString label          READ getLabel          WRITE setLabel         )
  Q_PROPERTY(QFont   labelFont      READ getLabelFont      WRITE setLabelFont     )
  Q_PROPERTY(QColor  labelColor     READ getLabelColor     WRITE setLabelColor    )
  Q_PROPERTY(bool    lineDisplayed  READ getLineDisplayed  WRITE setLineDisplayed )
  Q_PROPERTY(bool    labelDisplayed READ getLabelDisplayed WRITE setLabelDisplayed)
  Q_PROPERTY(QColor  lineColor      READ getLineColor      WRITE setLineColor     )
  Q_PROPERTY(QColor  gridColor      READ getGridColor      WRITE setGridColor     )
  Q_PROPERTY(bool    gridDisplayed  READ getGridDisplayed  WRITE setGridDisplayed )

 public:
  enum Direction {
    DIR_HORIZONTAL,
    DIR_VERTICAL
  };

  enum Side {
    SIDE_BOTTOM_LEFT,
    SIDE_TOP_RIGHT
  };

 public:
  CQChartsAxisBase(CQChartsPlot *plot, Direction direction=DIR_HORIZONTAL,
                   double start=0.0, double end=1.0);

  virtual ~CQChartsAxisBase() { }

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

  virtual void draw(CQChartsPlot *, QPainter *) = 0;

  void addProperties(CQPropertyTree *tree, const QString &path);

 protected:
  virtual void calc() { }

 protected:
  CQChartsPlot* plot_           { nullptr };
  bool          visible_        { true };
  Direction     direction_      { DIR_HORIZONTAL };
  Side          side_           { SIDE_BOTTOM_LEFT };
  double        start_          { 0.0 };
  double        end_            { 1.0 };
  QString       label_;
  QFont         labelFont_;
  QColor        labelColor_     { 0, 0, 0 };
  bool          lineDisplayed_  { true };
  QColor        lineColor_      { 0, 0, 0 };
  bool          labelDisplayed_ { true };
  QColor        gridColor_      { 0, 0, 0 };
  bool          gridDisplayed_  { false };
};

#endif
