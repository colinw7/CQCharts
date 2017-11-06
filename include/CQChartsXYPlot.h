#ifndef CQChartsXYPlot_H
#define CQChartsXYPlot_H

#include <CQChartsPlot.h>
#include <CQChartsPlotObj.h>
#include <CQChartsLineObj.h>
#include <CQChartsPointObj.h>
#include <CQChartsFillObj.h>
#include <CQChartsUtil.h>

class CQChartsXYPlot;

class CQChartsXYBiLineObj : public CQChartsPlotObj {
  Q_OBJECT

 public:
  CQChartsXYBiLineObj(CQChartsXYPlot *plot, const CBBox2D &rect, double x,
                      double y1, double y2, int i, int n);

  bool visible() const override;

  bool inside(const CPoint2D &p) const override;

  void mousePress(const CPoint2D &) override;

  void draw(QPainter *p, const CQChartsPlot::Layer &) override;

 private:
  CQChartsXYPlot *plot_ { nullptr };
  double          x_    { 0.0 };
  double          y1_   { 0.0 };
  double          y2_   { 0.0 };
  int             i_    { -1 };
  int             n_    { -1 };
};

//---

class CQChartsXYImpulseLineObj : public CQChartsPlotObj {
  Q_OBJECT

 public:
  CQChartsXYImpulseLineObj(CQChartsXYPlot *plot, const CBBox2D &rect, double x1, double y1,
                           double x2, double y2, int i, int n);

  bool visible() const override;

  bool inside(const CPoint2D &p) const override;

  void mousePress(const CPoint2D &) override;

  void draw(QPainter *p, const CQChartsPlot::Layer &) override;

 private:
  CQChartsXYPlot *plot_ { nullptr };
  double          x1_   { 0.0 };
  double          y1_   { 0.0 };
  double          x2_   { 0.0 };
  double          y2_   { 0.0 };
  int             i_    { -1 };
  int             n_    { -1 };
};

//---

class CQChartsXYPointObj : public CQChartsPlotObj {
  Q_OBJECT

 public:
  CQChartsXYPointObj(CQChartsXYPlot *plot, const CBBox2D &rect, double x, double y, double size,
                     int iset, int nset, int i, int n);

 ~CQChartsXYPointObj();

  void setLabel(const QString &label);

  void setColor(const QColor &c);

  void setSymbol(CSymbol2D::Type type);

  bool visible() const override;

  bool inside(const CPoint2D &p) const override;

  void mousePress(const CPoint2D &) override;

  void draw(QPainter *p, const CQChartsPlot::Layer &) override;

 private:
  struct ExtraData {
    QString         label;
    QColor          c;
    CSymbol2D::Type symbol { CSymbol2D::Type::NONE };
  };

  CQChartsXYPlot* plot_  { nullptr };
  double          x_     { 0.0 };
  double          y_     { 0.0 };
  double          size_  { -1 };
  int             iset_  { -1 };
  int             nset_  { -1 };
  int             i_     { -1 };
  int             n_     { -1 };
  ExtraData*      edata_ { nullptr };
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

  void mousePress(const CPoint2D &) override;

  void draw(QPainter *p, const CQChartsPlot::Layer &) override;

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

  void mousePress(const CPoint2D &) override;

  void draw(QPainter *p, const CQChartsPlot::Layer &) override;

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

  QBrush fillBrush() const override;
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

class CQChartsXYKeyText : public CQChartsKeyText {
  Q_OBJECT

 public:
  CQChartsXYKeyText(CQChartsXYPlot *plot, int i, const QString &text);

  QColor textColor() const override;

 private:
  int i_ { 0 };
};

//---

class CQChartsXYPlotType : public CQChartsPlotType {
 public:
  CQChartsXYPlotType();

  QString name() const override { return "xy"; }
  QString desc() const override { return "XY"; }

  CQChartsPlot *create(CQChartsView *view, const ModelP &model) const override;
};

//---

class CQChartsXYPlot : public CQChartsPlot {
  Q_OBJECT

  // columns
  Q_PROPERTY(int     xColumn            READ xColumn            WRITE setXColumn           )
  Q_PROPERTY(int     yColumn            READ yColumn            WRITE setYColumn           )
  Q_PROPERTY(QString yColumns           READ yColumnsStr        WRITE setYColumnsStr       )
  Q_PROPERTY(int     nameColumn         READ nameColumn         WRITE setNameColumn        )
  Q_PROPERTY(int     sizeColumn         READ sizeColumn         WRITE setSizeColumn        )
  Q_PROPERTY(int     pointLabelColumn   READ pointLabelColumn   WRITE setPointLabelColumn  )
  Q_PROPERTY(int     pointColorColumn   READ pointColorColumn   WRITE setPointColorColumn  )
  Q_PROPERTY(int     pointSymbolColumn  READ pointSymbolColumn  WRITE setPointSymbolColumn )

  // display:
  //  bivariate, stacked, cumulative, impulse
  Q_PROPERTY(bool    bivariate          READ isBivariate        WRITE setBivariate         )
  Q_PROPERTY(bool    stacked            READ isStacked          WRITE setStacked           )
  Q_PROPERTY(bool    cumulative         READ isCumulative       WRITE setCumulative        )
  Q_PROPERTY(bool    impulse            READ isImpulse          WRITE setImpulse           )

  // point:
  //  display, color, symbol, size
  Q_PROPERTY(bool    points             READ isPoints           WRITE setPoints            )
  Q_PROPERTY(QString pointsColor        READ pointsColorStr     WRITE setPointsColorStr    )
  Q_PROPERTY(QString symbolName         READ symbolName         WRITE setSymbolName        )
  Q_PROPERTY(double  symbolSize         READ symbolSize         WRITE setSymbolSize        )
  Q_PROPERTY(bool    symbolFilled       READ isSymbolFilled     WRITE setSymbolFilled      )

  // line:
  //  display, stroke
  Q_PROPERTY(bool    lines              READ isLines            WRITE setLines             )
  Q_PROPERTY(bool    linesSelectable    READ isLinesSelectable  WRITE setLinesSelectable   )
  Q_PROPERTY(QString linesColor         READ linesColorStr      WRITE setLinesColorStr     )
  Q_PROPERTY(double  linesWidth         READ linesWidth         WRITE setLinesWidth        )
  Q_PROPERTY(double  bivariateLineWidth READ bivariateLineWidth WRITE setBivariateLineWidth)

  // fill under:
  //  display, brush
  Q_PROPERTY(bool    fillUnder          READ isFillUnder        WRITE setFillUnder         )
  Q_PROPERTY(QString fillUnderColor     READ fillUnderColorStr  WRITE setFillUnderColorStr )
  Q_PROPERTY(double  fillUnderAlpha     READ fillUnderAlpha     WRITE setFillUnderAlpha    )
  Q_PROPERTY(Pattern fillUnderPattern   READ fillUnderPattern   WRITE setFillUnderPattern  )
  Q_PROPERTY(QString fillUnderPos       READ fillUnderPosStr    WRITE setFillUnderPosStr   )
  Q_PROPERTY(QString fillUnderSide      READ fillUnderSide      WRITE setFillUnderSide     )

  // data label
  Q_PROPERTY(QColor  dataLabelColor     READ dataLabelColor     WRITE setDataLabelColor    )
  Q_PROPERTY(double  dataLabelAngle     READ dataLabelAngle     WRITE setDataLabelAngle    )

  Q_ENUMS(Pattern)

 public:
  enum class Pattern {
    SOLID,
    HATCH,
    DENSE,
    HORIZ,
    VERT,
    FDIAG,
    BDIAG
  };

 private:
  struct FillUnderData {
    CQChartsFillObj fillObj;
    QString         posStr { "ymin" };
    QString         side   { "both" };

    FillUnderData() {
     fillObj.setColor(QColor(128, 128, 128));
     fillObj.setAlpha(0.5);
    }

    void setPosStr(const QString &s) {
      posStr = s;
    }
  };

  struct DataLabelData {
    bool   shown { true };
    QColor color;
    double angle { 0.0 };
  };

 public:
  CQChartsXYPlot(CQChartsView *view, const ModelP &model);

  //---

  // columns
  int xColumn() const { return xColumn_; }
  void setXColumn(int i) { xColumn_ = i; update(); }

  int yColumn() const { return yColumn_; }

  void setYColumn(int i) {
    yColumn_ = i;

    yColumns_.clear();

    if (yColumn_ >= 0)
      yColumns_.push_back(yColumn_);

    update();
  }

  const Columns &yColumns() const { return yColumns_; }

  void setYColumns(const Columns &yColumns) {
    yColumns_ = yColumns;

    if (! yColumns_.empty())
      yColumn_ = yColumns_[0];
    else
      yColumn_ = -1;

    update();
  }

  QString yColumnsStr() const;
  bool setYColumnsStr(const QString &s);

  int nameColumn() const { return nameColumn_; }
  void setNameColumn(int i) { nameColumn_ = i; update(); }

  int sizeColumn() const { return sizeColumn_; }
  void setSizeColumn(int i) { sizeColumn_ = i; update(); }

  int pointLabelColumn() const { return pointLabelColumn_; }
  void setPointLabelColumn(int i) { pointLabelColumn_ = i; update(); }

  int pointColorColumn() const { return pointColorColumn_; }
  void setPointColorColumn(int i) { pointColorColumn_ = i; update(); }

  int pointSymbolColumn() const { return pointSymbolColumn_; }
  void setPointSymbolColumn(int i) { pointSymbolColumn_ = i; update(); }

  //---

  // bivariate, stacked, cumulative
  bool isBivariate() const { return bivariate_; }
  void setBivariate(bool b) { bivariate_ = b; updateObjs(); }

  bool isStacked() const { return stacked_; }
  void setStacked(bool b) { stacked_ = b; updateObjs(); }

  bool isCumulative() const { return cumulative_; }
  void setCumulative(bool b) { cumulative_ = b; updateObjs(); }

  //---

  // points
  bool isPoints() const { return pointObj_.isDisplayed(); }
  void setPoints(bool b) { pointObj_.setDisplayed(b); updateObjs(); }

  QString pointsColorStr() const;
  void setPointsColorStr(const QString &str);

  //---

  // lines
  bool isLines() const { return lineObj_.isDisplayed(); }
  void setLines(bool b) { lineObj_.setDisplayed(b); updateObjs(); }

  bool isLinesSelectable() const { return linesSelectable_; }
  void setLinesSelectable(bool b) { linesSelectable_ = b; update(); }

  QString linesColorStr() const;
  void setLinesColorStr(const QString &str);

  double linesWidth() const { return lineObj_.width(); }
  void setLinesWidth(double w) { lineObj_.setWidth(w); update(); }

  //---

  // fill under
  bool isFillUnder() const { return fillUnderData_.fillObj.isVisible(); }
  void setFillUnder(bool b) { fillUnderData_.fillObj.setVisible(b); updateObjs(); }

  QString fillUnderColorStr() const;
  void setFillUnderColorStr(const QString &str);

  double fillUnderAlpha() const { return fillUnderData_.fillObj.alpha(); }
  void setFillUnderAlpha(double r) { fillUnderData_.fillObj.setAlpha(r); }

  Pattern fillUnderPattern() const { return (Pattern) fillUnderData_.fillObj.pattern(); }
  void setFillUnderPattern(const Pattern &p) {
    fillUnderData_.fillObj.setPattern((CQChartsFillObj::Pattern) p); update(); }

  const QString &fillUnderPosStr() const { return fillUnderData_.posStr; }
  void setFillUnderPosStr(const QString &s) { fillUnderData_.setPosStr(s); updateObjs(); }

  QPointF fillUnderPos(double x, double y) const;

  const QString &fillUnderSide() const { return fillUnderData_.side; }
  void setFillUnderSide(const QString &s) { fillUnderData_.side = s; updateObjs(); }

  //---

  // impulse
  bool isImpulse() const { return impulse_; }
  void setImpulse(bool b) { impulse_ = b; updateObjs(); }

  //---

  // symbol
  double symbolSize() const { return pointObj_.size(); }
  void setSymbolSize(double r) { pointObj_.setSize(r); update(); }

  CSymbol2D::Type symbolType() const { return pointObj_.symbolType(); }
  void setSymbolType(CSymbol2D::Type t) { pointObj_.setSymbolType(t); update(); }

  QString symbolName() const { return pointObj_.symbolName(); }
  void setSymbolName(const QString &s) { pointObj_.setSymbolName(s); update(); }

  bool isSymbolFilled() const { return pointObj_.isFilled(); }
  void setSymbolFilled(bool b) { pointObj_.setFilled(b); update(); }

  //---

  // data label
  const QColor &dataLabelColor() const { return dataLabelData_.color; }
  void setDataLabelColor(const QColor &c) { dataLabelData_.color = c; update(); }

  double dataLabelAngle() const { return dataLabelData_.angle; }
  void setDataLabelAngle(double r) { dataLabelData_.angle = r; }

  //---

  // bivariate line
  double bivariateLineWidth() const { return bivariateLineObj_.width(); }
  void setBivariateLineWidth(double r) { bivariateLineObj_.setWidth(r); update(); }

  void drawBivariateLine(QPainter *painter, const QPointF &p1, const QPointF &p2, const QColor &c);

  //---

  QColor pointColor    (int i, int n) const;
  QColor lineColor     (int i, int n) const;
  QColor fillUnderColor(int i, int n) const;

  QColor paletteColor(int i, int n, const QColor &def=QColor(0,0,0)) const override;

  //---

  void addProperties() override;

  void updateRange(bool apply=true) override;

  void postInit() override;

  void initObjs() override;

  //---

  void addPolyLine(const QPolygonF &polyLine, int i, int n, const QString &name);

  void addPolygon(const QPolygonF &poly, int i, int n, const QString &name);

  void addKeyItems(CQChartsKey *key) override;

  int numSets() const;

  int getSetColumn(int i) const;

  //---

  bool interpY(double x, std::vector<double> &yvals) const override;

  void draw(QPainter *) override;

 private:
  int              xColumn_           { 0 };
  int              yColumn_           { 1 };
  Columns          yColumns_;
  int              nameColumn_        { -1 };
  int              sizeColumn_        { -1 };
  int              pointLabelColumn_  { -1 };
  int              pointColorColumn_  { -1 };
  int              pointSymbolColumn_ { -1 };
  bool             bivariate_         { false };
  bool             stacked_           { false };
  bool             cumulative_        { false };
  CQChartsPointObj pointObj_;
  bool             linesSelectable_   { false };
  CQChartsLineObj  lineObj_;
  FillUnderData    fillUnderData_;
  bool             impulse_           { false };
  DataLabelData    dataLabelData_;
  CQChartsLineObj  bivariateLineObj_;
};

#endif
