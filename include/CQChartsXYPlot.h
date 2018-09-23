#ifndef CQChartsXYPlot_H
#define CQChartsXYPlot_H

#include <CQChartsGroupPlot.h>
#include <CQChartsPlotObj.h>
#include <CQChartsFillUnder.h>
#include <CQChartsFitData.h>
#include <CQChartsUtil.h>

class CQChartsXYPlot;
class CQChartsArrow;

//---

class CQChartsXYPlotType : public CQChartsGroupPlotType {
 public:
  CQChartsXYPlotType();

  QString name() const override { return "xy"; }
  QString desc() const override { return "XY"; }

  Dimension dimension() const override { return Dimension::TWO_D; }

  void addParameters() override;

  QString description() const override;

  const char *xColumnName() const override { return "x"; }
  const char *yColumnName() const override { return "y"; }

  CQChartsPlot *create(CQChartsView *view, const ModelP &model) const override;
};

//---

class CQChartsXYBiLineObj : public CQChartsPlotObj {
  Q_OBJECT

  Q_PROPERTY(double x  READ x  WRITE setX )
  Q_PROPERTY(double y1 READ y1 WRITE setY1)
  Q_PROPERTY(double y2 READ y2 WRITE setY2)

 public:
  CQChartsXYBiLineObj(CQChartsXYPlot *plot, int groupInd, const CQChartsGeom::BBox &rect,
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
  CQChartsXYPlot* plot_     { nullptr }; // parent plot
  int             groupInd_ { -1 };      // group ind
  double          x_        { 0.0 };     // x
  double          y1_       { 0.0 };     // start y
  double          y2_       { 0.0 };     // end y
  QModelIndex     ind_;                  // model index
  int             iset_     { -1 };      // iset
  int             nset_     { -1 };      // nset
  int             i_        { -1 };      // i
  int             n_        { -1 };      // n
};

//---

class CQChartsXYImpulseLineObj : public CQChartsPlotObj {
  Q_OBJECT

  Q_PROPERTY(double x  READ x  WRITE setX )
  Q_PROPERTY(double y1 READ y1 WRITE setY1)
  Q_PROPERTY(double y2 READ y2 WRITE setY2)

 public:
  CQChartsXYImpulseLineObj(CQChartsXYPlot *plot, int groupInd, const CQChartsGeom::BBox &rect,
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
  CQChartsXYPlot* plot_     { nullptr }; // parent plot
  int             groupInd_ { -1 };      // group ind
  double          x_        { 0.0 };     // x
  double          y1_       { 0.0 };     // start y
  double          y2_       { 0.0 };     // end y
  QModelIndex     ind_;                  // model index
  int             iset_     { -1 };      // iset
  int             nset_     { -1 };      // nset
  int             i_        { -1 };      // i
  int             n_        { -1 };      // n
};

//---

class CQChartsXYPointObj : public CQChartsPlotObj {
  Q_OBJECT

  Q_PROPERTY(double x    READ x    WRITE setX   )
  Q_PROPERTY(double y    READ y    WRITE setY   )
  Q_PROPERTY(double size READ size WRITE setSize)

 public:
  CQChartsXYPointObj(CQChartsXYPlot *plot, int groupInd, const CQChartsGeom::BBox &rect,
                     double x, double y, double size, const QModelIndex &ind,
                     int iset, int nset, int i, int n);

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
    CQChartsColor  color;
    CQChartsSymbol symbol { CQChartsSymbol::Type::NONE };
    OptPoint       vector;
  };

  CQChartsXYPlot* plot_     { nullptr }; // parent plot
  int             groupInd_ { -1 };      // group ind
  QPointF         pos_;                  // position
  double          size_     { -1 };      // size : TODO support units
  QModelIndex     ind_;                  // model index
  int             iset_     { -1 };      // sets index
  int             nset_     { -1 };      // sets size
  int             i_        { -1 };      // points index
  int             n_        { -1 };      // points size
  ExtraData*      edata_    { nullptr }; // extra data
};

//---

class CQChartsXYLabelObj : public CQChartsPlotObj {
  Q_OBJECT

  Q_PROPERTY(double  x     READ x     WRITE setX    )
  Q_PROPERTY(double  y     READ y     WRITE setY    )
  Q_PROPERTY(QString label READ label WRITE setLabel)

 public:
  CQChartsXYLabelObj(CQChartsXYPlot *plot, int groupInd, const CQChartsGeom::BBox &rect,
                     double x, double y, const QString &label, const QModelIndex &ind,
                     int iset, int nset, int i, int n);

 ~CQChartsXYLabelObj();

  //---

  CQChartsXYPlot *plot() const { return plot_; }

  double x() const { return pos_.x(); }
  void setX(double r) { pos_.setX(r); }

  double y() const { return pos_.y(); }
  void setY(double r) { pos_.setY(r); }

  const QString &label() const { return label_; }
  void setLabel(const QString &v) { label_ = v; }

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
  using OptPoint = boost::optional<QPointF>;

  CQChartsXYPlot* plot_     { nullptr }; // parent plot
  int             groupInd_ { -1 };      // group ind
  QPointF         pos_;                  // position
  QString         label_;                // label
  QModelIndex     ind_;                  // model index
  int             iset_     { -1 };      // sets index
  int             nset_     { -1 };      // sets size
  int             i_        { -1 };      // points index
  int             n_        { -1 };      // points size
};

//---

class CQChartsSmooth;

class CQChartsXYPolylineObj : public CQChartsPlotObj {
  Q_OBJECT

  Q_PROPERTY(QString name READ name WRITE setName)

 public:
  CQChartsXYPolylineObj(CQChartsXYPlot *plot, int groupInd, const CQChartsGeom::BBox &rect,
                        const QPolygonF &poly, const QString &name,
                        int ig, int ng, int is, int ns);

 ~CQChartsXYPolylineObj();

  //---

  CQChartsXYPlot *plot() const { return plot_; }

  const QString &name() const { return name_; }
  void setName(const QString &s) { name_ = s; }

  int ig() const { return ig_; }
  int ng() const { return ng_; }

  int is() const { return is_; }
  int ns() const { return ns_; }

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
  CQChartsXYPlot* plot_     { nullptr }; // parent plot
  int             groupInd_ { -1 };      // group ind
  QPolygonF       poly_;                 // polygon
  QString         name_;                 // name
  int             ig_       { -1 };      // group ind
  int             ng_       { -1 };      // num groups
  int             is_       { -1 };      // set ind
  int             ns_       { -1 };      // num sets
  CQChartsSmooth* smooth_   { nullptr }; // smooth object
  CQChartsFitData fit_;                  // fit data
};

//---

class CQChartsXYPolygonObj : public CQChartsPlotObj {
  Q_OBJECT

  Q_PROPERTY(QString name READ name WRITE setName)

 public:
  CQChartsXYPolygonObj(CQChartsXYPlot *plot, int groupInd, const CQChartsGeom::BBox &rect,
                       const QPolygonF &poly, const QString &name,
                       int ig, int ng, int is, int ns);

 ~CQChartsXYPolygonObj();

  //---

  CQChartsXYPlot *plot() const { return plot_; }

  const QString &name() const { return name_; }
  void setName(const QString &s) { name_ = s; }

  int ig() const { return ig_; }
  int ng() const { return ng_; }

  int is() const { return is_; }
  int ns() const { return ns_; }

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
  CQChartsXYPlot* plot_     { nullptr }; // parent plot
  int             groupInd_ { -1 };      // group ind
  QPolygonF       poly_;                 // polygon
  QString         name_;                 // name
  int             ig_       { -1 };      // group ind
  int             ng_       { -1 };      // num groups
  int             is_       { -1 };      // set ind
  int             ns_       { -1 };      // num sets
  CQChartsSmooth* smooth_   { nullptr }; // smooth object
};

//---

#include <CQChartsKey.h>

class CQChartsXYKeyColor : public CQChartsKeyColorBox {
  Q_OBJECT

 public:
  CQChartsXYKeyColor(CQChartsXYPlot *plot, int i, int n);

  bool selectPress(const CQChartsGeom::Point &p, CQChartsSelMod selMod) override;

  bool selectMove(const CQChartsGeom::Point &) override { return true; }

  QBrush fillBrush() const override;
};

class CQChartsXYKeyLine : public CQChartsKeyItem {
  Q_OBJECT

 public:
  CQChartsXYKeyLine(CQChartsXYPlot *plot, int i, int n);

  QSizeF size() const override;

  bool selectPress(const CQChartsGeom::Point &p, CQChartsSelMod selMod) override;

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
  CQChartsXYKeyText(CQChartsXYPlot *plot, const QString &text, int i, int n);

  QColor interpTextColor(int i, int n) const override;
};

//---

CQCHARTS_NAMED_LINE_DATA(Impulse,impulse)
CQCHARTS_NAMED_LINE_DATA(Bivariate,bivariate)
CQCHARTS_NAMED_FILL_DATA(FillUnder,fillUnder)
CQCHARTS_NAMED_TEXT_DATA(DataLabel,dataLabel)

class CQChartsXYPlot : public CQChartsGroupPlot,
 public CQChartsObjLineData         <CQChartsXYPlot>,
 public CQChartsObjPointData        <CQChartsXYPlot>,
 public CQChartsObjImpulseLineData  <CQChartsXYPlot>,
 public CQChartsObjBivariateLineData<CQChartsXYPlot>,
 public CQChartsObjFillUnderFillData<CQChartsXYPlot>,
 public CQChartsObjDataLabelTextData<CQChartsXYPlot> {
  Q_OBJECT

  // columns
  Q_PROPERTY(CQChartsColumn  xColumn           READ xColumn           WRITE setXColumn          )
  Q_PROPERTY(CQChartsColumns yColumns          READ yColumns          WRITE setYColumns         )
  Q_PROPERTY(CQChartsColumn  nameColumn        READ nameColumn        WRITE setNameColumn       )
  Q_PROPERTY(CQChartsColumn  sizeColumn        READ sizeColumn        WRITE setSizeColumn       )
  Q_PROPERTY(CQChartsColumn  pointLabelColumn  READ pointLabelColumn  WRITE setPointLabelColumn )
  Q_PROPERTY(CQChartsColumn  pointColorColumn  READ pointColorColumn  WRITE setPointColorColumn )
  Q_PROPERTY(CQChartsColumn  pointSymbolColumn READ pointSymbolColumn WRITE setPointSymbolColumn)
  Q_PROPERTY(CQChartsColumn  vectorXColumn     READ vectorXColumn     WRITE setVectorXColumn    )
  Q_PROPERTY(CQChartsColumn  vectorYColumn     READ vectorYColumn     WRITE setVectorYColumn    )

  // bivariate
  CQCHARTS_NAMED_LINE_DATA_PROPERTIES(Bivariate, bivariate)

  // stacked, cumulative
  Q_PROPERTY(bool stacked    READ isStacked    WRITE setStacked   )
  Q_PROPERTY(bool cumulative READ isCumulative WRITE setCumulative)
  Q_PROPERTY(bool fitted     READ isFitted     WRITE setFitted    )

  // vectors
  Q_PROPERTY(bool vectors READ isVectors WRITE setVectors)

  // impulse
  CQCHARTS_NAMED_LINE_DATA_PROPERTIES(Impulse,impulse)

  // point: (display, symbol)
  CQCHARTS_POINT_DATA_PROPERTIES

  // lines (selectable, rounded, display, stroke)
  Q_PROPERTY(bool linesSelectable READ isLinesSelectable WRITE setLinesSelectable)
  Q_PROPERTY(bool roundedLines    READ isRoundedLines    WRITE setRoundedLines   )

  CQCHARTS_LINE_DATA_PROPERTIES

  // fill under:
  Q_PROPERTY(bool                  fillUnderSelectable
             READ isFillUnderSelectable WRITE setFillUnderSelectable)
  Q_PROPERTY(CQChartsFillUnderPos  fillUnderPos
             READ fillUnderPos          WRITE setFillUnderPos       )
  Q_PROPERTY(CQChartsFillUnderSide fillUnderSide
             READ fillUnderSide         WRITE setFillUnderSide      )

  CQCHARTS_NAMED_FILL_DATA_PROPERTIES(FillUnder,fillUnder)

  // data label
  CQCHARTS_NAMED_TEXT_DATA_PROPERTIES(DataLabel,dataLabel)

 private:
  struct FillUnderData {
    bool                  selectable { false }; // is fill under selectable
    CQChartsFillUnderPos  pos;                  // fill under position
    CQChartsFillUnderSide side;                 // fill under side
  };

 public:
  CQChartsXYPlot(CQChartsView *view, const ModelP &model);

 ~CQChartsXYPlot();

  //---

  // columns
  const CQChartsColumn &xColumn() const { return xColumn_; }
  void setXColumn(const CQChartsColumn &c);

  const CQChartsColumns &yColumns() const { return yColumns_; }
  void setYColumns(const CQChartsColumns &c);

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

  // stacked, cumulative
  bool isStacked() const { return stacked_; }

  bool isCumulative() const { return cumulative_; }

  //---

  // lines selectable, rounded
  bool isLinesSelectable() const { return linesSelectable_; }
  void setLinesSelectable(bool b);

  bool isRoundedLines() const { return roundedLines_; }
  void setRoundedLines(bool b);

  //---

  // fill under
  bool isFillUnderSelectable() const { return fillUnderData_.selectable; }
  void setFillUnderSelectable(bool b);

  const CQChartsFillUnderPos &fillUnderPos() const { return fillUnderData_.pos; }
  void setFillUnderPos(const CQChartsFillUnderPos &p);

  const CQChartsFillUnderSide &fillUnderSide() const { return fillUnderData_.side; }
  void setFillUnderSide(const CQChartsFillUnderSide &s);

  QPointF calcFillUnderPos(double x, double y) const;

  //---

  // vectors
  bool isVectors() const;

  // fitted
  bool isFitted() const { return fitted_; }

  //---

  QColor interpPaletteColor(int i, int n, bool scale=false) const override;

  //---

  void addProperties() override;

  CQChartsGeom::Range calcRange() override;

  void postInit() override;

  bool initObjs() override;

  //---

  bool rowData(const ModelVisitor::VisitData &data, double &x, std::vector<double> &yv,
               QModelIndex &ind, bool skipBad) const;

  //---

  void addPolyLine(const QPolygonF &polyLine, int groupInd, int ig, int ng, int is, int ns,
                   const QString &name);

  void addPolygon(const QPolygonF &poly, int groupInd, int ig, int ng, int is, int ns,
                  const QString &name);

  QString valueName(int iset, int irow) const;

  void addKeyItems(CQChartsPlotKey *key) override;

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
  void setPointsSlot(bool b);

  // set lines visible
  void setLinesSlot(bool b);

  // set bivariate
  void setBivariateLinesSlot(bool b);

  // set stacked
  void setStacked(bool b);

  // set cumulative
  void setCumulative(bool b);

  // set impulse
  void setImpulseLinesSlot(bool b);

  // set vectors
  void setVectors(bool b);

  // set fitted
  void setFitted(bool b);

  // set fill under
  void setFillUnderFilledSlot(bool b);

 private:
  QString xAxisName() const;
  QString yAxisName() const;

 private:
  CQChartsColumn  xColumn_;                                   // x column
  CQChartsColumns yColumns_;                                  // y columns
  CQChartsColumn  nameColumn_;                                // name column
  CQChartsColumn  sizeColumn_;                                // size column
  CQChartsColumn  pointLabelColumn_;                          // point label column
  CQChartsColumn  pointColorColumn_;                          // point color column
  CQChartsColumn  pointSymbolColumn_;                         // point symbol column
  CQChartsColumn  vectorXColumn_;                             // vector x direction column
  CQChartsColumn  vectorYColumn_;                             // vector y direction column
  bool            stacked_              { false };            // is stacked
  bool            cumulative_           { false };            // cumulate values
  bool            linesSelectable_      { false };            // are lines selectable
  bool            roundedLines_         { false };            // draw rounded (smooth) lines
  FillUnderData   fillUnderData_;                             // fill under data
  CQChartsArrow*  arrowObj_             { nullptr };          // vectors data
  ColumnType      pointColorColumnType_ { ColumnType::NONE }; // point color column type
  bool            fitted_               { false };            // is fitted
  mutable double  symbolWidth_          { 1.0 };              // current symbol width
  mutable double  symbolHeight_         { 1.0 };              // current symbol height
};

#endif
