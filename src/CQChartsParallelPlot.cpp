#include <CQChartsParallelPlot.h>
#include <CQChartsView.h>
#include <CQChartsAxis.h>
#include <CQChartsUtil.h>
#include <CQCharts.h>

#include <QAbstractItemModel>
#include <QPainter>

CQChartsParallelPlotType::
CQChartsParallelPlotType()
{
  addColumnParameter ("x", "X", "xColumn" , "", 0);
  addColumnsParameter("y", "Y", "yColumns", "", "1");
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
  //addKey(); TODO

  addTitle();
}

QString
CQChartsParallelPlot::
yColumnsStr() const
{
  return CQChartsUtil::toString(yColumns_);
}

bool
CQChartsParallelPlot::
setYColumnsStr(const QString &s)
{
  std::vector<int> yColumns;

  if (! CQChartsUtil::fromString(s, yColumns))
    return false;

  setYColumns(yColumns);

  return true;
}

void
CQChartsParallelPlot::
addProperties()
{
  CQChartsPlot::addProperties();

  addProperty("columns", this, "xColumn"     , "x"     );
  addProperty("columns", this, "yColumn"     , "y"     );
  addProperty("columns", this, "yColumns"    , "yset"  );
  addProperty("points" , this, "points"      , "shown" );
  addProperty("points" , this, "pointsColor" , "color" );
  addProperty("points" , this, "symbolName"  , "symbol");
  addProperty("points" , this, "symbolSize"  , "size"  );
  addProperty("points" , this, "symbolFilled", "filled");
  addProperty("lines"  , this, "lines"       , "shown" );
  addProperty("lines"  , this, "linesColor"  , "color" );
  addProperty("lines"  , this, "linesWidth"  , "width" );
}

void
CQChartsParallelPlot::
updateRange()
{
  QAbstractItemModel *model = this->model();

  if (! model)
    return;

  int n = numValues();

  for (int j = 0; j < numSets(); ++j) {
    yRanges_.emplace_back();

    CQChartsAxis *axis = new CQChartsAxis(this, CQChartsAxis::Direction::VERTICAL, 0, 1);

    yAxes_.push_back(axis);
  }

  for (int j = 0; j < numSets(); ++j) {
    CRange2D &range = yRanges_[j];

    int yColumn = getSetColumn(j);

    for (int i = 0; i < n; ++i) {
      bool ok;

      double x = 0;
      double y = CQChartsUtil::modelReal(model, i, yColumn, ok);

      if (! ok)
        y = i;

      if (CQChartsUtil::isNaN(y))
        continue;

      range.updateRange(x, y);
    }

    range.updateRange(          - 0.5, range.ymin());
    range.updateRange(numSets() - 0.5, range.ymin());
  }

  dataRange_.updateRange(          - 0.5, 0);
  dataRange_.updateRange(numSets() - 0.5, 1);

  for (int j = 0; j < numSets(); ++j) {
    const CRange2D &range = yRange(j);

    int yColumn = getSetColumn(j);

    QString name = model->headerData(yColumn, Qt::Horizontal).toString();

    setDataRange(range);

    yAxes_[j]->setRange(dataRange_.ymin(), dataRange_.ymax());
    yAxes_[j]->setLabel(name);
  }

  displayRange_.setWindowRange(-0.5, 0, numSets() - 0.5, 1);
}

void
CQChartsParallelPlot::
initObjs(bool force)
{
  if (force) {
    clearPlotObjects();

    dataRange_.reset();
  }

  //---

  if (! dataRange_.isSet()) {
    updateRange();

    if (! dataRange_.isSet())
      return;
  }

  //---

  if (! plotObjs_.empty())
    return;

  //---

  QAbstractItemModel *model = this->model();

  if (! model)
    return;

  std::vector<QPolygonF> polys;

  int n = numValues();

  for (int i = 0; i < n; ++i)
    polys.emplace_back();

  for (int i = 0; i < n; ++i) {
    QPolygonF &poly = polys[i];

    for (int j = 0; j < numSets(); ++j) {
      int yColumn = getSetColumn(j);

      bool ok;

      double x = j;
      double y = CQChartsUtil::modelReal(model, i, yColumn, ok);

      if (! ok)
        y = i;

      if (CQChartsUtil::isNaN(y))
        continue;

      poly << QPointF(x, y);
    }
  }

  //---

  // TODO: use actual symbol size
  double sw = (dataRange_.xmax() - dataRange_.xmin())/100.0;
  double sh = (dataRange_.ymax() - dataRange_.ymin())/100.0;

  for (int i = 0; i < n; ++i) {
    bool ok;

    QString xname = CQChartsUtil::modelString(model, i, xColumn_, ok);

    QPolygonF &poly = polys[i];

    CBBox2D bbox(-0.5, 0, numSets() - 0.5, 1);

    CQChartsParallelLineObj *lineObj = new CQChartsParallelLineObj(this, bbox, poly, i, n);

    int nl = poly.count();

    QString id = xname + "\n";

    for (int j = 0; j < nl; ++j) {
      int yColumn = getSetColumn(j);

      QString yname = model->headerData(yColumn, Qt::Horizontal).toString();

      id += QString("  %1\t%2\n").arg(yname).arg(poly[j].y());
    }

    lineObj->setId(id);

    addPlotObject(lineObj);

    //---

    for (int j = 0; j < nl; ++j) {
      const CRange2D &range = yRange(j);

      const QPointF &p = poly[j];

      double y1 = (p.y() - range.ymin())/(range.ymax() - range.ymin());

      CBBox2D bbox(j - sw/2, y1 - sh/2, j + sw/2, y1 + sh/2);

      CQChartsParallelPointObj *pointObj =
        new CQChartsParallelPointObj(this, bbox, j, y1, i, n, j, nl);

      int yColumn = getSetColumn(j);

      QString yname = model->headerData(yColumn, Qt::Horizontal).toString();

      QString id = QString("%1:%2=%3").arg(xname).arg(yname).arg(p.y());

      pointObj->setId(id);

      addPlotObject(pointObj);
    }
  }
}

int
CQChartsParallelPlot::
numSets() const
{
  if (yColumns_.empty())
    return 1;

  return yColumns_.size();
}

int
CQChartsParallelPlot::
getSetColumn(int i) const
{
  if (! yColumns_.empty())
    return yColumns_[i];
  else
    return yColumn_;
}

int
CQChartsParallelPlot::
numValues() const
{
  QAbstractItemModel *model = this->model();

  if (! model)
    return 0;

  return model->rowCount(QModelIndex());
}

void
CQChartsParallelPlot::
draw(QPainter *p)
{
  initObjs();

  //---

  drawBackground(p);

  //---

  QFontMetrics fm(view()->font());

  displayRange_.setWindowRange(dataRange_.xmin(), 0, dataRange_.xmax(), 1);

  drawObjs(p, Layer::MID);

  for (int j = 0; j < numSets(); ++j) {
    const CRange2D &range = yRange(j);

    setDataRange(range);

    displayRange_.setWindowRange(-0.5, dataRange_.ymin(), numSets() - 0.5, dataRange_.ymax());

    yAxes_[j]->setPos(j);

    yAxes_[j]->draw(this, p);

    QString label = yAxes_[j]->getLabel();

    double px, py;

    windowToPixel(j, dataRange_.ymax(), px, py);

    p->setPen(yAxes_[j]->getTickLabelColor());

    p->drawText(QPointF(px - fm.width(label)/2.0, py - fm.height()), label);
  }

  displayRange_.setWindowRange(-0.5, 0, numSets() - 0.5, 1);

  //---

  drawTitle(p);
}

//------

CQChartsParallelLineObj::
CQChartsParallelLineObj(CQChartsParallelPlot *plot, const CBBox2D &rect,
                        const QPolygonF &poly, int i, int n) :
 CQChartsPlotObj(rect), plot_(plot), poly_(poly), i_(i), n_(n)
{
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
inside(const CPoint2D &p) const
{
  if (! plot_->isLines())
    return false;

  QPolygonF poly;

  for (int i = 0; i < poly_.count(); ++i) {
    const CRange2D &range = plot_->yRange(i);

    double x = poly_[i].x();
    double y = (poly_[i].y() - range.ymin())/range.ysize();

    poly << QPointF(x, y);
  }

  for (int i = 1; i < poly.count(); ++i) {
    double x1 = poly[i - 1].x();
    double y1 = poly[i - 1].y();
    double x2 = poly[i    ].x();
    double y2 = poly[i    ].y();

    double d;

    CPoint2D pl1(x1, y1);
    CPoint2D pl2(x2, y2);

    if (CQChartsUtil::PointLineDistance(p, pl1, pl2, &d) && d < 1E-3)
      return true;
  }

  return false;
}

// TODO : interpY

void
CQChartsParallelLineObj::
draw(QPainter *p, const CQChartsPlot::Layer &)
{
  if (! plot_->isLines())
    return;

  QPolygonF poly;

  for (int i = 0; i < poly_.count(); ++i) {
    const CRange2D &range = plot_->yRange(i);

    double x = poly_[i].x();
    double y = (poly_[i].y() - range.ymin())/range.ysize();

    poly << QPointF(x, y);
  }

  QColor    lc = plot_->objectColor(this, i_, n_, plot_->linesColor());
  double    lw = plot_->linesWidth();
  CLineDash ld;

  if (isInside()) {
    if (lw <= 0)
      lw = 1;

    lw *= 3;
  }

  for (int i = 1; i < poly.count(); ++i) {
    double x1 = poly[i - 1].x();
    double y1 = poly[i - 1].y();
    double x2 = poly[i    ].x();
    double y2 = poly[i    ].y();

    double px1, py1, px2, py2;

    plot_->windowToPixel(x1, y1, px1, py1);
    plot_->windowToPixel(x2, y2, px2, py2);

    CQChartsLineObj::draw(p, QPointF(px1, py1), QPointF(px2, py2), lc, lw, ld);
  }
}

//------

CQChartsParallelPointObj::
CQChartsParallelPointObj(CQChartsParallelPlot *plot, const CBBox2D &rect, double x, double y,
                         int iset, int nset, int i, int n) :
 CQChartsPlotObj(rect), plot_(plot), x_(x), y_(y), iset_(iset), nset_(nset), i_(i), n_(n)
{
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
inside(const CPoint2D &p) const
{
  double px, py;

  plot_->windowToPixel(x_, y_, px, py);

  double s = plot_->symbolSize();

  CBBox2D pbbox(px - s, py - s, px + s, py + s);

  CPoint2D pp;

  plot_->windowToPixel(p, pp);

  return pbbox.inside(pp);
}

void
CQChartsParallelPointObj::
draw(QPainter *p, const CQChartsPlot::Layer &)
{
  if (plot_->isPoints()) {
    CSymbol2D::Type symbol = plot_->symbolType();
    QColor          c      = plot_->objectColor(this, i_, n_, plot_->pointsColor());
    bool            filled = plot_->isSymbolFilled();
    double          s      = plot_->symbolSize();

    CPoint2D pp(x_, y_);

    double px, py;

    plot_->windowToPixel(pp.x, pp.y, px, py);

    CQChartsPointObj::draw(p, QPointF(px, py), symbol, s, c, filled);
  }
}
