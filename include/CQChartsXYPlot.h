#ifndef CQChartsXYPlot_H
#define CQChartsXYPlot_H

#include <CQChartsPlot.h>
#include <CQChartsPlotObj.h>
#include <CQUtil.h>
#include <CPen.h>
#include <CBrush.h>
#include <CSymbolType.h>

class CQChartsXYPlot;

class CQChartsXYBiLineObj : public CQChartsPlotObj {
  Q_OBJECT

 public:
  CQChartsXYBiLineObj(CQChartsXYPlot *plot, const CBBox2D &rect, double x,
                      double y1, double y2, int ind);

  bool visible() const override;

  bool inside(const CPoint2D &p) const override;

  void draw(QPainter *p) override;

 private:
  CQChartsXYPlot *plot_ { nullptr };
  double          x_    { 0.0 };
  double          y1_   { 0.0 };
  double          y2_   { 0.0 };
  int             ind_  { -1 };
};

//---

class CQChartsXYPointObj : public CQChartsPlotObj {
  Q_OBJECT

 public:
  CQChartsXYPointObj(CQChartsXYPlot *plot, const CBBox2D &rect, double x, double y,
                     int iset, int ind);

  bool visible() const override;

  bool inside(const CPoint2D &p) const override;

  void draw(QPainter *p) override;

 private:
  CQChartsXYPlot *plot_ { nullptr };
  double          x_    { 0.0 };
  double          y_    { 0.0 };
  int             iset_ { -1 };
  int             ind_  { -1 };
};

//---

class CQChartsXYPolylineObj : public CQChartsPlotObj {
  Q_OBJECT

 public:
  CQChartsXYPolylineObj(CQChartsXYPlot *plot, const CBBox2D &rect, const QPolygonF &poly, int ind);

  bool visible() const override;

  bool inside(const CPoint2D &p) const override;

  void draw(QPainter *p) override;

 private:
  CQChartsXYPlot *plot_ { nullptr };
  QPolygonF       poly_;
  int             ind_  { -1 };
};

//---

class CQChartsXYPolygonObj : public CQChartsPlotObj {
  Q_OBJECT

 public:
  CQChartsXYPolygonObj(CQChartsXYPlot *plot, const CBBox2D &rect, const QPolygonF &poly, int ind);

  bool visible() const override;

  bool inside(const CPoint2D &p) const override;

  void draw(QPainter *p) override;

 private:
  CQChartsXYPlot *plot_ { nullptr };
  QPolygonF       poly_;
  int             ind_  { -1 };
};

//---

#include <CQChartsKey.h>

class CQChartsXYKeyColor : public CQChartsKeyItem {
  Q_OBJECT

 public:
  CQChartsXYKeyColor(CQChartsXYPlot *plot, bool valueColor, int ind);

  QSizeF size() const override;

  void draw(QPainter *p, const CBBox2D &rect) override;

 private:
  CQChartsXYPlot *plot_       { nullptr };
  int             valueColor_ { false };
  int             ind_        { 0 };
};

//---

class CQChartsXYPlot : public CQChartsPlot {
  Q_OBJECT

  // properties
  //  display
  //   bivariate
  //   stacked
  //  point
  //   display, color, symbol, size
  //  line
  //   display, stroke
  //  fill under
  //   display, brush
  //  margin
  //  key

  Q_PROPERTY(int     xColumn        READ xColumn           WRITE setXColumn          )
  Q_PROPERTY(int     yColumn        READ yColumn           WRITE setYColumn          )
  Q_PROPERTY(int     nameColumn     READ nameColumn        WRITE setNameColumn       )
  Q_PROPERTY(bool    bivariate      READ isBivariate       WRITE setBivariate        )
  Q_PROPERTY(bool    stacked        READ isStacked         WRITE setStacked          )
  Q_PROPERTY(bool    points         READ isPoints          WRITE setPoints           )
  Q_PROPERTY(QString pointsColor    READ pointsColorStr    WRITE setPointsColorStr   )
  Q_PROPERTY(bool    lines          READ isLines           WRITE setLines            )
  Q_PROPERTY(QString linesColor     READ linesColorStr     WRITE setLinesColorStr    )
  Q_PROPERTY(bool    fillUnder      READ isFillUnder       WRITE setFillUnder        )
  Q_PROPERTY(QString fillUnderColor READ fillUnderColorStr WRITE setFillUnderColorStr)
  Q_PROPERTY(QString symbolName     READ symbolName        WRITE setSymbolName       )
  Q_PROPERTY(double  symbolSize     READ symbolSize        WRITE setSymbolSize       )

 private:
  struct PointData {
    bool            shown   { true };
    CSymbol2D::Type symbol  { CSymbol2D::Type::CROSS };
    QColor          color   { 0, 0, 0 };
    bool            palette { true };
    double          size    { 4 };

    PointData() { }
  };

  struct LineData {
    bool   shown   { true };
    QColor color   { 0, 0, 0 };
    bool   palette { true };

    LineData() { }
  };

  struct FillUnderData {
    bool   shown   { false };
    QColor color   { 128, 128, 200 };
    bool   palette { false };

    FillUnderData() { }
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
  void setBivariate(bool b) { bivariate_ = b; initObjs(/*force*/true); update(); }

  bool isStacked() const { return stacked_; }
  void setStacked(bool b) { stacked_ = b; initObjs(/*force*/true); update(); }

  //---

  bool isPoints() const { return pointData_.shown; }
  void setPoints(bool b) { pointData_.shown = b; update(); }

  QString pointsColorStr() const;
  void setPointsColorStr(const QString &str);

  //---

  bool isLines() const { return lineData_.shown; }
  void setLines(bool b) { lineData_.shown = b; update(); }

  QString linesColorStr() const;
  void setLinesColorStr(const QString &str);

  //---

  bool isFillUnder() const { return fillUnderData_.shown; }
  void setFillUnder(bool b) { fillUnderData_.shown = b; update(); }

  QString fillUnderColorStr() const;
  void setFillUnderColorStr(const QString &str);

  //---

  double symbolSize() const { return pointData_.size; }
  void setSymbolSize(double r) { pointData_.size = r; update(); }

  CSymbol2D::Type symbolType() const { return pointData_.symbol; }
  void setSymbolType(CSymbol2D::Type t) { pointData_.symbol = t; update(); }

  QString symbolName() const;
  void setSymbolName(const QString &s);

  //---

  QColor pointColor    (int i, int n) const;
  QColor lineColor     (int i, int n) const;
  QColor fillUnderColor(int i, int n) const;

  //---

  void updateRange();

  void initObjs(bool force=false);

  int numSets() const;

  int getSetColumn(int i) const;

  void paintEvent(QPaintEvent *) override;

 private:
  int           xColumn_    { 0 };
  int           yColumn_    { 1 };
  Columns       yColumns_;
  int           nameColumn_ { -1 };
  bool          bivariate_  { false };
  bool          stacked_    { false };
  PointData     pointData_;
  LineData      lineData_;
  FillUnderData fillUnderData_;
};

#endif
