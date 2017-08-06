#ifndef CQChartsXYPlot_H
#define CQChartsXYPlot_H

#include <CQChartsPlot.h>
#include <CQChartsPlotObj.h>
#include <CQUtil.h>
#include <CPen.h>
#include <CBrush.h>
#include <CSymbolType.h>

class CQChartsXYPlot;

class CQChartsXYLineObj : public CQChartsPlotObj {
  Q_OBJECT

 public:
  CQChartsXYLineObj(CQChartsXYPlot *plot, const CBBox2D &rect, double x,
                    double y1, double y2, double s, int ind);

  bool inside(const CPoint2D &p) const override;

  void draw(QPainter *p) override;

 private:
  CQChartsXYPlot *plot_ { nullptr };
  double          x_    { 0.0 };
  double          y1_   { 0.0 };
  double          y2_   { 0.0 };
  double          s_    { 0.0 };
  int             ind_  { -1 };
};

class CQChartsXYPointObj : public CQChartsPlotObj {
  Q_OBJECT

 public:
  CQChartsXYPointObj(CQChartsXYPlot *plot, const CBBox2D &rect, double x,
                     double y, double s, int ind);

  bool inside(const CPoint2D &p) const override;

  void draw(QPainter *p) override;

 private:
  CQChartsXYPlot *plot_ { nullptr };
  double          x_    { 0.0 };
  double          y_    { 0.0 };
  double          s_    { 0.0 };
  int             ind_  { -1 };
};

class CQChartsXYPolygonObj : public CQChartsPlotObj {
  Q_OBJECT

 public:
  CQChartsXYPolygonObj(CQChartsXYPlot *plot, const CBBox2D &rect, const QPolygonF &poly, int ind);

  bool inside(const CPoint2D &p) const override;

  void draw(QPainter *p) override;

 private:
  CQChartsXYPlot *plot_ { nullptr };
  QPolygonF       poly_;
  int             ind_  { -1 };
};

//---

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
  Q_PROPERTY(int    nameColumn     READ nameColumn     WRITE setNameColumn    )
  Q_PROPERTY(bool   bivariate      READ isBivariate    WRITE setBivariate     )
  Q_PROPERTY(bool   points         READ isPoints       WRITE setPoints        )
  Q_PROPERTY(QColor pointsColor    READ pointsColor    WRITE setPointsColor   )
  Q_PROPERTY(bool   lines          READ isLines        WRITE setLines         )
  Q_PROPERTY(QColor linesColor     READ linesColor     WRITE setLinesColor    )
  Q_PROPERTY(bool   fillUnder      READ isFillUnder    WRITE setFillUnder     )
  Q_PROPERTY(QColor fillUnderColor READ fillUnderColor WRITE setFillUnderColor)

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

  int nameColumn() const { return nameColumn_; }
  void setNameColumn(int i) { nameColumn_ = i; update(); }

  bool isBivariate() const { return bivariate_; }
  void setBivariate(bool b) { bivariate_ = b; update(); }

  bool isPoints() const { return pointData_.shown; }
  void setPoints(bool b) { pointData_.shown = b; update(); }

  const CPen &pointsPen() const { return pointData_.pen; }

  QColor pointsColor() const { return CQUtil::toQColor(pointsPen().getColor()); }
  void setPointsColor(const QColor &c) {
    pointData_.pen.setColor(CQUtil::fromQColor(c)); update(); }

  bool isLines() const { return lineData_.shown; }
  void setLines(bool b) { lineData_.shown = b; update(); }

  QColor linesColor() const { return CQUtil::toQColor(lineData_.pen.getColor()); }
  void setLinesColor(const QColor &c) {
    lineData_.pen.setColor(CQUtil::fromQColor(c)); update(); }

  bool isFillUnder() const { return fillUnderData_.shown; }
  void setFillUnder(bool b) { fillUnderData_.shown = b; update(); }

  const CBrush &fillUnderBrush() const { return fillUnderData_.brush; }

  QColor fillUnderColor() const { return CQUtil::toQColor(fillUnderBrush().getColor()); }
  void setFillUnderColor(const QColor &c) {
    fillUnderData_.brush.setColor(CQUtil::fromQColor(c)); update(); }

  void updateRange();

  void initObjs();

  int numSets() const;

  void paintEvent(QPaintEvent *) override;

 private:
  int           xColumn_    { 0 };
  int           yColumn_    { 1 };
  Columns       yColumns_;
  int           nameColumn_ { -1 };
  bool          bivariate_  { false };
  PointData     pointData_;
  LineData      lineData_;
  FillUnderData fillUnderData_;
};

#endif
