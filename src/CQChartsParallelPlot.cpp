#include <CQChartsParallelPlot.h>
#include <CQChartsView.h>
#include <CQChartsAxis.h>
#include <CQChartsUtil.h>
#include <CQChartsTip.h>
#include <CQCharts.h>
#include <CQChartsDisplayRange.h>
#include <CQChartsPaintDevice.h>
#include <CQChartsDrawUtil.h>
#include <CQChartsPaintDevice.h>
#include <CQChartsHtml.h>

#include <CQPropertyViewItem.h>
#include <CQPerfMonitor.h>
#include <CMathRound.h>

#include <QApplication>
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
  addColumnParameter("x", "X", "xColumn").
    setNumeric().setRequired().setTip("X value column");

  addColumnsParameter("y", "Y", "yColumns").
    setNumeric().setRequired().setTip("Y value columns");

  addBoolParameter("horizontal", "Horizontal", "horizontal").
    setTip("Draw horizontal");

  endParameterGroup();

  //---

  CQChartsPlotType::addParameters();
}

QString
CQChartsParallelPlotType::
description() const
{
  auto IMG = [](const QString &src) { return CQChartsHtml::Str::img(src); };

  return CQChartsHtml().
   h2("Parallel Plot").
    h3("Summary").
     p("Draws lines through values of multiple column values for each row.").
    h3("Limitations").
     p("None.").
    h3("Example").
     p(IMG("images/parallelplot.png"));
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
 CQChartsObjLineData <CQChartsParallelPlot>(this),
 CQChartsObjPointData<CQChartsParallelPlot>(this)
{
  NoUpdate noUpdate(this);

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
setYColumns(const CQChartsColumns &c)
{
  CQChartsUtil::testAndSet(yColumns_, c, [&]() { updateRangeAndObjs(); } );
}

//---

void
CQChartsParallelPlot::
setHorizontal(bool b)
{
  CQChartsUtil::testAndSet(horizontal_, b, [&]() {
    //CQChartsAxis::swap(xAxis(), yAxis());

    updateRangeAndObjs();
  } );
}

//---

void
CQChartsParallelPlot::
setLinesSelectable(bool b)
{
  CQChartsUtil::testAndSet(linesSelectable_, b, [&]() { drawObjs(); } );
}

//------

void
CQChartsParallelPlot::
addProperties()
{
  auto addProp = [&](const QString &path, const QString &name, const QString &alias,
                     const QString &desc) {
    return &(this->addProperty(path, this, name, alias)->setDesc(desc));
  };

  //---

  CQChartsPlot::addProperties();

  // columns
  addProp("columns", "xColumn" , "x", "X column");
  addProp("columns", "yColumns", "y", "Y columns");

  // options
  addProp("options", "horizontal", "", "Draw horizontally");

  // points
  addProp("points", "points", "visible", "Points visible");

  addSymbolProperties("points/symbol", "", "Points");

  // lines
  addProp("lines", "lines"          , "visible"   , "Lines visible");
  addProp("lines", "linesSelectable", "selectable", "Lines selectable");

  addLineProperties("lines/stroke", "lines", "");
}

CQChartsGeom::Range
CQChartsParallelPlot::
calcRange() const
{
  CQPerfTrace trace("CQChartsParallelPlot::calcRange");

  CQChartsParallelPlot *th =  const_cast<CQChartsParallelPlot *>(this);

  // create axes
  Qt::Orientation adir = (! isHorizontal() ? Qt::Vertical : Qt::Horizontal);

  if (axes_.empty() || adir_ != adir) {
    std::unique_lock<std::mutex> lock(axesMutex_);

    if (axes_.empty() || adir_ != adir) {
      th->adir_ = adir;

      for (auto &axis : th->axes_)
        delete axis;

      th->axes_.clear();

      int ns = yColumns().count();

      for (int j = 0; j < ns; ++j) {
        CQChartsAxis *axis = new CQChartsAxis(this, adir_, 0, 1);

        axis->moveToThread(th->thread());

        axis->setParent(th);
        axis->setPlot  (this);

        th->axes_.push_back(axis);
      }
    }
  }

  //---

  // calc range for each value column (set)
  class RowVisitor : public ModelVisitor {
   public:
    RowVisitor(const CQChartsParallelPlot *plot) :
     plot_(plot) {
      ns_ = plot_->yColumns().count();

      for (int i = 0; i < ns_; ++i)
        setRanges_.emplace_back();
    }

    State visit(const QAbstractItemModel *, const VisitData &data) override {
      for (int i = 0; i < ns_; ++i) {
        CQChartsGeom::Range &range = setRanges_[i];

        const CQChartsColumn &setColumn = plot_->yColumns().getColumn(i);

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
    const CQChartsParallelPlot *plot_ { nullptr };
    int                         ns_   { 0 };
    Ranges                      setRanges_;
  };

  RowVisitor visitor(this);

  visitModel(visitor);

  th->setRanges_ = visitor.setRanges();

  //---

  // set range from data
  int ns = yColumns().count();

  for (int j = 0; j < ns; ++j) {
    CQChartsGeom::Range &range = th->setRanges_[j];

    if (! range.isSet())
      continue;

    if (! isHorizontal()) {
      range.updateRange(   - 0.5, range.ymin());
      range.updateRange(ns - 0.5, range.ymax());
    }
    else {
      range.updateRange(range.xmin(),    - 0.5);
      range.updateRange(range.xmax(), ns - 0.5);
    }
  }

  //---

  CQChartsGeom::Range dataRange;

  auto updateRange = [&](double x, double y) {
    if (! isHorizontal())
      dataRange.updateRange(x, y);
    else
      dataRange.updateRange(y, x);
  };

  //---

  // set plot range
  updateRange(   - 0.5, 0);
  updateRange(ns - 0.5, 1);

  th->normalizedDataRange_ = dataRange;

  //---

  // set axes range and name
  for (int j = 0; j < ns; ++j) {
    CQChartsAxis *axis = axes_[j];

    const CQChartsGeom::Range &range     = setRange(j);
    const CQChartsColumn      &setColumn = yColumns().getColumn(j);

    bool ok;

    QString name = modelHeaderString(setColumn, ok);

    const_cast<CQChartsParallelPlot *>(this)->setDataRange(range);

    if (range.isSet()) {
      if (! isHorizontal()) {
        axis->setRange(range.ymin(), range.ymax());
        axis->setLabel(name);
      }
      else {
        axis->setRange(range.xmin(), range.xmax());
        axis->setLabel(name);
      }
    }
  }

  //---

  displayRange_->setWindowRange(normalizedDataRange_.xmin(), normalizedDataRange_.ymin(),
                                normalizedDataRange_.xmax(), normalizedDataRange_.ymax());

  dataRange = normalizedDataRange_;

  //---

  return dataRange;
}

bool
CQChartsParallelPlot::
createObjs(PlotObjs &objs) const
{
  CQPerfTrace trace("CQChartsParallelPlot::createObjs");

  NoUpdate noUpdate(this);

  //---

  // create polyline for value from each set
  using Polygons = std::vector<QPolygonF>;
  using Indices  = std::vector<QModelIndex>;

  class RowVisitor : public ModelVisitor {
   public:
    RowVisitor(const CQChartsParallelPlot *plot) :
     plot_(plot) {
      ns_ = plot_->yColumns().count();
    }

    State visit(const QAbstractItemModel *, const VisitData &data) override {
      QPolygonF poly;

      QModelIndex xind = plot_->modelIndex(data.row, plot_->xColumn(), data.parent);

      xinds_.push_back(xind);

      //---

      for (int i = 0; i < ns_; ++i) {
        const CQChartsColumn &setColumn = plot_->yColumns().getColumn(i);

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
    const CQChartsParallelPlot *plot_ { nullptr };
    int                         ns_   { 0 };
    Polygons                    polys_;
    Indices                     xinds_;
  };

  RowVisitor visitor(this);

  visitModel(visitor);

  const Polygons &polys = visitor.polys();
  const Indices  &xinds = visitor.xinds();

  //---

  // TODO: use actual symbol size
  const CQChartsGeom::Range &dataRange = this->dataRange();

  double sw = 0.01, sh = 0.01;

  if (dataRange.isSet()) {
    sw = (dataRange.xmax() - dataRange.xmin())/100.0;
    sh = (dataRange.ymax() - dataRange.ymin())/100.0;
  }

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

    ColorInd is(i, n);

    CQChartsParallelLineObj *lineObj =
      new CQChartsParallelLineObj(this, bbox, poly, xind1, is);

    objs.push_back(lineObj);

    //---

    // create point object for each poly point
    int nl = poly.count();

    for (int j = 0; j < nl; ++j) {
      const CQChartsColumn &setColumn = yColumns().getColumn(j);

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

      ColorInd is(i, n);
      ColorInd iv(j, nl);

      CQChartsParallelPointObj *pointObj =
        new CQChartsParallelPointObj(this, bbox, p.y(), x, y, yind1, is, iv);

      //bool ok;

      //QString yname = modelHeaderString(setColumn, ok);

      //QString id = QString("%1:%2=%3").arg(xname).arg(yname).arg(p.y());

      //pointObj->setId(id);

      objs.push_back(pointObj);
    }
  }

  //---

  return true;
}

bool
CQChartsParallelPlot::
rowColValue(int row, const CQChartsColumn &column, const QModelIndex &parent,
            double &value, double defVal) const
{
  bool ok;

  value = modelReal(row, column, parent, ok);

  if (! ok)
    value = defVal;

  if (CMathUtil::isNaN(value))
    return false;

  return true;
}

bool
CQChartsParallelPlot::
probe(ProbeData &probeData) const
{
  int n = yColumns().count();

  if (! isHorizontal()) {
#if 0
    int x1 = std::min(std::max(CMathRound::RoundDown(probeData.p.x), 0), n - 1);
    int x2 = std::min(std::max(CMathRound::RoundUp  (probeData.p.x), 0), n - 1);

    const CQChartsGeom::Range &range1 = setRanges_[x1];
    const CQChartsGeom::Range &range2 = setRanges_[x2];

    for (const auto &plotObj : plotObjs_) {
      CQChartsParallelLineObj *obj = dynamic_cast<CQChartsParallelLineObj *>(plotObj);
      if (! obj) continue;

      std::vector<double> yvals;

      // interpolate to normalized (0-1) y
      if (! obj->interpY(probeData.p.x, yvals))
        continue;

      for (const auto &y : yvals) {
        double y1 = y*range1.ysize() + range1.ymin();
        double y2 = y*range2.ysize() + range2.ymin();

        double dx1 = probeData.p.x - x1;
        double dx2 = x2 - probeData.p.x;

        double yi;

        if (x1 != x2)
          yi = y1*(1 - dx1) + y2*(1 - dx2);
        else
          yi = y1;

        probeData.yvals.emplace_back(y, QString("%1").arg(yi));
      }
    }
#else
    int x = CMathRound::RoundNearest(probeData.p.x);

    x = std::min(std::max(x, 0), n - 1);

    const CQChartsGeom::Range &range = setRanges_[x];

    probeData.p.x = x;

    probeData.yvals.emplace_back(probeData.p.y,
      QString("%1").arg(probeData.p.y*range.ysize() + range.ymin()));
#endif
  }
  else {
    int y = CMathRound::RoundNearest(probeData.p.y);

    y = std::max(y, 0    );
    y = std::min(y, n - 1);

    const CQChartsGeom::Range &range = setRanges_[y];

    probeData.p.y = y;

    probeData.xvals.emplace_back(probeData.p.x,
      QString("%1").arg(probeData.p.x*range.xsize() + range.xmin()));
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
drawFgAxes(CQChartsPaintDevice *device) const
{
  CQChartsParallelPlot *th = const_cast<CQChartsParallelPlot *>(this);

  //th->setObjRange(device);

  //---

  th->axesBBox_ = CQChartsGeom::BBox();

  th->max_tw_ = 0.0;

  double tm = 4.0;

  // draw axes
  int ns = yColumns().count();

  for (int j = 0; j < ns; ++j) {
    CQChartsAxis *axis = axes_[j];

    view()->setPlotPainterFont(this, device, axis->axesLabelTextFont());

    QFontMetricsF fm(device->font());

    //---

    const CQChartsGeom::Range &range = setRange(j);

    const_cast<CQChartsParallelPlot *>(this)->dataRange_ = range;
  //setDataRange(range); // will clear objects

    // set display range to set range
    if (dataRange_.isSet()) {
      if (! isHorizontal())
        displayRange_->setWindowRange(-0.5, dataRange_.ymin(), ns - 0.5, dataRange_.ymax());
      else
        displayRange_->setWindowRange(dataRange_.xmin(), -0.5, dataRange_.xmax(), ns - 0.5);

      //---

      if (device->type() == CQChartsPaintDevice::Type::SCRIPT) {
        CQChartsScriptPainter *painter = dynamic_cast<CQChartsScriptPainter *>(device);

        writeScriptRange(painter->os());
      }
    }

    //---

    // draw set axis
    axis->setPosition(CQChartsOptReal(j));

    axis->draw(this, device);

    //---

    // draw set label
    QString label = axis->label();

    CQChartsGeom::Point p;

    if (dataRange_.isSet()) {
      if (! isHorizontal())
        p = windowToPixel(CQChartsGeom::Point(j, dataRange_.ymax()));
      else
        p = windowToPixel(CQChartsGeom::Point(dataRange_.xmax(), j));
    }

    double tw = fm.width(label);
    double ta = fm.ascent();
    double td = fm.descent();

    th->max_tw_ = std::max(max_tw_, tw);

    QPen tpen;

    QColor tc = axis->interpAxesTickLabelTextColor(ColorInd());

    setPen(tpen, true, tc, axis->axesTickLabelTextAlpha());

    device->setPen(tpen);

    QPointF tp;

    if (! isHorizontal())
      tp = QPointF(p.x - tw/2.0, p.y - td - tm);
    else
      tp = QPointF(p.x + tm, p.y - (ta - td)/2);

    CQChartsDrawUtil::drawSimpleText(device, device->pixelToWindow(tp), label);

    //---

    th->axesBBox_ += windowToPixel(axis->fitBBox());
  }

  //---

  th->setNormalizedRange(device);

  th->axesBBox_ = pixelToWindow(axesBBox_);
}

void
CQChartsParallelPlot::
postDraw()
{
  //CQChartsParallelPlot *th = const_cast<CQChartsParallelPlot *>(this);

  //th->setNormalizedRange(device);

  rangeType_ = RangeType::NONE;
}

//---

void
CQChartsParallelPlot::
setObjRange(CQChartsPaintDevice *device)
{
  if (rangeType_ == RangeType::OBJ)
    return;

  rangeType_ = RangeType::OBJ;

  //---

  // set display range to data range
  const CQChartsGeom::Range &dataRange = this->dataRange();

  if (dataRange.isSet()) {
    if (! isHorizontal())
      displayRange_->setWindowRange(dataRange.xmin(), 0, dataRange.xmax(), 1);
    else
      displayRange_->setWindowRange(0, dataRange.ymin(), 1, dataRange.ymax());
  }

  //---

  if (device->type() == CQChartsPaintDevice::Type::SCRIPT) {
    CQChartsScriptPainter *painter = dynamic_cast<CQChartsScriptPainter *>(device);

    writeScriptRange(painter->os());
  }
}

void
CQChartsParallelPlot::
setNormalizedRange(CQChartsPaintDevice *device)
{
  if (rangeType_ == RangeType::NORMALIZED)
    return;

  rangeType_ = RangeType::NORMALIZED;

  //---

  // set display range to normalized range
  displayRange_->setWindowRange(normalizedDataRange_.xmin(), normalizedDataRange_.ymin(),
                                normalizedDataRange_.xmax(), normalizedDataRange_.ymax());

  dataRange_ = normalizedDataRange_;

  //---

  if (device->type() == CQChartsPaintDevice::Type::SCRIPT) {
    CQChartsScriptPainter *painter = dynamic_cast<CQChartsScriptPainter *>(device);

    writeScriptRange(painter->os());
  }
}

//------

CQChartsParallelLineObj::
CQChartsParallelLineObj(const CQChartsParallelPlot *plot, const CQChartsGeom::BBox &rect,
                        const QPolygonF &poly, const QModelIndex &ind, const ColorInd &is) :
 CQChartsPlotObj(const_cast<CQChartsParallelPlot *>(plot), rect, is, ColorInd(), ColorInd()),
 plot_(plot), poly_(poly)
{
  setModelInd(ind);
}

QString
CQChartsParallelLineObj::
calcId() const
{
  bool ok;

  QString xname = plot_->modelString(modelInd().row(), plot_->xColumn(), modelInd().parent(), ok);

  return QString("%1:%2").arg(typeName()).arg(xname);
}

QString
CQChartsParallelLineObj::
calcTipId() const
{
  bool ok;

  QString xname = plot_->modelString(modelInd().row(), plot_->xColumn(), modelInd().parent(), ok);

  CQChartsTableTip tableTip;

  tableTip.addBoldLine(xname);

  int nl = poly_.count();

  for (int j = 0; j < nl; ++j) {
    const CQChartsColumn &yColumn = plot_->yColumns().getColumn(j);

    bool ok;

    QString yname = plot_->modelHeaderString(yColumn, ok);

    tableTip.addTableRow(yname, poly_[j].y());
  }

  //---

  plot()->addTipColumns(tableTip, modelInd());

  //---

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

#if 0
bool
CQChartsParallelLineObj::
interpY(double x, std::vector<double> &yvals) const
{
  if (! visible())
    return false;

  QPolygonF poly;

  getPolyLine(poly);

  for (int i = 1; i < poly.count(); ++i) {
    double x1 = poly[i - 1].x();
    double y1 = poly[i - 1].y();
    double x2 = poly[i    ].x();
    double y2 = poly[i    ].y();

    if (x >= x1 && x <= x2) {
      double y = (y2 - y1)*(x - x1)/(x2 - x1) + y1;

      yvals.push_back(y);
    }
  }

  return ! yvals.empty();
}
#endif

void
CQChartsParallelLineObj::
getSelectIndices(Indices &inds) const
{
  addColumnSelectIndex(inds, plot_->xColumn());

  for (const auto &c : plot_->yColumns())
    addColumnSelectIndex(inds, c);
}

void
CQChartsParallelLineObj::
draw(CQChartsPaintDevice *device)
{
  if (! visible())
    return;

  //---

  // set pen and brush
  ColorInd colorInd = calcColorInd();

  QPen   pen;
  QBrush brush;

  plot_->setLineDataPen(pen, colorInd);

  plot_->setBrush(brush, false);

  plot_->updateObjPenBrushState(this, pen, brush);

  device->setPen(pen);

  //---

  // draw polyline (using unnormalized polygon)
  QPolygonF poly;

  getPolyLine(poly);

  for (int i = 1; i < poly.count(); ++i)
    device->drawLine(poly[i - 1], poly[i]);
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
CQChartsParallelPointObj(const CQChartsParallelPlot *plot, const CQChartsGeom::BBox &rect,
                         double yval, double x, double y, const QModelIndex &ind,
                         const ColorInd &is, const ColorInd &iv) :
 CQChartsPlotObj(const_cast<CQChartsParallelPlot *>(plot), rect, is, ColorInd(), iv),
 plot_(plot), yval_(yval), x_(x), y_(y)
{
  setModelInd(ind);
}

QString
CQChartsParallelPointObj::
calcId() const
{
  bool ok;

  QString xname = plot_->modelString(modelInd().row(), plot_->xColumn(), modelInd().parent(), ok);

  const CQChartsColumn &yColumn = plot_->yColumns().getColumn(iv_.i);

  QString yname = plot_->modelHeaderString(yColumn, ok);

  return QString("%1:%2:%3=%4").arg(typeName()).arg(xname).arg(yname).arg(yval_);
}

QString
CQChartsParallelPointObj::
calcTipId() const
{
  CQChartsTableTip tableTip;

  bool ok;

  QString xname = plot_->modelString(modelInd().row(), plot_->xColumn(), modelInd().parent(), ok);

  tableTip.addBoldLine(xname);

  const CQChartsColumn &yColumn = plot_->yColumns().getColumn(iv_.i);

  QString yname = plot_->modelHeaderString(yColumn, ok);

  tableTip.addTableRow(yname, yval_);

  //---

  plot()->addTipColumns(tableTip, modelInd());

  //---

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

  CQChartsGeom::Point p1 = plot_->windowToPixel(CQChartsGeom::Point(x_, y_));

  double sx, sy;

  plot_->pixelSymbolSize(plot_->symbolSize(), sx, sy);

  CQChartsGeom::BBox pbbox(p1.x - sx, p1.y - sy, p1.x + sx, p1.y + sy);

  CQChartsGeom::Point pp = plot_->windowToPixel(p);

  return pbbox.inside(pp);
}

void
CQChartsParallelPointObj::
getSelectIndices(Indices &inds) const
{
  addColumnSelectIndex(inds, modelInd().column());
}

void
CQChartsParallelPointObj::
draw(CQChartsPaintDevice *device)
{
  if (! visible())
    return;

  //---

  CQChartsParallelPlot *plot = const_cast<CQChartsParallelPlot *>(plot_);

  plot->setObjRange(device);

  //---

  // set pen and brush
  ColorInd colorInd = calcColorInd();

  QPen   pen;
  QBrush brush;

  plot_->setSymbolPenBrush(pen, brush, colorInd);

  plot_->updateObjPenBrushState(this, pen, brush, CQChartsPlot::DrawType::SYMBOL);

  //---

  // get symbol type and size
  CQChartsSymbol symbolType = plot_->symbolType();
  CQChartsLength symbolSize = plot_->symbolSize();

  double sx, sy;

  plot_->pixelSymbolSize(symbolSize, sx, sy);

  if (isInside() || isSelected()) {
    sx *= 2;
    sy *= 2;
  }

  CQChartsLength symbolSize1 = CQChartsLength(CMathUtil::avg(sx, sy), CQChartsUnits::PIXEL);

  //---

  // draw symbol
  QPointF p(x_, y_);

  const_cast<CQChartsParallelPlot *>(plot_)->
    drawSymbol(device, p, symbolType, symbolSize1, pen, brush);

  //---

  //plot->setNormalizedRange(device);
}
