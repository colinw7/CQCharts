#include <CQChartsXYPlot.h>
#include <CQChartsView.h>
#include <CQChartsAxis.h>
#include <CQChartsUtil.h>
#include <CQCharts.h>
#include <CQChartsRotatedText.h>
#include <CQChartsArrow.h>
#include <CQChartsSmooth.h>
#include <CQChartsRoundedPolygon.h>
#include <QMenu>
#include <QPainter>

CQChartsXYPlotType::
CQChartsXYPlotType()
{
}

void
CQChartsXYPlotType::
addParameters()
{
  // columns
  addColumnParameter ("x", "X", "xColumn" , 0  ).setRequired().setMonotonic().setNumeric();
  addColumnsParameter("y", "Y", "yColumns", "1").setRequired().setNumeric();

  addColumnParameter("name", "Name", "nameColumn").setString();
  addColumnParameter("size", "Size", "sizeColumn").setNumeric();

  addColumnParameter("pointLabel" , "PointLabel" , "pointLabelColumn" ).setString();
  addColumnParameter("pointColor" , "PointColor" , "pointColorColumn" ).setColor ();
  addColumnParameter("pointSymbol", "PointSymbol", "pointSymbolColumn").setString();

  addColumnParameter("vectorX", "VectorX", "vectorXColumn").setNumeric();
  addColumnParameter("vectorY", "VectorY", "vectorYColumn").setNumeric();

  // bool parameters
  addBoolParameter("lines"     , "Lines"     , "lines"     , true);
  addBoolParameter("points"    , "Points"    , "points"    );
  addBoolParameter("bivariate" , "Bivariate" , "bivariate" );
  addBoolParameter("stacked"   , "Stacked"   , "stacked"   );
  addBoolParameter("cumulative", "Cumulative", "cumulative");
  addBoolParameter("fillUnder" , "FillUnder" , "fillUnder" );
  addBoolParameter("impulse"   , "Impulse"   , "impulse"   );
  addBoolParameter("vectors"   , "Vectors"   , "vectors"   );

  CQChartsPlotType::addParameters();
}

CQChartsPlot *
CQChartsXYPlotType::
create(CQChartsView *view, const ModelP &model) const
{
  return new CQChartsXYPlot(view, model);
}

//---

CQChartsXYPlot::
CQChartsXYPlot(CQChartsView *view, const ModelP &model) :
 CQChartsPlot(view, view->charts()->plotType("xy"), model), fillUnderData_(this)
{
  impulseData_      .visible = false;
  bivariateLineData_.visible = false;

  lineData_.color = CQChartsColor(CQChartsColor::Type::PALETTE);

  arrowObj_ = new CQChartsArrow(this);

  arrowObj_->setVisible(false);

  connect(arrowObj_, SIGNAL(dataChanged()), this, SLOT(updateSlot()));

  addAxes();

  addKey();

  addTitle();
}

CQChartsXYPlot::
~CQChartsXYPlot()
{
  delete arrowObj_;
}

//---

void
CQChartsXYPlot::
setXColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(xColumn_, c, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsXYPlot::
setYColumn(const CQChartsColumn &c)
{
  if (c != yColumn_) {
    yColumn_ = c;

    yColumns_.clear();

    if (yColumn_.isValid())
      yColumns_.push_back(yColumn_);

    updateRangeAndObjs();
  }
}

void
CQChartsXYPlot::
setYColumns(const Columns &yColumns)
{
  yColumns_ = yColumns;

  if (! yColumns_.empty())
    yColumn_ = yColumns_[0];
  else
    yColumn_ = -1;

  updateRangeAndObjs();
}

QString
CQChartsXYPlot::
yColumnsStr() const
{
  return CQChartsColumn::columnsToString(yColumns_);
}

bool
CQChartsXYPlot::
setYColumnsStr(const QString &s)
{
  if (s != yColumnsStr()) {
    Columns yColumns;

    if (! CQChartsColumn::stringToColumns(s, yColumns))
      return false;

    setYColumns(yColumns);
  }

  return true;
}

void
CQChartsXYPlot::
setNameColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(nameColumn_, c, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsXYPlot::
setSizeColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(sizeColumn_, c, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsXYPlot::
setPointLabelColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(pointLabelColumn_, c, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsXYPlot::
setPointColorColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(pointColorColumn_, c, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsXYPlot::
setPointSymbolColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(pointSymbolColumn_, c, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsXYPlot::
setVectorXColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(vectorXColumn_, c, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsXYPlot::
setVectorYColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(vectorYColumn_, c, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsXYPlot::
setBivariateLineWidth(const CQChartsLength &l)
{
  CQChartsUtil::testAndSet(bivariateLineData_.width, l, [&]() { update(); } );
}

//---

void
CQChartsXYPlot::
addProperties()
{
  CQChartsPlot::addProperties();

  // columns
  addProperty("columns", this, "xColumn"           , "x"          );
  addProperty("columns", this, "yColumn"           , "y"          );
  addProperty("columns", this, "yColumns"          , "yset"       );
  addProperty("columns", this, "nameColumn"        , "name"       );
  addProperty("columns", this, "sizeColumn"        , "size"       );
  addProperty("columns", this, "pointLabelColumn"  , "pointLabel" );
  addProperty("columns", this, "pointColorColumn"  , "pointColor" );
  addProperty("columns", this, "pointSymbolColumn" , "pointSymbol");
  addProperty("columns", this, "vectorXColumn"     , "vectorX"    );
  addProperty("columns", this, "vectorYColumn"     , "vectorY"    );

  // bivariate
  addProperty("bivariate", this, "bivariate"         , "enabled");
  addProperty("bivariate", this, "bivariateLineWidth", "width"  );

  // stacked
  addProperty("stacked", this, "stacked", "enabled");

  // cumulative
  addProperty("cumulative", this, "cumulative", "enabled");

  // points
  addProperty("points"       , this, "points"           , "visible");
  addProperty("points"       , this, "symbolName"       , "symbol" );
  addProperty("points"       , this, "symbolSize"       , "size"   );
  addProperty("points/stroke", this, "symbolStroked"    , "visible");
  addProperty("points/stroke", this, "pointsStrokeColor", "color"  );
  addProperty("points/stroke", this, "pointsStrokeAlpha", "alpha"  );
  addProperty("points/stroke", this, "symbolLineWidth"  , "width"  );
  addProperty("points/fill"  , this, "symbolFilled"     , "visible");
  addProperty("points/fill"  , this, "pointsFillColor"  , "color"  );
  addProperty("points/fill"  , this, "pointsFillAlpha"  , "alpha"  );

  // lines
  addProperty("lines", this, "lines"          , "visible"   );
  addProperty("lines", this, "linesSelectable", "selectable");
  addProperty("lines", this, "linesColor"     , "color"     );
  addProperty("lines", this, "linesAlpha"     , "alpha"     );
  addProperty("lines", this, "linesWidth"     , "width"     );
  addProperty("lines", this, "roundedLines"   , "rounded"   );

  // fill under
  addProperty("fillUnder", this, "fillUnder"       , "visible" );
  addProperty("fillUnder", this, "fillUnderColor"  , "color"   );
  addProperty("fillUnder", this, "fillUnderAlpha"  , "alpha"   );
  addProperty("fillUnder", this, "fillUnderPattern", "pattern" );
  addProperty("fillUnder", this, "fillUnderPos"    , "position");
  addProperty("fillUnder", this, "fillUnderSide"   , "side"    );

  // impulse
  addProperty("impulse", this, "impulse"     , "visible");
  addProperty("impulse", this, "impulseColor", "color"  );
  addProperty("impulse", this, "impulseAlpha", "alpha"  );
  addProperty("impulse", this, "impulseWidth", "width"  );

  // vectors
  addProperty("vectors", this     , "vectors"  , "visible"  );
  addProperty("vectors", arrowObj_, "length"   , "length"   );
  addProperty("vectors", arrowObj_, "angle"    , "angle"    );
  addProperty("vectors", arrowObj_, "backAngle", "backAngle");
  addProperty("vectors", arrowObj_, "fhead"    , "fhead"    );
  addProperty("vectors", arrowObj_, "thead"    , "thead"    );
  addProperty("vectors", arrowObj_, "filled"   , "filled"   );
  addProperty("vectors", arrowObj_, "empty"    , "empty"    );
  addProperty("vectors", arrowObj_, "lineWidth", "lineWidth");
  addProperty("vectors", arrowObj_, "labels"   , "labels"   );

  // data label
  addProperty("dataLabel", this, "dataLabelColor", "color");
  addProperty("dataLabel", this, "dataLabelAngle", "angle");
}

//---

void
CQChartsXYPlot::
setBivariate(bool b)
{
  CQChartsUtil::testAndSet(bivariateLineData_.visible, b, [&]() { updateObjs(); } );
}

void
CQChartsXYPlot::
setStacked(bool b)
{
  CQChartsUtil::testAndSet(stacked_, b, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsXYPlot::
setCumulative(bool b)
{
  CQChartsUtil::testAndSet(cumulative_, b, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsXYPlot::
setImpulse(bool b)
{
  CQChartsUtil::testAndSet(impulseData_.visible, b, [&]() { updateObjs(); } );
}

bool
CQChartsXYPlot::
isVectors() const
{
  return arrowObj_->isVisible();
}

void
CQChartsXYPlot::
setVectors(bool b)
{
  if (b != isVectors()) {
    arrowObj_->setVisible(b);

    updateObjs();
  }
}

void
CQChartsXYPlot::
setFillUnder(bool b)
{
  CQChartsUtil::testAndSet(fillUnderData_.fillData.visible, b, [&]() { updateObjs(); } );
}

QString
CQChartsXYPlot::
symbolName() const
{
  return CQChartsPlotSymbolMgr::typeToName(pointData_.type);
}

void
CQChartsXYPlot::
setSymbolName(const QString &s)
{
  CQChartsPlotSymbol::Type type = CQChartsPlotSymbolMgr::nameToType(s);

  if (type != CQChartsPlotSymbol::Type::NONE) {
    pointData_.type = type;

    update();
  }
}

//---

void
CQChartsXYPlot::
setImpulseColor(const CQChartsColor &c)
{
  CQChartsUtil::testAndSet(impulseData_.color, c, [&]() { update(); } );
}

void
CQChartsXYPlot::
setImpulseAlpha(double a)
{
  CQChartsUtil::testAndSet(impulseData_.alpha, a, [&]() { update(); } );
}

void
CQChartsXYPlot::
setImpulseWidth(const CQChartsLength &w)
{
  CQChartsUtil::testAndSet(impulseData_.width, w, [&]() { update(); } );
}

QColor
CQChartsXYPlot::
interpImpulseColor(int i, int n) const
{
  return impulseColor().interpColor(this, i, n);
}

//---

const CQChartsColor &
CQChartsXYPlot::
pointsStrokeColor() const
{
  return pointData_.stroke.color;
}

void
CQChartsXYPlot::
setPointsStrokeColor(const CQChartsColor &c)
{
  pointData_.stroke.color = c;

  update();
}

QColor
CQChartsXYPlot::
interpPointStrokeColor(int i, int n) const
{
  return pointsStrokeColor().interpColor(this, i, n);
}

double
CQChartsXYPlot::
pointsStrokeAlpha() const
{
  return pointData_.stroke.alpha;
}

void
CQChartsXYPlot::
setPointsStrokeAlpha(double a)
{
  pointData_.stroke.alpha = a;

  update();
}

const CQChartsColor &
CQChartsXYPlot::
pointsFillColor() const
{
  return pointData_.fill.color;
}

void
CQChartsXYPlot::
setPointsFillColor(const CQChartsColor &c)
{
  pointData_.fill.color = c;

  update();
}

QColor
CQChartsXYPlot::
interpPointFillColor(int i, int n) const
{
  return pointsFillColor().interpColor(this, i, n);
}

double
CQChartsXYPlot::
pointsFillAlpha() const
{
  return pointData_.fill.alpha;
}

void
CQChartsXYPlot::
setPointsFillAlpha(double a)
{
  pointData_.fill.alpha = a;

  update();
}

//---

const CQChartsColor &
CQChartsXYPlot::
linesColor() const
{
  return lineData_.color;
}

void
CQChartsXYPlot::
setLinesColor(const CQChartsColor &c)
{
  lineData_.color = c;

  update();
}

QColor
CQChartsXYPlot::
interpLinesColor(int i, int n) const
{
  return linesColor().interpColor(this, i, n);
}

//---

const CQChartsColor &
CQChartsXYPlot::
fillUnderColor() const
{
  return fillUnderData_.fillData.color;
}

void
CQChartsXYPlot::
setFillUnderColor(const CQChartsColor &c)
{
  fillUnderData_.fillData.color = c;

  update();
}

QColor
CQChartsXYPlot::
interpFillUnderColor(int i, int n) const
{
  return fillUnderData_.fillData.color.interpColor(this, i, n);
}

//---

QColor
CQChartsXYPlot::
interpDataLabelColor(int i, int n)
{
  return dataLabelData_.color.interpColor(this, i, n);
}

//---

void
CQChartsXYPlot::
drawBivariateLine(QPainter *painter, const QPointF &p1, const QPointF &p2, const QColor &c)
{
  bivariateLineData_.color = c;

  drawLine(painter, p1, p2, bivariateLineData_);
}

QColor
CQChartsXYPlot::
interpPaletteColor(int i, int n, bool scale) const
{
  if (isOverlay()) {
    if (prevPlot() || nextPlot()) {
      CQChartsPlot *plot1 = prevPlot();
      CQChartsPlot *plot2 = nextPlot();

      while (plot1) { ++n; plot1 = plot1->prevPlot(); }
      while (plot2) { ++n; plot2 = plot2->nextPlot(); }
    }

    //---

    CQChartsPlot *plot1 = prevPlot();

    while (plot1) {
      ++i;

      plot1 = plot1->prevPlot();
    }

    return CQChartsPlot::interpPaletteColor(i, n, scale);
  }
  else {
    return CQChartsPlot::interpPaletteColor(i, n, scale);
  }
}

//---

void
CQChartsXYPlot::
updateRange(bool apply)
{
  QAbstractItemModel *model = this->model().data();

  if (! model)
    return;

  //---

  // calc data range (x, y values)
  class RowVisitor : public ModelVisitor {
   public:
    RowVisitor(CQChartsXYPlot *plot) :
     plot_(plot) {
      int ns = plot_->numSets();

      sum_.resize(ns);
    }

    State visit(QAbstractItemModel *, const QModelIndex &parent, int row) override {
      lastSum_ = sum_;

      //---

      double x; std::vector<double> y; QModelIndex ind;

      if (! plot_->rowData(parent, row, x, y, ind, /*skipBad*/true))
        return State::SKIP;

      int ny = y.size();

      //---

      if      (plot_->isBivariate()) {
        for (int i = 0; i < ny; ++i)
          range_.updateRange(x, y[i]);
      }
      else if (plot_->isStacked()) {
        double sum1 = 0.0;

        for (int i = 0; i < ny; ++i)
          sum1 += y[i];

        range_.updateRange(x, 0.0);
        range_.updateRange(x, sum1);
      }
      else {
        for (int i = 0; i < ny; ++i) {
          double y1 = y[i];

          if (plot_->isCumulative()) {
            y1 = y[i] + lastSum_[i];

            sum_[i] += y[i];
          }

          range_.updateRange(x, y1);
        }
      }

      return State::OK;
    }

    const CQChartsGeom::Range &range() const { return range_; }

   private:
    using Reals = std::vector<double>;

    CQChartsXYPlot*     plot_ { nullptr };
    CQChartsGeom::Range range_;
    Reals               sum_;
    Reals               lastSum_;
  };

  RowVisitor visitor(this);

  visitModel(visitor);

  dataRange_ = visitor.range();

  //---

  adjustDataRange();

  if (dataRange_.isSet()) {
    if (dataRange_.dx() < 1E-6) {
      double xm = dataRange_.xmid();

      dataRange_.setLeft (xm + 1.0);
      dataRange_.setRight(xm + 1.0);
    }

    if (dataRange_.dy() < 1E-6) {
      double ym = dataRange_.ymid();

      dataRange_.setBottom(ym - 1.0);
      dataRange_.setTop   (ym + 1.0);
    }
  }

  //---

  setXValueColumn(xColumn());
  setYValueColumn(yColumn());

  //---

  if (isOverlay()) {
    if (isFirstPlot())
      xAxis()->setColumn(xColumn());
  }
  else {
    xAxis()->setColumn(xColumn());
  }

  bool ok;

  QString xname = modelHeaderString(xColumn(), ok);

  if (isOverlay()) {
    if (isFirstPlot())
      xAxis()->setLabel(xname);
  }
  else {
    xAxis()->setLabel(xname);
  }

  int ns = numSets();

  if      (isBivariate() && ns > 1) {
    QString name = titleStr();

    if (! name.length()) {
      CQChartsColumn yColumn1 = getSetColumn(0);
      CQChartsColumn yColumn2 = getSetColumn(1);

      bool ok;

      QString yname1 = modelHeaderString(yColumn1, ok);
      QString yname2 = modelHeaderString(yColumn2, ok);

      name = QString("%1-%2").arg(yname1).arg(yname2);
    }

    if (isOverlay()) {
      if      (isFirstPlot())
        yAxis()->setLabel(name);
      else if (isOverlayOtherPlot())
        firstPlot()->yAxis()->setLabel(firstPlot()->yAxis()->label() + ", " + name);
    }
    else {
      yAxis()->setLabel(name);
    }
  }
  else if (isStacked()) {
  }
  else {
    CQChartsColumn yColumn = getSetColumn(0);

    if (isOverlay()) {
      if (isFirstPlot())
        yAxis()->setColumn(yColumn);
    }
    else
      yAxis()->setColumn(yColumn);

    QString yname;

    for (int j = 0; j < numSets(); ++j) {
      bool ok;

      CQChartsColumn yColumn = getSetColumn(j);

      QString yname1 = modelHeaderString(yColumn, ok);

      if (yname.length())
        yname += ", ";

      yname += yname1;
    }

    if (isOverlay()) {
      if      (isFirstPlot())
        yAxis()->setLabel(yname);
      else if (isOverlayOtherPlot())
        firstPlot()->yAxis()->setLabel(firstPlot()->yAxis()->label() + ", " + yname);
    }
    else {
      yAxis()->setLabel(yname);
    }
  }

  //---

  if (apply)
    applyDataRange();
}

void
CQChartsXYPlot::
postInit()
{
  if      (isBivariate()) {
    setLines (true);
    setPoints(false);
  }
  else if (isStacked()) {
    setFillUnder(true);
    setPoints   (false);
  }
}

bool
CQChartsXYPlot::
initObjs()
{
  if (! dataRange_.isSet()) {
    updateRange();

    if (! dataRange_.isSet())
      return false;
  }

  //---

  if (! plotObjs_.empty())
    return false;

  //---

  QAbstractItemModel *model = this->model().data();

  if (! model)
    return false;

  //---

  // TODO: use actual symbol size
  symbolWidth_  = (dataRange_.xmax() - dataRange_.xmin())/100.0;
  symbolHeight_ = (dataRange_.ymax() - dataRange_.ymin())/100.0;

  //---

  if (pointColorColumn().isValid())
    pointColorColumnType_ = columnValueType(pointColorColumn());

  //---

  struct IndPoly {
    using Inds = std::vector<QModelIndex>;

    Inds      inds;
    QPolygonF poly;
  };

  using Polygons = std::vector<IndPoly>;

  // create line per set
  class RowVisitor : public ModelVisitor {
   public:
    RowVisitor(CQChartsXYPlot *plot) :
     plot_(plot) {
      ns_ = plot_->numSets();

      setPoly_.resize(ns_);
    }

    State visit(QAbstractItemModel *, const QModelIndex &parent, int row) override {
      double x; std::vector<double> y; QModelIndex ind;

      (void) plot_->rowData(parent, row, x, y, ind, /*skipBad*/false);
      assert(int(y.size()) == ns_);

      for (int i = 0; i < ns_; ++i) {
        setPoly_[i].inds.push_back(ind);

        setPoly_[i].poly << QPointF(x, y[i]);
      }

      return State::OK;
    }

    // stack lines
    void stack() {
      for (int i = 1; i < ns_; ++i) {
        QPolygonF &poly1 = setPoly_[i - 1].poly;
        QPolygonF &poly2 = setPoly_[i    ].poly;

        int np = poly1.size();
        assert(poly2.size() == np);

        for (int j = 0; j < np; ++j) {
          const QPointF &p1 = poly1[j];

          double y1 = p1.y();

          const QPointF &p2 = poly2[j];

          double x2 = p2.x(), y2 = p2.y();

          if (CQChartsUtil::isNaN(y1) || CQChartsUtil::isInf(y1))
            continue;

          poly2[j] = QPointF(x2, y2 + y1);
        }
      }
    }

    // cumulate
    void cumulate() {
      for (int i = 0; i < ns_; ++i) {
        QPolygonF &poly = setPoly_[i].poly;

        int np = poly.size();

        for (int j = 1; j < np; ++j) {
          const QPointF &p1 = poly[j - 1];
          const QPointF &p2 = poly[j    ];

          double y1 = p1.y();
          double x2 = p2.x(), y2 = p2.y();

          if (CQChartsUtil::isNaN(y1) || CQChartsUtil::isInf(y1))
            continue;

          poly[j] = QPointF(x2, y1 + y2);
        }
      }
    }

    const Polygons &setPoly() const { return setPoly_; }

   private:
    CQChartsXYPlot* plot_ { nullptr };
    int             ns_;
    Polygons        setPoly_;
  };

  //---

  RowVisitor visitor(this);

  visitModel(visitor);

  if      (isStacked())
    visitor.stack();
  else if (isCumulative())
    visitor.cumulate();

  const Polygons &setPoly = visitor.setPoly();

  //---

  double sw = symbolWidth ();
  double sh = symbolHeight();

  //---

  if      (isBivariate()) {
    // convert lines bivariate lines (line connected each point pair)
    int ns = setPoly.size();

    if (ns < 1)
      return false;

    Polygons polygons1, polygons2;

    polygons1.resize(ns - 1);
    polygons2.resize(ns - 1);

    const QPolygonF &poly = setPoly[0].poly;

    int np = poly.size();

    for (int ip = 0; ip < np; ++ip) {
      double x = 0.0;

      // sorted y vals
      std::set<double> sortedYVals;

      for (int j = 0; j < ns; ++j) {
        bool hidden = isSetHidden(j);

        if (hidden)
          continue;

        //---

        const QPolygonF &poly = setPoly[j].poly;

        const QPointF &p = poly[ip];

        if (CQChartsUtil::isNaN(p.y()) || CQChartsUtil::isInf(p.y()))
          continue;

        if (j == 0)
          x = p.x();

        sortedYVals.insert(p.y());
      }

      // need at least 2 values
      if (sortedYVals.size() < 2)
        continue;

      //---

      std::vector<double> yVals;

      for (const auto &y : sortedYVals)
        yVals.push_back(y);

      //---

      QModelIndex parent; // TODO: parent

      QModelIndex xind  = model->index(ip, xColumn().column(), parent);
      QModelIndex xind1 = normalizeIndex(xind);

      //---

      // connect each y value to next y value
      double y1  = yVals[0];
      int    ny1 = yVals.size();

      for (int j = 1; j < ny1; ++j) {
        double y2 = yVals[j];

        CQChartsGeom::BBox bbox(x - sw/2, y1 - sh/2, x + sw/2, y2 + sh/2);

        if (! isFillUnder()) {
          // use vertical line object for each point pair if not fill under
          CQChartsXYBiLineObj *lineObj =
            new CQChartsXYBiLineObj(this, bbox, x, y1, y2, xind1, j - 1, ny1 - 1, ip, np);

          addPlotObject(lineObj);
        }
        else {
          QPolygonF &poly1 = polygons1[j - 1].poly;
          QPolygonF &poly2 = polygons2[j - 1].poly;

          // build lower and upper poly line for fill under polygon
          poly1 << QPointF(x, y1);
          poly2 << QPointF(x, y2);
        }

        y1 = y2;
      }
    }

    //---

    // add lower, upper and polygon objects for fill under
    if (isFillUnder()) {
      QString name = titleStr();

      if (! name.length()) {
        CQChartsColumn yColumn1 = getSetColumn(0);
        CQChartsColumn yColumn2 = getSetColumn(1);

        bool ok;

        QString yname1 = modelHeaderString(yColumn1, ok);
        QString yname2 = modelHeaderString(yColumn2, ok);

        name = QString("%1-%2").arg(yname1).arg(yname2);
      }

      int ns = numSets();

      for (int j = 1; j < ns; ++j) {
        bool hidden = isSetHidden(j);

        if (hidden)
          continue;

        //---

        QString name1;

        if (ns > 2)
          name1 = QString("%1:%2").arg(name).arg(j);
        else
          name1 = name;

        //---

        QPolygonF &poly1 = polygons1[j - 1].poly;
        QPolygonF &poly2 = polygons2[j - 1].poly;

        addPolyLine(poly1, j - 1, ns - 1, name1);
        addPolyLine(poly2, j - 1, ns - 1, name1);

        int len = poly1.size();

        if      (fillUnderSide() == "both") {
          // add upper poly line to lower one (points reversed) to build fill polygon
          for (int k = len - 1; k >= 0; --k)
            poly1 << poly2[k];
        }
        else if (fillUnderSide() == "above") {
          QPolygonF poly3, poly4;

          QPointF pa1, pb1; bool above1 = true;

          for (int k = 0; k < len; ++k) {
            const QPointF &pa2 = poly1[k];
            const QPointF &pb2 = poly2[k];

            bool above2 = (pa2.y() > pb2.y());

            if (k > 0 && above1 != above2) {
              QPointF pi;

              CQChartsUtil::intersectLines(pa1, pa2, pb1, pb2, pi);

              poly3 << pi;
              poly4 << pi;
            }

            if (above2) {
              poly3 << pa2;
              poly4 << pb2;
            }

            pa1 = pa2; pb1 = pb2; above1 = above2;
          }

          len = poly4.size();

          for (int k = len - 1; k >= 0; --k)
            poly3 << poly4[k];

          poly1 = poly3;
        }
        else if (fillUnderSide() == "below") {
          QPolygonF poly3, poly4;

          QPointF pa1, pb1; bool below1 = true;

          for (int k = 0; k < len; ++k) {
            const QPointF &pa2 = poly1[k];
            const QPointF &pb2 = poly2[k];

            bool below2 = (pa2.y() < pb2.y());

            if (k > 0 && below1 != below2) {
              QPointF pi;

              CQChartsUtil::intersectLines(pa1, pa2, pb1, pb2, pi);

              poly3 << pi;
              poly4 << pi;
            }

            if (below2) {
              poly3 << pa2;
              poly4 << pb2;
            }

            pa1 = pa2; pb1 = pb2; below1 = below2;
          }

          len = poly4.size();

          for (int k = len - 1; k >= 0; --k)
            poly3 << poly4[k];

          poly1 = poly3;
        }

        addPolygon(poly1, j - 1, ns - 1, name1);
      }
    }
  }
  else {
    // convert lines into set polygon and set poly lines (more than one if NaNs)
    int ns = numSets();

    for (int j = 0; j < ns; ++j) {
      bool hidden = isSetHidden(j);

      if (hidden)
        continue;

      //---

      // get column name
      CQChartsColumn yColumn = getSetColumn(j);

      bool ok;

      QString name = modelHeaderString(yColumn, ok);

      //---

      QPolygonF polyShape, polyLine;

      const QPolygonF &poly     = setPoly[j].poly;
      const QPolygonF &prevPoly = (j > 0 ? setPoly[j - 1].poly : poly);

      int np = poly.size();
      assert(prevPoly.size() == np);

      for (int ip = 0; ip < np; ++ip) {
        const QPointF &p = poly[ip];

        double x = p.x(), y = p.y();

        if (CQChartsUtil::isNaN(x) || CQChartsUtil::isInf(x) ||
            CQChartsUtil::isNaN(y) || CQChartsUtil::isInf(y)) {
          if (polyLine.count()) {
            addPolyLine(polyLine, j, ns, name);

            polyLine = QPolygonF();
          }

          continue;
        }

        //---

        // get point size
        double size = -1;

        if (sizeColumn().isValid()) {
          bool ok;

          QModelIndex parent; // TODO: parent

          size = modelReal(ip, sizeColumn(), parent, ok);

          if (! ok)
            size = -1;
        }

        //---

        // create point object
        QModelIndex parent; // TODO: parent

        QModelIndex xind  = model->index(ip, xColumn().column()); // TODO: parent
        QModelIndex xind1 = normalizeIndex(xind);

        CQChartsGeom::BBox bbox(x - sw/2, y - sh/2, x + sw/2, y + sh/2);

        CQChartsXYPointObj *pointObj =
          new CQChartsXYPointObj(this, bbox, x, y, size, xind1, j, ns, ip, np);

        addPlotObject(pointObj);

        //---

        // set vector data
        if (isVectors()) {
          double vx = 0.0, vy = 0.0;

          if (vectorXColumn().isValid()) {
            bool ok;

            QModelIndex parent; // TODO: parent

            vx = modelReal(ip, vectorXColumn(), parent, ok);
          }

          if (vectorYColumn().isValid()) {
            bool ok;

            QModelIndex parent; // TODO: parent

            vy = modelReal(ip, vectorYColumn(), parent, ok);
          }

          pointObj->setVector(vx, vy);
        }

        //---

        // set optional point label, color and symbol
        if (pointLabelColumn().isValid()) {
          QModelIndex parent; // TODO: parent

          bool ok;

          QString pointLabelStr = modelString(ip, pointLabelColumn(), parent, ok);

          if (ok && pointLabelStr.length())
            pointObj->setLabel(pointLabelStr);
        }

        if (pointColorColumn().isValid()) {
          QModelIndex parent; // TODO: parent

          CQChartsColor c;

          bool ok;

          if (pointColorColumnType_ == CQBaseModel::Type::COLOR) {
            c = modelColor(ip, pointColorColumn(), parent, ok);
          }
          else {
            QString str = modelString(ip, pointColorColumn(), parent, ok);

            if (ok && str.length())
              c = QColor(str);
          }

          if (c.isValid())
            pointObj->setColor(c);
        }

        if (pointSymbolColumn().isValid()) {
          QModelIndex parent; // TODO: parent

          bool ok;

          QString pointSymbolStr = modelString(ip, pointSymbolColumn(), parent, ok);

          if (ok && pointSymbolStr.length())
            pointObj->setSymbol(CQChartsPlotSymbolMgr::nameToType(pointSymbolStr));
        }

        //---

        // add impulse line (down to or up to zero)
        if (! isStacked() && isImpulse()) {
          double ys = std::min(y, 0.0);
          double ye = std::max(y, 0.0);

          CQChartsGeom::BBox bbox(x - sw/2, ys, x + sw/2, ye);

          CQChartsXYImpulseLineObj *impulseObj =
            new CQChartsXYImpulseLineObj(this, bbox, x, ys, x, ye, xind1, j, ns, ip, np);

          addPlotObject(impulseObj);
        }

        //---

        // add point to poly line
        polyLine << p;

        //---

        // add point to polygon

        // if first point then add first point of previous polygon
        if (ip == 0) {
          if (isStacked()) {
            double y1 = (j > 0 ? prevPoly[ip].y() : dataRange_.ymin());

            if (CQChartsUtil::isNaN(y1) || CQChartsUtil::isInf(y1))
              y1 = dataRange_.ymin();

            polyShape << QPointF(x, y1);
          }
          else
            polyShape << fillUnderPos(x, dataRange_.ymin());
        }

        polyShape << p;

        // if last point then add last point of previous polygon
        if (ip == np - 1) {
          if (isStacked()) {
            double y1 = (j > 0 ? prevPoly[ip].y() : dataRange_.ymin());

            if (CQChartsUtil::isNaN(y1) || CQChartsUtil::isInf(y1))
              y1 = dataRange_.ymin();

            polyShape << QPointF(x, y1);
          }
          else
            polyShape << fillUnderPos(x, dataRange_.ymin());
        }
      }

      //---

      if (isStacked()) {
        // add points from previous polygon to bottom of polygon
        if (j > 0) {
          for (int ip = np - 2; ip >= 1; --ip) {
            double x1 = prevPoly[ip].x();
            double y1 = prevPoly[ip].y();

            if (CQChartsUtil::isNaN(x1) || CQChartsUtil::isInf(x1))
              x1 = poly[ip].x();

            if (CQChartsUtil::isNaN(y1) || CQChartsUtil::isInf(y1))
              y1 = 0.0;

            polyShape << QPointF(x1, y1);
          }
        }
      }

      //---

      if (polyLine.count())
        addPolyLine(polyLine, j, ns, name);

      //---

      addPolygon(polyShape, j, ns, name);
    }
  }

  //----

  resetKeyItems();

  //---

  return true;
}

bool
CQChartsXYPlot::
rowData(const QModelIndex &parent, int row, double &x, std::vector<double> &y,
        QModelIndex &ind, bool skipBad) const
{
  QAbstractItemModel *model = this->model().data();
  assert(model);

  //---

  ind = model->index(row, xColumn().column(), parent);

  bool ok1 = modelMappedReal(row, xColumn(), parent, x, isLogX(), row);

  //---

  bool ok2 = true;

  int ns = numSets();

  for (int i = 0; i < ns; ++i) {
    CQChartsColumn yColumn = getSetColumn(i);

    double y1;

    bool ok3 = modelMappedReal(row, yColumn, parent, y1, isLogY(), row);

    if (! ok3) {
      if (skipBad)
        continue;

      y1  = CQChartsUtil::getNaN();
      ok2 = false;
    }

    //---

    y.push_back(y1);
  }

  if (ns && y.empty())
    return false;

  return (ok1 && ok2);
}

QPointF
CQChartsXYPlot::
fillUnderPos(double x, double y) const
{
  const QString &str = fillUnderPosStr();

  QStringList strs = str.split(" ", QString::KeepEmptyParts);

  if (! strs.length())
    return QPointF(x, y);

  double x1 = x;
  double y1 = y;

  if (strs.length() > 1) {
    const QString &xstr = strs[0];
    const QString &ystr = strs[1];

    if      (xstr == "min" || xstr == "xmin") {
      if (dataRange_.isSet())
        x1 = dataRange_.xmin();
    }
    else if (xstr == "max" || xstr == "xmax") {
      if (dataRange_.isSet())
        x1 = dataRange_.xmax();
    }
    else {
      bool ok;

      x1 = CQChartsUtil::toReal(xstr, ok);

      if (! ok)
        x1 = x;
    }

    if      (ystr == "min" || ystr == "ymin") {
      if (dataRange_.isSet())
        y1 = dataRange_.ymin();
    }
    else if (ystr == "max" || ystr == "ymax") {
      if (dataRange_.isSet())
        y1 = dataRange_.ymax();
    }
    else {
      bool ok;

      y1 = CQChartsUtil::toReal(ystr, ok);

      if (! ok)
        y1 = y;
    }
  }
  else {
    const QString &str = strs[0];

    if      (str == "xmin") {
      if (dataRange_.isSet())
        x1 = dataRange_.xmin();
    }
    else if (str == "xmax") {
      if (dataRange_.isSet())
        x1 = dataRange_.xmax();
    }
    else if (str == "min" || str == "ymin") {
      if (dataRange_.isSet())
        y1 = dataRange_.ymin();
    }
    else if (str == "max" || str == "ymax") {
      if (dataRange_.isSet())
        y1 = dataRange_.ymax();
    }
    else {
      bool ok;

      y1 = CQChartsUtil::toReal(str, ok);

      if (! ok)
        y1 = y;
    }
  }

  return QPointF(x1, y1);
}

void
CQChartsXYPlot::
addPolyLine(const QPolygonF &polyLine, int i, int n, const QString &name)
{
  CQChartsGeom::BBox bbox = CQChartsUtil::fromQRect(polyLine.boundingRect());

  CQChartsXYPolylineObj *lineObj =
    new CQChartsXYPolylineObj(this, bbox, polyLine, name, i, n);

  addPlotObject(lineObj);
}

void
CQChartsXYPlot::
addPolygon(const QPolygonF &poly, int i, int n, const QString &name)
{
  CQChartsGeom::BBox bbox = CQChartsUtil::fromQRect(poly.boundingRect());

  CQChartsXYPolygonObj *polyObj =
    new CQChartsXYPolygonObj(this, bbox, poly, name, i, n);

  addPlotObject(polyObj);
}

QString
CQChartsXYPlot::
valueName(int iset, int irow) const
{
  QString name;

  if (iset >= 0) {
    CQChartsColumn yColumn = getSetColumn(iset);

    bool ok;

    name = modelHeaderString(yColumn, ok);
  }

  if (nameColumn().isValid()) {
    QModelIndex parent; // TODO: parent

    bool ok;

    QString name1 = modelString(irow, nameColumn(), parent, ok);

    if (ok)
      return name1;
  }

  return name;
}

void
CQChartsXYPlot::
addKeyItems(CQChartsPlotKey *key)
{
  QAbstractItemModel *model = this->model().data();

  if (! model)
    return;

  int row, col;

  if (! key->isHorizontal()) {
    row = key->maxRow();
    col = 0;
  }
  else {
    row = 0;
    col = key->maxCol();
  }

  int ns = numSets();

  if      (isBivariate()) {
    QString name = titleStr();

    if (! name.length()) {
      CQChartsColumn yColumn1 = getSetColumn(0);
      CQChartsColumn yColumn2 = getSetColumn(1);

      bool ok;

      QString yname1 = modelHeaderString(yColumn1, ok);
      QString yname2 = modelHeaderString(yColumn2, ok);

      name = QString("%1-%2").arg(yname1).arg(yname2);
    }

    CQChartsXYKeyColor *color = new CQChartsXYKeyColor(this, 0, 1);
    CQChartsXYKeyText  *text  = new CQChartsXYKeyText (this, 0, name);

    key->addItem(color, row, col    );
    key->addItem(text , row, col + 1);
  }
  else if (isStacked()) {
    for (int i = 0; i < ns; ++i) {
      CQChartsColumn yColumn = getSetColumn(i);

      bool ok;

      QString name = modelHeaderString(yColumn, ok);

      CQChartsXYKeyLine *line = new CQChartsXYKeyLine(this, i, ns);
      CQChartsXYKeyText *text = new CQChartsXYKeyText(this, i, name);

      key->addItem(line, row + i, col    );
      key->addItem(text, row + i, col + 1);
    }
  }
  else {
    for (int i = 0; i < ns; ++i) {
      CQChartsColumn yColumn = getSetColumn(i);

      bool ok;

      QString name = modelHeaderString(yColumn, ok);

#if 0
      if (ns == 1 && (name == "" || name == QString("%1").arg(yColumn + 1))) {
        if      (titleStr().length())
          name = titleStr();
        else if (fileName().length())
          name = fileName();
      }
#endif
      if (ns == 1 && (titleStr().length() || fileName().length())) {
        if      (titleStr().length())
          name = titleStr();
        else if (fileName().length())
          name = fileName();
      }

      CQChartsXYKeyLine *line = new CQChartsXYKeyLine(this, i, ns);
      CQChartsXYKeyText *text = new CQChartsXYKeyText(this, i, name);

      if (! key->isHorizontal()) {
        key->addItem(line, row + i, col    );
        key->addItem(text, row + i, col + 1);
      }
      else {
        key->addItem(line, row, col + 2*i    );
        key->addItem(text, row, col + 2*i + 1);
      }
    }
  }

  key->plot()->updateKeyPosition(/*force*/true);
}

int
CQChartsXYPlot::
numSets() const
{
  if (yColumns_.empty())
    return 1;

  return yColumns_.size();
}

CQChartsColumn
CQChartsXYPlot::
getSetColumn(int i) const
{
  if (i < 0 || (i > 1 && i >= int(yColumns_.size())))
    return CQChartsColumn();

  if (! yColumns_.empty())
    return yColumns_[i];

  return yColumn();
}

bool
CQChartsXYPlot::
probe(ProbeData &probeData) const
{
  std::vector<double> yvals;

  if (! interpY(probeData.x, yvals))
    return false;

  for (const auto &yval : yvals)
    probeData.yvals.emplace_back(yval);

  return true;
}

bool
CQChartsXYPlot::
interpY(double x, std::vector<double> &yvals) const
{
  if (isBivariate()) {
    return false;
  }

  for (const auto &plotObj : plotObjs_) {
    CQChartsXYPolylineObj *polyObj = dynamic_cast<CQChartsXYPolylineObj *>(plotObj);

    if (! polyObj)
      continue;

    std::vector<double> yvals1;

    polyObj->interpY(x, yvals1);

    for (const auto &y1 : yvals1)
      yvals.push_back(y1);
  }

  return ! yvals.empty();
}

//------

bool
CQChartsXYPlot::
addMenuItems(QMenu *menu)
{
  QAction *bivariateAction  = new QAction("Bivariate" , menu);
  QAction *stackedAction    = new QAction("Stacked"   , menu);
  QAction *cumulativeAction = new QAction("Cumulative", menu);
  QAction *impulseAction    = new QAction("Impulse"   , menu);
  QAction *fullUnderAction  = new QAction("Fill Under", menu);

  bivariateAction->setCheckable(true);
  bivariateAction->setChecked(isBivariate());

  stackedAction->setCheckable(true);
  stackedAction->setChecked(isStacked());

  cumulativeAction->setCheckable(true);
  cumulativeAction->setChecked(isCumulative());

  impulseAction->setCheckable(true);
  impulseAction->setChecked(isImpulse());

  fullUnderAction->setCheckable(true);
  fullUnderAction->setChecked(isFillUnder());

  connect(bivariateAction , SIGNAL(triggered(bool)), this, SLOT(setBivariate(bool)));
  connect(stackedAction   , SIGNAL(triggered(bool)), this, SLOT(setStacked(bool)));
  connect(cumulativeAction, SIGNAL(triggered(bool)), this, SLOT(setCumulative(bool)));
  connect(impulseAction   , SIGNAL(triggered(bool)), this, SLOT(setImpulse(bool)));
  connect(fullUnderAction , SIGNAL(triggered(bool)), this, SLOT(setFillUnder(bool)));

  menu->addSeparator();

  menu->addAction(bivariateAction);
  menu->addAction(stackedAction);
  menu->addAction(cumulativeAction);
  menu->addAction(impulseAction);
  menu->addAction(fullUnderAction);

  return true;
}

//------

void
CQChartsXYPlot::
draw(QPainter *painter)
{
  initPlotObjs();

  //---

  drawParts(painter);
}

void
CQChartsXYPlot::
drawArrow(QPainter *painter, const QPointF &p1, const QPointF &p2)
{
  arrowObj_->setFrom(p1);
  arrowObj_->setTo  (p2);

  arrowObj_->draw(painter);
}

//------

CQChartsXYBiLineObj::
CQChartsXYBiLineObj(CQChartsXYPlot *plot, const CQChartsGeom::BBox &rect, double x,
                    double y1, double y2, const QModelIndex &ind, int iset, int nset,
                    int i, int n) :
 CQChartsPlotObj(plot, rect), plot_(plot), x_(x), y1_(y1), y2_(y2), ind_(ind),
 iset_(iset), nset_(nset), i_(i), n_(n)
{
}

QString
CQChartsXYBiLineObj::
calcId() const
{
  QModelIndex ind1 = plot_->unnormalizeIndex(ind_);

  QString idStr;

  if (calcColumnId(ind1, idStr))
    return idStr;

  //---

  QString name  = plot_->valueName(-1, ind_.row());
  QString y1str = plot_->yStr(y1_);
  QString y2str = plot_->yStr(y2_);

  if (name.length())
    return QString("%1:%2:%3").arg(name).arg(y1str).arg(y2str);

  QString xstr = plot_->xStr(x_);

  return QString("%1:%2:%3").arg(xstr).arg(y1str).arg(y2str);
}

bool
CQChartsXYBiLineObj::
visible() const
{
  if (! plot_->isLines() && ! plot_->isPoints())
    return false;

  return isVisible();
}

bool
CQChartsXYBiLineObj::
inside(const CQChartsGeom::Point &p) const
{
  if (! visible())
    return false;

  double px, py1, py2;

  plot_->windowToPixel(x_, y1_, px, py1);
  plot_->windowToPixel(x_, y2_, px, py2);

  double s = plot_->symbolSize();

  CQChartsGeom::BBox pbbox(px - s, py1 - s, px + s, py2 + s);

  CQChartsGeom::Point pp;

  plot_->windowToPixel(p, pp);

  return pbbox.inside(pp);
}

void
CQChartsXYBiLineObj::
getSelectIndices(Indices &inds) const
{
  if (! visible())
    return;

  addColumnSelectIndex(inds, ind_.column());
}

void
CQChartsXYBiLineObj::
addColumnSelectIndex(Indices &inds, const CQChartsColumn &column) const
{
  if (column.isValid())
    addSelectIndex(inds, ind_.row(), column, ind_.parent());
}

void
CQChartsXYBiLineObj::
draw(QPainter *painter, const CQChartsPlot::Layer &)
{
  if (! visible())
    return;

  double px, py1, py2;

  plot_->windowToPixel(x_, y1_, px, py1);
  plot_->windowToPixel(x_, y2_, px, py2);

  if (plot_->isLines()) {
    QPen   pen  (plot_->interpFillUnderColor(iset_, nset_));
    QBrush brush(Qt::NoBrush);

    plot_->updateObjPenBrushState(this, pen, brush);

    plot_->drawBivariateLine(painter, QPointF(px, py1), QPointF(px, py2), pen.color());
  }

  if (plot_->isPoints()) {
    CQChartsPlotSymbol::Type symbol      = plot_->symbolType();
    double                   s           = plot_->symbolSize();
    bool                     stroked     = plot_->isSymbolStroked();
    QColor                   strokeColor = plot_->interpPointStrokeColor(iset_, nset_);
    bool                     filled      = plot_->isSymbolFilled();
    QColor                   fillColor   = plot_->interpPointFillColor(iset_, nset_);

    QPen   pen  (strokeColor);
    QBrush brush(fillColor);

    plot_->updateObjPenBrushState(this, pen, brush);

    plot_->drawSymbol(painter, QPointF(px, py1), symbol, s,
                      stroked, pen.color(), 1, filled, brush.color());
    plot_->drawSymbol(painter, QPointF(px, py2), symbol, s,
                      stroked, pen.color(), 1, filled, brush.color());
  }
}

//------

CQChartsXYImpulseLineObj::
CQChartsXYImpulseLineObj(CQChartsXYPlot *plot, const CQChartsGeom::BBox &rect,
                         double x1, double y1, double x2, double y2, const QModelIndex &ind,
                         int iset, int nset, int i, int n) :
 CQChartsPlotObj(plot, rect), plot_(plot), x1_(x1), y1_(y1), x2_(x2), y2_(y2), ind_(ind),
 iset_(iset), nset_(nset), i_(i), n_(n)
{
}

QString
CQChartsXYImpulseLineObj::
calcId() const
{
  QModelIndex ind1 = plot_->unnormalizeIndex(ind_);

  QString idStr;

  if (calcColumnId(ind1, idStr))
    return idStr;

  //---

  QString name = plot_->valueName(iset_, ind_.row());
  QString xstr = plot_->xStr(x2_);
  QString ystr = plot_->yStr(y2_);

  if (name.length())
    return QString("%1:%2:%3").arg(name).arg(xstr).arg(ystr);

  return QString("%1:%2:%3:%4").arg(ind_.row()).arg(iset_).arg(xstr).arg(ystr);
}

bool
CQChartsXYImpulseLineObj::
visible() const
{
  return isVisible();
}

bool
CQChartsXYImpulseLineObj::
inside(const CQChartsGeom::Point &p) const
{
  if (! visible())
    return false;

  double px1, py1, px2, py2;

  plot_->windowToPixel(x1_, y1_, px1, py1);
  plot_->windowToPixel(x2_, y2_, px2, py2);

  double b = 2;

  double lw = std::max(plot_->lengthPixelWidth(plot_->impulseWidth()), 2*b);

  CQChartsGeom::BBox pbbox(px1 - lw/2, py1 - b, px2 + lw/2, py2 + b);

  CQChartsGeom::Point pp;

  plot_->windowToPixel(p, pp);

  return pbbox.inside(pp);
}

void
CQChartsXYImpulseLineObj::
getSelectIndices(Indices &inds) const
{
  if (! visible())
    return;

  addColumnSelectIndex(inds, ind_.column());
}

void
CQChartsXYImpulseLineObj::
addColumnSelectIndex(Indices &inds, const CQChartsColumn &column) const
{
  if (column.isValid())
    addSelectIndex(inds, ind_.row(), column, ind_.parent());
}

void
CQChartsXYImpulseLineObj::
draw(QPainter *painter, const CQChartsPlot::Layer &)
{
  if (! visible())
    return;

  double px1, py1, px2, py2;

  plot_->windowToPixel(x1_, y1_, px1, py1);
  plot_->windowToPixel(x2_, y2_, px2, py2);

  QColor strokeColor;

  if (nset_ > 1)
    strokeColor = plot_->interpImpulseColor(iset_, nset_);
  else
    strokeColor = plot_->interpImpulseColor(i_, n_);

  strokeColor.setAlphaF(plot_->impulseAlpha());

  double lw = plot_->lengthPixelWidth(plot_->impulseWidth());

  if (lw <= 1) {
    QBrush brush(Qt::NoBrush);
    QPen   pen  (strokeColor);

    pen.setWidthF(lw);

    plot_->updateObjPenBrushState(this, pen, brush);

    painter->setPen  (pen);
    painter->setBrush(brush);

    painter->drawLine(QPointF(px1, py1), QPointF(px2, py2));
  }
  else {
    QBrush brush(strokeColor);
    QPen   pen  (Qt::NoPen);

    plot_->updateObjPenBrushState(this, pen, brush);

    painter->setPen  (pen);
    painter->setBrush(brush);

    QRectF qrect(px1 - lw/2, py1, lw, py2 - py1);

    CQChartsRoundedPolygon::draw(painter, qrect, 0, 0);
  }
}

//------

CQChartsXYPointObj::
CQChartsXYPointObj(CQChartsXYPlot *plot, const CQChartsGeom::BBox &rect, double x, double y,
                   double size, const QModelIndex &ind, int iset, int nset, int i, int n) :
 CQChartsPlotObj(plot, rect), plot_(plot), pos_(x, y), size_(size), ind_(ind),
 iset_(iset), nset_(nset), i_(i), n_(n)
{
}

CQChartsXYPointObj::
~CQChartsXYPointObj()
{
  delete edata_;
}

QString
CQChartsXYPointObj::
calcId() const
{
  QModelIndex ind1 = plot_->unnormalizeIndex(ind_);

  QString idStr;

  if (calcColumnId(ind1, idStr))
    return idStr;

  //---

  QString name = plot_->valueName(iset_, ind_.row());
  QString xstr = plot_->xStr(pos_.x());
  QString ystr = plot_->yStr(pos_.y());

  if (name.length())
    return QString("%1:%2:%3").arg(name).arg(xstr).arg(ystr);

  return QString("%1:%2:%3:%4").arg(ind_.row()).arg(iset_).arg(xstr).arg(ystr);
}

void
CQChartsXYPointObj::
setLabel(const QString &label)
{
  if (! edata_)
    edata_ = new ExtraData;

  edata_->label = label;
}

void
CQChartsXYPointObj::
setColor(const CQChartsColor &c)
{
  if (! edata_)
    edata_ = new ExtraData;

  edata_->color = c;
}

void
CQChartsXYPointObj::
setSymbol(CQChartsPlotSymbol::Type symbol)
{
  if (! edata_)
    edata_ = new ExtraData;

  edata_->symbol = symbol;
}

void
CQChartsXYPointObj::
setVector(double vx, double vy)
{
  if (! edata_)
    edata_ = new ExtraData;

  edata_->vector = QPointF(vx, vy);
}

bool
CQChartsXYPointObj::
visible() const
{
  if (! plot_->isPoints())
    return false;

  return isVisible();
}

bool
CQChartsXYPointObj::
inside(const CQChartsGeom::Point &p) const
{
  if (! visible())
    return false;

  double px, py;

  plot_->windowToPixel(pos_.x(), pos_.y(), px, py);

  double s = (size_ <= 0 ? plot_->symbolSize() : size_);

  CQChartsGeom::BBox pbbox(px - s, py - s, px + s, py + s);

  CQChartsGeom::Point pp;

  plot_->windowToPixel(p, pp);

  return pbbox.inside(pp);
}

void
CQChartsXYPointObj::
getSelectIndices(Indices &inds) const
{
  if (! visible())
    return;

  addColumnSelectIndex(inds, plot_->xColumn());
  addColumnSelectIndex(inds, plot_->getSetColumn(iset_));
}

void
CQChartsXYPointObj::
addColumnSelectIndex(Indices &inds, const CQChartsColumn &column) const
{
  if (column.isValid())
    addSelectIndex(inds, ind_.row(), column, ind_.parent());
}

void
CQChartsXYPointObj::
draw(QPainter *painter, const CQChartsPlot::Layer &)
{
  if (! visible())
    return;

  CQChartsPlotSymbol::Type symbol      = plot_->symbolType();
  bool                     stroked     = plot_->isSymbolStroked();
  QColor                   strokeColor = plot_->interpPointStrokeColor(iset_, nset_);
  double                   lineWidth   = plot_->lengthPixelWidth(plot_->symbolLineWidth());
  bool                     filled      = plot_->isSymbolFilled();
  QColor                   fillColor   = plot_->interpPointFillColor(iset_, nset_);

  strokeColor.setAlphaF(plot_->pointsStrokeAlpha());
  fillColor  .setAlphaF(plot_->pointsFillAlpha());

  if (edata_ && edata_->symbol != CQChartsPlotSymbol::Type::NONE)
    symbol = edata_->symbol;

  if (edata_ && edata_->color)
    strokeColor = edata_->color->interpColor(plot_, 0, 1);

  double s = (size_ <= 0 ? plot_->symbolSize() : size_);

  QBrush brush(fillColor);
  QPen   pen  (strokeColor);

  plot_->updateObjPenBrushState(this, pen, brush);

  //---

  CQChartsGeom::Point pp = CQChartsUtil::fromQPoint(pos_);

  double px, py;

  plot_->windowToPixel(pp.x, pp.y, px, py);

  plot_->drawSymbol(painter, QPointF(px, py), symbol, s,
                    stroked, pen.color(), lineWidth, filled, brush.color());

  if (edata_ && edata_->label != "") {
    painter->setPen(plot_->interpDataLabelColor(0, 1));

    CQChartsRotatedText::drawRotatedText(painter, px, py, edata_->label, plot_->dataLabelAngle(),
                                         Qt::AlignHCenter | Qt::AlignBottom);
  }

  if (edata_ && edata_->vector) {
    QPointF p1(pp.x, pp.y);

    QPointF p2 = p1 + QPointF(*edata_->vector);

    plot_->drawArrow(painter, p1, p2);
  }
}

//------

CQChartsXYPolylineObj::
CQChartsXYPolylineObj(CQChartsXYPlot *plot, const CQChartsGeom::BBox &rect,
                      const QPolygonF &poly, const QString &name, int i, int n) :
 CQChartsPlotObj(plot, rect), plot_(plot), poly_(poly), name_(name), i_(i), n_(n)
{
}

CQChartsXYPolylineObj::
~CQChartsXYPolylineObj()
{
  delete smooth_;
}

QString
CQChartsXYPolylineObj::
calcId() const
{
  return QString("%1").arg(name_);
}

bool
CQChartsXYPolylineObj::
visible() const
{
  if (! plot_->isLines())
    return false;

  return isVisible();
}

bool
CQChartsXYPolylineObj::
inside(const CQChartsGeom::Point &p) const
{
  if (! visible())
    return false;

  if (! plot_->isLinesSelectable())
    return false;

  double px, py;

  plot_->windowToPixel(p.x, p.y, px, py);

  CQChartsGeom::Point pp(px, py);

  for (int i = 1; i < poly_.count(); ++i) {
    double x1 = poly_[i - 1].x();
    double y1 = poly_[i - 1].y();
    double x2 = poly_[i    ].x();
    double y2 = poly_[i    ].y();

    double px1, py1, px2, py2;

    plot_->windowToPixel(x1, y1, px1, py1);
    plot_->windowToPixel(x2, y2, px2, py2);

    CQChartsGeom::Point pl1(px1, py1);
    CQChartsGeom::Point pl2(px2, py2);

    double d;

    if (CQChartsUtil::PointLineDistance(pp, pl1, pl2, &d) && d < 1)
      return true;
  }

  return false;
}

bool
CQChartsXYPolylineObj::
interpY(double x, std::vector<double> &yvals) const
{
  if (! visible())
    return false;

  for (int i = 1; i < poly_.count(); ++i) {
    double x1 = poly_[i - 1].x();
    double y1 = poly_[i - 1].y();
    double x2 = poly_[i    ].x();
    double y2 = poly_[i    ].y();

    if (x >= x1 && x <= x2) {
      double y = (y2 - y1)*(x - x1)/(x2 - x1) + y1;

      yvals.push_back(y);
    }
  }

  return ! yvals.empty();
}

void
CQChartsXYPolylineObj::
getSelectIndices(Indices &) const
{
  if (! visible())
    return;

  // all objects part of line (dont support select)
}

void
CQChartsXYPolylineObj::
initSmooth()
{
  // init smooth if needed
  if (! smooth_) {
    CQChartsSmooth::Points points;

    int np = poly_.count();

    for (int i = 0; i < np; ++i) {
      const QPointF &p = poly_[i];

      points.emplace_back(p.x(), p.y());
    }

    smooth_ = new CQChartsSmooth(points, /*sorted*/false);
  }
}

void
CQChartsXYPolylineObj::
draw(QPainter *painter, const CQChartsPlot::Layer &)
{
  if (! visible())
    return;

  //---

  QColor c;

  if      (plot_->isBivariate())
    c = plot_->interpThemeColor(1);
  else if (plot_->isStacked()) {
    c = plot_->interpLinesColor(i_, n_);

    c.setAlphaF(plot_->linesAlpha());
  }
  else {
    c = plot_->interpLinesColor(i_, n_);

    c.setAlphaF(plot_->linesAlpha());
  }

  QPen   pen  (c);
  QBrush brush(Qt::NoBrush);

  if (isInside())
    pen.setWidthF(3);
  else {
    double lw = plot_->lengthPixelWidth(plot_->linesWidth());

    pen.setWidthF(lw);
  }

  plot_->updateObjPenBrushState(this, pen, brush);

  if (plot_->isRoundedLines()) {
    initSmooth();

    QPainterPath path;

    for (int i = 0; i < smooth_->numPoints(); ++i) {
      const CQChartsGeom::Point &p = smooth_->point(i);

      double px, py;

      plot_->windowToPixel(p.x, p.y, px, py);

      if (i == 0)
        path.moveTo(px, py);
      else {
        CQChartsSmooth::SegmentType type = smooth_->segmentType(i - 1);

        if      (type == CQChartsSmooth::SegmentType::CURVE3) {
          CQChartsGeom::Point c1 = smooth_->controlPoint1(i - 1);
          CQChartsGeom::Point c2 = smooth_->controlPoint2(i - 1);

          double pcx1, pcy1, pcx2, pcy2;

          plot_->windowToPixel(c1.x, c1.y, pcx1, pcy1);
          plot_->windowToPixel(c2.x, c2.y, pcx2, pcy2);

          path.cubicTo(pcx1, pcy1, pcx2, pcy2, px, py);
        }
        else if (type == CQChartsSmooth::SegmentType::CURVE2) {
          CQChartsGeom::Point c1 = smooth_->controlPoint1(i - 1);

          double pcx1, pcy1;

          plot_->windowToPixel(c1.x, c1.y, pcx1, pcy1);

          path.quadTo(pcx1, pcy1, px, py);
        }
        else if (type == CQChartsSmooth::SegmentType::LINE) {
          path.lineTo(px, py);
        }
      }
    }

    painter->setPen  (pen);
    painter->setBrush(brush);

    painter->drawPath(path);
  }
  else {
    painter->setPen  (pen);
    painter->setBrush(brush);

    int np = poly_.count();

    for (int i = 1; i < np; ++i) {
      double px1, py1, px2, py2;

      plot_->windowToPixel(poly_[i - 1].x(), poly_[i - 1].y(), px1, py1);
      plot_->windowToPixel(poly_[i    ].x(), poly_[i    ].y(), px2, py2);

      painter->drawLine(QPointF(px1, py1), QPointF(px2, py2));
    }
  }
}

//------

CQChartsXYPolygonObj::
CQChartsXYPolygonObj(CQChartsXYPlot *plot, const CQChartsGeom::BBox &rect,
                     const QPolygonF &poly, const QString &name, int i, int n) :
 CQChartsPlotObj(plot, rect), plot_(plot), poly_(poly), name_(name), i_(i), n_(n)
{
}

CQChartsXYPolygonObj::
~CQChartsXYPolygonObj()
{
  delete smooth_;
}

QString
CQChartsXYPolygonObj::
calcId() const
{
  return QString("%1").arg(name_);
}

bool
CQChartsXYPolygonObj::
visible() const
{
  if (! plot_->isFillUnder())
    return false;

  return isVisible();
}

bool
CQChartsXYPolygonObj::
inside(const CQChartsGeom::Point &p) const
{
  if (! visible())
    return false;

  return poly_.containsPoint(CQChartsUtil::toQPoint(p), Qt::OddEvenFill);
}

void
CQChartsXYPolygonObj::
getSelectIndices(Indices &) const
{
  if (! visible())
    return;

  // all objects part of polygon (dont support select)
}

void
CQChartsXYPolygonObj::
initSmooth()
{
  // init smooth if needed
  // (not first point and last point are the extra points to make the square protrusion
  if (! smooth_) {
    CQChartsSmooth::Points points;

    int np = poly_.count();

    for (int i = 1; i < np - 1; ++i) {
      const QPointF &p = poly_[i];

      points.emplace_back(p.x(), p.y());
    }

    smooth_ = new CQChartsSmooth(points, /*sorted*/false);
  }
}

void
CQChartsXYPolygonObj::
draw(QPainter *painter, const CQChartsPlot::Layer &)
{
  if (! visible())
    return;

  //---

  QBrush brush;

  QColor fillColor;

  if (plot_->isStacked())
    fillColor = plot_->interpFillUnderColor(i_, n_);
  else
    fillColor = plot_->interpFillUnderColor(i_, n_);

  fillColor.setAlphaF(plot_->fillUnderAlpha());

  brush.setColor(fillColor);

  brush.setStyle(CQChartsFillPattern::toStyle(
   (CQChartsFillPattern::Type) plot_->fillUnderPattern()));

  QPen pen(Qt::NoPen);

  plot_->updateObjPenBrushState(this, pen, brush);

  int np = poly_.count();

  if (plot_->isRoundedLines()) {
    initSmooth();

    QPainterPath path;

    if (np > 0) {
      double px, py;

      plot_->windowToPixel(poly_[0].x(), poly_[0].y(), px, py);

      path.moveTo(px, py);
    }

    for (int i = 0; i < smooth_->numPoints(); ++i) {
      const CQChartsGeom::Point &p = smooth_->point(i);

      double px, py;

      plot_->windowToPixel(p.x, p.y, px, py);

      if (i == 0)
        path.lineTo(px, py);
      else {
        CQChartsSmooth::SegmentType type = smooth_->segmentType(i - 1);

        if      (type == CQChartsSmooth::SegmentType::CURVE3) {
          CQChartsGeom::Point c1 = smooth_->controlPoint1(i - 1);
          CQChartsGeom::Point c2 = smooth_->controlPoint2(i - 1);

          double pcx1, pcy1, pcx2, pcy2;

          plot_->windowToPixel(c1.x, c1.y, pcx1, pcy1);
          plot_->windowToPixel(c2.x, c2.y, pcx2, pcy2);

          path.cubicTo(pcx1, pcy1, pcx2, pcy2, px, py);
        }
        else if (type == CQChartsSmooth::SegmentType::CURVE2) {
          CQChartsGeom::Point c1 = smooth_->controlPoint1(i - 1);

          double pcx1, pcy1;

          plot_->windowToPixel(c1.x, c1.y, pcx1, pcy1);

          path.quadTo(pcx1, pcy1, px, py);
        }
        else if (type == CQChartsSmooth::SegmentType::LINE) {
          path.lineTo(px, py);
        }
      }
    }

    if (np > 0) {
      double px, py;

      plot_->windowToPixel(poly_[np - 1].x(), poly_[np - 1].y(), px, py);

      path.lineTo(px, py);
    }

    path.closeSubpath();

    painter->setPen  (pen);
    painter->setBrush(brush);

    painter->drawPath(path);
  }
  else {
    QPolygonF poly;

    for (int i = 0; i < np; ++i) {
      double px, py;

      plot_->windowToPixel(poly_[i].x(), poly_[i].y(), px, py);

      poly << QPointF(px, py);
    }

    painter->setPen  (pen);
    painter->setBrush(brush);

    painter->drawPolygon(poly);
  }
}

//------

CQChartsXYKeyColor::
CQChartsXYKeyColor(CQChartsXYPlot *plot, int i, int n) :
 CQChartsKeyColorBox(plot, i, n)
{
}

bool
CQChartsXYKeyColor::
selectPress(const CQChartsGeom::Point &)
{
  CQChartsXYPlot *plot = qobject_cast<CQChartsXYPlot *>(plot_);

  plot->setSetHidden(i_, ! plot->isSetHidden(i_));

  plot->updateObjs();

  return true;
}

QBrush
CQChartsXYKeyColor::
fillBrush() const
{
  CQChartsXYPlot *plot = qobject_cast<CQChartsXYPlot *>(plot_);

  QBrush brush;

  QColor c;

  if      (plot->isBivariate()) {
    c = plot->interpFillUnderColor(i_, n_);

    c.setAlphaF(plot->fillUnderAlpha());

    brush.setStyle(CQChartsFillPattern::toStyle(
     (CQChartsFillPattern::Type) plot->fillUnderPattern()));
  }
  else if (plot_->isOverlay()) {
    if (plot->prevPlot() || plot->nextPlot()) {
      c = plot->interpLinesColor(i_, n_);

      c.setAlphaF(plot->linesAlpha());
    }
    else
      c = CQChartsKeyColorBox::fillBrush().color();
  }
  else
    c = CQChartsKeyColorBox::fillBrush().color();

  if (plot->isSetHidden(i_))
    c = CQChartsUtil::blendColors(c, key_->interpBgColor(), 0.5);

  brush.setColor(c);

  return brush;
}

//------

CQChartsXYKeyLine::
CQChartsXYKeyLine(CQChartsXYPlot *plot, int i, int n) :
 CQChartsKeyItem(plot->key()), plot_(plot), i_(i), n_(n)
{
}

QSizeF
CQChartsXYKeyLine::
size() const
{
  CQChartsXYPlot *plot = qobject_cast<CQChartsXYPlot *>(plot_);

  CQChartsXYPlot *keyPlot = qobject_cast<CQChartsXYPlot *>(key_->plot());

  if (! keyPlot)
    keyPlot = plot;

  QFontMetricsF fm(plot->view()->font());

  double w = fm.width("X-X");
  double h = fm.height();

  double ww = keyPlot->pixelToWindowWidth (w + 8);
  double wh = keyPlot->pixelToWindowHeight(h + 2);

  return QSizeF(ww, wh);
}

bool
CQChartsXYKeyLine::
selectPress(const CQChartsGeom::Point &)
{
  CQChartsXYPlot *plot = qobject_cast<CQChartsXYPlot *>(plot_);

  plot->setSetHidden(i_, ! plot->isSetHidden(i_));

  plot->updateObjs();

  return true;
}

void
CQChartsXYKeyLine::
draw(QPainter *painter, const CQChartsGeom::BBox &rect)
{
  painter->save();

  CQChartsXYPlot *plot = qobject_cast<CQChartsXYPlot *>(plot_);

  CQChartsPlot *keyPlot = qobject_cast<CQChartsPlot *>(key_->plot());

  //CQChartsXYPlot *xyKeyPlot = qobject_cast<CQChartsXYPlot *>(keyPlot);
  //if (! xyKeyPlot) xyKeyPlot = plot;

  CQChartsGeom::BBox prect;

  keyPlot->windowToPixel(rect, prect);

  QRectF prect1(QPointF(prect.getXMin() + 2, prect.getYMin() + 2),
                QPointF(prect.getXMax() - 2, prect.getYMax() - 2));

  painter->setClipRect(prect1);

  QColor pointStrokeColor = plot->interpPointStrokeColor(i_, n_);
  QColor pointFillColor   = plot->interpPointFillColor  (i_, n_);
  QColor lineColor        = plot->interpLinesColor      (i_, n_);
  QColor impulseColor     = plot->interpImpulseColor    (i_, n_);

  if (plot->isSetHidden(i_)) {
    QColor bg = key_->interpBgColor();

    pointStrokeColor = CQChartsUtil::blendColors(pointStrokeColor, bg, 0.5);
    pointFillColor   = CQChartsUtil::blendColors(pointFillColor  , bg, 0.5);
    lineColor        = CQChartsUtil::blendColors(lineColor       , bg, 0.5);
    impulseColor     = CQChartsUtil::blendColors(impulseColor    , bg, 0.5);
  }

  lineColor   .setAlphaF(plot->linesAlpha());
  impulseColor.setAlphaF(plot->impulseAlpha());

  if (plot->isFillUnder()) {
    QColor fillColor = plot->interpFillUnderColor(i_, n_);

    fillColor.setAlphaF(plot->fillUnderAlpha());

    QBrush fillBrush(fillColor);

    double x1 = prect.getXMin() + 4;
    double x2 = prect.getXMax() - 4;
    double y1 = prect.getYMin() + 4;
    double y2 = prect.getYMax() - 4;

    if (isInside())
      fillBrush.setColor(plot->insideColor(fillBrush.color()));

    painter->fillRect(CQChartsUtil::toQRect(CQChartsGeom::BBox(x1, y1, x2, y2)), fillBrush);
  }

  if (plot->isLines() || plot->isImpulse()) {
    double x1 = prect.getXMin() + 4;
    double x2 = prect.getXMax() - 4;
    double y  = prect.getYMid();

    if (! plot->isLines())
      lineColor = impulseColor;

    if (isInside())
      lineColor = plot->insideColor(lineColor);

    painter->setPen(lineColor);

    painter->drawLine(QPointF(x1, y), QPointF(x2, y));
  }

  if (plot->isPoints()) {
    double dx = keyPlot->pixelToWindowWidth(4);

    double x1 = rect.getXMin() + dx;
    double x2 = rect.getXMax() - dx;
    double y  = rect.getYMid();

    double px1, px2, py;

    keyPlot->windowToPixel(x1, y, px1, py);
    keyPlot->windowToPixel(x2, y, px2, py);

    CQChartsPlotSymbol::Type symbol  = plot->symbolType();
    double                   s       = plot->symbolSize();
    bool                     stroked = plot->isSymbolStroked();
    bool                     filled  = plot->isSymbolFilled();

    if (plot->isLines() || plot->isImpulse()) {
      plot_->drawSymbol(painter, QPointF(px1, py), symbol, s,
                        stroked, pointStrokeColor, 1, filled, pointFillColor);
      plot_->drawSymbol(painter, QPointF(px2, py), symbol, s,
                        stroked, pointStrokeColor, 1, filled, pointFillColor);
    }
    else {
      double px = CQChartsUtil::avg(px1, px2);

      plot_->drawSymbol(painter, QPointF(px, py), symbol, s,
                        stroked, pointStrokeColor, 1, filled, pointFillColor);
    }
  }

  painter->restore();
}

//------

CQChartsXYKeyText::
CQChartsXYKeyText(CQChartsXYPlot *plot, int i, const QString &text) :
 CQChartsKeyText(plot, text), i_(i)
{
}

QColor
CQChartsXYKeyText::
interpTextColor(int i, int n) const
{
  CQChartsXYPlot *plot = qobject_cast<CQChartsXYPlot *>(plot_);

  QColor c = CQChartsKeyText::interpTextColor(i, n);

  if (plot->isSetHidden(i_))
    c = CQChartsUtil::blendColors(c, CQChartsUtil::bwColor(c), 0.5);

  return c;
}
