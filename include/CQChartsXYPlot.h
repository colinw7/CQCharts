#ifndef CQChartsXYPlot_H
#define CQChartsXYPlot_H

#include <CQChartsPlot.h>
#include <CQChartsPlotObj.h>
#include <CQChartsUtil.h>

class CQChartsXYPlot;
class CQChartsArrow;

class CQChartsXYBiLineObj : public CQChartsPlotObj {
  Q_OBJECT

 public:
  CQChartsXYBiLineObj(CQChartsXYPlot *plot, const CQChartsGeom::BBox &rect, double x,
                      double y1, double y2, const QModelIndex &ind, int iset, int nset,
                      int i, int n);

  QString calcId() const override;

  bool visible() const override;

  bool inside(const CQChartsGeom::Point &p) const override;

  void getSelectIndices(Indices &inds) const override;

  void addColumnSelectIndex(Indices &inds, const CQChartsColumn &column) const override;

  void draw(QPainter *painter, const CQChartsPlot::Layer &) override;

 private:
  CQChartsXYPlot *plot_ { nullptr };
  double          x_    { 0.0 };
  double          y1_   { 0.0 };
  double          y2_   { 0.0 };
  QModelIndex     ind_;
  int             iset_ { -1 };
  int             nset_ { -1 };
  int             i_    { -1 };
  int             n_    { -1 };
};

//---

class CQChartsXYImpulseLineObj : public CQChartsPlotObj {
  Q_OBJECT

 public:
  CQChartsXYImpulseLineObj(CQChartsXYPlot *plot, const CQChartsGeom::BBox &rect,
                           double x1, double y1, double x2, double y2, const QModelIndex &ind,
                           int iset, int nset, int i, int n);

  QString calcId() const override;

  bool visible() const override;

  bool inside(const CQChartsGeom::Point &p) const override;

  void getSelectIndices(Indices &inds) const override;

  void addColumnSelectIndex(Indices &inds, const CQChartsColumn &column) const override;

  void draw(QPainter *painter, const CQChartsPlot::Layer &) override;

 private:
  CQChartsXYPlot *plot_ { nullptr };
  double          x1_   { 0.0 };
  double          y1_   { 0.0 };
  double          x2_   { 0.0 };
  double          y2_   { 0.0 };
  QModelIndex     ind_;
  int             iset_ { -1 };
  int             nset_ { -1 };
  int             i_    { -1 };
  int             n_    { -1 };
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

  void setColor(const CQChartsColor &c);

  void setSymbol(CQChartsPlotSymbol::Type type);

  void setVector(double vx, double vy);

  bool visible() const override;

  bool inside(const CQChartsGeom::Point &p) const override;

  void getSelectIndices(Indices &inds) const override;

  void addColumnSelectIndex(Indices &inds, const CQChartsColumn &column) const override;

  void draw(QPainter *painter, const CQChartsPlot::Layer &) override;

 private:
  using OptColor = boost::optional<CQChartsColor>;
  using OptPoint = boost::optional<QPointF>;

  struct ExtraData {
    QString                  label;
    OptColor                 color;
    CQChartsPlotSymbol::Type symbol { CQChartsPlotSymbol::Type::NONE };
    OptPoint                 vector;
  };

  CQChartsXYPlot* plot_  { nullptr }; // parent plot
  QPointF         pos_;               // position
  double          size_  { -1 };      // size : TODO support units
  QModelIndex     ind_;               // model index
  int             iset_  { -1 };      // sets index
  int             nset_  { -1 };      // sets size
  int             i_     { -1 };      // points index
  int             n_     { -1 };      // points size
  ExtraData*      edata_ { nullptr }; // extra data
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

  void getSelectIndices(Indices &inds) const override;

  void addColumnSelectIndex(Indices &, const CQChartsColumn &) const override { }

  void draw(QPainter *painter, const CQChartsPlot::Layer &) override;

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

  void getSelectIndices(Indices &inds) const override;

  void addColumnSelectIndex(Indices &, const CQChartsColumn &) const override { }

  void draw(QPainter *painter, const CQChartsPlot::Layer &) override;

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

  bool selectPress(const CQChartsGeom::Point &p) override;

  bool selectMove(const CQChartsGeom::Point &) override { return true; }

  QBrush fillBrush() const override;
};

class CQChartsXYKeyLine : public CQChartsKeyItem {
  Q_OBJECT

 public:
  CQChartsXYKeyLine(CQChartsXYPlot *plot, int i, int n);

  QSizeF size() const override;

  bool selectPress(const CQChartsGeom::Point &p) override;

  bool selectMove(const CQChartsGeom::Point &) override { return true; }

  void draw(QPainter *painter, const CQChartsGeom::BBox &rect) override;

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
  Q_PROPERTY(CQChartsColumn xColumn           READ xColumn           WRITE setXColumn          )
  Q_PROPERTY(CQChartsColumn yColumn           READ yColumn           WRITE setYColumn          )
  Q_PROPERTY(QString        yColumns          READ yColumnsStr       WRITE setYColumnsStr      )
  Q_PROPERTY(CQChartsColumn nameColumn        READ nameColumn        WRITE setNameColumn       )
  Q_PROPERTY(CQChartsColumn sizeColumn        READ sizeColumn        WRITE setSizeColumn       )
  Q_PROPERTY(CQChartsColumn pointLabelColumn  READ pointLabelColumn  WRITE setPointLabelColumn )
  Q_PROPERTY(CQChartsColumn pointColorColumn  READ pointColorColumn  WRITE setPointColorColumn )
  Q_PROPERTY(CQChartsColumn pointSymbolColumn READ pointSymbolColumn WRITE setPointSymbolColumn)
  Q_PROPERTY(CQChartsColumn vectorXColumn     READ vectorXColumn     WRITE setVectorXColumn    )
  Q_PROPERTY(CQChartsColumn vectorYColumn     READ vectorYColumn     WRITE setVectorYColumn    )

  // display:
  //  bivariate, stacked, cumulative, impulse, vectors
  Q_PROPERTY(bool           bivariate          READ isBivariate        WRITE setBivariate         )
  Q_PROPERTY(CQChartsLength bivariateLineWidth READ bivariateLineWidth WRITE setBivariateLineWidth)
  Q_PROPERTY(bool           stacked            READ isStacked          WRITE setStacked           )
  Q_PROPERTY(bool           cumulative         READ isCumulative       WRITE setCumulative        )
  Q_PROPERTY(bool           impulse            READ isImpulse          WRITE setImpulse           )
  Q_PROPERTY(CQChartsColor  impulseColor       READ impulseColor       WRITE setImpulseColor      )
  Q_PROPERTY(double         impulseAlpha       READ impulseAlpha       WRITE setImpulseAlpha      )
  Q_PROPERTY(CQChartsLength impulseWidth       READ impulseWidth       WRITE setImpulseWidth      )
  Q_PROPERTY(bool           vectors            READ isVectors          WRITE setVectors           )

  // point:
  //  display, color, symbol, size
  Q_PROPERTY(bool           points             READ isPoints          WRITE setPoints           )
  Q_PROPERTY(QString        symbolName         READ symbolName        WRITE setSymbolName       )
  Q_PROPERTY(double         symbolSize         READ symbolSize        WRITE setSymbolSize       )
  Q_PROPERTY(bool           symbolStroked      READ isSymbolStroked   WRITE setSymbolStroked    )
  Q_PROPERTY(CQChartsColor  pointsStrokeColor  READ pointsStrokeColor WRITE setPointsStrokeColor)
  Q_PROPERTY(double         pointsStrokeAlpha  READ pointsStrokeAlpha WRITE setPointsStrokeAlpha)
  Q_PROPERTY(CQChartsLength symbolLineWidth    READ symbolLineWidth   WRITE setSymbolLineWidth  )
  Q_PROPERTY(bool           symbolFilled       READ isSymbolFilled    WRITE setSymbolFilled     )
  Q_PROPERTY(CQChartsColor  pointsFillColor    READ pointsFillColor   WRITE setPointsFillColor  )
  Q_PROPERTY(double         pointsFillAlpha    READ pointsFillAlpha   WRITE setPointsFillAlpha  )

  // line:
  //  display, stroke
  Q_PROPERTY(bool           lines           READ isLines           WRITE setLines          )
  Q_PROPERTY(bool           linesSelectable READ isLinesSelectable WRITE setLinesSelectable)
  Q_PROPERTY(CQChartsColor  linesColor      READ linesColor        WRITE setLinesColor     )
  Q_PROPERTY(double         linesAlpha      READ linesAlpha        WRITE setLinesAlpha     )
  Q_PROPERTY(CQChartsLength linesWidth      READ linesWidth        WRITE setLinesWidth     )
  Q_PROPERTY(bool           roundedLines    READ isRoundedLines    WRITE setRoundedLines   )

  // fill under:
  //  display, brush
  Q_PROPERTY(bool          fillUnder        READ isFillUnder      WRITE setFillUnder       )
  Q_PROPERTY(CQChartsColor fillUnderColor   READ fillUnderColor   WRITE setFillUnderColor  )
  Q_PROPERTY(double        fillUnderAlpha   READ fillUnderAlpha   WRITE setFillUnderAlpha  )
  Q_PROPERTY(Pattern       fillUnderPattern READ fillUnderPattern WRITE setFillUnderPattern)
  Q_PROPERTY(QString       fillUnderPos     READ fillUnderPosStr  WRITE setFillUnderPosStr )
  Q_PROPERTY(QString       fillUnderSide    READ fillUnderSide    WRITE setFillUnderSide   )

  // data label
  Q_PROPERTY(CQChartsColor dataLabelColor READ dataLabelColor WRITE setDataLabelColor)
  Q_PROPERTY(double        dataLabelAngle READ dataLabelAngle WRITE setDataLabelAngle)

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

  using Columns = std::vector<CQChartsColumn>;

 private:
  struct FillUnderData {
    CQChartsFillData fillData;
    QString          posStr { "ymin" };
    QString          side   { "both" };

    FillUnderData(CQChartsXYPlot *) {
      fillData.visible = false;
      fillData.color   = CQChartsColor(CQChartsColor::Type::PALETTE);
      fillData.alpha   = 0.5;
    }

    void setPosStr(const QString &s) {
      posStr = s;
    }
  };

 public:
  CQChartsXYPlot(CQChartsView *view, const ModelP &model);

 ~CQChartsXYPlot();

  //---

  // columns
  const CQChartsColumn &xColumn() const { return xColumn_; }
  void setXColumn(const CQChartsColumn &c);

  const CQChartsColumn &yColumn() const { return yColumn_; }
  void setYColumn(const CQChartsColumn &c);

  const Columns &yColumns() const { return yColumns_; }
  void setYColumns(const Columns &yColumns);

  QString yColumnsStr() const;
  bool setYColumnsStr(const QString &s);

  const CQChartsColumn &nameColumn() const { return nameColumn_; }
  void setNameColumn(const CQChartsColumn &c);

  const CQChartsColumn &sizeColumn() const { return sizeColumn_; }
  void setSizeColumn(const CQChartsColumn &c);

  const CQChartsColumn &pointLabelColumn() const { return pointLabelColumn_; }
  void setPointLabelColumn(const CQChartsColumn &c);

  const CQChartsColumn &pointColorColumn() const { return pointColorColumn_; }
  void setPointColorColumn(const CQChartsColumn &c);

  const CQChartsColumn &pointSymbolColumn() const { return pointSymbolColumn_; }
  void setPointSymbolColumn(const CQChartsColumn &c);

  const CQChartsColumn & vectorXColumn() const { return vectorXColumn_; }
  void setVectorXColumn(const CQChartsColumn &c);

  const CQChartsColumn & vectorYColumn() const { return vectorYColumn_; }
  void setVectorYColumn(const CQChartsColumn &c);

  //---

  // bivariate
  bool isBivariate() const { return bivariateLineData_.visible; }

  const CQChartsLength &bivariateLineWidth() const { return bivariateLineData_.width; }
  void setBivariateLineWidth(const CQChartsLength &l);

  //---

  // stacked, cumulative
  bool isStacked() const { return stacked_; }

  bool isCumulative() const { return cumulative_; }

  //---

  // points
  bool isPoints() const { return pointData_.visible; }
  void setPoints(bool b) { pointData_.visible = b; updateObjs(); }

  const CQChartsColor &pointsStrokeColor() const;
  void setPointsStrokeColor(const CQChartsColor &c);

  QColor interpPointStrokeColor(int i, int n) const;

  double pointsStrokeAlpha() const;
  void setPointsStrokeAlpha(double a);

  const CQChartsColor &pointsFillColor() const;
  void setPointsFillColor(const CQChartsColor &c);

  QColor interpPointFillColor(int i, int n) const;

  double pointsFillAlpha() const;
  void setPointsFillAlpha(double a);

  //---

  // lines
  bool isLines() const { return lineData_.visible; }
  void setLines(bool b) { lineData_.visible = b; updateObjs(); }

  bool isLinesSelectable() const { return linesSelectable_; }
  void setLinesSelectable(bool b) { linesSelectable_ = b; update(); }

  const CQChartsColor &linesColor() const;
  void setLinesColor(const CQChartsColor &c);

  QColor interpLinesColor(int i, int n) const;

  double linesAlpha() const { return lineData_.alpha; }
  void setLinesAlpha(double a) { lineData_.alpha = a; update(); }

  const CQChartsLength &linesWidth() const { return lineData_.width; }
  void setLinesWidth(const CQChartsLength &l) { lineData_.width = l; update(); }

  bool isRoundedLines() const { return roundedLines_; }
  void setRoundedLines(bool b) { roundedLines_ = b; update(); }

  //---

  // fill under
  bool isFillUnder() const { return fillUnderData_.fillData.visible; }

  const CQChartsColor &fillUnderColor() const;
  void setFillUnderColor(const CQChartsColor &c);

  QColor interpFillUnderColor(int i, int n) const;

  double fillUnderAlpha() const { return fillUnderData_.fillData.alpha; }
  void setFillUnderAlpha(double a) { fillUnderData_.fillData.alpha = a; }

  Pattern fillUnderPattern() const { return (Pattern) fillUnderData_.fillData.pattern; }
  void setFillUnderPattern(const Pattern &p) {
    fillUnderData_.fillData.pattern = (CQChartsFillPattern::Type) p; update(); }

  const QString &fillUnderPosStr() const { return fillUnderData_.posStr; }
  void setFillUnderPosStr(const QString &s) { fillUnderData_.setPosStr(s); updateObjs(); }

  QPointF fillUnderPos(double x, double y) const;

  const QString &fillUnderSide() const { return fillUnderData_.side; }
  void setFillUnderSide(const QString &s) { fillUnderData_.side = s; updateObjs(); }

  //---

  // impulse
  bool isImpulse() const { return impulseData_.visible; }

  const CQChartsColor &impulseColor() const { return impulseData_.color; }
  void setImpulseColor(const CQChartsColor &c);

  double impulseAlpha() const { return impulseData_.alpha; }
  void setImpulseAlpha(double a);

  const CQChartsLength &impulseWidth() const { return impulseData_.width; }
  void setImpulseWidth(const CQChartsLength &l);

  QColor interpImpulseColor(int i, int n) const;

  //---

  // vectors
  bool isVectors() const;

  //---

  // symbol
  QString symbolName() const;
  void setSymbolName(const QString &s);

  CQChartsPlotSymbol::Type symbolType() const { return pointData_.type; }
  void setSymbolType(CQChartsPlotSymbol::Type t) { pointData_.type = t; update(); }

  double symbolSize() const { return pointData_.size; }
  void setSymbolSize(double s) { pointData_.size = s; update(); }

  bool isSymbolStroked() const { return pointData_.stroke.visible; }
  void setSymbolStroked(bool b) { pointData_.stroke.visible = b; update(); }

  const CQChartsLength &symbolLineWidth() const { return pointData_.stroke.width; }
  void setSymbolLineWidth(const CQChartsLength &l) { pointData_.stroke.width = l; update(); }

  bool isSymbolFilled() const { return pointData_.fill.visible; }
  void setSymbolFilled(bool b) { pointData_.fill.visible = b; update(); }

  //---

  // data label
  const CQChartsColor &dataLabelColor() const { return dataLabelData_.color; }
  void setDataLabelColor(const CQChartsColor &c) { dataLabelData_.color = c; update(); }

  QColor interpDataLabelColor(int i, int n);

  double dataLabelAngle() const { return dataLabelData_.angle; }
  void setDataLabelAngle(double r) { dataLabelData_.angle = r; }

  //---

  // bivariate line
  void drawBivariateLine(QPainter *painter, const QPointF &p1, const QPointF &p2,
                         const QColor &c);

  //---

  QColor interpPaletteColor(int i, int n, bool scale=false) const override;

  //---

  void addProperties() override;

  void updateRange(bool apply=true) override;

  void postInit() override;

  bool initObjs() override;

  //---

  bool rowData(const QModelIndex &parent, int row, double &x, std::vector<double> &yv,
               QModelIndex &ind, bool skipBad) const;

  //---

  void addPolyLine(const QPolygonF &polyLine, int i, int n, const QString &name);

  void addPolygon(const QPolygonF &poly, int i, int n, const QString &name);

  QString valueName(int iset, int irow) const;

  void addKeyItems(CQChartsPlotKey *key) override;

  int numSets() const;

  CQChartsColumn getSetColumn(int i) const;

  //---

  double symbolWidth () const { return symbolWidth_ ; }
  double symbolHeight() const { return symbolHeight_; }

  //---

  bool probe(ProbeData &probeData) const override;

  bool interpY(double x, std::vector<double> &yvals) const;

  bool addMenuItems(QMenu *menu) override;

  void draw(QPainter *) override;

  void drawArrow(QPainter *painter, const QPointF &p1, const QPointF &p2);

 public slots:
  // set bivariate
  void setBivariate(bool b);

  // set stacked
  void setStacked(bool b);

  // set cumulative
  void setCumulative(bool b);

  // set impulse
  void setImpulse(bool b);

  // set vectors
  void setVectors(bool b);

  // set fill under
  void setFillUnder(bool b);

 private:
  struct VectorsData {
    bool visible { false };
  };

  CQChartsColumn     xColumn_              { 0 };                // x column
  CQChartsColumn     yColumn_              { 1 };                // y column
  Columns            yColumns_;                                  // multiple y columns
  CQChartsColumn     nameColumn_;                                // name column
  CQChartsColumn     sizeColumn_;                                // size column
  CQChartsColumn     pointLabelColumn_;                          // point label column
  CQChartsColumn     pointColorColumn_;                          // point color column
  CQChartsColumn     pointSymbolColumn_;                         // point symbol column
  CQChartsColumn     vectorXColumn_;                             // vector x direction column
  CQChartsColumn     vectorYColumn_;                             // vector y direction column
  bool               stacked_              { false };            // is stacked
  bool               cumulative_           { false };            // cumulate values
  CQChartsSymbolData pointData_;                                 // point data
  bool               linesSelectable_      { false };            // are lines selectable
  CQChartsLineData   lineData_;                                  // line data
  bool               roundedLines_         { false };            // draw rounded (smooth) lines
  FillUnderData      fillUnderData_;                             // fill under data
  CQChartsLineData   impulseData_;                               // impulse line data
  CQChartsArrow*     arrowObj_             { nullptr };          // vectors data
  CQChartsTextData   dataLabelData_;                             // data label text data
  CQChartsLineData   bivariateLineData_;                         // bivariate line object
  ColumnType         pointColorColumnType_ { ColumnType::NONE }; // point color column type
  mutable double     symbolWidth_          { 1.0 };              // current symbol width
  mutable double     symbolHeight_         { 1.0 };              // current symbol height
};

#endif
