#include <CQChartsParallelPlot.h>
#include <CQChartsWindow.h>
#include <CQChartsAxis.h>
#include <CQChartsUtil.h>
#include <CQUtil.h>
#include <CMathGeom2D.h>

#include <QAbstractItemModel>
#include <QPainter>

CQChartsParallelPlot::
CQChartsParallelPlot(CQChartsWindow *window, QAbstractItemModel *model) :
 CQChartsPlot(window, model)
{
}

void
CQChartsParallelPlot::
addProperties()
{
  addProperty("columns", this, "xColumn", "x");
  addProperty("columns", this, "yColumn", "y");
}

void
CQChartsParallelPlot::
updateRange()
{
  int n = numValues();

  for (int j = 0; j < numSets(); ++j) {
    yRanges_.push_back(CRange2D());

    yAxes_.push_back(new CQChartsAxis(this, CQChartsAxis::DIR_VERTICAL, 0, 1));
  }

  for (int j = 0; j < numSets(); ++j) {
    int yColumn = getSetColumn(j);

    for (int i = 0; i < n; ++i) {
      double x = 0;
      double y = CQChartsUtil::modelReal(model_, i, yColumn);

      yRanges_[j].updateRange(x, y);
    }

    yRanges_[j].updateRange(          - 0.5, yRanges_[j].ymin());
    yRanges_[j].updateRange(numSets() - 0.5, yRanges_[j].ymin());
  }

  dataRange_.updateRange(          - 0.5, 0);
  dataRange_.updateRange(numSets() - 0.5, 1);

  for (int j = 0; j < numSets(); ++j) {
    int yColumn = getSetColumn(j);

    QString name = model_->headerData(yColumn, Qt::Horizontal).toString();

    setDataRange(yRanges_[j]);

    yAxes_[j]->setRange(dataRange_.ymin(), dataRange_.ymax());
    yAxes_[j]->setLabel(name);
  }

  displayRange_.setWindowRange(-0.5, 0, numSets() - 0.5, 1);
}

void
CQChartsParallelPlot::
initObjs()
{
  if (! plotObjs_.empty())
    return;

  std::vector<QPolygonF> polys;

  int n = numValues();

  for (int i = 0; i < n; ++i)
    polys.push_back(QPolygonF());

  for (int i = 0; i < n; ++i) {
    QPolygonF &poly = polys[i];

    for (int j = 0; j < numSets(); ++j) {
      int yColumn = getSetColumn(j);

      double x = j;
      double y = CQChartsUtil::modelReal(model_, i, yColumn);

      poly << QPointF(x, y);
    }
  }

  //---

  for (int i = 0; i < n; ++i) {
    QString xname = CQChartsUtil::modelString(model_, i, xColumn_);

    QPolygonF &poly = polys[i];

    CBBox2D bbox(-0.5, 0, numSets() - 0.5, 1);

    CQChartsParallelLineObj *lineObj = new CQChartsParallelLineObj(this, bbox, poly, i);

    int nl = poly.count();

    QString id = xname + "\n";

    for (int j = 0; j < nl; ++j) {
      int yColumn = getSetColumn(j);

      QString yname = model_->headerData(yColumn, Qt::Horizontal).toString();

      id += QString("  %1\t%2\n").arg(yname).arg(poly[j].y());
    }

    lineObj->setId(id);

    addPlotObject(lineObj);
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
  return model_->rowCount(QModelIndex());
}

void
CQChartsParallelPlot::
draw(QPainter *p)
{
  initObjs();

  //---

  drawBackground(p);

  //---

  QFontMetrics fm(window()->font());

  displayRange_.setWindowRange(dataRange_.xmin(), 0, dataRange_.xmax(), 1);

  for (const auto &plotObj : plotObjs_)
    plotObj->draw(p);

  for (int j = 0; j < numSets(); ++j) {
    setDataRange(yRanges_[j]);

    displayRange_.setWindowRange(-0.5, dataRange_.ymin(), numSets() - 0.5, dataRange_.ymax());

    yAxes_[j]->setPos(j);

    yAxes_[j]->draw(this, p);

    QString label = yAxes_[j]->getLabel();

    double px, py;

    windowToPixel(j, dataRange_.ymax(), px, py);

    p->setPen(Qt::black);

    p->drawText(QPointF(px - fm.width(label)/2.0, py - fm.height()), label);
  }

  displayRange_.setWindowRange(-0.5, 0, numSets() - 0.5, 1);
}

//------

CQChartsParallelLineObj::
CQChartsParallelLineObj(CQChartsParallelPlot *plot, const CBBox2D &rect,
                        const QPolygonF &poly, int ind) :
 CQChartsPlotObj(rect), plot_(plot), poly_(poly), ind_(ind)
{
}

bool
CQChartsParallelLineObj::
inside(const CPoint2D &p) const
{
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

    CLine2D line(CPoint2D(x1, y1), CPoint2D(x2, y2));

    double d;

    if (CMathGeom2D::PointLineDistance(p, line, &d) && d < 1E-3)
      return true;
  }

  return false;
}

void
CQChartsParallelLineObj::
draw(QPainter *p)
{
  p->setPen(Qt::black);

  QPolygonF poly;

  for (int i = 0; i < poly_.count(); ++i) {
    const CRange2D &range = plot_->yRange(i);

    double x = poly_[i].x();
    double y = (poly_[i].y() - range.ymin())/range.ysize();

    poly << QPointF(x, y);
  }

  QColor lineColor = plot_->objectColor(this, ind_, plot_->numValues(), Qt::black);

  QPen pen(lineColor);

  if (isInside())
    pen.setWidth(3);

  p->setPen(pen);

  for (int i = 1; i < poly.count(); ++i) {
    double x1 = poly[i - 1].x();
    double y1 = poly[i - 1].y();
    double x2 = poly[i    ].x();
    double y2 = poly[i    ].y();

    double px1, py1, px2, py2;

    plot_->windowToPixel(x1, y1, px1, py1);
    plot_->windowToPixel(x2, y2, px2, py2);

    p->drawLine(px1, py1, px2, py2);
  }
}
