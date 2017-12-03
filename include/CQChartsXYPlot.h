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
  CQChartsXYBiLineObj(CQChartsXYPlot *plot, const CQChartsGeom::BBox &rect, double x,
                      double y1, double y2, const QModelIndex &ind, int i, int n);

  QString calcId() const override;

  bool visible() const override;

  bool inside(const CQChartsGeom::Point &p) const override;

  void mousePress(const CQChartsGeom::Point &) override;

  bool isIndex(const QModelIndex &) const override;

  void draw(QPainter *p, const CQChartsPlot::Layer &) override;

 private:
  CQChartsXYPlot *plot_ { nullptr };
  double          x_    { 0.0 };
  double          y1_   { 0.0 };
  double          y2_   { 0.0 };
  QModelIndex     ind_;
  int             i_    { -1 };
  int             n_    { -1 };
};

//---

class CQChartsXYImpulseLineObj : public CQChartsPlotObj {
  Q_OBJECT

 public:
  CQChartsXYImpulseLineObj(CQChartsXYPlot *plot, const CQChartsGeom::BBox &rect,
                           double x1, double y1, double x2, double y2,
                           const QModelIndex &ind, int iset, int nset);

  QString calcId() const override;

  bool visible() const override;

  bool inside(const CQChartsGeom::Point &p) const override;

  void mousePress(const CQChartsGeom::Point &) override;

  bool isIndex(const QModelIndex &) const override;

  void draw(QPainter *p, const CQChartsPlot::Layer &) override;

 private:
  CQChartsXYPlot *plot_ { nullptr };
  double          x1_   { 0.0 };
  double          y1_   { 0.0 };
  double          x2_   { 0.0 };
  double          y2_   { 0.0 };
  QModelIndex     ind_;
  int             iset_ { -1 };
  int             nset_ { -1 };
};

//---

class CQChartsXYPointObj : public CQChartsPlotObj {
  Q_OBJECT

 public:
  CQChartsXYPointObj(CQChartsXYPlot *plot, const CQChartsGeom::BBox &rect, double x, double y,
                     double size, const QModelIndex &ind, int iset, int nset, int i, int n);

 ~CQChartsXYPointObj();

  QString calcId() const override;

  void setLabel(const QString &label);

  void setColor(const QColor &c);

  void setSymbol(CQChartsPlotSymbol::Type type);

  bool visible() const override;

  bool inside(const CQChartsGeom::Point &p) const override;

  void mousePress(const CQChartsGeom::Point &) override;

  bool isIndex(const QModelIndex &) const override;

  void draw(QPainter *p, const CQChartsPlot::Layer &) override;

 private:
  using OptColor = boost::optional<CQChartsPaletteColor>;

  struct ExtraData {
    QString                  label;
    OptColor                 color;
    CQChartsPlotSymbol::Type symbol { CQChartsPlotSymbol::Type::NONE };
  };

  CQChartsXYPlot* plot_  { nullptr };
  double          x_     { 0.0 };
  double          y_     { 0.0 };
  double          size_  { -1 };
  QModelIndex     ind_;
  int             iset_  { -1 };
  int             nset_  { -1 };
  int             i_     { -1 };
  int             n_     { -1 };
  ExtraData*      edata_ { nullptr };
};

//---

class CQChartsSmooth;

class CQChartsXYPolylineObj : public CQChartsPlotObj {
  Q_OBJECT

 public:
  CQChartsXYPolylineObj(CQChartsXYPlot *plot, const CQChartsGeom::BBox &rect,
                        const QPolygonF &poly, const QString &name, int i, int n);

 ~CQChartsXYPolylineObj();

  QString calcId() const override;

  bool visible() const override;

  bool inside(const CQChartsGeom::Point &p) const override;

  bool interpY(double x, std::vector<double> &yvals) const;

  void mousePress(const CQChartsGeom::Point &) override;

  bool isIndex(const QModelIndex &) const override;

  void draw(QPainter *p, const CQChartsPlot::Layer &) override;

 private:
  void initSmooth();

 private:
  CQChartsXYPlot *plot_   { nullptr };
  QPolygonF       poly_;
  QString         name_;
  int             i_      { -1 };
  int             n_      { -1 };
  CQChartsSmooth* smooth_ { nullptr };
};

//---

class CQChartsXYPolygonObj : public CQChartsPlotObj {
  Q_OBJECT

 public:
  CQChartsXYPolygonObj(CQChartsXYPlot *plot, const CQChartsGeom::BBox &rect,
                       const QPolygonF &poly, const QString &name, int i, int n);

 ~CQChartsXYPolygonObj();

  QString calcId() const override;

  bool visible() const override;

  bool inside(const CQChartsGeom::Point &p) const override;

  void mousePress(const CQChartsGeom::Point &) override;

  bool isIndex(const QModelIndex &) const override;

  void draw(QPainter *p, const CQChartsPlot::Layer &) override;

 private:
  void initSmooth();

 private:
  CQChartsXYPlot *plot_   { nullptr };
  QPolygonF       poly_;
  QString         name_;
  int             i_      { -1 };
  int             n_      { -1 };
  CQChartsSmooth* smooth_ { nullptr };
};

//---

#include <CQChartsKey.h>

class CQChartsXYKeyColor : public CQChartsKeyColorBox {
  Q_OBJECT

 public:
  CQChartsXYKeyColor(CQChartsXYPlot *plot, int i, int n);

  bool mousePress(const CQChartsGeom::Point &p) override;

  bool mouseMove(const CQChartsGeom::Point &) override { return true; }

  QBrush fillBrush() const override;
};

class CQChartsXYKeyLine : public CQChartsKeyItem {
  Q_OBJECT

 public:
  CQChartsXYKeyLine(CQChartsXYPlot *plot, int i, int n);

  QSizeF size() const override;

  bool mousePress(const CQChartsGeom::Point &p) override;

  bool mouseMove(const CQChartsGeom::Point &) override { return true; }

  void draw(QPainter *p, const CQChartsGeom::BBox &rect) override;

protected:
  CQChartsPlot *plot_ { nullptr };
  int           i_    { 0 };
  int           n_    { 0 };
};

class CQChartsXYKeyText : public CQChartsKeyText {
  Q_OBJECT

 public:
  CQChartsXYKeyText(CQChartsXYPlot *plot, int i, const QString &text);

  QColor interpTextColor(int i, int n) const override;

 private:
  int i_ { 0 };
};

//---

class CQChartsXYPlotType : public CQChartsPlotType {
 public:
  CQChartsXYPlotType();

  QString name() const override { return "xy"; }
  QString desc() const override { return "XY"; }

  void addParameters() override;

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
  Q_PROPERTY(double  bivariateLineWidth READ bivariateLineWidth WRITE setBivariateLineWidth)
  Q_PROPERTY(bool    stacked            READ isStacked          WRITE setStacked           )
  Q_PROPERTY(bool    cumulative         READ isCumulative       WRITE setCumulative        )
  Q_PROPERTY(bool    impulse            READ isImpulse          WRITE setImpulse           )
  Q_PROPERTY(QString impulseColor       READ impulseColorStr    WRITE setImpulseColorStr   )
  Q_PROPERTY(double  impulseWidth       READ impulseWidth       WRITE setImpulseWidth      )

  // point:
  //  display, color, symbol, size
  Q_PROPERTY(bool    points             READ isPoints             WRITE setPoints              )
  Q_PROPERTY(QString symbolName         READ symbolName           WRITE setSymbolName          )
  Q_PROPERTY(double  symbolSize         READ symbolSize           WRITE setSymbolSize          )
  Q_PROPERTY(bool    symbolStroked      READ isSymbolStroked      WRITE setSymbolStroked       )
  Q_PROPERTY(QString pointsStrokeColor  READ pointsStrokeColorStr WRITE setPointsStrokeColorStr)
  Q_PROPERTY(double  symbolLineWidth    READ symbolLineWidth      WRITE setSymbolLineWidth     )
  Q_PROPERTY(bool    symbolFilled       READ isSymbolFilled       WRITE setSymbolFilled        )
  Q_PROPERTY(QString pointsFillColor    READ pointsFillColorStr   WRITE setPointsFillColorStr  )

  // line:
  //  display, stroke
  Q_PROPERTY(bool    lines              READ isLines            WRITE setLines             )
  Q_PROPERTY(bool    linesSelectable    READ isLinesSelectable  WRITE setLinesSelectable   )
  Q_PROPERTY(QString linesColor         READ linesColorStr      WRITE setLinesColorStr     )
  Q_PROPERTY(double  linesWidth         READ linesWidth         WRITE setLinesWidth        )
  Q_PROPERTY(bool    roundedLines       READ isRoundedLines     WRITE setRoundedLines      )

  // fill under:
  //  display, brush
  Q_PROPERTY(bool    fillUnder          READ isFillUnder        WRITE setFillUnder         )
  Q_PROPERTY(QString fillUnderColor     READ fillUnderColorStr  WRITE setFillUnderColorStr )
  Q_PROPERTY(double  fillUnderAlpha     READ fillUnderAlpha     WRITE setFillUnderAlpha    )
  Q_PROPERTY(Pattern fillUnderPattern   READ fillUnderPattern   WRITE setFillUnderPattern  )
  Q_PROPERTY(QString fillUnderPos       READ fillUnderPosStr    WRITE setFillUnderPosStr   )
  Q_PROPERTY(QString fillUnderSide      READ fillUnderSide      WRITE setFillUnderSide     )

  // data label
  Q_PROPERTY(QString dataLabelColor     READ dataLabelColorStr  WRITE setDataLabelColorStr )
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

    FillUnderData(CQChartsXYPlot *plot) :
     fillObj(plot) {
      fillObj.setAlpha(0.5);
    }

    void setPosStr(const QString &s) {
      posStr = s;
    }
  };

  struct DataLabelData {
    bool                 shown { true };
    CQChartsPaletteColor color;
    double               angle { 0.0 };
  };

 public:
  CQChartsXYPlot(CQChartsView *view, const ModelP &model);

 ~CQChartsXYPlot();

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

  // bivariate
  bool isBivariate() const { return bivariateLineObj_->isDisplayed(); }
  void setBivariate(bool b) { bivariateLineObj_->setDisplayed(b); updateObjs(); }

  double bivariateLineWidth() const { return bivariateLineObj_->width(); }
  void setBivariateLineWidth(double r) { bivariateLineObj_->setWidth(r); update(); }

  //---

  // stacked, cumulative
  bool isStacked() const { return stacked_; }
  void setStacked(bool b) { stacked_ = b; updateRangeAndObjs(); }

  bool isCumulative() const { return cumulative_; }
  void setCumulative(bool b) { cumulative_ = b; updateObjs(); }

  //---

  // points
  bool isPoints() const { return pointObj_->isDisplayed(); }
  void setPoints(bool b) { pointObj_->setDisplayed(b); updateObjs(); }

  QString pointsStrokeColorStr() const;
  void setPointsStrokeColorStr(const QString &str);

  QColor interpPointStrokeColor(int i, int n) const;

  QString pointsFillColorStr() const;
  void setPointsFillColorStr(const QString &str);

  QColor interpPointFillColor(int i, int n) const;

  //---

  // lines
  bool isLines() const { return lineObj_->isDisplayed(); }
  void setLines(bool b) { lineObj_->setDisplayed(b); updateObjs(); }

  bool isLinesSelectable() const { return linesSelectable_; }
  void setLinesSelectable(bool b) { linesSelectable_ = b; update(); }

  QString linesColorStr() const;
  void setLinesColorStr(const QString &str);

  QColor interpLineColor(int i, int n) const;

  double linesWidth() const { return lineObj_->width(); }
  void setLinesWidth(double w) { lineObj_->setWidth(w); update(); }

  bool isRoundedLines() const { return roundedLines_; }
  void setRoundedLines(bool b) { roundedLines_ = b; update(); }

  //---

  // fill under
  bool isFillUnder() const { return fillUnderData_.fillObj.isVisible(); }
  void setFillUnder(bool b) { fillUnderData_.fillObj.setVisible(b); updateObjs(); }

  QString fillUnderColorStr() const;
  void setFillUnderColorStr(const QString &str);

  QColor interpFillUnderColor(int i, int n) const;

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
  bool isImpulse() const { return impulseObj_->isDisplayed(); }
  void setImpulse(bool b) { impulseObj_->setDisplayed(b); updateObjs(); }

  QString impulseColorStr() const;
  void setImpulseColorStr(const QString &str);

  QColor interpImpulseColor(int i, int n) const;

  double impulseWidth() const { return impulseObj_->width(); }
  void setImpulseWidth(double w) { impulseObj_->setWidth(w); update(); }

  //---

  // symbol
  QString symbolName() const { return pointObj_->symbolName(); }
  void setSymbolName(const QString &s) { pointObj_->setSymbolName(s); update(); }

  CQChartsPlotSymbol::Type symbolType() const { return pointObj_->symbolType(); }
  void setSymbolType(CQChartsPlotSymbol::Type t) { pointObj_->setSymbolType(t); update(); }

  double symbolSize() const { return pointObj_->size(); }
  void setSymbolSize(double r) { pointObj_->setSize(r); update(); }

  bool isSymbolStroked() const { return pointObj_->isStroked(); }
  void setSymbolStroked(bool b) { pointObj_->setStroked(b); update(); }

  double symbolLineWidth() const { return pointObj_->lineWidth(); }
  void setSymbolLineWidth(double r) { pointObj_->setLineWidth(r); update(); }

  bool isSymbolFilled() const { return pointObj_->isFilled(); }
  void setSymbolFilled(bool b) { pointObj_->setFilled(b); update(); }

  //---

  // data label
  QString dataLabelColorStr() const { return dataLabelData_.color.colorStr(); }
  void setDataLabelColorStr(const QString &s) { dataLabelData_.color.setColorStr(s); update(); }

  QColor interpDataLabelColor(int i, int n);

  double dataLabelAngle() const { return dataLabelData_.angle; }
  void setDataLabelAngle(double r) { dataLabelData_.angle = r; }

  //---

  // bivariate line
  void drawBivariateLine(QPainter *painter, const QPointF &p1, const QPointF &p2, const QColor &c);

  //---

  QColor interpPaletteColor(int i, int n, bool scale=false) const override;

  //---

  void addProperties() override;

  void updateRange(bool apply=true) override;

  void postInit() override;

  void initObjs() override;

  //---

  void addPolyLine(const QPolygonF &polyLine, int i, int n, const QString &name);

  void addPolygon(const QPolygonF &poly, int i, int n, const QString &name);

  QString valueName(int iset, int irow) const;

  void addKeyItems(CQChartsKey *key) override;

  int numSets() const;

  int getSetColumn(int i) const;

  //---

  bool probe(ProbeData &probeData) const override;

  bool interpY(double x, std::vector<double> &yvals) const;

  void draw(QPainter *) override;

 private:
  int               xColumn_           { 0 };
  int               yColumn_           { 1 };
  Columns           yColumns_;
  int               nameColumn_        { -1 };
  int               sizeColumn_        { -1 };
  int               pointLabelColumn_  { -1 };
  int               pointColorColumn_  { -1 };
  int               pointSymbolColumn_ { -1 };
  bool              stacked_           { false };
  bool              cumulative_        { false };
  CQChartsPointObj* pointObj_          { nullptr };
  bool              linesSelectable_   { false };
  CQChartsLineObj*  lineObj_           { nullptr };
  bool              roundedLines_      { false };
  FillUnderData     fillUnderData_;
  CQChartsLineObj*  impulseObj_        { nullptr };
  DataLabelData     dataLabelData_;
  CQChartsLineObj*  bivariateLineObj_  { nullptr };
};

#endif
