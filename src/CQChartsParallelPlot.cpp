#include <CQChartsParallelPlot.h>
#include <CQChartsView.h>
#include <CQChartsAxis.h>
#include <CQChartsUtil.h>
#include <CQChartsTip.h>
#include <CQCharts.h>
#include <CQChartsDisplayRange.h>

#include <QPainter>
#include <QMenu>

CQChartsParallelPlotType::
CQChartsParallelPlotType()
{
}

void
CQChartsParallelPlotType::
addParameters()
{
  startParameterGroup("Parallel");

  // columns
  addColumnParameter ("x", "X", "xColumn" , 0  ).setRequired();
  addColumnsParameter("y", "Y", "yColumns", "1").setRequired();

  addBoolParameter("horizontal", "Horizontal", "horizontal").setTip("draw horizontal");

  endParameterGroup();

  //---

  CQChartsPlotType::addParameters();
}

QString
CQChartsParallelPlotType::
description() const
{
  return "<h2>Summary</h2>\n"
         "<p>Draws lines through values of multiple column values for each row.\n";
}

CQChartsPlot *
CQChartsParallelPlotType::
create(CQChartsView *view, const ModelP &model) const
{
  return new CQChartsParallelPlot(view, model);
}

//---

CQChartsParallelPlot::
CQChartsParallelPlot(CQChartsView *view, const ModelP &model) :
 CQChartsPlot(view, view->charts()->plotType("parallel"), model)
{
  lineData_ .color = CQChartsColor(CQChartsColor::Type::PALETTE);

  pointData_.type         = CQChartsSymbol::Type::CIRCLE;
  pointData_.stroke.alpha = 0.25;
  pointData_.fill.visible = true;
  pointData_.fill.color   = CQChartsColor(CQChartsColor::Type::PALETTE);
  pointData_.fill.alpha   = 0.5;

  pointData_.visible = true;

  //addKey(); TODO

  addTitle();
}

CQChartsParallelPlot::
~CQChartsParallelPlot()
{
  for (auto &axis : axes_)
    delete axis;
}

//---

void
CQChartsParallelPlot::
setXColumn(const CQChartsColumn &c)
{
  CQChartsUtil::testAndSet(xColumn_, c, [&]() { updateRangeAndObjs(); } );
}

void
CQChartsParallelPlot::
setYColumn(const CQChartsColumn &c)
{
  yColumns_.setColumn(c);

  updateRangeAndObjs();
}

void
CQChartsParallelPlot::
setYColumns(const Columns &cols)
{
  yColumns_.setColumns(cols);

  updateRangeAndObjs();
}

bool
CQChartsParallelPlot::
setYColumnsStr(const QString &s)
{
  return yColumns_.setColumnsStr(s);
}

//---

void
CQChartsParallelPlot::
setHorizontal(bool b)
{
  CQChartsUtil::testAndSet(horizontal_, b, [&]() { updateRangeAndObjs(); } );
}

//---

void
CQChartsParallelPlot::
setPoints(bool b)
{
  CQChartsUtil::testAndSet(pointData_.visible, b, [&]() { updateObjs(); } );
}

//---

const CQChartsColor &
CQChartsParallelPlot::
symbolStrokeColor() const
{
  return pointData_.stroke.color;
}

void
CQChartsParallelPlot::
setSymbolStrokeColor(const CQChartsColor &c)
{
  CQChartsUtil::testAndSet(pointData_.stroke.color, c, [&]() { invalidateLayers(); } );
}

QColor
CQChartsParallelPlot::
interpSymbolStrokeColor(int i, int n) const
{
  return symbolStrokeColor().interpColor(this, i, n);
}

double
CQChartsParallelPlot::
symbolStrokeAlpha() const
{
  return pointData_.stroke.alpha;
}

void
CQChartsParallelPlot::
setSymbolStrokeAlpha(double a)
{
  CQChartsUtil::testAndSet(pointData_.stroke.alpha, a, [&]() { invalidateLayers(); } );
}

const CQChartsColor &
CQChartsParallelPlot::
symbolFillColor() const
{
  return pointData_.fill.color;
}

void
CQChartsParallelPlot::
setSymbolFillColor(const CQChartsColor &c)
{
  CQChartsUtil::testAndSet(pointData_.fill.color, c, [&]() { invalidateLayers(); } );
}

QColor
CQChartsParallelPlot::
interpSymbolFillColor(int i, int n) const
{
  return symbolFillColor().interpColor(this, i, n);
}

double
CQChartsParallelPlot::
symbolFillAlpha() const
{
  return pointData_.fill.alpha;
}

void
CQChartsParallelPlot::
setSymbolFillAlpha(double a)
{
  CQChartsUtil::testAndSet(pointData_.fill.alpha, a, [&]() { invalidateLayers(); } );
}

CQChartsParallelPlot::Pattern
CQChartsParallelPlot::
symbolFillPattern() const
{
  return (Pattern) pointData_.fill.pattern;
}

void
CQChartsParallelPlot::
setSymbolFillPattern(const Pattern &pattern)
{
  if (pattern != (Pattern) pointData_.fill.pattern) {
    pointData_.fill.pattern = (CQChartsFillData::Pattern) pattern;

    invalidateLayers();
  }
}

//---

void
CQChartsParallelPlot::
setLines(bool b)
{
  CQChartsUtil::testAndSet(lineData_.visible, b, [&]() { updateObjs(); } );
}

void
CQChartsParallelPlot::
setLinesSelectable(bool b)
{
  CQChartsUtil::testAndSet(linesSelectable_, b, [&]() { invalidateLayers(); } );
}

const CQChartsColor &
CQChartsParallelPlot::
linesColor() const
{
  return lineData_.color;
}

void
CQChartsParallelPlot::
setLinesColor(const CQChartsColor &c)
{
  CQChartsUtil::testAndSet(lineData_.color, c, [&]() { invalidateLayers(); } );
}

QColor
CQChartsParallelPlot::
interpLinesColor(int i, int n) const
{
  return linesColor().interpColor(this, i, n);
}

void
CQChartsParallelPlot::
setLinesAlpha(double a)
{
  CQChartsUtil::testAndSet(lineData_.alpha, a, [&]() { invalidateLayers(); } );
}

void
CQChartsParallelPlot::
setLinesWidth(const CQChartsLength &l)
{
  CQChartsUtil::testAndSet(lineData_.width, l, [&]() { invalidateLayers(); } );
}

void
CQChartsParallelPlot::
setLinesDash(const CQChartsLineDash &d)
{
  CQChartsUtil::testAndSet(lineData_.dash, d, [&]() { invalidateLayers(); } );
}

//------

void
CQChartsParallelPlot::
setSymbolType(const CQChartsSymbol &t)
{
  CQChartsUtil::testAndSet(pointData_.type, t, [&]() { invalidateLayers(); } );
}

void
CQChartsParallelPlot::
setSymbolSize(const CQChartsLength &s)
{
  CQChartsUtil::testAndSet(pointData_.size, s, [&]() { invalidateLayers(); } );
}

void
CQChartsParallelPlot::
setSymbolStroked(bool b)
{
  CQChartsUtil::testAndSet(pointData_.stroke.visible, b, [&]() { invalidateLayers(); } );
}

void
CQChartsParallelPlot::
setSymbolStrokeWidth(const CQChartsLength &l)
{
  CQChartsUtil::testAndSet(pointData_.stroke.width, l, [&]() { invalidateLayers(); } );
}

void
CQChartsParallelPlot::
setSymbolFilled(bool b)
{
  CQChartsUtil::testAndSet(pointData_.fill.visible, b, [&]() { invalidateLayers(); } );
}

//------

void
CQChartsParallelPlot::
addProperties()
{
  CQChartsPlot::addProperties();

  // columns
  addProperty("columns", this, "xColumn" , "x"   );
  addProperty("columns", this, "yColumn" , "y"   );
  addProperty("columns", this, "yColumns", "yset");

  addProperty("options", this, "horizontal");

  // points
  addProperty("points", this, "points", "visible");

  addSymbolProperties("points/symbol");

  // lines
  addProperty("lines", this, "lines"          , "visible"   );
  addProperty("lines", this, "linesSelectable", "selectable");

  addLineProperties("lines", "lines");
}

void
CQChartsParallelPlot::
updateRange(bool apply)
{
  // create axes
  AxisDir adir = (! isHorizontal() ? AxisDir::VERTICAL : AxisDir::HORIZONTAL);

  if (axes_.empty() || adir_ != adir) {
    adir_ = adir;

    for (auto &axis : axes_)
      delete axis;

    axes_.clear();

    for (int j = 0; j < numSets(); ++j) {
      CQChartsAxis *axis = new CQChartsAxis(this, adir_, 0, 1);

      axes_.push_back(axis);
    }
  }

  //---

  // calc range for each value column (set)
  class RowVisitor : public ModelVisitor {
   public:
    RowVisitor(CQChartsParallelPlot *plot) :
     plot_(plot) {
      ns_ = plot_->numSets();

      for (int i = 0; i < ns_; ++i)
        setRanges_.emplace_back();
    }

    State visit(QAbstractItemModel *, const QModelIndex &parent, int row) override {
      for (int i = 0; i < ns_; ++i) {
        CQChartsGeom::Range &range = setRanges_[i];

        const CQChartsColumn &setColumn = plot_->getSetColumn(i);

        //---

        double x = 0;
        double y = i;

        // TODO: control default value ?
        if (! plot_->rowColValue(row, setColumn, parent, y, /*defVal*/y))
          continue;

        if (! plot_->isHorizontal())
          range.updateRange(x, y);
        else
          range.updateRange(y, x);
      }

      return State::OK;
    }

    const Ranges &setRanges() const { return setRanges_; }

   private:
    CQChartsParallelPlot *plot_ { nullptr };
    int                   ns_   { 0 };
    Ranges                setRanges_;
  };

  RowVisitor visitor(this);

  visitModel(visitor);

  setRanges_ = visitor.setRanges();

  //---

  // set range from data
  for (int j = 0; j < numSets(); ++j) {
    CQChartsGeom::Range &range = setRanges_[j];

    if (! isHorizontal()) {
      range.updateRange(          - 0.5, range.ymin());
      range.updateRange(numSets() - 0.5, range.ymax());
    }
    else {
      range.updateRange(range.xmin(),           - 0.5);
      range.updateRange(range.xmax(), numSets() - 0.5);
    }
  }

  //---

  // set plot range
  if (! isHorizontal()) {
    dataRange_.updateRange(          - 0.5, 0);
    dataRange_.updateRange(numSets() - 0.5, 1);
  }
  else {
    dataRange_.updateRange(0,           - 0.5);
    dataRange_.updateRange(1, numSets() - 0.5);
  }

  //---

  // set axes range and name
  for (int j = 0; j < numSets(); ++j) {
    const CQChartsGeom::Range &range     = setRange(j);
    const CQChartsColumn      &setColumn = getSetColumn(j);

    bool ok;

    QString name = modelHeaderString(setColumn, ok);

    setDataRange(range);

    if (! isHorizontal()) {
      axes_[j]->setRange(dataRange_.ymin(), dataRange_.ymax());
      axes_[j]->setLabel(name);
    }
    else {
      axes_[j]->setRange(dataRange_.xmin(), dataRange_.xmax());
      axes_[j]->setLabel(name);
    }
  }

  //---

  if (! isHorizontal())
    displayRange_->setWindowRange(-0.5, 0, numSets() - 0.5, 1);
  else
    displayRange_->setWindowRange(0, -0.5, 1, numSets() - 0.5);

  //---

  if (apply)
    applyDataRange();
}

bool
CQChartsParallelPlot::
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

  // create polyline for value from each set
  using Polygons = std::vector<QPolygonF>;
  using Indices  = std::vector<QModelIndex>;

  class RowVisitor : public ModelVisitor {
   public:
    RowVisitor(CQChartsParallelPlot *plot) :
     plot_(plot) {
      ns_ = plot_->numSets();
    }

    State visit(QAbstractItemModel *, const QModelIndex &parent, int row) override {
      QPolygonF poly;

      QModelIndex xind = plot_->modelIndex(row, plot_->xColumn(), parent);

      xinds_.push_back(xind);

      //---

      for (int i = 0; i < ns_; ++i) {
        const CQChartsColumn &setColumn = plot_->getSetColumn(i);

        //---

        double x = i;
        double y = i;

        // TODO: control default value ?
        if (! plot_->rowColValue(row, setColumn, parent, y, /*defVal*/y))
          continue;

        if (! plot_->isHorizontal())
          poly << QPointF(x, y);
        else
          poly << QPointF(y, x);
      }

      polys_.push_back(poly);

      return State::OK;
    }

    const Polygons &polys() const { return polys_; }

    const Indices &xinds() const { return xinds_; }

   private:
    CQChartsParallelPlot *plot_ { nullptr };
    int                   ns_   { 0 };
    Polygons              polys_;
    Indices               xinds_;
  };

  RowVisitor visitor(this);

  visitModel(visitor);

  const Polygons &polys = visitor.polys();
  const Indices  &xinds = visitor.xinds();

  //---

  // TODO: use actual symbol size
  double sw = (dataRange_.xmax() - dataRange_.xmin())/100.0;
  double sh = (dataRange_.ymax() - dataRange_.ymin())/100.0;

  int n = polys.size();

  for (int i = 0; i < n; ++i) {
    const QPolygonF   &poly = polys[i];
    const QModelIndex &xind = xinds[i];

    QModelIndex xind1 = normalizeIndex(xind);

    //---

    // add poly line object
    bool ok;

    QString xname = modelString(xind.row(), xind.column(), xind.parent(), ok);

    CQChartsGeom::BBox bbox;

    if (! isHorizontal())
      bbox = CQChartsGeom::BBox(-0.5, 0, numSets() - 0.5, 1);
    else
      bbox = CQChartsGeom::BBox(0, -0.5, 1, numSets() - 0.5);

    CQChartsParallelLineObj *lineObj =
      new CQChartsParallelLineObj(this, bbox, poly, xind1, i, n);

    addPlotObject(lineObj);

    //---

    // create point object for each poly point
    int nl = poly.count();

    for (int j = 0; j < nl; ++j) {
      const CQChartsColumn &setColumn = getSetColumn(j);

      QModelIndex yind  = modelIndex(i, setColumn, xind.parent());
      QModelIndex yind1 = normalizeIndex(yind);

      //---

      const CQChartsGeom::Range &range = setRange(j);

      const QPointF &p = poly[j];

      // scale point to range
      double pos;

      if (! isHorizontal())
        pos = (p.y() - range.ymin())/(range.ymax() - range.ymin());
      else
        pos = (p.x() - range.xmin())/(range.xmax() - range.xmin());

      double x, y;

      if (! isHorizontal()) {
        x = j;
        y = pos;
      }
      else {
        x = pos;
        y = j;
      }

      CQChartsGeom::BBox bbox(x - sw/2, y - sh/2, x + sw/2, y + sh/2);

      CQChartsParallelPointObj *pointObj =
        new CQChartsParallelPointObj(this, bbox, x, y, yind1, i, n, j, nl);

      bool ok;

      QString yname = modelHeaderString(setColumn, ok);

      QString id = QString("%1:%2=%3").arg(xname).arg(yname).arg(p.y());

      pointObj->setId(id);

      addPlotObject(pointObj);
    }
  }

  //---

  return true;
}

bool
CQChartsParallelPlot::
rowColValue(int row, const CQChartsColumn &column, const QModelIndex &parent,
            double &value, double defVal)
{
  bool ok;

  value = modelReal(row, column, parent, ok);

  if (! ok)
    value = defVal;

  if (CQChartsUtil::isNaN(value))
    return false;

  return true;
}

int
CQChartsParallelPlot::
numSets() const
{
  return yColumns_.count();
}

const CQChartsColumn &
CQChartsParallelPlot::
getSetColumn(int i) const
{
  return yColumns_.getColumn(i);
}

bool
CQChartsParallelPlot::
probe(ProbeData &probeData) const
{
  int n = numSets();

  if (! isHorizontal()) {
    int x = std::round(probeData.x);

    x = std::max(x, 0    );
    x = std::min(x, n - 1);

    const CQChartsGeom::Range &range = setRanges_[x];

    probeData.x = x;

    probeData.yvals.emplace_back(probeData.y,
      QString("%1").arg(probeData.y*range.ysize() + range.ymin()));
  }
  else {
    int y = std::round(probeData.y);

    y = std::max(y, 0    );
    y = std::min(y, n - 1);

    const CQChartsGeom::Range &range = setRanges_[y];

    probeData.y = y;

    probeData.xvals.emplace_back(probeData.x,
      QString("%1").arg(probeData.x*range.xsize() + range.xmin()));
  }

  return true;
}

//------

bool
CQChartsParallelPlot::
addMenuItems(QMenu *menu)
{
  QAction *horizontalAction = new QAction("Horizontal", menu);

  horizontalAction->setCheckable(true);
  horizontalAction->setChecked(isHorizontal());

  connect(horizontalAction, SIGNAL(triggered(bool)), this, SLOT(setHorizontal(bool)));

  menu->addSeparator();

  menu->addAction(horizontalAction);

  return true;
}

void
CQChartsParallelPlot::
drawParts(QPainter *painter)
{
  // set display range to data range
  if (! isHorizontal())
    displayRange_->setWindowRange(dataRange_.xmin(), 0, dataRange_.xmax(), 1);
  else
    displayRange_->setWindowRange(0, dataRange_.ymin(), 1, dataRange_.ymax());

  drawObjs(painter, CQChartsLayer::Type::MID_PLOT);

  //---

  QFontMetricsF fm(view()->font());

  for (int j = 0; j < numSets(); ++j) {
    const CQChartsGeom::Range &range = setRange(j);

    dataRange_ = range;
  //setDataRange(range); // will clear objects

    // set display range to set range
    if (! isHorizontal())
      displayRange_->setWindowRange(-0.5, dataRange_.ymin(), numSets() - 0.5, dataRange_.ymax());
    else
      displayRange_->setWindowRange(dataRange_.xmin(), -0.5, dataRange_.xmax(), numSets() - 0.5);

    //---

    // draw set axis
    axes_[j]->setPos(j);

    axes_[j]->draw(this, painter);

    //---

    // draw set label
    QString label = axes_[j]->label();

    double px, py;

    if (! isHorizontal())
      windowToPixel(j, dataRange_.ymax(), px, py);
    else
      windowToPixel(dataRange_.xmax(), j, px, py);

    painter->setPen(axes_[j]->interpTickLabelColor(0, 1));

    painter->drawText(QPointF(px - fm.width(label)/2.0, py - fm.height()), label);
  }

  // reset display range to normalized range
  if (! isHorizontal())
    displayRange_->setWindowRange(-0.5, 0, numSets() - 0.5, 1);
  else
    displayRange_->setWindowRange(0, -0.5, 1, numSets() - 0.5);

  //---

  drawTitle(painter);
}

//------

CQChartsParallelLineObj::
CQChartsParallelLineObj(CQChartsParallelPlot *plot, const CQChartsGeom::BBox &rect,
                        const QPolygonF &poly, const QModelIndex &ind, int i, int n) :
 CQChartsPlotObj(plot, rect), plot_(plot), poly_(poly), ind_(ind), i_(i), n_(n)
{
}

QString
CQChartsParallelLineObj::
calcId() const
{
  bool ok;

  QString xname = plot_->modelString(ind_.row(), plot_->xColumn(), ind_.parent(), ok);

  return xname;
}

QString
CQChartsParallelLineObj::
calcTipId() const
{
  bool ok;

  QString xname = plot_->modelString(ind_.row(), plot_->xColumn(), ind_.parent(), ok);

  CQChartsTableTip tableTip;

  tableTip.addBoldLine(xname);

  int nl = poly_.count();

  for (int j = 0; j < nl; ++j) {
    const CQChartsColumn &yColumn = plot_->getSetColumn(j);

    bool ok;

    QString yname = plot_->modelHeaderString(yColumn, ok);

    tableTip.addTableRow(yname, poly_[j].y());
  }

  return tableTip.str();
}

bool
CQChartsParallelLineObj::
visible() const
{
  if (! plot_->isLines())
    return false;

  return isVisible();
}

bool
CQChartsParallelLineObj::
inside(const CQChartsGeom::Point &p) const
{
  if (! visible())
    return false;

  if (! plot_->isLinesSelectable())
    return false;

  // TODO: check as lines created, only need to create lines close to point

  // create unnormalized polygon
  QPolygonF poly;

  getPolyLine(poly);

  // check if close enough to each line to be inside
  for (int i = 1; i < poly.count(); ++i) {
    double x1 = poly[i - 1].x();
    double y1 = poly[i - 1].y();
    double x2 = poly[i    ].x();
    double y2 = poly[i    ].y();

    double d;

    CQChartsGeom::Point pl1(x1, y1);
    CQChartsGeom::Point pl2(x2, y2);

    if (CQChartsUtil::PointLineDistance(p, pl1, pl2, &d) && d < 1E-3)
      return true;
  }

  return false;
}

// TODO : interpY

void
CQChartsParallelLineObj::
getSelectIndices(Indices &inds) const
{
  addColumnSelectIndex(inds, plot_->xColumn());
  addColumnSelectIndex(inds, plot_->yColumn());
}

void
CQChartsParallelLineObj::
addColumnSelectIndex(Indices &inds, const CQChartsColumn &column) const
{
  if (column.isValid())
    addSelectIndex(inds, ind_.row(), column, ind_.parent());
}

void
CQChartsParallelLineObj::
draw(QPainter *painter)
{
  if (! visible())
    return;

  //---

  // create unnormalized polygon
  QPolygonF poly;

  getPolyLine(poly);

  //---

  QColor lc = plot_->interpLinesColor(i_, n_);

  lc.setAlphaF(plot_->linesAlpha());

  QPen   pen(lc);
  QBrush brush(Qt::NoBrush);

  double lw = plot_->lengthPixelWidth(plot_->linesWidth());

  if (lw > 0)
    pen.setWidthF(lw);

  plot_->updateObjPenBrushState(this, pen, brush);

#if 0
  if (isInside()) {
    if (lw <= 0)
      lw = 1;

    lw *= 3;
  }
#endif

  // draw polyline
  painter->setPen(pen);

  for (int i = 1; i < poly.count(); ++i)
    painter->drawLine(plot_->windowToPixel(poly[i - 1]), plot_->windowToPixel(poly[i]));
}

void
CQChartsParallelLineObj::
getPolyLine(QPolygonF &poly) const
{
  // create unnormalized polylne
  for (int i = 0; i < poly_.count(); ++i) {
    const CQChartsGeom::Range &range = plot_->setRange(i);

    double x, y;

    if (! plot_->isHorizontal()) {
      x = poly_[i].x();
      y = (poly_[i].y() - range.ymin())/range.ysize();
    }
    else {
      x = (poly_[i].x() - range.xmin())/range.xsize();
      y = poly_[i].y();
    }

    poly << QPointF(x, y);
  }
}

//------

CQChartsParallelPointObj::
CQChartsParallelPointObj(CQChartsParallelPlot *plot, const CQChartsGeom::BBox &rect,
                         double x, double y, const QModelIndex &ind, int iset, int nset,
                         int i, int n) :
 CQChartsPlotObj(plot, rect), plot_(plot), x_(x), y_(y), ind_(ind), iset_(iset), nset_(nset),
 i_(i), n_(n)
{
}

QString
CQChartsParallelPointObj::
calcId() const
{
  bool ok;

  QString xname = plot_->modelString(ind_.row(), plot_->xColumn(), ind_.parent(), ok);

  const CQChartsColumn &yColumn = plot_->getSetColumn(i_);

  QString yname = plot_->modelHeaderString(yColumn, ok);

  return QString("point:%1:%2=%3").arg(xname).arg(yname).arg(y_);
}

QString
CQChartsParallelPointObj::
calcTipId() const
{
  CQChartsTableTip tableTip;

  bool ok;

  QString xname = plot_->modelString(ind_.row(), plot_->xColumn(), ind_.parent(), ok);

  tableTip.addBoldLine(xname);

  const CQChartsColumn &yColumn = plot_->getSetColumn(i_);

  QString yname = plot_->modelHeaderString(yColumn, ok);

  tableTip.addTableRow(yname, y_);

  return tableTip.str();
}

bool
CQChartsParallelPointObj::
visible() const
{
  if (! plot_->isPoints())
    return false;

  return isVisible();
}

bool
CQChartsParallelPointObj::
inside(const CQChartsGeom::Point &p) const
{
  if (! visible())
    return false;

  double px, py;

  plot_->windowToPixel(x_, y_, px, py);

  double s = plot_->lengthPixelWidth(plot_->symbolSize());

  CQChartsGeom::BBox pbbox(px - s, py - s, px + s, py + s);

  CQChartsGeom::Point pp;

  plot_->windowToPixel(p, pp);

  return pbbox.inside(pp);
}

void
CQChartsParallelPointObj::
getSelectIndices(Indices &inds) const
{
  addColumnSelectIndex(inds, ind_.column());
}

void
CQChartsParallelPointObj::
addColumnSelectIndex(Indices &inds, const CQChartsColumn &column) const
{
  if (column.isValid())
    addSelectIndex(inds, ind_.row(), column, ind_.parent());
}

void
CQChartsParallelPointObj::
draw(QPainter *painter)
{
  if (! visible())
    return;

  CQChartsSymbol symbol      = plot_->symbolType();
  bool           stroked     = plot_->isSymbolStroked();
  QColor         strokeColor = plot_->interpSymbolStrokeColor(iset_, nset_);
  double         lineWidth   = plot_->lengthPixelWidth(plot_->symbolStrokeWidth());
  bool           filled      = plot_->isSymbolFilled();
  QColor         fillColor   = plot_->interpSymbolFillColor(iset_, nset_);

  strokeColor.setAlphaF(plot_->symbolStrokeAlpha());
  fillColor  .setAlphaF(plot_->symbolFillAlpha());

  double s = plot_->lengthPixelWidth(plot_->symbolSize());

  QBrush brush(fillColor);
  QPen   pen  (strokeColor);

  plot_->updateObjPenBrushState(this, pen, brush);

  //---

  CQChartsGeom::Point pp(x_, y_);

  double px, py;

  plot_->windowToPixel(pp.x, pp.y, px, py);

  if (isInside() || isSelected())
    s *= 2;

  plot_->drawSymbol(painter, QPointF(px, py), symbol, s,
                    stroked, pen.color(), lineWidth, filled, brush.color());
}
