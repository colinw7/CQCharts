#ifndef CQChartsXYPlot_H
#define CQChartsXYPlot_H

#include <CQChartsPlot.h>
#include <CQUtil.h>
#include <CPen.h>
#include <CBrush.h>
#include <CSymbolType.h>

class CQChartsAxis;

class CQChartsXYPlot : public CQChartsPlot {
  Q_OBJECT

  // properties
  //  display
  //   bivariate
  //  line
  //   display, stroke
  //  fill under
  //   display, brush
  //  symbol
  //   display, color, shape. ...
  //  margin
  //  key

  Q_PROPERTY(int    xColumn        READ xColumn        WRITE setXColumn       )
  Q_PROPERTY(int    yColumn        READ yColumn        WRITE setYColumn       )
  Q_PROPERTY(bool   bivariate      READ isBivariate    WRITE setBivariate     )
  Q_PROPERTY(bool   points         READ isPoints       WRITE setPoints        )
  Q_PROPERTY(QColor pointsColor    READ pointsColor    WRITE setPointsColor   )
  Q_PROPERTY(bool   lines          READ isLines        WRITE setLines         )
  Q_PROPERTY(QColor linesColor     READ linesColor     WRITE setLinesColor    )
  Q_PROPERTY(bool   fillUnder      READ isFillUnder    WRITE setFillUnder     )
  Q_PROPERTY(QColor fillUnderColor READ fillUnderColor WRITE setFillUnderColor)

 public:
  typedef std::vector<int> Columns;

 private:
  struct PointData {
    bool        shown  { true };
    CSymbolType symbol { CSYMBOL_CROSS };
    CPen        pen;

    PointData() {
      pen.setColor(CRGBA(0.5, 0.5, 0.5));
    }
  };

  struct LineData {
    bool shown { true };
    CPen pen;

    LineData() {
      pen.setColor(CRGBA(0, 0, 0));
    }
  };

  struct FillUnderData {
    bool   shown { false };
    CBrush brush;

    FillUnderData() {
     brush.setStyle(CBRUSH_STYLE_SOLID);
     brush.setColor(CRGBA(0.5, 0.5, .8));
    }
  };

 public:
  CQChartsXYPlot(QAbstractItemModel *model);

  int xColumn() const { return xColumn_; }
  void setXColumn(int i) { xColumn_ = i; update(); }

  int yColumn() const { return yColumn_; }
  void setYColumn(int i) { yColumn_ = i; update(); }

  const Columns &yColumns() const { return yColumns_; }
  void setYColumns(const Columns &yColumns) { yColumns_ = yColumns; update(); }

  bool isBivariate() const { return bivariate_; }
  void setBivariate(bool b) { bivariate_ = b; update(); }

  bool isPoints() const { return pointData_.shown; }
  void setPoints(bool b) { pointData_.shown = b; update(); }

  QColor pointsColor() const { return CQUtil::toQColor(pointData_.pen.getColor()); }
  void setPointsColor(const QColor &c) {
    pointData_.pen.setColor(CQUtil::fromQColor(c)); update(); }

  bool isLines() const { return lineData_.shown; }
  void setLines(bool b) { lineData_.shown = b; update(); }

  QColor linesColor() const { return CQUtil::toQColor(lineData_.pen.getColor()); }
  void setLinesColor(const QColor &c) {
    lineData_.pen.setColor(CQUtil::fromQColor(c)); update(); }

  bool isFillUnder() const { return fillUnderData_.shown; }
  void setFillUnder(bool b) { fillUnderData_.shown = b; update(); }

  QColor fillUnderColor() const { return CQUtil::toQColor(fillUnderData_.brush.getColor()); }
  void setFillUnderColor(const QColor &c) {
    fillUnderData_.brush.setColor(CQUtil::fromQColor(c)); update(); }

  void updateRange();

  void paintEvent(QPaintEvent *) override;

 private:
  double modelReal(int row, int col) const;

 private:
  int           xColumn_   { 0 };
  int           yColumn_   { 1 };
  Columns       yColumns_;
  bool          bivariate_ { false };
  CQChartsAxis* xAxis_     { nullptr };
  CQChartsAxis* yAxis_     { nullptr };
  PointData     pointData_;
  LineData      lineData_;
  FillUnderData fillUnderData_;
};

#endif
