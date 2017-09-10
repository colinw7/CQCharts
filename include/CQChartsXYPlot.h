#ifndef CQChartsXYPlot_H
#define CQChartsXYPlot_H

#include <CQChartsPlot.h>
#include <CQChartsPlotObj.h>
#include <CQUtil.h>

class CQChartsXYPlot;

class CQChartsXYBiLineObj : public CQChartsPlotObj {
  Q_OBJECT

 public:
  CQChartsXYBiLineObj(CQChartsXYPlot *plot, const CBBox2D &rect, double x,
                      double y1, double y2, int i, int n);

  bool visible() const override;

  bool inside(const CPoint2D &p) const override;

  void draw(QPainter *p) override;

 private:
  CQChartsXYPlot *plot_ { nullptr };
  double          x_    { 0.0 };
  double          y1_   { 0.0 };
  double          y2_   { 0.0 };
  int             i_    { -1 };
  int             n_    { -1 };
};

//---

class CQChartsXYPointObj : public CQChartsPlotObj {
  Q_OBJECT

 public:
  CQChartsXYPointObj(CQChartsXYPlot *plot, const CBBox2D &rect, double x, double y,
                     int iset, int nset, int i, int n);

  bool visible() const override;

  bool inside(const CPoint2D &p) const override;

  void draw(QPainter *p) override;

 private:
  CQChartsXYPlot *plot_ { nullptr };
  double          x_    { 0.0 };
  double          y_    { 0.0 };
  int             iset_ { -1 };
  int             nset_ { -1 };
  int             i_    { -1 };
  int             n_    { -1 };
};

//---

class CQChartsXYPolylineObj : public CQChartsPlotObj {
  Q_OBJECT

 public:
  CQChartsXYPolylineObj(CQChartsXYPlot *plot, const CBBox2D &rect,
                        const QPolygonF &poly, int i, int n);

  bool visible() const override;

  bool inside(const CPoint2D &p) const override;

  bool interpY(double x, std::vector<double> &yvals) const;

  void draw(QPainter *p) override;

 private:
  CQChartsXYPlot *plot_ { nullptr };
  QPolygonF       poly_;
  int             i_    { -1 };
  int             n_    { -1 };
};

//---

class CQChartsXYPolygonObj : public CQChartsPlotObj {
  Q_OBJECT

 public:
  CQChartsXYPolygonObj(CQChartsXYPlot *plot, const CBBox2D &rect,
                       const QPolygonF &poly, int i, int n);

  bool visible() const override;

  bool inside(const CPoint2D &p) const override;

  void draw(QPainter *p) override;

 private:
  CQChartsXYPlot *plot_ { nullptr };
  QPolygonF       poly_;
  int             i_    { -1 };
  int             n_    { -1 };
};

//---

#include <CQChartsKey.h>

class CQChartsXYKeyColor : public CQChartsKeyColorBox {
  Q_OBJECT

 public:
  CQChartsXYKeyColor(CQChartsXYPlot *plot, int i, int n);

  bool mousePress(const CPoint2D &p) override;

  bool mouseMove(const CPoint2D &) override { return true; }

  QColor fillColor() const override;
};

class CQChartsXYKeyLine : public CQChartsKeyItem {
  Q_OBJECT

 public:
  CQChartsXYKeyLine(CQChartsXYPlot *plot, int i, int n);

  QSizeF size() const override;

  bool mousePress(const CPoint2D &p) override;

  bool mouseMove(const CPoint2D &) override { return true; }

  void draw(QPainter *p, const CBBox2D &rect) override;

protected:
  CQChartsPlot *plot_ { nullptr };
  int           i_    { 0 };
  int           n_    { 0 };
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
  Q_PROPERTY(double  bivariateWidth READ bivariateWidth    WRITE setBivariateWidth   )
  Q_PROPERTY(bool    stacked        READ isStacked         WRITE setStacked          )
  Q_PROPERTY(bool    cumulative     READ isCumulative      WRITE setCumulative       )
  Q_PROPERTY(bool    points         READ isPoints          WRITE setPoints           )
  Q_PROPERTY(QString pointsColor    READ pointsColorStr    WRITE setPointsColorStr   )
  Q_PROPERTY(bool    lines          READ isLines           WRITE setLines            )
  Q_PROPERTY(QString linesColor     READ linesColorStr     WRITE setLinesColorStr    )
  Q_PROPERTY(double  linesWidth     READ linesWidth        WRITE setLinesWidth       )
  Q_PROPERTY(bool    fillUnder      READ isFillUnder       WRITE setFillUnder        )
  Q_PROPERTY(QString fillUnderColor READ fillUnderColorStr WRITE setFillUnderColorStr)
  Q_PROPERTY(QString symbolName     READ symbolName        WRITE setSymbolName       )
  Q_PROPERTY(double  symbolSize     READ symbolSize        WRITE setSymbolSize       )
  Q_PROPERTY(bool    symbolFilled   READ isSymbolFilled    WRITE setSymbolFilled     )

 private:
  struct PointData {
    bool            shown   { true };
    CSymbol2D::Type symbol  { CSymbol2D::Type::CROSS };
    QColor          color   { 0, 0, 0 };
    bool            palette { true };
    double          size    { 4 };
    bool            filled  { false };

    PointData() { }
  };

  struct LineData {
    bool   shown   { true };
    QColor color   { 0, 0, 0 };
    double width   { 0 };
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
  CQChartsXYPlot(CQChartsView *view, QAbstractItemModel *model);

  const char *typeName() const override { return "XY"; }

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

  double bivariateWidth() const { return bivariateWidth_; }
  void setBivariateWidth(double r) { bivariateWidth_ = r; update(); }

  bool isStacked() const { return stacked_; }
  void setStacked(bool b) { stacked_ = b; initObjs(/*force*/true); update(); }

  bool isCumulative() const { return cumulative_; }
  void setCumulative(bool b) { cumulative_ = b; initObjs(/*force*/true); update(); }

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

  double linesWidth() const { return lineData_.width; }
  void setLinesWidth(double w) { lineData_.width = w; update(); }

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

  bool isSymbolFilled() const { return pointData_.filled; }
  void setSymbolFilled(bool b) { pointData_.filled = b; update(); }

  //---

  QColor pointColor    (int i, int n) const;
  QColor lineColor     (int i, int n) const;
  QColor fillUnderColor(int i, int n) const;

  QColor paletteColor(int i, int n, const QColor &def=QColor(0,0,0)) const override;

  //---

  void addProperties();

  void updateRange();

  void initObjs(bool force=false);

  void addPolyLine(const QPolygonF &polyLine, int i, int n, const QString &name);

  void addPolygon(const QPolygonF &poly, int i, int n, const QString &name);

  void addKeyItems(CQChartsKey *key) override;

  int numSets() const;

  int getSetColumn(int i) const;

  //---

  bool isSetHidden(int i) const {
    auto p = idHidden_.find(i);

    if (p == idHidden_.end())
      return false;

    return (*p).second;
  }

  void setSetHidden(int i, bool hidden) { idHidden_[i] = hidden; }

  //---

  bool interpY(double x, std::vector<double> &yvals) const override;

  void draw(QPainter *) override;

 private:
  typedef std::map<int,bool> IdHidden;

  int           xColumn_        { 0 };
  int           yColumn_        { 1 };
  Columns       yColumns_;
  int           nameColumn_     { -1 };
  bool          bivariate_      { false };
  double        bivariateWidth_ { 0.0 };
  bool          stacked_        { false };
  bool          cumulative_     { false };
  PointData     pointData_;
  LineData      lineData_;
  FillUnderData fillUnderData_;
  IdHidden      idHidden_;
};

#endif
