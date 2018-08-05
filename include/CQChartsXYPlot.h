#ifndef CQChartsXYPlot_H
#define CQChartsXYPlot_H

#include <CQChartsPlot.h>
#include <CQChartsPlotType.h>
#include <CQChartsPlotObj.h>
#include <CQChartsUtil.h>

class CQChartsXYPlot;
class CQChartsArrow;

//---

class CQChartsXYPlotType : public CQChartsPlotType {
 public:
  CQChartsXYPlotType();

  QString name() const override { return "xy"; }
  QString desc() const override { return "XY"; }

  void addParameters() override;

  QString description() const override;

  const char *xColumnName() const override { return "x"; }
  const char *yColumnName() const override { return "y"; }

  Dimension dimension() const override { return Dimension::TWO_D; }

  CQChartsPlot *create(CQChartsView *view, const ModelP &model) const override;
};

//---

class CQChartsXYBiLineObj : public CQChartsPlotObj {
  Q_OBJECT

  Q_PROPERTY(double x  READ x  WRITE setX )
  Q_PROPERTY(double y1 READ y1 WRITE setY1)
  Q_PROPERTY(double y2 READ y2 WRITE setY2)

 public:
  CQChartsXYBiLineObj(CQChartsXYPlot *plot, const CQChartsGeom::BBox &rect, double x,
                      double y1, double y2, const QModelIndex &ind, int iset, int nset,
                      int i, int n);

  //---

  CQChartsXYPlot *plot() const { return plot_; }

  double x() const { return x_; }
  void setX(double r) { x_ = r; }

  double y1() const { return y1_; }
  void setY1(double r) { y1_ = r; }

  double y2() const { return y2_; }
  void setY2(double r) { y2_ = r; }

  const QModelIndex &ind() const { return ind_; }

  int iset() const { return iset_; }
  int nset() const { return nset_; }

  int i() const { return i_; }
  int n() const { return n_; }

  //---

  QString calcId() const override;

  QString calcTipId() const override;

  //---

  bool visible() const override;

  bool inside(const CQChartsGeom::Point &p) const override;

  void getSelectIndices(Indices &inds) const override;

  void addColumnSelectIndex(Indices &inds, const CQChartsColumn &column) const override;

  void draw(QPainter *painter) override;

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

  Q_PROPERTY(double x  READ x  WRITE setX )
  Q_PROPERTY(double y1 READ y1 WRITE setY1)
  Q_PROPERTY(double y2 READ y2 WRITE setY2)

 public:
  CQChartsXYImpulseLineObj(CQChartsXYPlot *plot, const CQChartsGeom::BBox &rect,
                           double x, double y1, double y2, const QModelIndex &ind,
                           int iset, int nset, int i, int n);

  //---

  CQChartsXYPlot *plot() const { return plot_; }

  double x() const { return x_; }
  void setX(double r) { x_ = r; }

  double y1() const { return y1_; }
  void setY1(double r) { y1_ = r; }

  double y2() const { return y2_; }
  void setY2(double r) { y2_ = r; }

  const QModelIndex &ind() const { return ind_; }

  int iset() const { return iset_; }
  int nset() const { return nset_; }

  int i() const { return i_; }
  int n() const { return n_; }

  //---

  QString calcId() const override;

  QString calcTipId() const override;

  //---

  bool visible() const override;

  bool inside(const CQChartsGeom::Point &p) const override;

  void getSelectIndices(Indices &inds) const override;

  void addColumnSelectIndex(Indices &inds, const CQChartsColumn &column) const override;

  void draw(QPainter *painter) override;

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

class CQChartsXYPointObj : public CQChartsPlotObj {
  Q_OBJECT

  Q_PROPERTY(double x    READ x    WRITE setX   )
  Q_PROPERTY(double y    READ y    WRITE setY   )
  Q_PROPERTY(double size READ size WRITE setSize)

 public:
  CQChartsXYPointObj(CQChartsXYPlot *plot, const CQChartsGeom::BBox &rect, double x, double y,
                     double size, const QModelIndex &ind, int iset, int nset, int i, int n);

 ~CQChartsXYPointObj();

  //---

  CQChartsXYPlot *plot() const { return plot_; }

  double x() const { return pos_.x(); }
  void setX(double r) { pos_.setX(r); }

  double y() const { return pos_.y(); }
  void setY(double r) { pos_.setY(r); }

  double size() const { return size_; }
  void setSize(double r) { size_ = r; }

  const QModelIndex &ind() const { return ind_; }

  int iset() const { return iset_; }
  int nset() const { return nset_; }

  int i() const { return i_; }
  int n() const { return n_; }

  //---

  QString calcId() const override;

  QString calcTipId() const override;

  //---

  void setLabel(const QString &label);

  void setColor(const CQChartsColor &c);

  void setSymbol(CQChartsSymbol type);

  void setVector(double vx, double vy);

  bool visible() const override;

  bool inside(const CQChartsGeom::Point &p) const override;

  void getSelectIndices(Indices &inds) const override;

  void addColumnSelectIndex(Indices &inds, const CQChartsColumn &column) const override;

  void draw(QPainter *painter) override;

 private:
  using OptPoint = boost::optional<QPointF>;

  struct ExtraData {
    QString        label;
    CQChartsColor  color;
    CQChartsSymbol symbol { CQChartsSymbol::Type::NONE };
    OptPoint       vector;
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

  Q_PROPERTY(QString name READ name WRITE setName)

 public:
  CQChartsXYPolylineObj(CQChartsXYPlot *plot, const CQChartsGeom::BBox &rect,
                        const QPolygonF &poly, const QString &name, int i, int n);

 ~CQChartsXYPolylineObj();

  //---

  CQChartsXYPlot *plot() const { return plot_; }

  const QString &name() const { return name_; }
  void setName(const QString &s) { name_ = s; }

  int i() const { return i_; }
  int n() const { return n_; }

  //---

  QString calcId() const override;

  QString calcTipId() const override;

  //---

  bool visible() const override;

  bool inside(const CQChartsGeom::Point &p) const override;

  bool interpY(double x, std::vector<double> &yvals) const;

  void getSelectIndices(Indices &inds) const override;

  void addColumnSelectIndex(Indices &, const CQChartsColumn &) const override { }

  void draw(QPainter *painter) override;

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

  Q_PROPERTY(QString name READ name WRITE setName)

 public:
  CQChartsXYPolygonObj(CQChartsXYPlot *plot, const CQChartsGeom::BBox &rect,
                       const QPolygonF &poly, const QString &name, int i, int n);

 ~CQChartsXYPolygonObj();

  //---

  CQChartsXYPlot *plot() const { return plot_; }

  const QString &name() const { return name_; }
  void setName(const QString &s) { name_ = s; }

  int i() const { return i_; }
  int n() const { return n_; }

  //---

  QString calcId() const override;

  QString calcTipId() const override;

  //---

  bool visible() const override;

  bool inside(const CQChartsGeom::Point &p) const override;

  void getSelectIndices(Indices &inds) const override;

  void addColumnSelectIndex(Indices &, const CQChartsColumn &) const override { }

  void draw(QPainter *painter) override;

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
  Q_PROPERTY(bool           vectors            READ isVectors          WRITE setVectors           )

  Q_PROPERTY(bool             impulse      READ isImpulse    WRITE setImpulse     )
  Q_PROPERTY(CQChartsColor    impulseColor READ impulseColor WRITE setImpulseColor)
  Q_PROPERTY(double           impulseAlpha READ impulseAlpha WRITE setImpulseAlpha)
  Q_PROPERTY(CQChartsLength   impulseWidth READ impulseWidth WRITE setImpulseWidth)
  Q_PROPERTY(CQChartsLineDash impulseDash  READ impulseDash  WRITE setImpulseDash )

  // point:
  //  display, color, symbol, size
  Q_PROPERTY(bool           points            READ isPoints          WRITE setPoints           )
  Q_PROPERTY(CQChartsSymbol symbolType        READ symbolType        WRITE setSymbolType       )
  Q_PROPERTY(CQChartsLength symbolSize        READ symbolSize        WRITE setSymbolSize       )
  Q_PROPERTY(bool           symbolStroked     READ isSymbolStroked   WRITE setSymbolStroked    )
  Q_PROPERTY(CQChartsColor  symbolStrokeColor READ symbolStrokeColor WRITE setSymbolStrokeColor)
  Q_PROPERTY(double         symbolStrokeAlpha READ symbolStrokeAlpha WRITE setSymbolStrokeAlpha)
  Q_PROPERTY(CQChartsLength symbolStrokeWidth READ symbolStrokeWidth WRITE setSymbolStrokeWidth)
  Q_PROPERTY(bool           symbolFilled      READ isSymbolFilled    WRITE setSymbolFilled     )
  Q_PROPERTY(CQChartsColor  symbolFillColor   READ symbolFillColor   WRITE setSymbolFillColor  )
  Q_PROPERTY(double         symbolFillAlpha   READ symbolFillAlpha   WRITE setSymbolFillAlpha  )
  Q_PROPERTY(Pattern        symbolFillPattern READ symbolFillPattern WRITE setSymbolFillPattern)

  // line:
  //  display, stroke
  Q_PROPERTY(bool             lines           READ isLines           WRITE setLines          )
  Q_PROPERTY(bool             linesSelectable READ isLinesSelectable WRITE setLinesSelectable)
  Q_PROPERTY(CQChartsColor    linesColor      READ linesColor        WRITE setLinesColor     )
  Q_PROPERTY(double           linesAlpha      READ linesAlpha        WRITE setLinesAlpha     )
  Q_PROPERTY(CQChartsLength   linesWidth      READ linesWidth        WRITE setLinesWidth     )
  Q_PROPERTY(CQChartsLineDash linesDash       READ linesDash         WRITE setLinesDash      )
  Q_PROPERTY(bool             roundedLines    READ isRoundedLines    WRITE setRoundedLines   )

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

  const CQChartsColumn &yColumn() const { return yColumns_.column(); }
  void setYColumn(const CQChartsColumn &c);

  const Columns &yColumns() const { return yColumns_.columns(); }
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

  const CQChartsColor &symbolStrokeColor() const;
  void setSymbolStrokeColor(const CQChartsColor &c);

  QColor interpSymbolStrokeColor(int i, int n) const;

  double symbolStrokeAlpha() const;
  void setSymbolStrokeAlpha(double a);

  const CQChartsColor &symbolFillColor() const;
  void setSymbolFillColor(const CQChartsColor &c);

  QColor interpSymbolFillColor(int i, int n) const;

  double symbolFillAlpha() const;
  void setSymbolFillAlpha(double a);

  Pattern symbolFillPattern() const;
  void setSymbolFillPattern(const Pattern &p);

  //---

  // lines
  bool isLines() const { return lineData_.visible; }

  bool isLinesSelectable() const { return linesSelectable_; }
  void setLinesSelectable(bool b);

  const CQChartsColor &linesColor() const;
  void setLinesColor(const CQChartsColor &c);

  QColor interpLinesColor(int i, int n) const;

  double linesAlpha() const { return lineData_.alpha; }
  void setLinesAlpha(double a);

  const CQChartsLength &linesWidth() const { return lineData_.width; }
  void setLinesWidth(const CQChartsLength &l);

  const CQChartsLineDash &linesDash() const { return lineData_.dash; }
  void setLinesDash(const CQChartsLineDash &d);

  bool isRoundedLines() const { return roundedLines_; }
  void setRoundedLines(bool b);

  //---

  // fill under
  bool isFillUnder() const { return fillUnderData_.fillData.visible; }

  const CQChartsColor &fillUnderColor() const;
  void setFillUnderColor(const CQChartsColor &c);

  QColor interpFillUnderColor(int i, int n) const;

  double fillUnderAlpha() const { return fillUnderData_.fillData.alpha; }
  void setFillUnderAlpha(double a);

  Pattern fillUnderPattern() const { return (Pattern) fillUnderData_.fillData.pattern; }
  void setFillUnderPattern(const Pattern &p);

  const QString &fillUnderPosStr() const { return fillUnderData_.posStr; }
  void setFillUnderPosStr(const QString &s);

  QPointF fillUnderPos(double x, double y) const;

  const QString &fillUnderSide() const { return fillUnderData_.side; }
  void setFillUnderSide(const QString &s);

  //---

  // impulse
  bool isImpulse() const { return impulseData_.visible; }

  const CQChartsColor &impulseColor() const { return impulseData_.color; }
  void setImpulseColor(const CQChartsColor &c);

  double impulseAlpha() const { return impulseData_.alpha; }
  void setImpulseAlpha(double a);

  const CQChartsLength &impulseWidth() const { return impulseData_.width; }
  void setImpulseWidth(const CQChartsLength &l);

  const CQChartsLineDash &impulseDash() const { return impulseData_.dash; }
  void setImpulseDash(const CQChartsLineDash &d);

  QColor interpImpulseColor(int i, int n) const;

  //---

  // vectors
  bool isVectors() const;

  //---

  // symbol
  const CQChartsSymbol &symbolType() const { return pointData_.type; }
  void setSymbolType(const CQChartsSymbol &t);

  const CQChartsLength &symbolSize() const { return pointData_.size; }
  void setSymbolSize(const CQChartsLength &s);

  bool isSymbolStroked() const { return pointData_.stroke.visible; }
  void setSymbolStroked(bool b);

  const CQChartsLength &symbolStrokeWidth() const { return pointData_.stroke.width; }
  void setSymbolStrokeWidth(const CQChartsLength &l);

  bool isSymbolFilled() const { return pointData_.fill.visible; }
  void setSymbolFilled(bool b);

  //---

  // data label
  const CQChartsColor &dataLabelColor() const { return dataLabelData_.color; }
  void setDataLabelColor(const CQChartsColor &c);

  QColor interpDataLabelColor(int i, int n);

  double dataLabelAngle() const { return dataLabelData_.angle; }
  void setDataLabelAngle(double r);

  //---

  // bivariate line
  void drawBivariateLine(QPainter *painter, const QPointF &p1, const QPointF &p2,
                         const QColor &c);

  //---

  QColor interpPaletteColor(int i, int n, bool scale=false) const override;

  //---

  void addProperties() override;

  void calcRange() override;

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

  void drawArrow(QPainter *painter, const QPointF &p1, const QPointF &p2);

 public slots:
  // set points visible
  void setPoints(bool b);

  // set lines visible
  void setLines(bool b);

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
  CQChartsColumns    yColumns_             { 1 };                // y columns
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
