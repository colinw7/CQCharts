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
 CQChartsPlot(view, view->charts()->plotType("parallel"), model),
 CQChartsPlotLineData <CQChartsParallelPlot>(this),
 CQChartsPlotPointData<CQChartsParallelPlot>(this)
{
  setLinesColor(CQChartsColor(CQChartsColor::Type::PALETTE));

  setPoints(true);

  setSymbolStrokeAlpha(0.25);
  setSymbolFilled     (true);
  setSymbolFillColor  (CQChartsColor(CQChartsColor::Type::PALETTE));
  setSymbolFillAlpha  (0.5);

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
setLinesSelectable(bool b)
{
  CQChartsUtil::testAndSet(linesSelectable_, b, [&]() { invalidateLayers(); } );
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
calcRange()
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

    State visit(QAbstractItemModel *, const VisitData &data) override {
      for (int i = 0; i < ns_; ++i) {
        CQChartsGeom::Range &range = setRanges_[i];

        const CQChartsColumn &setColumn = plot_->getSetColumn(i);

        //---

        double x = 0;
        double y = i;

        // TODO: control default value ?
        if (! plot_->rowColValue(data.row, setColumn, data.parent, y, /*defVal*/y))
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

  normalizedDataRange_ = dataRange_;

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

  displayRange_->setWindowRange(normalizedDataRange_.xmin(), normalizedDataRange_.ymin(),
                                normalizedDataRange_.xmax(), normalizedDataRange_.ymax());

  dataRange_ = normalizedDataRange_;
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

    State visit(QAbstractItemModel *, const VisitData &data) override {
      QPolygonF poly;

      QModelIndex xind = plot_->modelIndex(data.row, plot_->xColumn(), data.parent);

      xinds_.push_back(xind);

      //---

      for (int i = 0; i < ns_; ++i) {
        const CQChartsColumn &setColumn = plot_->getSetColumn(i);

        //---

        double x = i;
        double y = i;

        // TODO: control default value ?
        if (! plot_->rowColValue(data.row, setColumn, data.parent, y, /*defVal*/y))
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

    CQChartsGeom::BBox bbox =
      CQChartsGeom::BBox(normalizedDataRange_.xmin(), normalizedDataRange_.ymin(),
                         normalizedDataRange_.xmax(), normalizedDataRange_.ymax());

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
      double pos = 0.0;

      if (! isHorizontal()) {
        double dry = range.ymax() - range.ymin();

        if (dry)
          pos = (p.y() - range.ymin())/dry;
      }
      else {
        double drx = range.xmax() - range.xmin();

        if (drx)
          pos = (p.x() - range.xmin())/drx;
      }

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

  if (CMathUtil::isNaN(value))
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

//---

CQChartsGeom::BBox
CQChartsParallelPlot::
axesFitBBox() const
{
  return axesBBox_;
}

CQChartsGeom::BBox
CQChartsParallelPlot::
annotationBBox() const
{
  QFont font = view()->plotFont(this, view()->font());

  QFontMetricsF fm(font);

  double tm = 4.0;

  double ts;

  if (! isHorizontal())
    ts = pixelToWindowHeight(fm.height() + tm);
  else
    ts = pixelToWindowWidth(max_tw_ + tm);

  CQChartsGeom::BBox bbox;

  if (! isHorizontal())
    bbox = CQChartsGeom::BBox(normalizedDataRange_.xmin(), normalizedDataRange_.ymin(),
                              normalizedDataRange_.xmax(), normalizedDataRange_.ymax() + ts);
  else
    bbox = CQChartsGeom::BBox(normalizedDataRange_.xmin(), normalizedDataRange_.ymin(),
                              normalizedDataRange_.xmax() + ts, normalizedDataRange_.ymax());

  return bbox;
}

//---

bool
CQChartsParallelPlot::
hasFgAxes() const
{
  return true;
}

void
CQChartsParallelPlot::
drawFgAxes(QPainter *painter)
{
  setObjRange();

  //---

  axesBBox_ = CQChartsGeom::BBox();

  max_tw_ = 0.0;

  double tm = 4.0;

  // draw axes
  for (int j = 0; j < numSets(); ++j) {
    CQChartsAxis *axis = axes_[j];

    view()->setPlotPainterFont(this, painter, axis->axesLabelTextFont());

    QFontMetricsF fm(painter->font());

    //---

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
    axis->setPos(j);

    axis->draw(this, painter);

    //---

    // draw set label
    QString label = axis->label();

    double px, py;

    if (! isHorizontal())
      windowToPixel(j, dataRange_.ymax(), px, py);
    else
      windowToPixel(dataRange_.xmax(), j, px, py);

    double tw = fm.width(label);
    double ta = fm.ascent();
    double td = fm.descent();

    max_tw_ = std::max(max_tw_, tw);

    QPen tpen;

    QColor tc = axis->interpAxesTickLabelTextColor(0, 1);

    setPen(tpen, true, tc, axis->axesTickLabelTextAlpha());

    painter->setPen(tpen);

    if (! isHorizontal())
      painter->drawText(QPointF(px - tw/2.0, py - td - tm), label);
    else
      painter->drawText(QPointF(px + tm, py - (ta - td)/2), label);

    //---

    axesBBox_ += windowToPixel(axis->fitBBox());
  }

  //---

  setNormalizedRange();

  axesBBox_ = pixelToWindow(axesBBox_);
}

//---

void
CQChartsParallelPlot::
setObjRange()
{
  // set display range to data range
  if (! isHorizontal())
    displayRange_->setWindowRange(dataRange_.xmin(), 0, dataRange_.xmax(), 1);
  else
    displayRange_->setWindowRange(0, dataRange_.ymin(), 1, dataRange_.ymax());
}

void
CQChartsParallelPlot::
setNormalizedRange()
{
  // set display range to normalized range
  displayRange_->setWindowRange(normalizedDataRange_.xmin(), normalizedDataRange_.ymin(),
                                normalizedDataRange_.xmax(), normalizedDataRange_.ymax());

  dataRange_ = normalizedDataRange_;
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

  // set pen and brush
  QPen   pen;
  QBrush brush;

  plot_->setLineDataPen(pen, i_, n_);

  plot_->setBrush(brush, false);

  plot_->updateObjPenBrushState(this, pen, brush);

  painter->setPen(pen);

  //---

  // draw polyline (using unnormalized polygon)
  QPolygonF poly;

  getPolyLine(poly);

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

  double sx, sy;

  plot_->pixelSymbolSize(plot_->symbolSize(), sx, sy);

  CQChartsGeom::BBox pbbox(px - sx, py - sy, px + sx, py + sy);

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

  //---

  plot_->setObjRange();

  //---

  QPen   pen;
  QBrush brush;

  plot_->setSymbolPenBrush(pen, brush, iset_, nset_);

  plot_->updateObjPenBrushState(this, pen, brush);

  //---

  CQChartsSymbol symbol = plot_->symbolType();

  double sx, sy;

  plot_->pixelSymbolSize(plot_->symbolSize(), sx, sy);

  if (isInside() || isSelected()) {
    sx *= 2;
    sy *= 2;
  }

  QPointF p = plot_->windowToPixel(QPointF(x_, y_));

  plot_->drawSymbol(painter, p, symbol, CMathUtil::avg(sx, sy), pen, brush);

  //---

  plot_->setNormalizedRange();
}
